FROM registry.access.redhat.com/ubi9/ubi:latest as compile
MAINTAINER billy@gonoph.net
ARG COMMAND=eatmemory
# install packages, but ensure it's only UBI content
RUN sed -i 's/enabled=1/enabled=0/' /etc/dnf/plugins/subscription-manager.conf \
	&& rm -f /etc/yum.repos.d/redhat.repo \ 
	&& dnf -y install make gcc \
	&& dnf clean all
RUN useradd compile \
	&& mkdir -p /compile \
	&& chown compile /compile
WORKDIR /compile
USER compile
COPY Makefile *.c *.h /compile
RUN make \
	&& mkdir -p dest/bin dest/proc dest/sys dest/tmp \
	&& for i in $(ldd $COMMAND | grep / | xargs echo) ; do test -r "$i" && (mkdir -p dest/$(dirname $i) ; cp -v $i dest/$i) || true ; done \
	&& cp -v $COMMAND dest

FROM scratch
MAINTAINER billy@gonoph.net
ARG COMMAND=eatmemory
COPY --from=compile /compile/dest/ /
ENTRYPOINT [ "/eatmemory" ]
ENV EATMEGS 25%
