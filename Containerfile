## Stage 1 : Compile program and stage dependencies
FROM registry.access.redhat.com/ubi9/ubi-minimal:latest as compile
MAINTAINER billy@gonoph.net
ARG COMMAND=eatmemory
# install packages, but ensure it's only UBI content
RUN	(test -r /etc/dnf/plugins/subscription-manager.conf \
	&& sed -i 's/enabled=1/enabled=0/' /etc/dnf/plugins/subscription-manager.confs)  \
	|| true \
	&& rm -f /etc/yum.repos.d/redhat.repo \ 
	&& microdnf -y install make gcc \
	&& microdnf clean all \
	&& mkdir -p /compile
RUN	ls -l
COPY	Makefile *.c *.h /compile
RUN	make --version
RUN	gcc --version
RUN	make
WORKDIR /compile
RUN	mkdir -p dest/bin dest/proc dest/sys dest/tmp dest/lib64 \
	&& cp -v /lib64/libc.so.6 /lib64/ld-linux-x86-64.so.2 dest/lib64 \
	&& cp -v $COMMAND dest/

## Stage 2 : Create the final container image
FROM scratch
MAINTAINER billy@gonoph.net
ARG COMMAND=eatmemory
COPY --from=compile /compile/dest/ /
ENTRYPOINT [ "/eatmemory" ]
ENV EATMEGS 25%
