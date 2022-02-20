# Scheme-y

<figure style="text-align: center">
<img src="knifey-schemey.png">
<figcaption align="center">
  <b>"I see you've played knifey-scheme-y before"</b>
</figcaption>
</figure>

<br>

Scheme-y is yet another implementation of Scheme. It is decidedly unstable, but I hope somebody might be able to learn something from reading its source. It is broken up into the following parts:

`builtins` - Standard procedures.

`object` - Scheme's object model.

`runtime` - Memory management.

`scanner` - Token scanner.

Installation requires `gcc`, `make`. Optionally, `libreadline` for Linux may be used to make the REPL easier to use. You can build and run tests using `make tests`. Otherwise, you can just build using `make`.
