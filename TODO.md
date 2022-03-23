# TODO List

- finish meta-circular evaluator

- make heap structures global (separate out from SchemeY state)
- save parse results left-to-right (ensuring they are reachable by GC)

- allocate interned symbols outside of managed heap - they are never collected
- roots are then values in global table, along with registers/stack
