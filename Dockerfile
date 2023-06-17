FROM alpine:latest
RUN apk --no-cache add make gcc g++ musl-dev binutils autoconf automake libtool pkgconfig check-dev file patch
COPY . /usr/src/radahn
WORKDIR /usr/src/radahn
RUN make
RUN make check
CMD ["./server"]
