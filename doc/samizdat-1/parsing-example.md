Samizdat Layer 1
================

Parsing Example
---------------

The classic "four function calculator" as an example. In this case, it is
done as a unified tokenizer / tree parser. For simplicity, we don't bother
with whitespace-related rules.

Note: This example doesn't use all of the syntactic forms mentioned above.

```
digit = {/
    ch=["0123456789"]
    {
        <> isub (intFromString ch) (intFromString "0")
    }
/};

number = {/
    digits=digit+
    {
        <> listReduce 0 digits
            { result digit :: <> iadd digit (imul result 10) }
    }
/};

atom = {/
    number
|
    "(" ex=addExpression ")"
    { <> ex }
/};

addExpression = {/
    ex1=mulExpression op=addOp ex2=addExpression
    { <> op ex1 ex2 }
/};

addOp = {/
    "+" { <> iadd }
|
    "-" { <> isub }
/};

mulExpression = {/
    ex1=unaryExpression op=mulOp ex2=mulExpression
    { <> op ex1 ex2 }
/};

mulOp = {/
    "*" { <> imul }
|
    "/" { <> idiv }
/};

unaryExpression = {/
    op=unaryOp
    ex=unaryExpression
    { <> op ex }
|
    atom
/};

unaryOp = {/
    "-" { <> ineg }
/};

main = {/
    (
        ex = addExpression
        "\n"
        {
            io0Note (format "%q" ex)
            # Explicit yield here to indicate successful parsing.
            <> null
        }
    )*
/};
```


Example Translation to Samizdat Layer 0
---------------------------------------

```
digit = pegMakeMainSequence
    (pegMakeCharSet "0123456789")
    (pegMakeCode { ch ::
        <> isub (intFromString ch) (intFromString "0")
    });

number = pegMakeMainSequence
    (pegMakePlus digit)
    (pegMakeCode { digits ::
        <> listReduce 0 digits
            { result digit :: <> iadd digit (imul result 10) }
    });

atom = pegMakeChoice
    number
    (pegMakeMainSequence
        (pegMakeToken "(")
        addExpression
        (pegMakeToken ")")
        (pegMakeCode { . ex . :: <> ex }));

addExpression = pegMakeMainSequence
    mulExpression
    addOp
    addExpression
    (pegMakeCode { ex1 op ex2 :: <> op ex1 ex2 });

addOp = pegMakeChoice
    (pegMakeSequence
        (pegMakeToken "+")
        (pegMakeCode { <> iadd }))
    (pegMakeSequence
        (pegMakeToken "-")
        (pegMakeCode { <> isub }));

mulExpression = pegMakeMainSequence
    unaryExpression
    mulOp
    mulExpression
    (pegMakeCode { ex1 op ex2 :: <> op ex1 ex2 });

mulOp = pegMakeChoice
    (pegMakeSequence
        (pegMakeToken "*")
        (pegMakeCode { <> imul }))
    (pegMakeSequence
        (pegMakeToken "/")
        (pegMakeCode { <> idiv }));

unaryExpression = pegMakeMainSequence
    unaryOp
    unaryExpression
    (pegMakeCode { op ex :: <> op ex });

unaryOp = pegMakeMainSequence
    (pegMakeToken "-")
    (pegMakeCode { <> ineg });

main = pegMakeStar
    (pegMakeMainSequence
        addExpression
        (pegMakeToken "\n")
        (pegMakeCode { ex . ::
            io0Note (format "%q" ex);
            <> null
        }));
```
