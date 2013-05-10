Parsing in Samizdat
===================

[It's not yet clear what layer this will end up at.]

Since parsing is something so many programs have to do, Samizdat offers
language-level facilities for building parsers.

Example
-------

The classic "four function calculator" as an example. In this case, it is
done as a unified tokenizer / tree parser. For simplicity, we don't bother
with whitespace-related rules.

```
digit = {:
    ch=("0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9")
    :: <> isub (intFromString ch) (intFromString "0")
:};

number = {:
    digits=digit+
    ::
    <> listReduce 0 digits { result digit :: <> iadd digit (imul result 10) }
:};

atom = {:
    number
    |
    "(" ex=addExpression ")" :: <> ex
:};

addExpression = {:
    ex1=mulExpression op=addOp ex2=addExpression
    :: <> op ex1 ex2
:};

addOp = {:
    "+" :: <> iadd
    |
    "-" :: <> isub
:};

mulExpression = {:
    ex1=unaryExpression op=mulOp ex2=mulExpression
    :: <> op ex1 ex2
:};

mulOp = {:
    "*" :: <> imul
    |
    "/" :: <> idiv
:};

unaryExpression = {:
    op=unaryOp ex=unaryExpression :: <> unaryOp ex
    |
    atom
:};

unaryOp = {:
    "-" :: <> ineg
:}

main = {:
    (ex=addExpression "\n" :: io0Note (format "%q" ex))*
:};
```

Example Translation to Samizdat-0
---------------------------------

Note: The `yieldFilter` definitions would need to be refactored in order
to make the translations "hygenic" (that is, avoid having them inadvertently
bind to parser-expansion-internal variables). Basically, this:

```
rule = { yield input ::
    yieldFilter = { ... filter ... }
    ... rest of rule ...
};

=>

rule = { yieldFilter ::
    <> { yield input :: ... rest of rule ... }
}
{ ... filter ... };
```

Assumption: `yield` is only called on a successful parse.

```
digit = { yield input ::
    yieldFilter = { ch ::
        <> isub (integerFromString ch) (integerFromString "0")
    };

    <> parseOr yieldFilter input
        { yield input :: <> parseChars yield input "0" }
        { yield input :: <> parseChars yield input "1" }
        { yield input :: <> parseChars yield input "2" }
        { yield input :: <> parseChars yield input "3" }
        { yield input :: <> parseChars yield input "4" }
        { yield input :: <> parseChars yield input "5" }
        { yield input :: <> parseChars yield input "6" }
        { yield input :: <> parseChars yield input "7" }
        { yield input :: <> parseChars yield input "8" }
        { yield input :: <> parseChars yield input "9" }
};

number = { yield input ::
    yieldFilter = { digits ::
        <> listReduce 0 digits
            { result digit :: <> iadd digit (imul result 10) }
    };

    <> parsePlus yieldFilter input digit
};

atom = { yield input ::
    yieldFilter = { value ::
        <> apply { ignored_1 ex ignored_2 :: <> ex } value
    };

    <> parseOr yield input
        number
        { yield input ::
            <> parseSeq yieldFilter input
                { yield input :: <> parseChars yield input "(" }
                addExpression
                { yield input :: <> parseChars yield input ")" }
        }
};

addExpression = { yield input ::
    yieldFilter = { value ::
        <> apply { ex1 op ex2 :: <> op ex1 ex2 } value
    };

    <> parseSeq yieldFilter input mulExpression addOp addExpression
};

addOp = { yield input ::
    yieldFilter1 = { value :: <> iadd };
    yieldFilter2 = { value :: <> isub };

    <> parseOr yield input
        { yield input :: <> parseChars yieldFilter1 input "+" }
        { yield input :: <> parseChars yieldFilter2 input "-" }
};

mulExpression = { yield input ::
    yieldFilter = { value ::
        <> apply { ex1 op ex2 :: <> op ex1 ex2 } value
    };

    <> parseSeq yieldFilter input unaryExpression mulOp mulExpression
};

addOp = { yield input ::
    yieldFilter1 = { value :: <> imul };
    yieldFilter2 = { value :: <> idiv };

    <> parseOr yield input
        { yield input :: <> parseChars yieldFilter1 input "*" }
        { yield input :: <> parseChars yieldFilter2 input "/" }
};

unaryExpression = { yield input ::
    yieldFilter = { value ::
        <> apply { op ex :: <> op ex } value
    };

    <> parseOr yield input
        { yield input ::
            <> parseSeq yieldFilter input unaryOp unaryExpression
        }
        atom
};

unaryOp = { yield input ::
    yieldFilter = { value :: <> ineg };

    <> parseChars yieldFilter input "-"
};

main = { yield input ::
    yieldFilter = { value ::
        <> apply { ex ignored :: io0Note (format "%q" ex) } value
    };

    <> parseStar yield input
        { yield input ::
            <> parseSeq yieldFilter input
                addExpression
                { yield input :: <> parseChars yield input "\n" }
        }
};
```
