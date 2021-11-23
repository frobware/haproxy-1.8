#!/usr/bin/env bash

set -eu

bear -- make -f Makefile.haproxy -j1 \
 CPU="generic" \
     TARGET="linux-2628" \
     USE_OPENSSL=1 \
     USE_PCRE=1 \
     USE_ZLIB=1 \
     USE_CRYPT_H=1 \
     USE_LINUX_TPROXY=1 \
     USE_GETADDRINFO=1 \
     USE_THREADS=1 \
     "$@"
