# Infix-evaluator

Reads an arithmetic expression in infix notation, converts it to postfix (Reverse Polish Notation), prompts the user for the value of each variable, and prints the evaluated result.

---

## Compilation

Requires a C++20-capable compiler (GCC 10+ or Clang 12+).

```bash
g++ -std=c++20 -o infix-evaluator main.cpp
```

---

## Usage

```bash
./infix-evaluator
```

The program reads **one line** from `stdin` — the infix expression — then prompts for variable values on `stderr`. The postfix form and final result are written to `stdout`.

### Supported syntax

| Element | Examples |
|---|---|
| Binary operators | `+` `-` `*` `/` |
| Multiplication shorthand | `x` or `×` as alias for `*` |
| Grouping symbols | `()` `[]` `{}` |
| Integer constants | `0`, `42`, `-5`, `-100` |
| Variables | Any valid C++ identifier (`a`, `x1`, `my_var`) |
| Variable values | Can be negative (e.g. enter `-3` when prompted) |

Operator precedence follows standard arithmetic rules (`*` `/` bind tighter than `+` `-`). All three bracket pair types are interchangeable for grouping and must be properly matched (e.g. `(` must close with `)`, not `]`).

---

## Example

```
$ ./infix-evaluator
a + b * (c + 2)
Enter value for a: 3
Enter value for b: 5
Enter value for c: 2
a b c 2 + * +
23
```

### With `x` as multiplication sign

```
$ ./infix-evaluator
a x (b + c)
Enter value for a: 2
Enter value for b: 3
Enter value for c: 4
a b c + *
14
```

### With negative literals and values

```
$ ./infix-evaluator
-3 + a
Enter value for a: -7
-3 a +
-10
```

---

## Exit codes

| Code | Meaning | Example trigger |
|---|---|---|
| `0` | Success | Expression evaluated correctly |
| `1` | Syntax error | `a ++ b`, `(a + b]`, missing operand |
| `2` | Runtime error | Malformed postfix stack |
| `3` | Logical error | Division by zero |

All error messages go to `stderr`. Only the postfix string and the final result go to `stdout`, making the program safe for automated testing pipelines.

---

## Running with I/O redirection

```bash
# pipe expression and variable values in one shot
printf "a + b * (c + 2)\n3\n5\n2\n" | ./infix-evaluator

# redirect only stdout to a file (stderr prompts still appear on terminal)
./infix-evaluator > result.txt

# suppress prompts entirely and capture only the result
printf "a x b\n4\n5\n" | ./infix-evaluator 2>/dev/null
```
