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

def atom = {/
    # ... original alternates from the base grammar ...
|
    # The lookahead is just to make it clear that Samizdat Layer 1 can
    # only be "activated" with that one specific token.
    &@"{/" parser
/};

def parser = {/
    @"{/"
    pex = choicePex
    @"/}"
    { <> @[parser: pex] }
/};

def parenPex = {/
    @"("
    pex = choicePex
    @")"
    { <> pex }
/};

def parserString = {/
    s = @string
    {
        def value = tokenValue(s);
        <> ifTrue { <> eq(lowSize(value), 1) }
            { <> @[token: value] }
            { <> s }
    }
/};

def parserToken = {/
    @"@"
    type = identifierString
    { <> @[token: tokenValue(type)] }
/};

# Handles regular string literals and character ranges.
def parserSetString = {/
    s = @string
    (
        @".."
        end = @string
        {
            def startChar = tokenValue(s);
            def endChar = tokenValue(end);
            <> ifTrue
                { <> and
                    { <> eq(lowSize(startChar), 1) }
                    { <> eq(lowSize(endChar), 1) } }
                { <> @[string: stringAdd([startChar..endChar]*)] }
        }
    |
        { <> s }
    )
/};

def parserSet = {/
    @"["

    type = (
        @"!" { <> "[!]" }
    |
        { <> "[]" }
    )

    terminals = (
        strings = parserSetString+
        {
            def oneString = listReduce("", strings)
                { result, ., s :: <> stringAdd(result, tokenValue(s)) };
            <> stringReduce([], oneString)
                { result, ., ch :: <> [result*, ch] }
        }
    |
        tokens = parserToken+
        {
            def tokens = [first, rest*];
            <> listMap(tokens) { ., t :: <> tokenValue(t) }
        }
    |
        { <> [] }
    )

    @"]"

    { <> @[(type): terminals] }
/};

def parserCode = {/
    closure = parseNullaryClosure
    { <> @["{}": tokenValue(closure)] }
/};

def parserPredicate = {/
    @"&&"
    predicate = parenExpression
    { <> @["&&": predicate] }
/};

def parserAtom = {/
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
    parserPredicate
|
    @"."
|
    @"()"
|
    parenPex
/};

def repeatPex = {/
    atom = parserAtom
    (
        repeat = [@"?" @"*" @"+"]
        { <> @[tokenType(repeat): atom] }
    |
        { <> atom }
    )
/};

def lookaheadPex = {/
    (
        lookahead = [@"&" @"!"]
        pex = repeatPex
        { <> @[tokenType(lookahead): pex] }
    )
|
    repeatPex
/};

def namePex = {/
    (
        name = @identifier
        @"="
        pex = lookaheadPex
        { <> @[varDef: [name: tokenValue(name), value: pex]] }
    )
|
    lookaheadPex
/};

def sequencePex = {/
    items = namePex+
    { <> @[sequence: items] }
/};

def choicePex = {/
    first = sequencePex
    rest = (@"|" sequencePex)*
    { <> @[choice: [first, rest*]] }
/};
```
