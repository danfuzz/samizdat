Samizdat Layer 0
================

Tree Syntax
-----------

The following is a BNF/PEG-like description of the node / tree syntax.
This definition uses syntax which is identical to the parser syntax
implemented at a higher layer.

A program is parsed by matching the `program` rule, which yields a
`function` node.

```
# Function that returns an appropriately-formed `call` node.
makeCall = { function actuals* ::
    <> @["call" ["function"=function "actuals"=actuals]]
};

# forward-declaration: atom
# forward-declaration: expression
# forward-declaration: function

int = {/
    @int
    { <> @["literal" (highletValue int)] }
/};

string = {/
    @string
    { <> @["literal" (highletValue string)] }
/};

emptyList = {/
    @"[" @"]"
    { <> @["literal" []] }
/};

list = {/
    @"["
    atoms = atom+
    @"]"
    { <> apply makeCall @["varRef" "makeList"] atoms }
/};

emptyMap = {/
    @"[" @"=" @"]"
    { <> @["literal" [=]] }
/};

binding = {/
    key = atom
    @"="
    value = atom
    { <> [key value] }
/};

map = {/
    @"["
    bindings = binding+
    @"]"
    { <> apply makeCall @["varRef" "makeMap"] (apply listAdd bindings) }
/};

highlet = {/
    @"@" @"[" type=atom value=atom? @"]"
    { <> apply makeCall @["varRef" "makeHighlet"] type value }
|
    @"@" type=(@string | @identifier)
    { <> makeCall @["varRef" "makeHighlet"] @["literal" (highletValue type)] }
/};

uniqlet = {/
    @"@@"
    { <> makeCall @["varRef" "makeUniqlet"] }
/};

varRef = {/
    name = @identifier
    { <> @["varRef" (highletValue name)] }
/};

varDef ::= {/
    name = @identifier
    @"="
    ex = expression
    { <> @["varDef" ["name"=(highletValue name) "value"=ex]] }
/};

parenExpression ::= {/
    @"("
    ex = expression
    @")"
    { <> ex }
/};

atom = {/
    varRef | int | string |
    emptyList | list | emptyMap | map |
    uniqlet | highlet | function | parenExpression
/};

callExpression = {/
    function = atom
    args = atom+
    { <> apply makeCall function args }
/};

unaryCallExpression = {/
    function = atom
    calls = @"()"+
    # Note: One `makeCall` per pair of parens.
    { <> (... (makeCall (makeCall atom))) }
/};

unaryExpression = {/
    unaryCallExpression | atom
/};

expression = {/
    callExpression | unaryExpression
/};

statement = {/
    varDef | expression
/};

nonlocalExit = {/
    @"<"
    name = varRef
    @">"
    ex = expression?
    { <> apply makeCall name ex }
/};

yield = {/
    @"<>"
    expression
/};

yieldDef = {/
    @"<"
    name = @identifier
    @">"
    { <> highletValue identifier }
/};

formal = {/
    name = (
        n=@identifier { <> ["name" = n] }
    |
        @"." { <> [=] }
    )

    repeat = (
        r=[@"?" @"*" @"+"] { <> ["repeat" = (highletType r)] }
    |
        { <> [=] }
    )
    { <> mapAdd name repeat }
/};

programBody = {/
    @";"*

    most = (
        s = statement
        @";"+
        { <> s }
    )*

    last = (
        s = (statement | nonlocalExit)
        { <> ["statements" = [s]] }
    |
        y = yield
        { <> ["statements" = [] "yield" = y] }
    |
        { <> ["statements" = []] }
    )

    @";"*

    {
        allStatements = listAdd most (mapGet last "statements");
        <> mapPut last "statements" allStatements
    }
/};

programDeclarations = {/
    formals = (
        fs = formal+
        { <> ["formals" = fs] }
    |
        { <> [=] }
    )

    yieldDef = (
        y = yieldDef
        { <> ["yieldDef" = y] }
    |
        { <> [=] }
    )

    @"::"

    { <> mapAdd formals yieldDef }
/};

program = {/
    decls = (programDeclarations | { <> [=] })
    body = programBody
    { <> mapAdd decls body }
/};

function = {/
    @"{"
    prog = program
    @"}"
    { <> prog }
/};
```
