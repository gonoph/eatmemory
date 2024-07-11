## Stage 1 : Compile program and stage dependencies
FROM registry.access.redhat.com/ubi9/ubi-minimal:latest as compile
MAINTAINER billy@gonoph.net
ARG COMMAND=eatmemory
# install packages, but ensure it's only UBI content
RUN	(test -r /etc/dnf/plugins/subscription-manager.conf \
	&& sed -i 's/enabled=1/enabled=0/' /etc/dnf/plugins/subscription-manager.conf)  \
	|| true \
	&& rm -f /etc/yum.repos.d/redhat.repo \ 
	&& microdnf -y install make gcc findutils \
	&& microdnf clean all \
	&& mkdir -p /compile \
	&& useradd compile \
	&& chown -Rc compile:compile /compile
USER	compile
COPY	Makefile *.c *.h /compile/
RUN	make -C /compile
USER	root
RUN	cd /compile \
	&& mkdir -p dest/bin dest/proc dest/sys dest/tmp dest/lib64 \
	&& for i in $(ldd $COMMAND | grep / | xargs echo) ; do \
	test -r "$i" && (mkdir -p dest/$(dirname $i) ; cp -v $i dest/$i) || true ; done \
	&& cp -v /compile/$COMMAND dest/

## Stage 2 : Create the final container image
FROM scratch
MAINTAINER billy@gonoph.net
ARG COMMAND=eatmemory
COPY --from=compile /compile/dest/ /
ENTRYPOINT [ "/eatmemory" ]
ENV EATMEGS 25%
