Conventions, Decisions, and Guidelines
======================================

Code
----

This section is about coding in general, no matter what the language

### File header

All source files must start with a header. See the
[license file](../../LICENSE.md) for details about the content.

### File Length

Files of more than about 1,000 lines are generally considered to be
too long. The general aim is to split them into two or more files.

### Line length

Lines should should be made to contain 79 columns or fewer of text.

**Rationale:** Deeply-nested code is hard to read and follow. And in terms of
prose (e.g. comments or string literals), a very wide column of text is harder
to read than a narrow one. Finally, even today there are many contexts in
which it is only safe to assume that 79 columns can be printed without
inducing a spurious newline.

The only exception is "tabular" code (e.g., a series of calls with similar
structure). For these cases (and only these), it is acceptable to hit 100
columns. Beyond that, figure out a better way to format the code.

### Indentation and tabs

Indentation is four spaces for nested contructs. Tab characters are never
allowed, except when made a hard requirement by the source code format
(such as, notably, by the standard makefile syntax).

When splitting up a long statement or expression onto two or more lines,
use one extra level of indentation, unless that would cause confusion with
indentation on subsequent lines. In the latter case, use either
one-and-a-half or two extra levels of indentation, with two being preferred
except when that plays poorly with other alignment considerations.

For example, in the following, extra indentation is used to keep a function
prototype visually distinct from the implementation code:

```c
void someLongFunction(int with, int lots, int of, int arguments, int and,
        int so, int forth) {
    someCode();
    someMoreCode();
}
```

As an example of one-and-a-half indentation, take this case where it makes
sense to align tests within an `if`:

```c
if (     (someCall()      < oneKindOfValue)
      && (someOtherCall() < anotherKindOfValue)) {
    someCode();
    someMoreCode();
}
```

### End-of-line comments

When adding comment to the end of a line, use the single-line comment
form in the language (e.g. `// ...` in C or C++), and separate the comment
from the code with two spaces.

```c
someLineOfCode(stuff);  // Commentary here.
```

If multiple single-line comments are related, then they can optionally
be aligned by adding extra spaces, if it aids readability.

```c
doThing();            // There's something to say about this.
doAnotherThing();
doYetOneMoreThing();  // There's also something to say about this.
```
