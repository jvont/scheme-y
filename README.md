# Scheme-y

<figure style="text-align: center">
  <img src="knifey-schemey.png">
  <figcaption align="center">
    <b>
      "That's not a knife, it's a Scheme!"
      <br>
      "I see you've played Knifey-Scheme-y before"
    </b>
  </figcaption>
</figure>
<br>

## Overview

Scheme-y is an incomplete implementation of Scheme. The main objective of this project is to create a minimal Scheme interpreter in C, making it possible to implement all remaining procedures using a C API or in Scheme itself. It is broken up into the following parts (in recommended reading order):

`object` - Scheme's object representation. Stores all of objects in 2 words, with a minimum alignment of 8 bytes.

`scheme-y` - C API, along with program entry point.

`heap` - Memory management. Contains methods for allocating objects and contiguous memory. Garbage collection is performed usinga generational (copying) garbage collector, based loosely on [Erlang's garbage collector](https://www.erlang.org/doc/apps/erts/garbagecollection). The collector is designed favouring simplicity.

`lib`-suffixed files - R7RS Standard procedures implemented in C.

`runtime` - Bytecode interpreter, including stack API. **Under construction.**

`compiler` - Bytecode compiler. Contains opcode definitions. **Under construction.**

For more information, see `doc/` folder.

## Installation

Installation requires `gcc` and `make`. You can build and run tests using 

```sh
make tests
```

otherwise you can just build using

```sh
make build
```
