# Tyger

An experimental dynamically typed scripting lanugae, written as an extension to my
time reading and working through [Thorsten Ball's](https://thorstenball.com/)
[Writing An Interpreter In Go](https://interpreterbook.com/). Project interpreter
is implemented using _C_ (_C99_) as I wanted to improve some of my low level programming
ability and knowledge.

The language and interpreter are distributed under the terms of the [Apache 2.0](./LICENSE)
license.

---

# Samples

## Hello, World

```go
var message = "Hello, World!";
println(message);
```

## Fibonacci Series

```go
var fib = func(n) {
    return if n < 2 {
        1
    } else {
        fib(n - 1) + fib(n - 2)
    };
};
```
