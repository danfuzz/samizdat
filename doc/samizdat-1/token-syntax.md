Samizdat Layer 1
================

Token Syntax
------------

The following is an in-language description of the parser tokens, as
modifications to the *Samizdat Layer 0* tokenization syntax.

```
punctuation = {/
    # ... original alternates from the base grammar ...
|
    "{/" |
    "/}" |
    ".." |
    "&&" |
    ["&" "|" "!"]
/};
```
