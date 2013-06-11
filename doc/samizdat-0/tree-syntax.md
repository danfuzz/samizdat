Samizdat Layer 0
================

Tree Syntax
-----------

The following is a BNF/PEG-like description of the node / tree syntax.
This definition uses syntax which is identical to the parser syntax
implemented at a higher layer.

A program is parsed by matching the `program` rule, which yields a
`closure` node. For simple error handling, the rule `programOrError`
can be used.

```
# Returns a `call` node.
makeCall = { function, actuals* ::
    <> @["call" = ["function"=function, "actuals"=actuals]]
};

# Returns a `varRef` node.
makeVarRef = { name ::
    <> @["varRef" = name]
};

# Returns a `call` node that names a function as a `varRef`.
makeCallName = { name, actuals* ::
    <> @["call" = ["function"=(makeVarRef(name)), "actuals"=actuals]]
};

# Returns a `literal` node.
makeLiteral = { value ::
    <> @["literal" = value]
};

# Returns a `closure` node representing a thunk of an expression.
makeThunk = { expression ::
    <> @["closure" = @["statements"=[], "yield"=expression]];
};

# forward declaration: closure
# forward declaration: expression

int = {/
    i = @int
    { <> makeLiteral(tokenValue(i)) }
/};

string = {/
    s = @string
    { <> makeLiteral(tokenValue(s)) }
/};

listElement = {/
    ex = expression

    (
        @"*"
        { <> @["interpolate" = ex] }
    |
        @".."
        end = expression
        { <> @["interpolate" = makeCallName("makeRange", ex, end)] }
    |
        { <> ex }
    )
/};

unadornedList = {/
    first = listElement
    rest = (@"," listElement)*
    { <> listPrepend(first, rest) }
|
    { <> [] }
/};

list = {/
    @"["
    expressions = unadornedList
    @"]"
    {
        <> ifTrue { <> eq(expressions, []) }
            { <> makeLiteral([]) }
            { <> makeCallName("makeList", expressions*) }
    }
/};

emptyMap = {/
    @"[" @"=" @"]"
    { <> makeLiteral([=]) }
/};

mapping = {/
    key = expression
    @"="
    value = expression
    { <> [key, value] }
/};

map = {/
    @"["
    first = mapping
    rest = (@"," mapping)*
    @"]"
    {
        mappings = listAdd(first, rest*);
        <> makeCallName("makeMap", mappings*)
    }
/};

token = {/
    @"@"
    (
        @"["
        type = expression
        value = (@"=" expression)?
        @"]"
        { <> makeCallName("makeToken", type, value*) }
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
    { <> @["varDef" = ["name"=(tokenValue(name)), "value"=ex]] }
/};

parenExpression = {/
    @"("
    ex = expression
    @")"
    { <> ex }
/};

atom = {/
    varRef | int | string |
    list | emptyMap | map |
    uniqlet | token | closure | parenExpression
/};

actualsList = {/
    @"()"
    closure*
|
    @"("
    normalActuals = unadornedList
    @")"
    closureActuals = closure*
    { <> listAdd(normalActuals, closureActuals) }
|
    closure+
/};

callExpression = {/
    base = atom
    actualsLists = actualsList*

    {
        <> listReduce(base, actualsLists) { result . list ::
            <> makeCall(result, list*)
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

    (
        ex = expression
        { <> makeCall(makeVarRef("nonlocalExit"), name, makeThunk(ex)) }
    |
        { <> makeCall(makeVarRef("nonlocalExit"), name) }
    )
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

optYieldDef = {/
    @"<"
    name = @identifier
    @">"
    { <> ["yieldDef" = tokenValue(name)] }
|
    { <> [=] }
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

formalsList = {/
    first = formal
    rest = (@"," formal)*
    { <> ["formals" = listPrepend(first, rest)] }
|
    { <> [=] }
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
    yieldDef = optYieldDef
    formals = formalsList

    @"::"

    { <> mapAdd(formals, yieldDef) }
/};

program = {/
    decls = (programDeclarations | { <> [=] })
    body = programBody
    { <> @["closure" = (mapAdd(decls, body))] }
/};

closure = {/
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
