# Radahn
Radahn is a simple yet fast in-memory key-value store that speaks [Redis serialization protocol](https://redis.io/docs/reference/protocol-spec/).

## Overview

Radahn is designed to be simple. It supports a tiny fraction of Redis commands, only the most useful ones, so that there are minimum overhead costs. As a result of simple design philosophy, Radahn is ~1.4x faster than Redis ([see benchmarks below](##benchmarks)).

### Supported commands

### Use case

Radahn is suitable to be used as a simple in-memory cache server that only needs to operate GET, SET and DEL commands. However, if your use case allows for multiple commands to be executed once, consider Redis as it supports pipelining.

## Installation

Radahn can be installed by building from source. Currently, only the Linux platform is supported due to the use of its epoll API.

### Prerequisites

- `gcc`
- `make`

## Usage

Once the Radahn server is running, you can use any Redis client to connect and send RESP-compatible commands. Remember to the change the TCP port that your client connects to. Radahn by default listens on port 6369. The following demo uses `redis-cli`

## Benchmarks

```sh
redis-benchmark -n 10000000 -c 10000 -q -t get,set
```

Radahn
```sh
SET: 52716.20 requests per second, p50=94.783 msec
GET: 52951.24 requests per second, p50=94.335 msec
```
Redis
```sh
SET: 38965.39 requests per second, p50=130.431 msec
GET: 39210.15 requests per second, p50=128.767 msec
```

Although Radahn is faster in basic benchmark, Redis can be configured to use pipelining to execute multiple commands at once. Radahn does not support pipelining. Here's an excerpt from Redis' website: 

> By default, `redis-benchmark` does not represent the maximum throughput a Redis instance can sustain. Actually, by using pipelining and a fast client (hiredis), it is fairly easy to write a program generating more throughput than redis-benchmark.
