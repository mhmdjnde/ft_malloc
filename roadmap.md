# ft_malloc — Roadmap

A 10-part plan to build `libft_malloc_$HOSTTYPE.so`: our own `malloc`, `free` and
`realloc`, backed by `mmap(2)`/`munmap(2)`, plus the full bonus.

**How to use this file.** The parts are ordered so that each one compiles, runs and can
be tested on its own. Do part *N*, understand it completely, then move to *N+1*. Nothing
in a later part is needed to make an earlier part work.

**Ground rules that apply to every single part** (straight from the subject — breaking any
of them is an instant fail, so re-read this block before each part):

- The library is named `libft_malloc_$HOSTTYPE.so`, with a symlink `libft_malloc.so` next to it.
- **Exactly one global variable for allocations, and exactly one for thread-safety.** Not two, not three.
- **No `libc` `malloc` anywhere inside the project** — not for our data, not indirectly through
  `printf`, `strdup`, `getline` or anything else. Every byte we use comes from our own `mmap`.
- Only `mmap`, `munmap`, `sysconf(_SC_PAGESIZE)` / `getpagesize`, `getrlimit`, other syscalls,
  and `libpthread`. **We are not shipping a `libft`** — any helper we need (`ft_memcpy`,
  `ft_memset`, number and pointer printing on top of `write`) is reimplemented inside `srcs/`
  as we need it, which keeps the whole project in one place and one build.
- No undefined behaviour, no segfault, ever — including on garbage input like `free(some_random_pointer)`.
- Clean code even though there is no norm. "If it's ugly, you will get 0."

The picture of the whole data path lives in [`malloc_path.svg`](malloc_path.svg) — open it
next to this file while reading.

---

## Part 1 — Repository skeleton and the build system ✅ done

Before writing a single line of allocator logic, get the thing that the evaluator types
first — `make` — completely right, because it has hard requirements of its own. Lay the repo
out as `srcs/` (sources plus `ft_malloc.h`) and a root `Makefile`. That `Makefile` must handle
`$HOSTTYPE` (`ifeq ($(HOSTTYPE),)` → `HOSTTYPE := $(shell uname -m)_$(shell uname -s)`), build
a **shared object** — so every source is compiled `-fPIC` and linked `-shared` — producing
`libft_malloc_$(HOSTTYPE).so`, and then `ln -sf` a `libft_malloc.so` symlink onto it. Include
the usual rules (`all`, `clean`, `fclean`, `re`), and make the object rule depend on the header
so relinking only happens when it is actually necessary. **Deliverable:** a `malloc`/`free`/
`realloc`/`show_alloc_mem` that do nothing yet, but which compile clean under
`-Wall -Wextra -Werror`, produce both the `.so` and the symlink, export all four symbols, and
already get bound by the dynamic loader under `LD_PRELOAD`. Get this boring part perfect now —
it is the one thing you never want to debug at 3 a.m. later.

---

## Part 2 — The memory model: pick `n`, `m`, `N`, `M` and design the headers

This is a pen-and-paper part, and it is the single most important design decision in the
project. The subject says TINY allocations of `1..n` bytes live in `N`-byte zones, SMALL
allocations of `n+1..m` bytes live in `M`-byte zones, LARGE (`m+1` and up) gets its own
`mmap`, **each zone must hold at least 100 allocations**, and every zone size must be a
multiple of the page size. So the sizes are not arbitrary — they follow from one formula:

```
ZONE_SIZE = page_align( sizeof(t_zone) + 100 * (align16(MAX_ALLOC) + sizeof(t_block)) )
```

Design two structs. `t_block` is the per-allocation header — payload size, an in-use flag,
and `prev`/`next` pointers so blocks form a doubly-linked list inside their zone (the `prev`
link is what makes backward coalescing in Part 7 possible). `t_zone` is the per-mapping
header — its kind (TINY/SMALL/LARGE), its total mapped size, a pointer to its first block,
and a `next` pointer chaining zones of the same kind. **Alignment is a graded requirement:**
the pointer we hand back is `header + 1`, so `sizeof(t_block)` must itself be a multiple of
16 (the x86-64 `max_align_t`), and `sizeof(t_zone)` must be too, otherwise every payload in
the zone is misaligned. Pad both explicitly. A worked example with a 4096-byte page,
`sizeof(t_zone) == 48` and `sizeof(t_block) == 32`:

| kind  | max alloc (`n`, `m`) | required bytes                | zone size (`N`, `M`) |
|-------|----------------------|-------------------------------|----------------------|
| TINY  | 128                  | `48 + 100*(128+32) = 16048`   | 4 pages = **16384**  |
| SMALL | 1024                 | `48 + 100*(1024+32) = 105648` | 26 pages = **106496** |
| LARGE | anything above 1024  | computed per call             | `page_align(48 + 32 + size)` |

**Deliverable:** `srcs/ft_malloc.h` grown to hold the two structs, the constants, the `align16()`
and `page_align()` helpers, and compile-time `static_assert`-style checks (or a comment
proving the arithmetic). Write these numbers down — you will be asked to justify every one
of them at the defence, and "I copied them" is not an answer.

---

## Part 3 — Zones: getting memory from the kernel

Now write the only code in the project that talks to the kernel for memory. `create_zone(kind)`
computes the right size from Part 2, calls
`mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)`, checks the result
against `MAP_FAILED` (**not** `NULL` — this is the classic bug), writes the `t_zone` header at
offset 0, writes one giant free `t_block` covering the rest of the mapping, and pushes the
zone onto the right list. This is also where **the one allowed global** appears: a single
`static t_arena g_arena;` holding three list heads (`tiny`, `small`, `large`). Three heads in
one struct is one global; three separate variables is three globals and a failed defence.
Cache the page size once in that same struct rather than calling `sysconf` on every
allocation. Optionally call `getrlimit(RLIMIT_AS)` here to refuse absurd requests before
`mmap` does. **Deliverable:** a zone can be created and its `t_zone` fields inspected in a
debugger; nothing user-facing works yet. Understand exactly why the first block's payload
lands on a 16-byte boundary before moving on.

---

## Part 4 — `malloc` for TINY and SMALL

The core of the project. `malloc(size)`: reject nonsense first (a `size` of 0 — pick a
behaviour and document it; a `size` that overflows when you add the header and round up),
classify by size into TINY or SMALL, then walk that kind's zone list looking for a free block
whose payload is at least `align16(size)`. If none of the existing zones has room, create a
new zone of that kind (Part 3) and search again. When a suitable block is found, **split it**:
if the leftover after taking `align16(size)` is big enough to hold a header plus a minimum
useful payload, carve a new free block out of the tail and link it in; if not, hand over the
whole block rather than creating an unusable sliver. Mark the block in use and return
`(void *)(block + 1)`. First-fit is fine and is what most implementations do; if you go
best-fit, be ready to explain the trade-off. Two things to be strict about: never return a
pointer into the header area, and never let the split leave a zone's block list inconsistent.
**Deliverable:** a test that mallocs a few hundred TINY and SMALL blocks, writes a pattern
into every one, reads it back, and confirms no two returned pointers overlap and all are
16-byte aligned.

---

## Part 5 — `malloc` for LARGE

LARGE is the easy case and a good breather after Part 4. Any request above `m` gets its own
`mmap` of `page_align(sizeof(t_zone) + sizeof(t_block) + align16(size))`, holding exactly one
block that is always in use, chained onto the `large` list. There is no pre-allocation and no
splitting here — the whole point of the LARGE class is that these are rare and big enough that
one syscall each is acceptable, whereas doing that for TINY would destroy performance (which is
precisely why the subject demands pre-allocated zones). The reason we still give LARGE a
`t_zone` header instead of tracking it in a side table is uniformity: `show_alloc_mem` and
`free` then walk one kind of structure. Watch the overflow check carefully here, since this is
the path a hostile `malloc(SIZE_MAX)` takes — `page_align(x + overhead)` must not wrap around to
a tiny number. **Deliverable:** `malloc(1)`, `malloc(1024)`, `malloc(10 * 1024 * 1024)` and
`malloc(SIZE_MAX)` all behave (the last returning `NULL`, not crashing).

---

## Part 6 — `show_alloc_mem()`

Implement the required visualiser now, while `free` does not exist yet — it becomes your
debugger for every remaining part, so building it early pays for itself many times over.
The prototype is fixed: `void show_alloc_mem(void);`. Walk the three zone lists and print,
for each zone, its kind and base address, then one line per **in-use** block with its start
address, end address and payload size, finishing with the grand total of allocated bytes:

```
TINY : 0xA0000
0xA0020 - 0xA004A : 42 bytes
SMALL : 0xAD000
0xAD020 - 0xADEAD : 3725 bytes
LARGE : 0xB0000
0xB0020 - 0xBBEEF : 48847 bytes
Total : 52698 bytes
```

Two constraints that are easy to miss. First, output must be **sorted by increasing address** —
zones are pushed onto the head of their list, so the list order is roughly *decreasing*
address; insert zones in address order, or sort while printing. Second, you cannot use
`printf` — it may call `malloc` and would recurse into your own allocator. Write the number
and pointer formatting yourself on top of `write(2)`, into a small stack buffer.
**Deliverable:** run the Part 4/5 test, call `show_alloc_mem()`, and check the byte total by
hand against what you asked for.

---

## Part 7 — `free()`, coalescing, and giving zones back

`free(ptr)` must be paranoid, because it is where a wrong pointer turns into a segfault and a
failed defence. `free(NULL)` is a documented no-op. For anything else, **validate before you
trust**: walk the zone lists and confirm the pointer actually falls inside one of our mappings
and lands exactly on a `block + 1` boundary; if it does not, return silently rather than
dereferencing a header that is not there. Once validated, clear the in-use flag, then
**coalesce** — merge the block with its `next` neighbour if that one is free, and with its
`prev` neighbour if that one is free. This immediate two-sided merge is what keeps
fragmentation from growing without bound and is the foundation the bonus defragmentation
builds on. Finally, if a zone has become one single free block spanning its whole mapping,
`munmap` it and unlink it — **except** that you should keep one TINY and one SMALL zone alive,
because the subject explicitly asks you to limit `munmap` calls, and a program that allocates
and frees in a loop would otherwise thrash the kernel. LARGE zones are always unmapped
immediately. **Deliverable:** allocate a lot, free everything, and `show_alloc_mem()` prints
`Total : 0 bytes` with at most the two retained zones still mapped.

---

## Part 8 — `realloc()`

`realloc` looks small and hides more edge cases than the rest of the project combined, so give
it its own part and enumerate them explicitly. `realloc(NULL, size)` is exactly `malloc(size)`.
`realloc(ptr, 0)` frees and returns whatever your `malloc(0)` returns — be consistent with the
choice you made in Part 4. `realloc(bad_ptr, size)` must not crash: reuse the Part 7 validator.
For a real resize, first look for a way to avoid copying. Shrinking within the same size class
can stay in place, splitting off the tail into a new free block. Growing can also stay in place
if the `next` block is free and the two together are large enough — absorb it and split the
remainder. Only when neither works do you fall back to `malloc` the new size, `memcpy`
**`min(old_size, new_size)`** bytes (copying `new_size` reads past the end of the old block;
copying `old_size` when shrinking writes past the end of the new one — get this exactly right),
`free` the old pointer, and return the new one. If the `malloc` fails, return `NULL` **and leave
the original allocation untouched** — a `realloc` that fails must not have freed anything.
Crossing a size-class boundary (TINY → SMALL, SMALL → LARGE) always takes the copy path.
**Deliverable:** a test that grows a buffer one byte at a time from 1 to 100000, checking after
every step that the previously written contents are still intact.

---

## Part 9 — Hardening against real programs

The mandatory part is now feature-complete, and this part is what turns "it passes my tests"
into "it passes the defence". Run real binaries against it:
`LD_PRELOAD=./libft_malloc.so ls -la`, then `cat`, `wc`, `sed`, `git`, `bash`, `python3`.
Expect breakage, and expect it in a specific place: programs also call `calloc`, and glibc's
`calloc` uses glibc's `malloc`, so its pointers will later reach *your* `free`, which correctly
rejects them — and the program leaks or dies. So implement `calloc(nmemb, size)` (with the
`nmemb * size` overflow check) and, on Linux, `reallocarray` if you hit it; the subject allows
extra functions as long as you can justify them at the defence, and "the loader will hand my
`free` pointers I never allocated otherwise" is a perfect justification. Also check the
reentrancy story: your allocator must survive being called before `main`, from a signal-adjacent
context, and from inside the dynamic loader. Then build a proper `tests/` directory covering
zero-size, huge-size, double-free, free-of-stack-pointer, free-of-middle-of-block, allocate-free
churn, and a fragmentation stress test — plus a timing comparison against system `malloc` so you
have a number to quote. **Deliverable:** `ls -la` and `bash -c 'echo hi'` run cleanly under
`LD_PRELOAD`, and every test in `tests/` passes.

---

## Part 10 — The bonus: thread-safety, debug env vars, `show_alloc_mem_ex()`, defragmentation

Only start this once Part 9 is genuinely finished — the subject is blunt that a bonus is not
graded at all unless the mandatory part is *perfect*. Four pieces, in this order.
**(a) Thread-safety**, which is the first bonus and the one that matters most: add the second
allowed global, a `pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;`, and lock at the entry
of the four public functions. Do this by splitting each into a locking public wrapper and an
unlocked internal `_impl` — that way `realloc` can call the internals without deadlocking on a
non-recursive mutex. Register a `pthread_atfork` handler so a `fork()` from a thread holding the
lock does not leave the child with a permanently locked allocator, and prove it with a stress
test hammering all four functions from many threads.
**(b) Debug environment variables**, read once via `getenv` at first use: something like
`MALLOC_SCRIBBLE` (fill fresh allocations with `0xAA` and freed ones with `0x55`, which makes
use-after-free crash loudly instead of silently working), `MALLOC_GUARD` (canary values around
each payload, checked on `free` to catch overflows), `MALLOC_TRACE` (log every operation to
stderr via `write`), and `MALLOC_ABORT_ON_ERROR`.
**(c) `show_alloc_mem_ex()`**, the extended view: everything `show_alloc_mem` prints, plus free
blocks, per-zone fragmentation percentages, a hex + ASCII dump of each allocated payload, and a
ring-buffer history of the last *N* operations (kept in memory you allocated yourself, of course).
**(d) Defragmentation** beyond the immediate coalescing of Part 7: a pass that merges every
adjacent free pair across all zones, releases any zone that is now entirely free, and — if you
want the full mark — reports before/after fragmentation figures through `show_alloc_mem_ex()`.
**Deliverable:** the multi-threaded stress test passes under `-fsanitize=thread`, each env var
demonstrably changes behaviour, and you can show a fragmented heap being cleaned up on screen.

---

## Quick reference

| | |
|---|---|
| Library | `libft_malloc_$HOSTTYPE.so` + symlink `libft_malloc.so` |
| Mandatory API | `malloc` · `free` · `realloc` · `show_alloc_mem` |
| Bonus API | `show_alloc_mem_ex` · `calloc` · debug env vars |
| Globals allowed | **1** for allocations, **1** for thread-safety |
| Syscalls | `mmap` · `munmap` · `sysconf(_SC_PAGESIZE)` · `getrlimit` |
| Forbidden | libc `malloc`/`free`/`realloc`, `printf` and anything that allocates |
| Try it | `LD_PRELOAD=./libft_malloc.so ls -la` |
