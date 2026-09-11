# jnde notes — part 1

**1. `.so` library, not an executable, not a `.a`** ✅
`.a` = code copied into your binary at link time. `.so` = stays separate, loaded at run time — which is the only reason `LD_PRELOAD` can swap out libc's malloc. You never `#include` a library, only its `.h`.

**2. `-fPIC` = works at any memory address** ✅
Default is already `-fPIE` (also address-independent), but `-fPIE` lets the compiler call its own functions directly, skipping the lookup table. `-fPIC` keeps them replaceable — which is the whole point here.

**3. 2 files: the library + a symlink to it** ✅
`libft_malloc_x86_64_Linux.so` (real) + `libft_malloc.so` (symlink, stable name). `ln -sf` needs the `-f` or the 2nd `make` fails; `fclean` must delete both.

**4. `HOSTTYPE` = arch + OS, like `x86_64_Linux`** ⚠️
Careful: `x86_64_Linux` is *our* fallback (`uname -m`_`uname -s`) — bash usually sets `x86_64-linux-gnu`, and sometimes nothing at all. We only fill it in when it's empty.

5. the kernel handles the allocations in pages, so each 1 byte is 4096 pages. 
    for the free function to know how many bytes it needs to free at this address (the size), the malloc function saves that info to the address directly before it, so it is accessable by the free function.
    that is reserved in an extra 32 bytes header by malloc(t_block, its size if 32 bytes).

6. the header itself actually looks like this:
    t_block — 32 bytes
    ┌────────┬────────┬────────┬────────┬────────┐
    │ 0 – 7  │ 8 – 11 │ 12 – 15│ 16 – 23│ 24 – 31│
    ├────────┼────────┼────────┼────────┼────────┤
    │  size  │  free  │ padding│  prev  │  next  │
    │ size_t │  int   │ (hole) │ ptr    │  ptr   │
    └────────┴────────┴────────┴────────┴────────┘


    okayyy so, as the subject is asking, if the allocation if of type small or tiny, yaane <128 bytes aw < 1024 bytes, mnaamol 100 allocations of that type size for future use, if its large we don't, we only do the needed size.

    jnde's example: malloc(50) → rounded to 64 (ALIGNMENT is 16) → under 128 so TINY → take TINY_MAX = 128 → zone needs 32 + 100 × (128 + 32) = 16032 → round up to 4 pages = 16384

