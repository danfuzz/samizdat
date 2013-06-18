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
# Set-like map of all lowercase identifier characters. Used to figure
# out if we're looking at a keyword in the `identifierString` rule.
def LOWER_ALPHA = ["a".."z": true];

# Returns a `call` node.
def makeCall = { function, actuals* ::
    <> @[call: [function: function, actuals: actuals]]
};

# Returns a `varRef` node.
def makeVarRef = { name ::
    <> @[varRef: name]
};

# Returns a `call` node that names a function as a `varRef`.
def makeCallName = { name, actuals* ::
    <> @[call: [function: makeVarRef(name), actuals: actuals]]
};

# Returns a `literal` node.
def makeLiteral = { value ::
    <> @[literal: value]
};

# Returns a `closure` node representing a thunk of an expression.
def makeThunk = { expression ::
    <> @[closure: @[statements: [], yield: expression]];
};

# forward declaration: programBody
# forward declaration: expression

def optYieldDef = {/
    @"<"
    name = @identifier
    @">"
    { <> [yieldDef: tokenValue(name)] }
|
    { <> [:] }
/};

def formal = {/
    name = (
        n = @identifier
        { <> [name: tokenValue(n)] }
    |
        @"." { <> [:] }
    )

    repeat = (
        r = [@"?" @"*" @"+"]
        { <> [repeat: tokenType(r)] }
    |
        { <> [:] }
    )

    { <> [:, name*, repeat*] }
/};

def formalsList = {/
    first = formal
    rest = (@"," formal)*
    { <> [formals: [first, rest*]] }
|
    { <> [:] }
/};

def programDeclarations = {/
    yieldDef = optYieldDef
    formals = formalsList

    @"::"

    { <> [:, formals*, yieldDef*] }
/};

def program = {/
    decls = (programDeclarations | { <> [:] })
    body = programBody
    { <> @[closure: [:, decls*, body*]] }
/};

def closure = {/
    @"{"
    prog = program
    @"}"
    { <> prog }
/};

# Parses a closure which must not define any formal arguments. This is done
# by parsing an arbitrary closure and then verifying that it does not
# declare formals. This is preferable to not-including formal argument
# syntax, because (a) no rule wants to differentiate these cases (rules either
# want an arbitrary closure or a specifically-constrained kind); (b) it
# reduces redundancy in the syntax, and (c) the error case on the former
# would be more obscure (as in just something like "unexpected token" on
# the would-be formal argument).
def nullaryClosure = {/
    c = closure

    {
        ifTrue { <> mapHasKey(tokenValue(c), "formals") }
            { io0Die("Invalid formal argument in code block.") };
        <> c
    }
/};

# Parses a closure which must have neither formal arguments nor a yield
# definition. See `parseNullaryClosure` above for discussion.
def codeOnlyClosure = {/
    c = nullaryClosure

    {
        ifTrue { <> mapHasKey(tokenValue(c), "yieldDef") }
            { io0Die("Invalid yield definition in code block.") };
        <> c
    }
/};

def int = {/
    i = @int
    { <> makeLiteral(tokenValue(i)) }
/};

def string = {/
    s = @string
    { <> makeLiteral(tokenValue(s)) }
/};

def identifierString = {/
    s = [@identifier @string]
    { <> makeLiteral(tokenValue(s)) }
|
    token = .
    {
        <> ifFalse { <> tokenHasValue(token) }
            {
                def type = tokenType(token);
                def firstCh = stringNth(type, 0);
                <> ifTrue { <> mapHasKey(LOWER_ALPHA, firstCh) }
                    { <> makeLiteral(type) }
            }
    }
/};

def listElement = {/
    ex = expression

    (
        @"*"
        { <> @[interpolate: ex] }
    |
        @".."
        end = expression
        { <> @[interpolate: makeCallName("makeRange", ex, end)] }
    |
        { <> ex }
    )
/};

def unadornedList = {/
    first = listElement
    rest = (@"," listElement)*
    { <> [first, rest*] }
|
    { <> [] }
/};

def list = {/
    @"["
    expressions = unadornedList
    @"]"
    {
        <> ifTrue { <> eq(expressions, []) }
            { <> makeLiteral([]) }
            { <> makeCallName("makeList", expressions*) }
    }
/};

def emptyMap = {/
    @"[" @":" @"]"
    { <> makeLiteral([:]) }
/};

def mapping = {/
    key = (
        k = identifierString
        @":"
        { <> k }
    |
        k = listElement
        @":"
        { <> k }
    )

    value = expression
    { <> makeCallName("makeList", value, key) }
|
    map = expression
    @"*"
    { <> map }
/};

def map = {/
    @"["
    (@":" @",")?
    first = mapping
    rest = (@"," mapping)*
    @"]"
    { <> makeCallName("makeMap", first, rest*) }
/};

def token = {/
    @"@"

    tokenArgs = (
        @"["
        type = identifierString
        value = (@":" expression)?
        @"]"
        { <> [type, value*] }
    |
        @"["
        type = expression
        value = (@":" expression)?
        @"]"
        { <> [type, value*] }
    |
        type = identifierString
        { <> [type] }
    )

    { <> makeCallName("makeToken", tokenArgs*) }
/};

def uniqlet = {/
    @"@@"
    { <> makeCallName("makeUniqlet") }
/};

def varRef = {/
    name = @identifier
    { <> makeVarRef(tokenValue(name)) }
/};

def varDef = {/
    @"def"
    name = @identifier
    @"="
    ex = expression
    { <> @[varDef: [name: tokenValue(name), value: ex]] }
/};

def parenExpression = {/
    @"("
    ex = expression
    @")"
    { <> ex }
/};

def atom = {/
    varRef | int | string |
    list | emptyMap | map |
    uniqlet | token | closure | parenExpression
/};

def actualsList = {/
    @"()"
    closure*
|
    @"("
    normalActuals = unadornedList
    @")"
    closureActuals = closure*
    { <> [normalActuals*, closureActuals*] }
|
    closure+
/};

def callExpression = {/
    base = atom
    actualsLists = actualsList*

    {
        <> listReduce(base, actualsLists) { result, ., list ::
            <> makeCall(result, list*)
        }
    }
/};

def expression = {/
    callExpression
/};

def statement = {/
    varDef | expression
/};

def nonlocalExit = {/
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

def yield = {/
    @"<>"
    (
        ex = expression
        { <> [yield: ex] }
    |
        { <> [:] }
    )
/};

def programBody = {/
    @";"*

    most = (
        s = statement
        @";"+
        { <> s }
    )*

    last = (
        s = (statement | nonlocalExit)
        { <> [statements: [s]] }
    |
        y = yield
        { <> [statements: [], y*] }
    |
        { <> [statements: []] }
    )

    @";"*

    {
        def allStatements = listAdd(most, mapGet(last, "statements"));
        <> [last*, statements: allStatements]
    }
/};

def programOrError = {/
    prog = program
    (
        pending = .+
        { ... io0Die ... pending ... }
    )?
    { <> prog }
/};
```
