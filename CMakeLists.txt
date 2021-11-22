cmake_minimum_required(VERSION 3.20)

# Build setup configured for OpenShift 3.11.

project(haproxy)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

enable_language(C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(prevent-in-source-builds)
include(use-ccache)
include(use-gold-linker)
include(CheckCCompilerFlag)

find_package(Git QUIET REQUIRED)

execute_process(COMMAND
  "${GIT_EXECUTABLE}" config --get remote.origin.url
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_ORIGIN_URL
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --dirty
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE res
    OUTPUT_VARIABLE HAPROXY_GIT_VERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)

set_property(GLOBAL APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/.git/index")

option(ENABLE_COVERAGE "Build with code coverage" OFF)

if(ENABLE_COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif()

string(TIMESTAMP TODAY "%Y-%m-%dT%H:%M:%S")

add_compile_options(-g -fno-strict-aliasing -Wdeclaration-after-statement -fwrapv -Wno-address-of-packed-member -Wno-null-dereference -Wno-unused-label -Wno-stringop-overflow  -DCONFIG_HAP_LINUX_TPROXY)
add_compile_options(-DENABLE_POLL -DENABLE_EPOLL -DUSE_POLL -DUSE_EPOLL -DUSE_OPENSSL -DUSE_PCRE -DUSE_ZLIB -DUSE_CRYPT_H -DUSE_LINUX_TPROXY -DUSE_GETADDRINFO -DUSE_THREADS)
add_compile_options(-DCONFIG_HAPROXY_VERSION="${HAPROXY_GIT_VERSION} ${GIT_ORIGIN_URL}")
add_compile_options(-DCONFIG_HAPROXY_DATE="${TODAY}")
  
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wno-padded -Wno-unused-parameter")
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENABLE_ASAN "Enable Address Sanitizer" ON)

if(ENABLE_ASAN)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer -ggdb3")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/ebtree)

set(EBTREE_SRCS
  ebtree/ebtree.c
  ebtree/eb32sctree.c
  ebtree/eb32tree.c
  ebtree/eb64tree.c
  ebtree/ebmbtree.c
  ebtree/ebsttree.c
  ebtree/ebimtree.c
  ebtree/ebistree.c)

set(HAPROXY_SRCS
  src/acl.c
  src/action.c
  src/applet.c
  src/arg.c
  src/auth.c
  src/backend.c
  src/base64.c
  src/buffer.c
  src/cache.c
  src/cfgparse.c
  src/channel.c
  src/checks.c
  src/chunk.c
  src/cli.c
  src/compression.c
  src/connection.c
  src/dns.c
  src/ev_poll.c
  src/ev_select.c
  src/fd.c
  src/filters.c
  src/flt_http_comp.c
  src/flt_spoe.c
  src/flt_trace.c
  src/freq_ctr.c
  src/frontend.c
  src/h1.c
  src/h2.c
  src/hash.c
  src/hathreads.c
  src/hdr_idx.c
  src/hpack-dec.c
  src/hpack-enc.c
  src/hpack-huff.c
  src/hpack-tbl.c
  src/lb_chash.c
  src/lb_fas.c
  src/lb_fwlc.c
  src/lb_fwrr.c
  src/lb_map.c
  src/listener.c
  src/log.c
  src/lru.c
  src/mailers.c
  src/map.c
  src/memory.c
  src/mux_h2.c
  src/mux_pt.c
  src/namespace.c
  src/pattern.c
  src/payload.c
  src/peers.c
  src/pipe.c
  src/protocol.c
  src/proto_http.c
  src/proto_tcp.c
  src/proto_udp.c
  src/proto_uxst.c
  src/proxy.c
  src/queue.c
  src/raw_sock.c
  src/regex.c
  src/sample.c
  src/server.c
  src/session.c
  src/sha1.c
  src/shctx.c
  src/signal.c
  src/ssl_sock.c
  src/standard.c
  src/stats.c
  src/stick_table.c
  src/stream_interface.c
  src/stream.c
  src/task.c
  src/tcp_rules.c
  src/time.c
  src/uri_auth.c
  src/vars.c
  src/xxhash.c
  src/ev_poll.c
  src/ev_epoll.c

  src/health_check.c
  )

add_executable(haproxy src/haproxy.c ${HAPROXY_SRCS} ${EBTREE_SRCS})
target_link_libraries(haproxy -lz -lssl -lcrypto -L/usr/lib -lpcreposix -lpcre -lpthread -ldl)
