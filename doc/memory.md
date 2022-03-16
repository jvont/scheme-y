# Memory Management

## Object Memory Layout

There are two types of objects in Scheme: lists and atoms. Lists store a `car` and `cdr` pointer to other objects, representing a singly linked list. Atoms store tagged data, including numbers, strings, arrays, function pointers, and streams.

Below is an example of how the pair `(x . 42)` is stored in memory.

```
+-----------+    +-----------+
| car | cdr +--->| int | 42  |
+--+--------+    +-----------+
   |
   |    +-----------+
   +--->+ str | "x" |
        +-----------+
```

## Garbage Collector

G1 contains tenured objects

Heap contains our roots, stack contains objects which may or may not be 

| from      |           | | to        |           |
|-----------|-----------|-|-----------|-----------|
| **addr**  | **data**  | | **addr**  | **data**  |
| 4         | e         | | 9         |           |
| 3         | d         | | 8         |           |
| 2         | c         | | 7         |           |
| 1         | b         | | 6         |           |
| 0         | a         | | 5         |           |



| from      |           | | to        |           |
|-----------|-----------|-|-----------|-----------|
| **addr**  | **data**  | | **addr**  | **data**  |
| 4         | 7         | | 9         |           |
| 3         | d         | | 8         |           |
| 2         | c         | | 7         | e         |
| 1         | 6         | | 6         | b         |
| 0         | 5         | | 5         | a         |





