# Eatmemory

This is based on a github of the same name.

**[https://github.com/gonoph/eatmemory](https://github.com/gonoph/eatmemory)**

Written in C, this was used in kernel testing back in the day. It's been modernized with some helpful features.

# Usage

You can do something like this, and it will loop until you stop it.

* the default for the container limits it to 25% of visible RAM.
* it's cgroup aware, so it should behave correctly when given k8s or `--memory` limits with podman.
* you can attempt to avoid OOMKiller with the `--free` option to limit it to just free available memory.
* it churns through the memory with a delay to minimize CPU time and force the memory to stay out of swap.

```bash
podman run --rm -it quay.io/gonoph/eatmemory:latest
Trying to pull quay.io/gonoph/eatmemory:latest...
Getting image source signatures
Copying blob 2d5a31c2e9d9 skipped: already exists
Copying config bfbe95e936 done
Writing manifest to image destination
Storing signatures
using 25% of total memory 1710 megabytes for a target allocation of 427 megabytes
Consuming 427 megabytes
+ Created 427 megabytes
+ Allocated 447745368 bytes
++ Looped: 1
^C[exiting] Caught signal 2
```

# Configuration

For use with containers, it understands a single environment variable:

```yaml
# megabytes or % of available RAM to consume
env:
    name: EATMEGS
    value: 50%
```

Here is the more extensive help:

```bash
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
