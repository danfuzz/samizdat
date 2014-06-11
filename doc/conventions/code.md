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

### End-of-line comments

When adding comment to the end of a line, use the single-line comment
form in the language (e.g. `// ...` in C or C++), and separate the comment
from the code with two spaces.

```
someLineOfCode(stuff);  // Commentary here.
```

If multiple single-line comments are related, then they can optionally
be aligned by adding extra spaces, if it aids readability.

```
doThing();            // There's something to say about this.
doAnotherThing();
doYetOneMoreThing();  // There's also something to say about this.
```
