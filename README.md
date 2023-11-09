# eatmemory
Small program that I released in the Public Domain in 2003 to chew memory

[![Docker Repository on Quay](https://quay.io/repository/gonoph/eatmemory/status "Docker Repository on Quay")](https://quay.io/repository/gonoph/eatmemory)

# Build
You should just need to run make:

```bash
make
```

# Usage
You can run it without arguments or with arguments, and it will also take an ENV variable.

```bash
./eatmemory  -h
Usage: eatmemory [ -fqrh ] [ -m metabytes ]
    -f | --free  : attempt to only allocate free memory
    -m | --megs  : the number of megabytes to consume (default 50%)
                   when % is 100% or more, all available RAM will be consumed.
    -r | --random: randomize buffer in order to prevent memory compression
    -q | --quiet : suppress output
    -h | --help  : this help screen

  ENVIRONMENT VARIABLES
    EATMEGS      : alternative to -m / --megs
```

This is what it looks like on MacOS 14 using the free and random parameters:
```bash
$ ./eatmemory -f -r
Limiting to 50% of available free memory: free=121; total=16384
using 50% of available memory 121 megabytes for a target allocation of 60 megabytes
Consuming 60 megabytes
+ randomizing memory enabled
+ Created 60 megabytes
+ Allocated 62915040 bytes
++ Looped: 1
++ Looped: 2
++ Looped: 3
++ Looped: 4
++ Looped: 5
++ Looped: 6
```

This is what it looks like running on a podman container with memory limits:
```bash
podman run --rm -it -v `pwd`:/compile:z --workdir=/compile --memory=500m registry.redhat.io/ubi9:latest ./eatmemory -f
Limiting to 50% of available free memory: free=500; total=500
using 50% of available memory 500 megabytes for a target allocation of 250 megabytes
Consuming 250 megabytes
+ detected running in a cgroup with memory limits.
+ Created 250 megabytes
+ Allocated 262146000 bytes
++ Looped: 1
++ Looped: 2
++ Looped: 3
^C[exiting] Caught signal 2
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
  use as much memory as a single page of memory (depending on the
  architecture). To consume all the memory that you allocated in the array,
  you have to touch every page/element.
* To keep it out of swap, you can mark it protected with (1) `mlock()`, which
  won't let it be swapped **AT ALL**, or (2) you can keep touching the memory
  so the kernel **CAN** swap it out, but has to keep swapping it back in.
* By using option (2), you can mimic a real system that's busy and memory
  starved.
* Linux can `overcommit` memory via `vm.overcommit_memory` being set to `1`.
  The default is `0` which means over-commit, but be **smart** about it.
  [You can find more here][2].
* If there is more memory allocated than can be physically given, then Linux
  fires off it's [Out of Memory Killer][3].

# Updates in 2023
I made a few updates in 2023, namely:

* I broke it up into multiple files
* Added command line parameters
* Added detection code for cgroups for total memory and available memory on Linux
* Added code to parse total and available memory on Darwin (it's messy)
* Included a Containerfile in a staged build:
  * it compiles the source.
  * then it calculates the shared libraries and copies it and binary to a staging area.
  * then it commits a thin image with just the binary and libraries.

# Link Back
If you end up using it, and enjoy it - then please let me know!

[1]: https://lkml.org/lkml/2004/12/13/272
[2]: https://www.kernel.org/doc/Documentation/vm/overcommit-accounting
[3]: https://www.kernel.org/doc/gorman/html/understand/understand016.html

# TODO

* add a .spec file to create an rpm
* improve the avail/total/free memory logic
* add support for other OS/architectures
