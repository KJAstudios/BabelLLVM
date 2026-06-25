# Babel Programming Language

Babel is inspired by the story of the Tower of Babel in the Bible, and [APL (A Programming Language)](https://en.wikipedia.org/wiki/APL_(programming_language)).

The idea behind the language is that it should mostly look like average code, but with every keyword being in a different language, and every control and operator character different from the standard characters used in most programming languages.

---

## Compiling Babel Programs

The Babel compiler is a command-line tool. Source files must use the `.bbl` extension to be recognized as valid Babel programs. Babel binaries are compatible with `lldb` for debugging.

**Usage:**

Linux / macOS:
```bash
./babel  [OPTIONS]
```

Windows (PowerShell):
```powershell
.\babel.exe  [OPTIONS]
```

### Command Line Arguments

| Flag | Description |
|------|-------------|
| `<file>.bbl` | Input source file (required, must have `.bbl` extension) |
| `-c` | Output to object file instead of executable |
| `-o <filename>` | Set the output filename |
| `-target <triple>` / `--target=<triple>` | Select an alternate build target |
| `-sysroot <path>` / `--sysroot=<path>` | Provide an alternate sysroot |

---

## Building from Source

### Dependencies

- **CMake** >= 3.20
- **Ninja**
- **LLVM 20** (with CMake config files — e.g. via `llvm-20-dev` on Linux or the [LLVM Windows installer](https://releases.llvm.org/))

### Steps

```bash
cd Babel
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR=<path/to/llvm/lib/cmake/llvm>
cmake --build build --parallel
cmake --install build --prefix staging
```

---

## Keywords

| Keyword | Translation | Language |
|---------|-------------|----------|
| `function` | `funkcjonować` | Polish |
| `if` | `əgər` | Azerbaijani |
| `else` | `それ以外` | Japanese |
| `print` | `tisk` | Czech |

---

## Character Map

| Standard | Babel | Usage |
|----------|-------|-------|
| `(` `)` | `⟅` `⟆` | Grouping / Call Arguments |
| `{` `}` | `꧁` `꧂` | Block Delimiters |
| `+` | `⊕` | Addition |
| `-` | `⊖` | Subtraction |
| `*` | `×` | Multiplication |
| `/` | `÷` |  Division |
| `<` | `≺` | Less Than |
| `>` | `≻` | Greater Than |
| `<=` | `≼` | Less Than or Equal To |
| `>=` | `≽` | Greater Than or Equal To |
| `==` | `≡` | Equal To |
| `!=` | `≠` | Not Equal To |
| `=` | `≔` | Assignment  |
| `,` | `᨞` | Function Argument Delimiter |
| `;` | `~` | Statement terminator |
| `//` | `✎` | Comment | 

---

## Syntax

### Statements

Each statement must end with `~`.
```
x ≔ 1 ⊕ 1~
```

### Functions

Functions are declared in the format `function (args) FunctionName {}`. Arguments are separated by `᨞`.
 The main function **must** be named `主要的`. 

**Main function declaration:**
```
funkcjonować ⟅⟆ 主要的꧁
...
꧂
```

**Custom function declaration:**
```
funkcjonować ⟅arg1 ᨞ arg2⟆ functionName꧁
...
꧂
```

### Assignment

Only integers are supported. Results of arithmetic can be used, but not comparisons.  Assignment uses `≔`:
`x ≔ 42~`
`x ≔ 1 ⊕ 1~`
`z ≔ x ⊕ y~`

### If / Else

```
əgər⟅x ≺ y⟆꧁
꧂
それ以外꧁
꧂
```

### Print

`tisk` writes the variable or value to the standard output:
```
tisk⟅x⟆~
```

### Comments
Comments are single lines beginning with `✎`
```
✎ This is a comment
```

> **Note:** Loops and return values are not yet defined.

---

## Example Program
```
funkcjonować ⟅⟆ 主要的꧁
  x ≔ 1 ⊕ 1~
  y ≔ 2 ⊖ 1~
  əgər⟅x ≺ y⟆꧁
    tisk⟅x⟆~
  ꧂
  それ以外꧁
    tisk⟅y⟆~
  ꧂
꧂
```
This prints `2` if `x < y`, otherwise prints `1`.
