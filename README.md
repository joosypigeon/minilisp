# MiniLisp

**MiniLisp** is a small Lisp interpreter written in C, designed for clarity, extensibility, and educational use. It supports symbolic expressions, lexical scoping, closures, and large integer arithmetic using the GMP library.

---

## 🔧 Build Instructions

### Dependencies

- C compiler (e.g. GCC or Clang)  
- GMP (GNU Multiple Precision Arithmetic Library)  


### Build (Linux)

```bash
sudo apt install libgmp-dev

gcc -g -o minilisp *.c -lgmp
```



---

## 📁 Project Structure

- `main.c` — interpreter entry point and REPL  
- `eval.c` / `eval.h` — core evaluation logic  
- `env.c` / `env.h` — environment and variable binding  
- `object.c` / `object.h` — Lisp object representation  
- `parse.c` / `parse.h` — S-expression parser  
- `token.c` / `token.h` — lexer/tokenizer  
- `gc.c` / `gc.h` — basic mark-and-sweep garbage collector  
- `debug.h` — debug macros  
- `test/` — test scripts and test runner

---

## ✅ Current Features

- ✅ Symbols, integers, strings, lists  
- ✅ Large integer arithmetic via GMP (`mpz_t`)  
- ✅ Lexically scoped closures  
- ✅ First-class lambda functions  
- ✅ Quote reader macro and `quote` special form  
- ✅ Core special forms: `define`, `if`, `lambda`, `quote`, `begin`, `let`, `cond`  
- ✅ Primitive arithmetic and comparison operations  
- ✅ Structured error handling  
- ✅ Mark-and-sweep garbage collection  
- ✅ Regression tests for key language features
