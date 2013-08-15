Samizdat Language Guide
=======================

Appendix: *Samizdat Layer 1* Tree Grammar
-----------------------------------------

The following is a nearly complete tree grammar for *Samizdat Layer 1*,
written in *Samizdat Layer 1*, with commentary calling out the parts
that are needed specifically for *Layer 1*. Anything left unmarked is
also needed for *Layer 0*. In addition, places where higher layers add
or vary rules are noted in comments.

A program is parsed by matching the `program` rule, which yields a
`closure` node. For simple error handling, the rule `programOrError`
can be used.

```
#
# Helper Definitions
#

# Set-like map of all lowercase identifier characters. Used to figure
# out if we're looking at a keyword in the `identifierString` rule.
def LOWER_ALPHA = [
    inclusiveRange("a", 1, "z")*: true
];

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
    <> @[closure: [statements: [], yield: expression]]
};

# Returns an optional-value expression. This wraps `node` as
# `optValue { <> node }`.
fn makeOptValueExpression(node) {
    <> makeCallName("optValue", makeThunk(node))
};

# Returns a `call` node to a nonlocal exit with the given name and
# with optional expression value. If passed, the expression is allowed
# to evaluate to void, in which case the nonlocal exit yields void at
# its exit point.
fn makeCallNonlocalExit(name, optExpression?) {
    <> ifValue { <> optExpression* }
        { ex ::
            <> makeCallName("nonlocalExit",
                name,
                @[interpolate: makeOptValueExpression(ex)])
        }
        { <> makeCallName("nonlocalExit", name) }
};


#
# *Layer 0* Rules
#
# This section consists of the definitions required to implement *Layer 0*,
# with comments indicating the "hooks" for higher layers.
#

# Forward declarations.
def parProgramBody = forwardFunction();
def parExpression = forwardFunction();
def parVoidableExpression = forwardFunction();

# Forward declaration required for integrating layer 1 definitions.
def parParser = forwardFunction();

# Parses a yield / non-local exit definition, yielding the def name.
def parYieldDef = {/
    @"<"
    name = @identifier
    @">"
    { <> dataOf(name) }
/};

# Parses an optional yield / non-local exit definition, always yielding
# a map (an empty map if no yield def was present).
def parOptYieldDef = {/
    y = parYieldDef
    { <> [yieldDef: y] }
|
    { <> [:] }
/};

# Parses a formal argument decalaration.
def parFormal = {/
    name = (
        n = @identifier
        { <> [name: dataOf(n)] }
    |
        @"." { <> [:] }
    )

    repeat = (
        r = [@"?" @"*" @"+"]
        { <> [repeat: typeOf(r)] }
    |
        { <> [:] }
    )

    { <> [:, name*, repeat*] }
/};

# Parses a list of formal arguments, with no surrounding parentheses.
def parFormalsList = {/
    first = parFormal
    rest = (@"," parFormal)*
    { <> [formals: [first, rest*]] }
|
    { <> [:] }
/};

# Parses program / function declarations.
def parProgramDeclarations = {/
    yieldDef = parOptYieldDef
    formals = parFormalsList

    (@"::" | &@"<>")

    { <> [:, formals*, yieldDef*] }
/};

# Parses a program (top-level program or contents inside function braces).
def parProgram = {/
    decls = (parProgramDeclarations | { <> [:] })
    body = parProgramBody
    { <> @[closure: [:, decls*, body*]] }
/};

# Parses a closure (in-line anonymous function, with no extra bindings).
def parClosure = {/
    @"{"
    prog = parProgram
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
def parNullaryClosure = {/
    c = parClosure

    {
        ifIs { <> mapGet(dataOf(c), "formals") }
            { io0Die("Invalid formal argument in code block.") };
        <> c
    }
/};

# Parses a closure which must have neither formal arguments nor a yield
# definition. See `parseNullaryClosure` above for discussion.
def parCodeOnlyClosure = {/
    c = parNullaryClosure

    {
        ifIs { <> mapGet(dataOf(c), "yieldDef") }
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
# =>
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
def parFnCommon = {/
    @fn

    # This is a variable definition statement which binds the yield def
    # name to the `return` function, if there is in fact a yield def present.
    returnDef = (
        y = parYieldDef
        { <> makeVarDef(y, makeVarRef("return")) }
    )?

    name = (
        n = @identifier
        { <> [name: dataOf(n)] }
    |
        { <> [:] }
    )

    @"("
    formals = parFormalsList
    @")"

    code = parCodeOnlyClosure

    {
        def codeMap = dataOf(code);
        def statements = [returnDef*, mapGet(codeMap, "statements")*];
        <> [
            codeMap*, name*, formals*,
            yieldDef: "return",
            statements: statements
        ]
    }
/};

# Parses a `fn` definition statement. The syntax here is the same as
# what's recognized by `parFnCommon`, except that the name is required.
# We don't error out (terminate the runtime) on a missing name, though, as
# that just means that we're looking at a legit `fn` expression, which will
# get successfully parsed by the `expression` alternative of `statement`.
def parFnDef = {/
    funcMap = parFnCommon

    {
        <> ifIs { <> mapGet(funcMap, "name") }
            { <> @[fnDef: funcMap] }
    }
/};

# Parses a `fn` (function with `return` binding) expression. The translation
# is as described in `parFnCommon` (above) if the function is not given a
# name. If the function *is* given a name, the translation is along the
# following lines (so as to enable self-recursion):
#
# ```
# fn <out> name ...
# ```
# =>
# ```
# {
#     fn <out> name ...;
#     <> name
# }()
# ```
def parFnExpression = {/
    funcMap = parFnCommon
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

# Parses an integer literal.
def parInt = {/
    i = @int
    { <> makeLiteral(dataOf(i)) }
/};

# Parses a string literal.
def parString = {/
    s = @string
    { <> makeLiteral(dataOf(s)) }
/};

# Parses an identifier, identifier-like keyword, or string literal,
# returning a string literal in all cases.
def parIdentifierString = {/
    s = [@identifier @string]
    { <> makeLiteral(dataOf(s)) }
|
    token = .
    {
        <> ifNot { <> dataOf(token) }
            {
                def type = typeOf(token);
                def firstCh = stringNth(type, 0);
                <> ifIs { <> mapGet(LOWER_ALPHA, firstCh) }
                    { <> makeLiteral(type) }
            }
    }
/};

# Parses a possibly-voidable expression.
def parVoidableExpression = {/
    @"&"
    ex = parExpression
    { <> @[voidable: ex] }
|
    parExpression
/};

# Parses an "unadorned" (no bracketing) list. Yields a list (per se)
# of contents.
def parUnadornedList = {/
    first = parVoidableExpression
    rest = (@"," parVoidableExpression)*
    { <> [first, rest*] }
|
    { <> [] }
/};

# Parses a list literal.
def parList = {/
    @"["
    expressions = parUnadornedList
    @"]"
    {
        <> ifIs { <> eq(expressions, []) }
            { <> makeLiteral([]) }
            { <> makeCallName("makeList", expressions*) }
    }
/};

# Parses an empty map literal.
def parEmptyMap = {/
    @"[" @":" @"]"
    { <> makeLiteral([:]) }
/};

# Parses an "atomic" key (as opposed to, e.g., the parens-and-commas form
# of map keys). This rule is used for the left-hand side of both mappings
# and derived values.
def parKeyAtom = {/
    # The lookahead at the end of the rule is to ensure we are not looking
    # at a more complicated expression. `@","` and `@")"` are matched here,
    # so that this rule can stay the same in *Layer 2*.
    k = parIdentifierString
    &[@":" @"," @")" @"]"]
    { <> k }
|
    parExpression
/};

# Parses an arbitrary map key. Note: This rule is nontrivial in *Layer 2*.
def parMapKey = {/
    # *Layer 2* adds alternates here.
#|
    parKeyAtom
/};

# Parses a mapping (element of a map).
def parMapping = {/
    key = parMapKey
    @":"
    value = parExpression

    # The `value` is wrapped in an `expression` node here to prevent
    # interpolation from being applied to `makeMapping`.
    { <> makeCallName("makeMapping", key, @[expression: value]) }
|
    map = parExpression
    {
        # We do a check to make sure the given expression is an interpolate
        # (which is the only way it can be valid). Note that
        # `expression @"*"` won't do the trick, since by the time we're here,
        # if there was a `*` it would have become part of the expression.
        <> ifIs { <> eq(typeOf(map), "interpolate") }
            { <> dataOf(map) }
    }
/};

# Parses a map literal.
def parMap = {/
    @"["
    (@":" @",")?
    first = parMapping
    rest = (@"," parMapping)*
    @"]"
    { <> makeCallName("mapAdd", first, rest*) }
/};

# Parses a literal in derived value form.
def parDeriv = {/
    @"@"

    derivArgs = (
        @"["
        type = parKeyAtom
        value = (@":" parExpression)?
        @"]"
        { <> [type, value*] }
    |
        type = parIdentifierString
        { <> [type] }
    )

    { <> makeCallName("makeValue", derivArgs*) }
/};

# Parses a variable reference.
def parVarRef = {/
    name = @identifier
    { <> makeVarRef(dataOf(name)) }
/};

# Parses a variable definition.
def parVarDef = {/
    @def
    name = @identifier
    @"="
    ex = parExpression
    { <> makeVarDef(dataOf(name), ex) }
/};

# Parses a parenthesized expression.
def parParenExpression = {/
    @"("
    ex = parExpression
    @")"
    { <> @[expression: ex] }
/};

# Parses an atomic expression.
def parAtom = {/
    parVarRef | parInt | parString | parList | parEmptyMap | parMap |
    parDeriv | parClosure | parParenExpression
|
    # Defined by *Samizdat Layer 1*. The lookahead is just to make it clear
    # that *Layer 1* can only be "activated" with that one specific token.
    &@"{/" parParser
#|
    # Note: There are additional atom rules in *Samizdat Layer 2*.
/};

# Parses a list of "actual" (as opposed to formal) arguments to a function.
# Yields a list of expression nodes.
def parActualsList = {/
    @"("
    normalActuals = parUnadornedList
    @")"
    closureActuals = parClosure*
    { <> [closureActuals*, normalActuals*] }
|
    parClosure+
/};

# Note: There are additional prefix operators in *Layer 2* and beyond.
# This rule still exists in *Layer 2* but is totally rewritten.
def parPrefixOperator = {/
    @"-"
    { <> { node <> makeCallName("ineg", node) } }
/};

# Parses a unary postfix operator. This yields a function (per se) to call
# in order to construct a node that represents the appropriate ultimate
# function call.
def parPostfixOperator = {/
    actuals = parActualsList
    { <> { node <> makeCall(node, actuals*) } }
|
    # The lookahead failure here is to make the grammar prefer `*` to be
    # treated as a binary op. (`*` is only defined as postfix in *Layer 0*,
    # but higher layers augment its meaning.)
    @"*" !parExpression
    { <> { node <> @[interpolate: node] } }
#|
    # Note: *Layer 2* adds additional rules here.
/};

# Parses a unary expression. This is an atom, optionally surrounded on
# either side by any number of unary operators. Postfix operators
# take precedence over (are applied before) the prefix operators.
def parUnaryExpression = {/
    prefixes = parPrefixOperator*
    base = parAtom
    postfixes = parPostfixOperator*

    {
        def withPosts = doReduce1(postfixes, base)
            { op, result <> op(result) };
        <> doReduce1(listReverse(prefixes), withPosts)
            { op, result <> op(result) }
    }
/};

# Parses a possibly-voidable expression. This is done rather than including
# `&` as a prefix operator, since it is valid in more limited contexts than
# general expressions.
def implVoidableExpression = {/
    @"&"
    ex = parUnaryExpression
    { <> @[voidable: ex] }
|
    parExpression
/};
parVoidableExpression(implVoidableExpression);

# Note: There are additional expression rules in *Layer 2* and beyond.
# This rule is totally rewritten at that layer.
def parExpression = {/
    parUnaryExpression | parFnExpression
/};

# Note: There are additional expression rules in *Layer 2* and beyond.
# This rule is totally rewritten at that layer.
def parStatement = {/
    parVarDef | parFnDef | parExpression
/};

# Note: There are additional nonlocal exit rules in *Layer 2* and beyond.
# This rule still exists but has several additions.
def parNonlocalExit = {/
    name = (
        @"<"
        n = parVarRef
        @">"
        { <> n }
    |
        @return
        { <> makeVarRef("return") }
    )

    value = parExpression?
    { <> makeCallNonlocalExit(name, value*) }
/};

# Parses a local yield / return.
def parYield = {/
    @"<>"
    (
        ex = parExpression
        { <> [yield: ex] }
    |
        { <> [:] }
    )
/};

# Parses a program body (statements plus optional yield).
def implProgramBody = {/
    @";"*

    most = (
        s = parStatement
        @";"+
        { <> s }
    )*

    last = (
        s = (parStatement | parNonlocalExit)
        { <> [statements: [s]] }
    |
        y = parYield
        { <> [statements: [], y*] }
    |
        { <> [statements: []] }
    )

    @";"*

    {
        def allStatements = [most*, mapGet(last, "statements")*];
        <> [last*, statements: allStatements]
    }
/};
parProgramBody(implProgramBody);

# Top-level rule to parse a program with possible error afterwards.
# Note that an empty string is a valid program.
def parProgramOrError = {/
    prog = parProgram
    (
        pending = .+
        { reportError(pending) }
    )?
    { <> prog }
/};


#
# *Layer 1* Rules
#
# This section consists of the definitions required to implement *Layer 1*,
# above and beyond the preceding section.
#
# **Note:** The grammar uses the label "pex" to denote various
# "Parser EXpression" types.
#

# Forward declaration.
def parChoicePex = forwardFunction();

# Parses a parser function.
def implParser = {/
    @"{/"
    pex = parChoicePex
    @"/}"
    { <> @[parser: pex] }
/};
parParser(implParser);

# Parses a parenthesized parsing expression.
def parParenPex = {/
    @"("
    pex = parChoicePex
    @")"
    { <> pex }
/};

# Parses a string literal parsing expression.
def parParserString = {/
    s = @string
    {
        def value = dataOf(s);
        <> ifIs { <> eq(coreSizeOf(value), 1) }
            { <> @[token: value] }
            { <> s }
    }
/};

# Parses a token literal parsing expression.
def parParserToken = {/
    @"@"
    type = parIdentifierString
    { <> @[token: dataOf(type)] }
/};

# Parses a string or character range parsing expression, used when defining
# sets. Yields a string per se (not a token).
def parParserSetString = {/
    s = @string
    (
        @".."
        end = @string
        {
            def startChar = dataOf(s);
            def endChar = dataOf(end);
            <> ifIs
                { <> eq(1, &eq(coreSizeOf(startChar), coreSizeOf(endChar))) }
                { <> stringAdd(inclusiveRange(startChar, 1, endChar)*) }
        }
    |
        { <> dataOf(s) }
    )
/};

# Parses a set (or set complement) parsing expression.
def parParserSet = {/
    @"["

    type = (
        @"!" { <> "[!]" }
    |
        { <> "[]" }
    )

    terminals = (
        strings = parParserSetString+
        { <> [stringAdd(strings*)*] }
    |
        tokens = parParserToken+
        { <> collectFilter(tokens) { tok <> dataOf(tok) } }
    |
        { <> [] }
    )

    @"]"

    { <> @[(type): terminals] }
/};

# Parses a code block parsing expression.
def parParserCode = {/
    closure = parNullaryClosure
    { <> @["{}": dataOf(closure) ] }
/};

# Parses an atomic parsing expression.
def parParserAtom = {/
    @"."
|
    @"("
    @")"
    { <> @"()" }
|
    parVarRef | parParserString | parParserToken | parParserSet |
    parParserCode | parParenPex
/};

# Parses a repeat (or not) parsing expression.
def parRepeatPex = {/
    atom = parParserAtom
    (
        repeat = [@"?" @"*" @"+"]
        { <> @[typeOf(repeat): atom] }
    |
        { <> atom }
    )
/};

# Parses a lookahead (or not) parsing expression. This covers both lookahead
# success and lookahead failure.
def parLookaheadPex = {/
    (
        lookahead = [@"&" @"!"]
        pex = parRepeatPex
        { <> @[typeOf(lookahead): pex] }
    )
|
    parRepeatPex
/};

# Parses a name (or not) parsing expression.
def parNamePex = {/
    (
        name = @identifier
        @"="
        pex = parLookaheadPex
        { <> @[varDef: [name: dataOf(name), value: pex]] }
    )
|
    parLookaheadPex
/};

# Parses a sequence parsing expression. This includes sequences of length
# one, but it does *not* parse empty (zero-length) sequences.
def parSequencePex = {/
    items = parNamePex+
    { <> @[sequence: items] }
/};

# Parses a choice parsing expression. This includes a single choice.
def implChoicePex = {/
    first = parSequencePex
    rest = (@"|" parSequencePex)*
    { <> @[choice: [first, rest*]] }
/};
parChoicePex(implChoicePex);
```
