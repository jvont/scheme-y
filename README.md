# Scheme-y

<figure style="text-align: center">
<img src="knifey-schemey.png">
<figcaption align="center">
  <b>"I see you've played knifey-scheme-y before"</b>
</figcaption>
<br>
</figure>

<br>

Scheme-y is yet another implementation of Scheme. The main objective of this project is to create a full Scheme interpreter, prioritizing code readability and simplicity over performance. It is broken up into the following parts, in recommended reading order:

`object` - Scheme objects.

`scheme-y` - Program entry point.

`runtime` - Scheme interpreter/parser + memory management.

`builtins` - Standard library procedures + initialization.

Installation requires `gcc` and `make`. You can build and run tests using `make tests`, otherwise you can just build using `make`.
