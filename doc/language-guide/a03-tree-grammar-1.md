Samizdat Language Guide
=======================

Appendix: *Samizdat Layer 1* Tree Grammar
-----------------------------------------

The following is a nearly complete tree grammar for *Samizdat Layer 1*,
written in *Samizdat Layer 1*, with commentary calling out the parts
that are needed specifically for *Layer 1*. Anything left unmarked is
needed for *Layer 0*.

A program is parsed by matching the `program` rule, which yields a
`closure` node. For simple error handling, the rule `programOrError`
can be used.

```
#
# Helper functions
#

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

# Returns a node representing a thunk (no-arg function) that returns the
# expression represented by the given node.
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


#
# *Layer 0* rules.
#
# With the exception of the forward declaration of `parser` and the
# so-commented clause in the `atom` rule, the following is exactly and only
# the grammar for *Layer 0*.
#

# forward declaration: programBody
# forward declaration: expression
# forward declaration: parser       # Needed for *Layer 1*.

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
        <> ifVoid { <> tokenValue(token) }
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
    {
        # We do a check to make sure the given expression is an interpolate
        # (which is the only way it can be valid). Note that
        # `expression @"*"` won't do the trick, since by the time we're here,
        # if there was a `*` it would have become part of the expression.
        <> ifTrue { <> eq(tokenType(map), "interpolate") }
            { <> tokenValue(map) }
    }
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
    { <> @[expression: ex] }
/};

def atom = {/
    varRef | int | string | list | emptyMap | map | uniqlet | token |
    closure | parenExpression
|
    # The lookahead is just to make it clear that *Layer 1* can
    # only be "activated" with that one specific token.
    &@"{/" parser
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

# Note: There are additional prefix operators in *Layer 2* and beyond.
def prefixOperator = {/
    @"-"
    { <> { node :: <> makeCallName("ineg", node) } }
/};

# Note: There are additional postfix operators in *Layer 2* and beyond.
def postfixOperator = {/
    actuals = actualsList
    { <> { node :: <> makeCall(node, actuals*) } }
|
    # The lookahead failure here is to make the grammar prefer `*` to be
    # treated as a binary op. (`*` is only defined as postfix in layer 0,
    # but higher layers augment its meaning.)
    @"*" !expression
    { <> { node :: <> @[interpolate: node] } }
/};

def unaryExpression = {/
    prefixes = prefixOperator*
    base = atom
    postfixes = postfixOperator*

    {
        def withPosts = listReduce(base, postfixes)
            { result, ., op :: <> op(result) };
        <> listReduce(withPosts, listReverse(prefixes))
            { result, ., op :: <> op(result) }
    }
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

# Note: There are additional expression rules in *Layer 2* and beyond.
def expression = {/
    rangeExpression | fnExpression
/};

def statement = {/
    varDef | fnDef | expression
/};

# Note: There are additional nonlocal exit rules in *Layer 2* and beyond.
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


#
# *Layer 1* rules.
#
# The remainder of this grammar consists of the definitions required
# to implement *Layer 1*, above and beyond the above.
#
# **Note:** The grammar uses the label "pex" to denote various
# "Parser EXpression" types.

# forward declaration: choicePex

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
