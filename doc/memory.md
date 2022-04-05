# Memory Management

The garbage collection scheme outlined below is inspired by [Erlang's garbage collector](https://www.erlang.org/doc/apps/erts/garbagecollection).

## Allocation

All data is allocated in preallocated memory blocks called **Chunks**. Chunks represent system-allocated blocks of general-purpose memory, stored as elements in a singly-linked list. New data is allocated at the next available position of the current Chunk. Whenever more memory is needed, a Chunk of size greater than or equal to the requested memory size is pushed to the front of the list.

Separate lists of Chunks are used to store Scheme objects and their associated data (strings, vectors, etc.), to allow objects to be collected efficiently using Cheney's algorithm for copying collection. Each pair of lists of Chunks is called a **Generation**, and the heap is composed of a young and old generation.

## Garbage collection

The heap uses a generational garbage collector to manage memory. New objects are allocated in the young generation, `g0`. When a collection occurs, references held by root objects (located in a Scheme state) to objects in `g0` are copied to a transitory generation, called the to-space, using breadth-first search. Once all object references are copied, the to-space is swapped with `g0`. The new size of `g0` is marked, and the transitory generation is resized based on the current number of allocated objects, converting any additional Chunks into a single Chunk of the appropriate size.

Objects in `g0` which live longer than one collection (based on the marked size of `g0`) are copied to the old generation, `g1`. Collection of `g1` occurs after a specified number of collections of `g0`, using the same procedure as above. In a full collection, objects from both `g0` and `g1` are copied to the to-space, before swapping it with `g1`.
