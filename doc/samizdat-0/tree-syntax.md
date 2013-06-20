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
fn makeCall(function, actuals*) {
    <> @[call: [function: function, actuals: actuals]]
};

# Returns a `varDef` node.
fn makeVarDef(name, value) {
    <> @[varDef: [name: name, value: value]
};

# Returns a `varRef` node.
fn makeVarRef(name) {
    <> @[varRef: name]
};

# Returns a `call` node that names a function as a `varRef`.
fn makeCallName(name, actuals*) {
    <> @[call: [function: makeVarRef(name), actuals: actuals]]
};

# Returns a `literal` node.
fn makeLiteral(value) {
    <> @[literal: value]
};

# Returns a `closure` node representing a thunk of an expression.
fn makeThunk(expression) {
    <> @[closure: @[statements: [], yield: expression]];
};

# Returns a `call` node to a nonlocal exit with the given name and
# with optional expression value. The expression if supplied is automatically
# "thunked".
fn makeCallNonlocalExit(name, expression?) {
    <> ifValue { <> listFirst(expression) }
        { ex :: <> makeCall(makeVarRef("nonlocalExit"), name, makeThunk(ex)) }
        { <> makeCall(makeVarRef("nonlocalExit"), name) }
};

# forward declaration: programBody
# forward declaration: expression

def yieldDef = {/
    @"<"
    name = @identifier
    @">"
    { <> tokenValue(name) }
/};

def optYieldDef = {/
    y = yieldDef
    { <> [yieldDef: y] }
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

# Common parsing for `fn` statements and expressions. The syntax for
# both is identical, except that the statement form requires that the
# function be named. The result of this rule is a map identical in form to
# what's required for a closure payload, except that `name` may also
# be bound.
#
# The result of this rule is suitable for use as a `closure` node
# payload. And as long as `name` is bound, the result is valid to use
# as the payload for a `fnDef` node.
#
# The translation is along these lines:
#
# ```
# fn <out> name(arg1, arg2) { stat1; stat2 }
# ```
#
# ```
# { <\"return"> arg1, arg2 ::
#     def out = \"return";
#     stat1;
#     stat2
# }
# ```
#
# with:
#
# * no yield def binding statement if an explicit yield def was not present.
#
# * the key `name` bound to the function name, if a name was defined. (This
#   is not representable in lower-layer surface syntax.)
def fnCommon = {/
    @fn

    # This is a variable definition statement which binds the yield def
    # name to the `return` function, if there is in fact a yield def present.
    returnDef = (
        y = yieldDef
        { <> makeVarDef(y, makeVarRef("return")) }
    )?

    name = (
        n = @identifier
        { <> [name: tokenValue(n)] }
    |
        { <> [:] }
    )

    formals = (
        @"()"
        { <> [:] }
    |
        @"("
        f = formalsList
        @")"
        { <> f }
    )

    code = codeOnlyClosure

    {
        def codeMap = tokenValue(code);
        def statements = [returnDef*, mapGet(codeMap, "statements")*];
        <> [
            codeMap*, name*, formals*,
            yieldDef: "return",
            statements: statements
        ]
    }
/};

# Parses a `fn` definition statement. The syntax here is the same as
# what's recognized by `parseFnCommon`, except that the name is required.
# We don't error out (terminate the runtime) on a missing name, though, as
# that just means that we're looking at a legit `fn` expression, which will
# get successfully parsed by the `expression` alternative of `statement`.
def fnDef = {/
    funcMap = fnCommon

    {
        <> ifTrue { <> mapHasKey(funcMap, "name") }
            { <> @[fnDef: funcMap] }
    }
/};

# Parses a `fn` (function with `return` binding) expression. The translation
# is as described in `parseFnCommon` (above) if the function is not given a
# name. If the function *is* given a name, the translation is along the
# following lines (so as to enable self-recursion):
#
# ```
# fn <out> name ...
# ```
#
# =>
#
# ```
# {
#     fn <out> name ...;
#     <> name
# }()
# ```
def fnExpression = {/
    funcMap = fnCommon
    closure = { <> @[closure: funcMap] }

    (
        name = { <> mapGet(funcMap, "name") }
        {
            def mainClosure = @[closure: [
                statements: [@[fnDef: funcMap]],
                yield: makeVarRef(name)
            ]];

            <> makeCall(mainClosure)
        }
    |
        { <> closure }
    )
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

def unadornedList = {/
    first = expression
    rest = (@"," expression)*
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
        k = expression
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
    { <> makeVarDef(tokenValue(name), ex) }
/};

def parenExpression = {/
    @"("
    ex = expression
    @")"
    { <> ex }
/};

def atom = {/
    varRef | int | string | list | emptyMap | map | uniqlet | token |
    closure | parenExpression
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

def postfixOperator = {/
    actuals = actualsList
    { <> { node :: <> makeCall(node, actuals*) } }
|
    @"*"
    { <> { node :: <> @[interpolate: node] } }
/};

def unaryExpression = {/
    base = atom
    ops = postfixOperator*

    { <> listReduce(base, ops) { result, ., op :: <> op(result) } }
/};

def rangeExpression {/
    base = unaryExpression
    ops = (
        @".."
        ex = unaryExpression
        {
            <> { node ::
                <> @[interpolate: makeCallName("makeRange", node, ex)]
            }
        }
    )*

    { <> listReduce(base, ops) { result, ., op :: <> op(result) } }
/};

def expression = {/
    rangeExpression | fnExpression
/};

def statement = {/
    varDef | fnDef | expression
/};

def nonlocalExit = {/
    name = (
        @"<"
        n = varRef
        @">"
        { <> n }
    |
        @return
        { <> makeVarRef("return") }
    )

    value = expression?
    { <> makeCallNonlocalExit(name, value*) }
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
