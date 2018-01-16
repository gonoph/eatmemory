# eatmemory
Small program that I released in the Public Domain in 2003 to chew memory

# Build
You should just need to run make:

```bash
make
```

# Usage
You can run it without arguments or with arguments:

```bash
./eatmemory
Enter Megabytes to chew: 100

Consuming 100 megabytes
./eatmemory 100
Consuming 100 megabytes
```

# Con Kolivas kernel patch-set
Originally, I wrote this to help test the ck-kernel when I was busy helping out
gentoo Linux. I can't find the original posting to the mailing list, but
eventually, it was [picked up in the LKML.][1]

# Purpose
It was designed to not only eat a bunch of memory, but to keep it out of swap.
Some things to note about memory allocation (at least in Linux):

* Memory isn't actually allocated until touched: if you allocate an array a
  billion bytes long, it'll take up as much space as the pointer to describe
  it. If you touch an element in the array (read or write to it), then it'll
  use as much memory as a single page of memory (depends on the architecture).
  To consume all the memory that you allocated in the array, you have to touch
  every page/element.
* To keep it out of swap, you can mark it protected with (1) `mlock()`, which
  won't let it be swapped **AT ALL**, or (2) you can keep touching the memory
  so the kernel **CAN** swap it out, but has to keep swapping it back in.
* By using option (2), you can mimic a real system that's busy and memory
  starved.
* Linux can `overcommit` memory via `vm.overcommit_memory` being set to `1`.
  The default is `0` which mean over-commit, but be **smart** about it.
  [You can find more here][2].
* If there is more memory allocated than can be physically given, then Linux
  fires off it's [Out of Memory Killer][3].

# Link Back
If you end up using it, and enjoy it - then please let me know!

[1]: https://lkml.org/lkml/2004/12/13/272
[2]: https://www.kernel.org/doc/Documentation/vm/overcommit-accounting
[3]: https://www.kernel.org/doc/gorman/html/understand/understand016.html
