### ✅ Full `README.md

```markdown
# 🧠 minilisp

[![Build](https://img.shields.io/badge/build-manual-blue.svg)](https://gcc.gnu.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Requires GMP](https://img.shields.io/badge/dependency-GMP-lightgrey.svg)](https://gmplib.org/)

A minimalist Lisp interpreter written in C — with lexical scoping, closures, quoting, and arbitrary-precision integers via [GMP](https://gmplib.org/). Built for learning and hacking.

---

## 📚 Table of Contents

- [Requirements](#requirements)
- [Building](#building)
  - [Minimal Debug](#minimal-debug)
  - [Verbose Debug](#verbose-debug)
- [Features](#features)
- [Usage](#usage)
- [License](#license)
- [Contributing](#️contributing)

---

## ✅ Requirements

You’ll need GMP installed to support large (arbitrary-precision) integers.

```bash
sudo apt install libgmp-dev
```

---

## 🛠️ Building

### Minimal Debug

Compile with only essential error messages:

```bash
gcc -DDEBUG_LEVEL=DEBUG_LEVEL_ERROR -std=c99 -g -o minilisp *.c -lgmp
```

### Verbose Debug

Enable full internal logging for development and debugging:

```bash
gcc -DDEBUG_LEVEL=DEBUG_LEVEL_VERBOSE -std=c99 -g -o minilisp *.c -lgmp
```

> Make sure all `.c` files are in your current directory when building.

---

## ✨ Features

- ✅ **Lexical scoping** via closures
- ✅ **First-class functions** with lambdas
- ✅ **Basic arithmetic** and conditionals
- ✅ **Quote handling** via `'expr` → `(quote expr)`
- ✅ **Arbitrary-precision integers** via GMP
- ✅ **Minimalist core** — easy to read and extend
- 🛠️ **Debug logging** with adjustable levels
- 💡 **Modular codebase** (separate files for eval, env, parser, tokenizer, etc.)

---

## 🚀 Usage

After compiling, run the interpreter:

```bash
./minilisp
```

Try a quick test in the REPL:

```lisp
(+ 1 2)
; => 3

(define fact
  (lambda (n)
    (if (= n 0) 1 (* n (fact (- n 1))))))

(fact 5)
; => 120
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).  
Feel free to use, modify, and distribute it.

---

## ❤️ Contributing

Got an idea? Found a bug? Open an issue or submit a PR — tiny hacks welcome!

---
```
