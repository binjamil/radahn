<div align="center">
  <img src="https://github.com/binjamil/radahn/assets/90112974/26612bee-6891-4534-9ddb-cda73d3ddc66" alt="Starscourge Radahn" width="350"/>
  <h1>Radahn</h1>
</div>

<p align="center">Radahn is a simple yet fast in-memory key-value store that speaks <a href="https://redis.io/docs/reference/protocol-spec/">Redis serialization protocol</a>.</p>

<p align="center">
  <a href="#overview"><strong>Overview</strong></a> ·
  <a href="#installation"><strong>Installation</strong></a> ·
  <a href="#usage"><strong>Usage</strong></a> ·
  <a href="#benchmarks"><strong>Benchmarks</strong></a>
</p>

## Overview

Radahn is designed to be simple. It supports a tiny fraction of Redis commands, only the most useful ones, so that there are minimum overhead costs. As a result of simple design philosophy, Radahn is ~1.4x faster than Redis ([see benchmarks below](#benchmarks)).

### Supported commands

- [PING](https://redis.io/commands/ping/)
- [GET](https://redis.io/commands/get/)
- [SET](https://redis.io/commands/set/) (only EX option is supported)
- [DEL](https://redis.io/commands/del/)

### Use case

Radahn is suitable to be used as a simple in-memory cache server that only needs to operate GET, SET and DEL commands. However, if your use case allows for multiple commands to be executed once, consider Redis as it supports pipelining.

## Installation

Radahn can be quickly installed using Docker.

### Prerequisites

- `docker`

### Steps

- Build Docker image
```sh
docker build -t radahn:latest .
```

- Run Docker container
```sh
docker run --rm -p 6369:6369 --name radahn -d radahn:latest
```

Radahn can also be installed by building from source. Currently, only the Linux platform is supported due to the use of its epoll API.

### Prerequisites

- `gcc`
- `make`

### Steps

- Run tests (optional)
```sh
make check
```

- Build the binary
```sh
make
```

- Run the Radahn server
```sh
./server
```

## Usage

Once the Radahn server is running, you can use any Redis client to connect and send RESP-compatible commands. Remember to the change the TCP port that your client connects to. Radahn by default listens on port 6369. 

The following demo uses `redis-cli`

```sh
redis-cli -p 6369
127.0.0.1:6369> PING
PONG
127.0.0.1:6369> SET hello world EX 20
OK
127.0.0.1:6369> GET hello
"world"
127.0.0.1:6369> DEL hello
(integer) 1
127.0.0.1:6369> GET hello
(nil)
127.0.0.1:6369> QUIT
```

## Benchmarks

Used the benchmarking utility that comes alongwith a default Redis installation called `redis-benchmark`. Ran the benchmark for commands GET and SET using 10 million requests and 10 thousand concurrent clients on both Radahn and Redis servers. 

```sh
redis-benchmark -n 10000000 -c 10000 -q -t get,set
```

Here are the results:

**Radahn**
```sh
SET: 53216.40 requests per second, p50=94.015 msec                      
GET: 53019.46 requests per second, p50=94.399 msec
```

**Redis**
```sh
SET: 38965.39 requests per second, p50=130.431 msec
GET: 39210.15 requests per second, p50=128.767 msec
```

Although Radahn is faster in basic benchmark, Redis can be configured to use pipelining to execute multiple commands at once. Radahn does not support pipelining. Here's an excerpt from Redis' website: 

> By default, `redis-benchmark` does not represent the maximum throughput a Redis instance can sustain. Actually, by using pipelining and a fast client (hiredis), it is fairly easy to write a program generating more throughput than redis-benchmark.
