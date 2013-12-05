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
def LOWER_ALPHA = {
    (Range::makeInclusiveRange("a", 1, "z"))*: true
};

# Returns an `interpolate` node.
fn makeInterpolate(expression) {
    <> @interpolate(expression)
};

# Returns a `literal` node.
fn makeLiteral(value) {
    <> @literal(value)
};

# Returns a node representing a thunk (no-arg function) that returns the
# expression represented by the given node.
fn makeThunk(expression) {
    <> @closure{formals: [], statements: [], yield: expression}
};

# Returns a `varDef` node.
fn makeVarDef(name, value) {
    <> @varDef{name, value}
};

# Returns a `varRef` node.
fn makeVarRef(name) {
    <> @varRef(name)
};

# Returns a `call` node.
fn makeCall(function, actuals*) {
    <> @call{function, actuals}
};

# Returns a `call` node that names a function as a `varRef`.
fn makeCallName(name, actuals*) {
    <> @call{function: makeVarRef(name), actuals}
};

# Returns a collection access (`get`) expression. This is a `call` node
# of two arguments (a collection node and a key node).
fn makeGetExpression(collArg, keyArg) {
    <> makeCallName("get", collArg, keyArg)
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
        { ex ->
            <> makeCallName("nonlocalExit",
                name,
                makeInterpolate(makeOptValueExpression(ex)))
        }
        { <> makeCallName("nonlocalExit", name) }
};


#
# *Layer 0* Rules
#
# This section consists of the definitions required to implement *Layer 0*,
# with comments indicating the "hooks" for higher layers.
#

# Forward declaration required for integrating layer 1 definitions.
def parParser = makeParseForwarder();

# Forward declarations.
def parExpression = ParseForwarder::make();
def parProgramBody = ParseForwarder::make();
def parTerm = ParseForwarder::make();
def parUnaryExpression = ParseForwarder::make();

# Parses a parenthesized expression. This produces an `expression` node,
# which prevents void-contagion from escaping. If void-contagion-prevention
# is undesired, the result of this rule can be unwrapped with `dataOf`.
def parParenExpression = {/
    @"("
    ex = parExpression

    (
        # Reject commas explicitly, to make for a better error message and
        # also avoid letting a would-be parenthesized expression turn out to
        # be taken to be a function application argument list.
        @","
        { Io1::die("Comma not allowed within parenthesized expression.") }
    )?

    @")"

    { <> @expression(ex) }
/};

# Parses an identifier. **Note:** This is nontrivial in layer 2.
def parIdentifier = {/
    @identifier
/};

# Parses a variable reference.
def parVarRef = {/
    name = parIdentifier
    { <> makeVarRef(dataOf(name)) }
/};

# Parses a variable definition.
def parVarDef = {/
    @def
    name = parIdentifier
    @"="
    ex = parExpression
    { <> makeVarDef(dataOf(name), ex) }
/};

# Parses a yield / nonlocal exit definition, yielding the def name.
def parYieldDef = {/
    @"<"
    name = parIdentifier
    @">"
    { <> dataOf(name) }
/};

# Parses an optional yield / nonlocal exit definition, always yielding
# a map (an empty map if no yield def was present).
def parOptYieldDef = {/
    y = parYieldDef
    { <> {yieldDef: y} }
|
    { <> {} }
/};

# Parses a formal argument decalaration.
def parFormal = {/
    name = (
        n = parIdentifier
        { <> {name: dataOf(n)} }
    |
        @"." { <> {} }
    )

    repeat = (
        r = [@"?" @"*" @"+"]
        { <> {repeat: typeOf(r)} }
    |
        { <> {} }
    )

    { <> {name*, repeat*} }
/};

# Parses a list of formal arguments, with no surrounding parentheses.
def parFormalsList = {/
    one = parFormal
    rest = (@"," parFormal)*
    { <> [one, rest*] }
|
    { <> [] }
/};

# Parses program / function declarations.
def parProgramDeclarations = {/
    yieldDef = parOptYieldDef
    formals = parFormalsList

    (@"->" | &@"<>")

    { <> {formals, yieldDef*} }
|
    { <> {formals: []} }
/};

# Parses a program (top-level program or contents inside function braces).
def parProgram = {/
    decls = parProgramDeclarations
    body = parProgramBody
    { <> @closure{decls*, body*} }
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
        def formals = dataOf(c)::formals;
        ifIs { <> ne(formals, []) }
            { Io0::die("Invalid formal argument in code block.") };
        <> c
    }
/};

# Parses a closure which must have neither formal arguments nor a yield
# definition. See `parseNullaryClosure` above for discussion.
def parCodeOnlyClosure = {/
    c = parNullaryClosure

    {
        ifIs { <> dataOf(c)::yieldDef }
            { Io0::die("Invalid yield definition in code block.") };
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
# { <\"return"> arg1, arg2 ->
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
# * the key `name` bound to the function name, if a name was defined.
def parFnCommon = {/
    @fn

    # This is a variable definition statement which binds the yield def
    # name to the `return` function, if there is in fact a yield def present.
    returnDef = (
        y = parYieldDef
        { <> makeVarDef(y, makeVarRef("return")) }
    )?

    name = (
        n = parIdentifier
        { <> {name: dataOf(n)} }
    |
        { <> {} }
    )

    @"("
    formals = parFormalsList
    @")"

    code = parCodeOnlyClosure

    {
        def codeMap = dataOf(code);
        def statements = [returnDef*, codeMap::statements*];
        <> {
            codeMap*, name*,
            formals,
            yieldDef: "return",
            statements
        }
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
        <> ifIs { <> funcMap::name }
            { <> @fnDef(funcMap) }
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

    (
        name = { <> funcMap::name }
        {
            def mainClosure = @closure{
                formals: [],
                statements: [@fnDef(funcMap)],
                yield: makeVarRef(name)
            };

            <> makeCall(mainClosure)
        }
    |
        { <> @closure(funcMap) }
    )
/};

# Parses an integer literal. Note: This includes parsing a `-` prefix,
# so that simple negative numbers are considered terms and hence eligible
# to be used as map keys.
def parInt = {/
    @"-"
    i = @int
    { <> makeLiteral(Number::neg(dataOf(i))) }
|
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
    parString
|
    ident = parIdentifier
    { <> makeLiteral(dataOf(ident)) }
|
    token = .
    {
        <> ifNot { <> dataOf(token) }
            {
                def type = typeOf(token);
                def firstCh = Collection::nth(type, 0);
                <> ifIs { <> get(LOWER_ALPHA, firstCh) }
                    { <> makeLiteral(type) }
            }
    }
/};

# Parses a key term. This includes parsing of interpolation syntax.
def parKeyTerm = {/
    # The lookahead-failure is to ensure we don't match a variable being
    # interpolated, which is handled by the second alternative.
    key = parIdentifierString
    !@"*"
    { <> key }
|
    key = parTerm
    (
        @"*"
        { <> makeInterpolate(key) }
    |
        { <> key }
    )
/};

# Parses an arbitrary map key. **Note:** This is nontrivial in layer 2.
def parKey = parKeyTerm;

# Parses a mapping (element of a map).
def parMapping = {/
    key = parKey
    @":"
    value = parExpression

    # The `value` is wrapped in an `expression` node here to prevent
    # interpolation from being applied to `makeValueMap`.
    { <> makeCallName("makeValueMap", key, @expression(value)) }
|
    # The only acceptable expressions are interpolations and variable
    # references.
    elem = parExpression
    { <out> ->
        def type = typeOf(elem);
        def value = dataOf(elem);
        ifIs { <> eq(type, "interpolate") }
            { <out> value };
        ifIs { <> eq(type, "varRef") }
            { <out> makeCallName("makeValueMap", makeLiteral(value), elem) }
    }
/};

# Parses a map literal.
def parMap = {/
    @"{"

    result = (
        one = parMapping
        rest = (@"," parMapping)*
        {
            <> ifIs { <> eq(rest, []) }
                { <> one }
                { <> makeCallName("cat", one, rest*) }
        }
    |
        { <> makeLiteral({}) }
    )

    @"}"

    { <> result }
/};

# Parses a list item or function call argument. This handles all of:
#
# * accepting general expressions
# * accepting voidable-prefixed expressions
# * rejecting expressions that look like `key:value` mappings. This is
#   effectively "reserved syntax" (for future expansion); rejecting this
#   here means that `x:y` won't be mistaken for other valid syntax.
def parListItem = {/
    parIdentifierString
    @":"
    { Io1::die("Mapping syntax not valid as a list item or call argument.") }
|
    @"&"
    ex = parUnaryExpression
    { <> @voidable(ex) }
|
    parExpression
/};

# Parses an "unadorned" (no bracketing) list. Yields a list (per se)
# of contents.
def parUnadornedList = {/
    one = parListItem
    rest = (@"," parListItem)*
    { <> [one, rest*] }
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

# Parses a literal in derived value form.
def parDeriv = {/
    @"@"

    type = (parIdentifierString | parParenExpression)
    value = (parParenExpression | parMap | parList)?

    { <> makeCallName("makeValue", type, value*) }
/};

# Parses a term (basic expression unit). **Note:** Parsing for `Map` needs
# to be done before `List`, since the latter rejects "map-like" syntax as a
# fatal error.
def parTerm = {/
    parVarRef | parInt | parString | parMap | parList
    parDeriv | parClosure | parParenExpression
|
    # Defined by *Samizdat Layer 1*. The lookahead is just to make it clear
    # that *Layer 1* can only be "activated" with that one specific token.
    &@"{/" parParser
# |
    # Note: There are additional term rules in *Samizdat Layer 2*.
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

# Parses a unary postfix operator. This yields a function (per se) to call
# in order to construct a node that represents the appropriate ultimate
# function call.
def parPostfixOperator = {/
    actuals = parActualsList
    { <> { node <> makeCall(node, actuals*) } }
|
    # This is sorta-kinda a binary operator, but in terms of precedence it
    # fits better here.
    @"::"
    key = parIdentifierString
    { <> { node <> makeGetExpression(node, key) } }
|
    # The lookahead failure here is to make the grammar prefer `*` to be
    # treated as a binary op. (`*` is only defined as postfix in *Layer 0*,
    # but higher layers augment its meaning.)
    @"*" !parExpression
    { <> { node <> makeInterpolate(node) } }
# |
    # Note: *Layer 2* adds additional rules here.
/};

# Parses a unary expression. This is a term, optionally surrounded on
# either side by any number of unary operators. Postfix operators
# take precedence over (are applied before) the prefix operators.
def parUnaryExpression = {/
    # Note: Layer 2 adds prefix operator parsing here.
    base = parTerm
    postfixes = parPostfixOperator*

    { <> Generator::doReduce1(postfixes, base) { op, result <> op(result) } }
/};

# Note: There are additional expression rules in *Layer 2* and beyond.
# This rule is totally rewritten at that layer.
def implExpression = {/
    parUnaryExpression | parFnExpression
/};
Box::store(parExpression, implExpression);

# Note: There are additional expression rules in *Layer 2* and beyond.
def parStatement = {/
    parVarDef | parFnDef | parExpression
# |
    # Note: *Layer 2* adds additional rules here.
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
        { <> {yield: ex} }
    |
        { <> {} }
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
        { <> {statements: [s]} }
    |
        y = parYield
        { <> {statements: [], y*} }
    |
        { <> {statements: []} }
    )

    @";"*

    {
        def allStatements = [most*, last::statements*];
        <> [last*, statements: allStatements]
    }
/};
Box::store(parProgramBody, implProgramBody);

# Top-level rule to parse an expression with possible error afterwards.
def parExpressionOrError = {/
    ex = parExpression
    (
        pending = .+
        { reportError(pending) }
    )?
    { <> ex }
/};

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
def parChoicePex = makeParseForwarder();

# Map from parser tokens to derived value types for pexes.
def PEX_TYPES = {
    "&": "lookaheadSuccess",
    "!": "lookaheadFailure",
    "?": "opt",
    "*": "star",
    "+": "plus"
};

# Parses a parser function.
def implParser = {/
    @"{/"
    pex = parChoicePex
    @"/}"
    { <> @parser(pex) }
/};
Box::store(parParser, implParser);

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
        <> ifIs { <> eq(Collection::sizeOf(value), 1) }
            { <> @token(value) }
            { <> s }
    }
/};

# Parses a token literal parsing expression.
def parParserToken = {/
    @"@"
    type = parIdentifierString
    { <> @token(dataOf(type)) }
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
                {
                    <> eq(1,
                        &eq(Collection::sizeOf(startChar),
                            Collection::sizeOf(endChar)))
                }
                { <> cat(Range::makeInclusiveRange(startChar, 1, endChar)*) }
        }
    |
        { <> dataOf(s) }
    )
/};

# Parses a set (or set complement) parsing expression.
def parParserSet = {/
    @"["

    type = (
        @"!" { <> "tokenSetComplement" }
    |
        { <> "tokenSet" }
    )

    terminals = (
        strings = parParserSetString+
        { <> [cat(strings*)*] }
    |
        tokens = parParserToken+
        { <> Generator::filter(tokens, dataOf) }
    |
        { <> [] }
    )

    @"]"

    { <> @(type)(terminals) }
/};

# Parses a code block parsing expression.
def parParserCode = {/
    closure = parNullaryClosure
    { <> @code(dataOf(closure)) }
/};

# Parses a parsing expression term.
def parParserTerm = {/
    @"."
    { <> @any }
|
    @"("
    @")"
    { <> @empty }
|
    parVarRef | parParserString | parParserToken | parParserSet |
    parParserCode | parParenPex
/};

# Parses a repeat (or not) parsing expression.
def parRepeatPex = {/
    term = parParserTerm
    (
        repeat = [@"?" @"*" @"+"]
        { <> @(get(PEX_TYPES, typeOf(repeat)))(term) }
    |
        { <> term }
    )
/};

# Parses a lookahead (or not) parsing expression. This covers both lookahead
# success and lookahead failure.
def parLookaheadPex = {/
    (
        lookahead = [@"&" @"!"]
        pex = parRepeatPex
        { <> @(get(PEX_TYPES, typeOf(lookahead)))(pex) }
    )
|
    parRepeatPex
/};

# Parses a name (or not) parsing expression.
def parNamePex = {/
    (
        name = parIdentifier
        @"="
        pex = parLookaheadPex
        { <> @varDef{name: dataOf(name), value: pex} }
    )
|
    parLookaheadPex
/};

# Parses a sequence parsing expression. This includes sequences of length
# one, but it does *not* parse empty (zero-length) sequences.
def parSequencePex = {/
    items = parNamePex+
    { <> @sequence(items) }
/};

# Parses a choice parsing expression. This includes a single choice.
def implChoicePex = {/
    one = parSequencePex
    rest = (@"|" parSequencePex)*
    { <> @choice[one, rest*] }
/};
Box::store(parChoicePex, implChoicePex);
```
