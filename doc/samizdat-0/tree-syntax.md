Samizdat Layer 0
================

Tree Syntax
-----------

The following is a BNF/PEG-like description of the node / tree syntax.
This definition uses syntax which is identical to the parser syntax
implemented at a higher layer.

A program is parsed by matching the `program` rule, which yields a
`function` node. For simple error handling, the rule `programOrError`
can be used.

```
# Returns a `call` node.
makeCall = { function actuals* ::
    <> @["call" ["function"=function "actuals"=actuals]]
};

# Returns a `varRef` node.
makeVarRef = { name ::
    <> @["varRef" name]
};

# Returns a `call` node that names a function as a `varRef`.
makeCallName = { name actuals* ::
    <> @["call" ["function"=(makeVarRef(name)) "actuals"=actuals]]
};

# Returns a `literal` node.
makeLiteral = { value ::
    <> @["literal" value]
};

# forward declaration: atom
# forward declaration: expression
# forward declaration: function

int = {/
    i = @int
    { <> makeLiteral(tokenValue(i)) }
/};

string = {/
    s = @string
    { <> makeLiteral(tokenValue(s)) }
/};

emptyList = {/
    @"[" @"]"
    { <> makeLiteral([]) }
/};

unadornedList = {/
    first = atom
    rest = (@"," atom)*
    { <> apply(listPrepend, first, rest) }
|
    { <> [] }
/};

list = {/
    @"["
    atoms = atom+
    @"]"
    { <> apply(makeCallName, "makeList", atoms) }
/};

emptyMap = {/
    @"[" @"=" @"]"
    { <> makeLiteral([=]) }
/};

mapping = {/
    key = atom
    @"="
    value = atom
    { <> [key value] }
/};

map = {/
    @"["
    mappings = mapping+
    @"]"
    { <> apply(makeCallName, "makeMap", apply(listAdd, mappings)) }
/};

token = {/
    @"@"
    (
        @"[" type=atom value=atom? @"]"
        { <> apply(makeCallName, "makeToken", type, value) }
    |
        type = [@string @identifier]
        { <> makeCallName("makeToken", makeLiteral(tokenValue(type))) }
    )
/};

uniqlet = {/
    @"@@"
    { <> makeCallName("makeUniqlet") }
/};

varRef = {/
    name = @identifier
    { <> makeVarRef(tokenValue(name)) }
/};

varDef = {/
    name = @identifier
    @"="
    ex = expression
    { <> @["varDef" ["name"=(tokenValue(name)) "value"=ex]] }
/};

parenExpression = {/
    @"("
    ex = expression
    @")"
    { <> ex }
/};

atom = {/
    varRef | int | string |
    emptyList | list | emptyMap | map |
    uniqlet | token | function | parenExpression
/};

actualsList = {/
    @"()"
    function*
|
    @"("
    normalActuals = unadornedList
    functionActuals = function*
    @")"
    { <> listAdd(normalActuals, functionActuals) }
|
    function+
/};

callExpression = {/
    base = atom
    actualsLists = actualsList*

    {
        <> listReduce(base, actualsLists) { result . list ::
            <> apply(makeCall, result, list)
        }
    }
/};

expression = {/
    callExpression
/};

statement = {/
    varDef | expression
/};

nonlocalExit = {/
    @"<"
    name = varRef
    @">"
    ex = expression?
    { <> apply(makeCall, name, ex) }
/};

yield = {/
    @"<>"
    (
        ex = expression
        { <> ["yield" = ex] }
    |
        { <> [=] }
    )
/};

yieldDef = {/
    @"<"
    name = @identifier
    @">"
    { <> tokenValue(name) }
/};

formal = {/
    name = (
        n = @identifier
        { <> ["name" = (tokenValue(n))] }
    |
        @"." { <> [=] }
    )

    repeat = (
        r = [@"?" @"*" @"+"]
        { <> ["repeat" = (tokenType(r))] }
    |
        { <> [=] }
    )

    { <> mapAdd(name, repeat) }
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
        { <> mapAdd(["statements" = []], y) }
    |
        { <> ["statements" = []] }
    )

    @";"*

    {
        allStatements = listAdd(most, mapGet(last, "statements"));
        <> mapPut(last, "statements", allStatements)
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

    { <> mapAdd(formals, yieldDef) }
/};

program = {/
    decls = (programDeclarations | { <> [=] })
    body = programBody
    { <> @["function" (mapAdd(decls, body))] }
/};

function = {/
    @"{"
    prog = program
    @"}"
    { <> prog }
/};

programOrError = {/
    prog = program
    (
        pending = .+
        { ... io0Die ... pending ... }
    )?
    { <> prog }
/};
```
