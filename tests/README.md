# tests — the evaluation sheet, step by step

These are the seven programs the 42 evaluation sheet walks through. The sheet links
`test0.c` … `test6.c` as attachments, but those links are dead on EvalHub, so **these are
reconstructions written from the sheet's own descriptions**. The evaluator will bring their
own copies — use these to know your numbers before you walk in.

Every number below was measured on this repo.

---

## Setup — do this once

From the repo root:

```bash
make re
cd tests
cp ../libft_malloc*.so .
cp run_linux.sh run.sh && chmod +x run.sh
```

The `.so` files **must sit next to the test binaries**, because `run.sh` uses
`LD_LIBRARY_PATH=.` and `LD_PRELOAD=libft_malloc.so` — both relative, exactly as the sheet
specifies. On macOS use `run_mac.sh`.

```bash
gcc -o test0 test0.c
gcc -o test1 test1.c
gcc -o test2 test2.c
gcc -o test3 test3.c
gcc -o test4 test4.c
gcc -o test5 test5.c
gcc -o test6 test6.c -L. -lft_malloc      # only test6 links the library
```

**Why test6 is different:** `test0`–`test5` only call `malloc`, `free` and `realloc`, which
`<stdlib.h>` already declares, so `LD_PRELOAD` reaches them at run time with no linking.
`test6` calls `show_alloc_mem`, which is not a libc function, so it has to be linked: `-L.`
means "look here" and `-lft_malloc` becomes the filename `libft_malloc.so`. That is why the
`lib` prefix and `.so` suffix are mandatory.

---

## The two gates before any testing

### Gate 1 — `HOSTTYPE` and the symlink

```bash
export HOSTTYPE=Testing
make re
ls -l libft_malloc.so
```

Must show `libft_malloc.so -> libft_malloc_Testing.so`. If not, **the defence stops**.
Afterwards: `unset HOSTTYPE && make re`.

Note: the sheet also prints an `ln -s` command, but your Makefile already creates the
symlink — if the evaluator types that line it fails with "File exists", which is correct.

### Gate 2 — exported symbols

```bash
nm libft_malloc.so
```

```
0000000000002101 T calloc
00000000000016cd T free
                 U getpagesize@GLIBC_2.2.5
000000000000134d T malloc
                 U mmap@GLIBC_2.2.5
                 U munmap@GLIBC_2.2.5
000000000000195b T realloc
0000000000001b73 T show_alloc_mem
                 U write@GLIBC_2.2.5
```

`malloc`, `free`, `realloc`, `show_alloc_mem` must all be `T`. If not, **the defence stops**.
The sheet's example shows macOS output (`_malloc` with an underscore) — on Linux there are
no underscores and you get `@GLIBC` suffixes. That difference is expected.

---

## 1. Page faults — test0, test1, test2

The only item graded **0 to 5**, so it matters most.

The measurement needs `/usr/bin/time`, which is a separate package. The sheet warns about
this — install it first or the `-v` option will not exist:

```bash
sudo apt install time
```

On macOS the flag is `-l` instead of `-v`.

```bash
# reference: the system allocator
/usr/bin/time -v ./test0
/usr/bin/time -v ./test1

# our allocator
./run.sh /usr/bin/time -v ./test0
./run.sh /usr/bin/time -v ./test1
./run.sh /usr/bin/time -v ./test2
```

The line to read in each block of output:

```
Minor (reclaiming frame) page faults: 363
```

### Measured here

| | test0 | test1 | test1 − test0 |
|---|---|---|---|
| system malloc | 89 | 348 | 259 pages |
| **this library** | **93** | **362** | **269 pages** |

`test1` asks for 1024 × 1024 bytes = 1 Mb = 256 pages minimum.

These counts move by a page or two between runs (ASLR, loader state), so expect 267–270
rather than one fixed number. All of it is inside the top band.

### The grading scale

| pages | grade | |
|---|---|---|
| under 255 | **0** | not enough memory really allocated |
| 1023 and over | 1 | a whole page wasted per allocation |
| 513 – 1022 | 2 | overhead too big |
| 313 – 512 | 3 | overhead very big |
| 273 – 312 | 4 | overhead big |
| **255 – 272** | **5** | overhead is fine ← **269 lands here** |

### The two free checks

| | page faults |
|---|---|
| test0 | 93 |
| test1 | 362 |
| **test2** | **93** |

- *"fewer page reclaims than test1?"* → 93 vs 362 ✅
- *"at most 3 more than test0?"* → identical ✅

---

## 2. Pre-allocated zones — a source-code check

The sheet asks the evaluator to read the code, not run anything: zones must hold at least
100 allocations of their class maximum, and be a multiple of `getpagesize()`.

Both are in `srcs/align_and_size.c`, in `zone_size()`:

```
tiny   32 + 100 × (128  + 32) = 16032   → 4 pages  = 16384   (fits 102)
small  32 + 100 × (1024 + 32) = 105632  → 26 pages = 106496  (fits 100)
```

Nothing hardcoded — the page size is read at run time, so a 16 KB-page machine gets
different, still correct numbers.

---

## 3. realloc — test3 and test4

```bash
./run.sh ./test3 Bonjour Bonjour
```
```
BonjourBonjour
```

```bash
./run.sh ./test4 Bonjour Bonjour
```
```
5000 bytes grown one at a time, all intact
Bonjour
```

`test4` grows a buffer one byte at a time from 1 to 5000, dragging it across all three size
classes, then re-checks every byte. It catches the two classic realloc bugs: copying the
wrong length instead of `min(old, new)`, and losing the contents when the block has to move.

---

## 4. Error management — test5

```bash
./run.sh ./test5 Bonjour
```
```
Bonjour
```

**`Bonjour` alone is the pass.** test5 fires every bad input first:

```
free(NULL)              free(0x1234)            free(&stack_variable)
malloc(0)               malloc(SIZE_MAX)        realloc(NULL, 16)
realloc(ptr, SIZE_MAX)  realloc(0x1234, 64)     realloc(&stack_var, 64)
realloc(ptr, 0)
```

Each prints a complaint if it misbehaves, so **any extra output is a failure**. A segfault
here means the "crash" flag and the defence stops.

It also checks that a failed `realloc` returns `NULL` **and leaves the original block
untouched** — a failed realloc must not have freed anything.

---

## 5. show_alloc_mem — test6

```bash
gcc -o test6 test6.c -L. -lft_malloc
./run.sh ./test6
```
```
TINY : 0x7F27910B4000
0x7F27910B4040 - 0x7F27910B4070 : 48 bytes
SMALL : 0x7F2790E8F000
0x7F2790E8F040 - 0x7F2790E8F240 : 512 bytes
LARGE : 0x7F2790D8E000
0x7F2790D8E040 - 0x7F2790E8E040 : 1048576 bytes
Total : 1049136 bytes
```

Kind and zone base on their own line, then `start - end : N bytes` per block sorted by
increasing address, then a grand total. `end − start` equals the size on every line. 42
becomes 48 because everything rounds up to 16.

---

## Everything at once

```bash
cd tests
cp ../libft_malloc*.so . && cp run_linux.sh run.sh && chmod +x run.sh
for t in 0 1 2 3 4 5; do gcc -o test$t test$t.c; done
gcc -o test6 test6.c -L. -lft_malloc

echo "-- pages --"
/usr/bin/time -v ./test0        2>&1 | grep "Minor"
./run.sh /usr/bin/time -v ./test0 2>&1 | grep "Minor"
./run.sh /usr/bin/time -v ./test1 2>&1 | grep "Minor"
./run.sh /usr/bin/time -v ./test2 2>&1 | grep "Minor"
echo "-- realloc --";        ./run.sh ./test3 Bonjour Bonjour
echo "-- realloc++ --";      ./run.sh ./test4 Bonjour Bonjour
echo "-- errors --";         ./run.sh ./test5 Bonjour
echo "-- show_alloc_mem --"; ./run.sh ./test6
```

---

## Where this repo stands on the sheet

| item | |
|---|---|
| something in the git repository | ✅ |
| author file is valid | ⚠️ **missing** — `echo "your-login" > author` |
| Makefile with all requested rules | ✅ `all` `clean` `fclean` `re` |
| no norm errors | ⚠️ see below |
| no cheating, allowed functions only | ✅ `mmap` `munmap` `getpagesize` `write` |
| at most 2 globals | ✅ one (`g_zones[3]`); second reserved for the bonus |
| `HOSTTYPE` + symlink | ✅ |
| exports the four functions as `T` | ✅ |
| page-fault grade | **5** (269 pages) |
| zones ≥ 100 allocations, page multiple | ✅ |
| free works | ✅ |
| free quality (≤ 3 more than test0) | ✅ identical |
| realloc, realloc++ | ✅ |
| error management | ✅ |
| show_alloc_mem format | ✅ |

**On the norm line:** the sheet says *"No norm errors, Norminette is authoritative"*, but that
is boilerplate from the generic 42 template. This subject says the opposite in writing:
*"Your project must be clean code even without the norm."* Specialization projects are not
norm-checked. If it comes up, the subject is the answer.
