Samizdat Layer 1
================

Tree Syntax
-----------

The following is an in-language description of the tree grammar, as
modifications to the *Samizdat Layer 0* tree syntax.

**Note:** The grammar uses the label "pex" to denote various
"Parser EXpression" types.

```
# forward declaration: parser
# forward declaration: choicePex

atom = {/
    # ... original alternates from the base grammar ...
|
    # The lookahead is just to make it clear that Samizdat Layer 1 can
    # only be "activated" with that one specific token.
    &"{/" parser
/};

parser = {/
    @"{/"
    pex = choicePex
    @"/}"
    { <> @["parser" pex] }
/};

parenPex = {/
    @"("
    pex = choicePex
    @")"
    { <> pex }
/};

parserString = {/
    s = @string
    {
        value = tokenValue s;
        <> ifTrue { <> eq (lowSize value) 1 }
            { <> @["token" value] }
            { <> s }
    }
/};

parserToken = {/
    @"@"
    type = [@identifier @string]
    { <> @["token" (tokenValue type)] }
/};

parserSet = {/
    @"["

    type = (
        @"!" { <> "[!]" }
    |
        { <> "[]" }
    )

    terminals = (
        strings = @string*
        {
            oneString = listReduce "" strings
                { result . s :: <> stringAdd result (tokenValue s) };
            <> stringReduce [] oneString
                { result . ch :: <> listAppend result ch }
        }
    |
        tokens = parserToken*
        { <> listMap tokens { . t :: <> tokenValue t } }
    )

    @"]"

    { <> @[type terminals] }
/};

parserCode = {/
    @"{"

    yieldDef = (
        y = yieldDef
        @"::"
        { <> ["yieldDef" = y] }
    |
        @"::"?
        { <> [=] }
    )

    body = programBody
    @"}"

    { <> @["function" (mapAdd yieldDef body)] }
/};

parserAtom = {/
    varRef
|
    parserString
|
    parserToken
|
    parserSet
|
    parserCode
|
    @"."
|
    @"()"
|
    parenPex
/};

repeatPex = {/
    atom = parserAtom
    (
        repeat = [@"?" @"*" @"+"]
        { <> @[(tokenType repeat) atom] }
    |
        { <> atom }
    )
/};

lookaheadPex = {/
    (
        lookahead = [@"&" @"!"]
        pex = repeatPex
        { <> @[(tokenType lookahead) pex] }
    )
|
    repeatPex
/};

namePex = {/
    (
        name = @identifier
        @"="
        pex = lookaheadPex
        { <> @["varDef" ["name"=(tokenValue name) "value"=pex]] }
    )
|
    lookaheadPex
/};

sequencePex = {/
    items = namePex+
    { <> @["sequence" items] }
/};

choicePex = {/
    first = sequencePex
    rest = (@"|" sequencePex)*
    { <> @["choice" (listPrepend first rest)] }
/};
```
