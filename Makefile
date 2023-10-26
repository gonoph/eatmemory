.PHONY: all container container-run

CFLAGS := -Wall -Os
LDFLAGS := 
STRIP=/usr/bin/strip

CFILES := args.c cleanup.c eatmemory.c malloc.c sys_info.c
HFILES := args.h cleanup.h eatmemory.h malloc.h sys_info.h
OFILES := args.o cleanup.o eatmemory.o malloc.o sys_info.o

all: eatmemory

clean:
	$(RM) -fv eatmemory *.o .container-build
	
dist-clean: clean
	$(RM) -fv *~ tags

$(OFILES): $(CFILES) $(HFILES) Makefile

eatmemory: $(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) -o $@
	test -n "$(STRIP)" && $(STRIP) $@ || :

debug:
	$(MAKE) CFLAGS="-Wall -ggdb" STRIP="" clean all

container: .container-build

.container-build: Containerfile Makefile
	buildah build --tag registry.local/gonoph/eatmemory:latest .
	touch $@

container-run: .container-build
	podman run -it --memory 500m --name eatmemory --rm registry.local/gonoph/eatmemory:latest -m 100% -f
