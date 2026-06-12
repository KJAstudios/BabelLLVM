# Babel Programming Language

Babel is inspired by the story of the Tower of Babel in the Bible, and [APL (A Programming Language)](https://en.wikipedia.org/wiki/APL_(programming_language)).

The idea behind the language is that it should mostly look like average code, but with every keyword being in a different language, and every control and operator character different from the standard characters used in most programming languages.

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
| `=` | `≔` | Assignment  |
| `,` | `᨞` | Function Argument Delimiter |
| `;` | `~` | Statement terminator |

---

## Syntax

### Statements

Each statement must end with `~`.
```x ≔ 1 ⊕ 1~```

### Functions

Functions are declared in the format `function (args) FunctionName {}`. The main function **must** be named `主要的`.

**Main function declaration:**
```
funkcjonować ⟅⟆ 主要的꧁
...
꧂
```

### Assignment

Only integers are supported. Assignment uses `≔`:
`x ≔ 42~`

### If / Else

```
əgər⟅x ≺ y⟆꧁
꧂
それ以外꧁
꧂
```

### Print

'tisk' writes the variable or value to the standard output:
```
tisk⟅x⟆~
```

> **Note:** Loops, return values, and comments are not yet defined.

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
