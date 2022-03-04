# Scheme-y

<figure style="text-align: center">
<img src="knifey-schemey.png">
<figcaption align="center">
  <b>"I see you've played knifey-scheme-y before"</b>
</figcaption>
<br>
</figure>

<br>

Scheme-y is an incomplete implementation of Scheme. The main objective of this project is to create a minimal Scheme interpreter in C, making it possible to implement all remaining procedures in Scheme itself. It is broken up into the following parts (in recommended reading order):

`object` - Scheme's object model. Contains `read` and `write` operations for parsing and outputting object representations.

`scheme-y` - Scheme-y's API, along with program entry point.

`builtins` - Standard library procedures + loading.

`mem` - Memory management using a copying collector.

Installation requires `gcc` and `make`. You can build and run tests using `make tests`, otherwise you can just build using `make`.
