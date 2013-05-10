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
parser digit {
    ch=("0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9")
    :: <> isub (intFromString ch) (intFromString "0")
};

parser number {
    digits=digit+
    ::
    <> listReduce 0 digits { result digit :: <> iadd digit (imul result 10) }
};

parser atom {
    number
    |
    "(" ex=addExpression ")" :: <> ex
};

parser addExpression {
    ex1=mulExpression op=addOp ex2=addExpression
    :: <> op ex1 ex2
};

parser addOp {
    "+" :: <> iadd
    |
    "-" :: <> isub
};

parser mulExpression {
    ex1=unaryExpression op=mulOp ex2=mulExpression
    :: <> op ex1 ex2
};

parser mulOp {
    "*" :: <> imul
    |
    "/" :: <> idiv
};

parser unaryExpression {
    op=unaryOp ex=unaryExpression :: <> unaryOp ex
    |
    atom
};

parser main {
    (ex=addExpression "\n" :: io0Note (format "%q" ex))*
};
```

Example Translation to Samizdat-0
---------------------------------

```
digit = { yield input <out> ::
    parser_ch = multiResult [@rest @yield] parseOr input
        { yield input :: <> parseChars yield input "0" }
        { yield input :: <> parseChars yield input "1" }
        { yield input :: <> parseChars yield input "2" }
        { yield input :: <> parseChars yield input "3" }
        { yield input :: <> parseChars yield input "4" }
        { yield input :: <> parseChars yield input "5" }
        { yield input :: <> parseChars yield input "6" }
        { yield input :: <> parseChars yield input "7" }
        { yield input :: <> parseChars yield input "8" }
        { yield input :: <> parseChars yield input "9" };
    <> ifValue { <> mapGet parser_ch @yield }
        { ch ::
            yield (isub (intFromString ch) (intFromString "0"));
            <out> mapGet parser_ch @rest
        };

    # Indicate failure to parse.
    yield();
    <out>
};

number = { yield input <out> ::
    parser_digits = multiResult [@rest @yield] parsePlus digit;
    ifValue { <> mapGet parser_digits @yield }
        { digits ::
            yield
                (listReduce 0 digits
                    { result digit :: <> iadd digit (imul result 10) });
            <out> mapGet parser_digits @rest
        };

    # Indicate failure to parse.
    yield();
    <out>
};

atom = { yield input <out> ::
    parser_1 = multiResult [@rest @yield] parseOr input
        number
        { yield input <out> ::
            parser_2 = multiResult [@result @yield] parseSeq
                { yield input :: <> parseChars yield input "(" }
                addExpression
                { yield input :: <> parseChars yield input ")" };
            ifValue { <> mapGet parser_2 @yield }
                { value ::
                    apply { ignored_1 ex ignored_2 :: yield ex } value;
                    <out> mapGet parser_2 @rest
                };
        };
    ifValue { <> mapGet parser_1 @yield }
        { value ::
            yield value;
            <out> mapGet parser_1 @rest
        };

    # Indicate failure to parse.
    yield();
    <out>
};

addExpression = { yield input <out> ::
    parser_1 = multiResult [@result @yield] parseSeq
        mulExpression addOp addExpression;
    ifValue { <> mapGet parser_1 @yield }
        { value ::
            apply { ex1 op ex2 :: yield (op ex1 ex2) } value;
            <out> mapGet parser_1 @rest
        };

    # Indicate failure to parse.
    yield();
    <out>
};

addOp = { yield input <out> ::
    parser_1 = multiResult [@rest @yield] parseOr input
        { yield input <out> ::
            parser_2  = multiResult [@result @yield] parseChars "+";
            ifValue { <> mapGet parser_2 @yield }
                {
                    yield iadd;
                    <out> mapGet parser_ch @rest
                }
        }
        { yield input <out> ::
            parser_2  = multiResult [@result @yield] parseChars "+";
            ifValue { <> mapGet parser_2 @yield }
                {
                    yield isub;
                    <out> mapGet parser_ch @rest
                }
        };
    ifValue { <> mapGet parser_1 @yield }
        { value ::
            yield value;
            <out> mapGet parser_1 @rest
        };

    # Indicate failure to parse.
    yield();
    <out>
};

mulExpression = { yield input <out> ::
    parser_1 = multiResult [@result @yield] parseSeq
        unaryExpression mulOp mulExpression;
    ifValue { <> mapGet parser_1 @yield }
        { value ::
            apply { ex1 op ex2 :: yield (op ex1 ex2) } value;
            <out> mapGet parser_1 @rest
        };

    # Indicate failure to parse.
    yield();
    <out>
};

mulOp = { yield input <out> ::
    parser_1 = multiResult [@rest @yield] parseOr input
        { yield input <out> ::
            parser_2  = multiResult [@result @yield] parseChars "*";
            ifValue { <> mapGet parser_2 @yield }
                {
                    yield imul;
                    <out> mapGet parser_ch @rest
                }
        }
        { yield input <out> ::
            parser_2  = multiResult [@result @yield] parseChars "/";
            ifValue { <> mapGet parser_2 @yield }
                {
                    yield idiv;
                    <out> mapGet parser_ch @rest
                }
        };
    ifValue { <> mapGet parser_1 @yield }
        { value ::
            yield value;
            <out> mapGet parser_1 @rest
        };

    # Indicate failure to parse.
    yield();
    <out>
};

unaryExpression = { yield input <out> ::
    parser_1 = multiResult [@rest @yield] parseOr input
        { yield input <out> ::
            parser_2 = multiResult [@result @yield] parseSeq
                unaryOp
                unaryExpression;
            ifValue { <> mapGet parser_2 @yield }
                { value ::
                    apply { op ex :: yield (op ex) } value;
                    <out> mapGet parser_2 @rest
                };
        }
        atom;
    ifValue { <> mapGet parser_1 @yield }
        { value ::
            yield value;
            <out> mapGet parser_1 @rest
        };

    # Indicate failure to parse.
    yield();
    <out>
};

main = { yield input <out> ::
    parser_1 = multiResult [@rest @yield] parseStar
        { yield input <out> ::
            parser_2 = multiResult [@result @yield] parseSeq
                addExpression
                { yield input :: <> parseChars yield input "\n" };
            ifValue { <> mapGet parser_2 @yield }
                { value ::
                    apply { ex ignored :: io0Note (format "%q" ex) } value;
                    yield();
                    <out> mapGet parser_2 @rest
                };

            # Indicate failure to parse.
            yield();
            <out>
        };

    # Star rules always succeed.
    yield (mapGet parser_1 @yield);
    <out> mapGet parser_2 @rest
};
```
