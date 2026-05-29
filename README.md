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

| Standard | Babel |
|----------|-------|
| `(` `)` | `⟅` `⟆` |
| `{` `}` | `꧁` `꧂` |
| `+` | `⊕` |
| `-` | `⊖` |
| `*` | `×` |
| `/` | `÷` |
| `<` | `≻` |
| `>` | `≺` |
| `=` | `≔` |
| `,` | `᨞` |

---

## Syntax

### Functions

Functions are declared in the format `function (args) FunctionName {}`. The main function **must** be named `主要的`.

**Main function declaration:**
funkcjonować ⟅⟆ 主要的꧁꧂

### Statements

Each statement line must end with `~`.

### Assignment

No types are currently required, and standard math rules apply:
x ≔ 1 ⊕ 1~

### If / Else

If/Else statements follow standard C-style conventions with the substituted symbols:
əgər⟅x ≺ y⟆꧁
꧂
それ以外꧁
꧂

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
꧂```
