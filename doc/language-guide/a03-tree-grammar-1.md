Samizdat Language Guide
=======================

Appendix: Layer 1 Tree Grammar
------------------------------

The following is the complete tree grammar for Samizdat Layer 1,
written in Samizdat Layer 1, with commentary calling out the parts
that are needed specifically for Layer 1 as well as how Layer 2 hooks in.
Anything left unmarked is also needed for Layer 0.

A program is parsed by matching the `program` rule, which yields a
`closure` node. For simple error handling, the rule `programOrError`
can be used.

```
import core.Format;
import core.Generator :: forEachAll;
import core.Lang0;
import core.LangNode :: *;
import core.Peg;
import core.Range :: ClosedRange;


##
## Private Definitions
##

## Reports the given list of pending tokens as part of error processing.
fn reportError(pending) {
    note("Pending tokens:");

    forEachAll(pending, ClosedRange.newInclusive(1, 50))
        { token, . -> note("    ", $Format::source(token)) };

    die("\nExtra tokens at end of program.")
};


##
## Layer 0 Rules
##
## This section consists of the definitions required to implement Layer 0,
## with comments indicating the "hooks" for higher layers. Subsections are
## used here to separate three related sets of rules (and hopefully make the
## grammar easier to follow).
##

## Forward declarations required for layer 2. These are all add-ons to
## layer 0 or 1 rules, used to expand the syntactic possibilities of the
## indicated base forms.
def parExpression2;
def parPostfixOperator2;
def parPrefixOperator2;
def parTerm2;

## Forward declaration required for integrating layer 1 definitions.
def parPexBlock;

## Forward declarations.
def parAssignExpression;
def parRawClosure;

## Forward declaration for the "top" rule which parses operator expressions.
## This gets bound to `parUnaryExpression` in the Layer 0 and 1 grammars, but
## it's different in Layer 2.
def parOpExpression;

##
## Layer 0: Terms and expressions
##

## Parses an expression in general.
def parExpression = {:
    ## This one's the top level "regular-looking" expression (in that it
    ## covers the territory of C-style expressions).
    %parAssignExpression
|
    ## Note: Layer 2 adds additional rules here.
    %parExpression2
:};

## Parses a parenthesized expression. This produces a result identical to
## the inner `expression` node, except without an `interpolate` binding.
def parParenExpression = {:
    @"("
    ex = parExpression
    @")"

    { withoutIntermediates(ex) }
:};

## Parses a "name" of some sort. This is just an identifier, but with the
## result being a raw symbol (e.g., not wrapped in `@identifier{...}`).
def parNameSymbol = {:
    nameIdent = @identifier
    { nameIdent::value }
:};

## Parses a non-empty comma-separated list of "names." A "name" is as per
## `parNameSymbol` above. The result is a list of symbols (per se).
def parNameSymbolList = {:
    first = parNameSymbol
    rest = (@"," parNameSymbol)*
    { [first, rest*] }
:};

## Parses a variable reference. Returns a variable "fetch" with an `lvalue`
## binding for constructing a "store" as appropriate.
def parVarLvalue = {:
    name = parNameSymbol
    { makeVarFetchGeneral(name) }
:};

## Parses an identifier, identifier-like keyword, or string literal,
## returning a symbol literal in all cases.
def parIdentifierSymbol = {:
    s = @string
    { makeLiteral(cast(Symbol, s::value)) }
|
    name = parNameSymbol
    { makeLiteral(name) }
|
    token = .
    {
        def name = token.get_name();
        If.is { KEYWORDS.get(name) }
            { makeLiteral(name) }
    }
:};

## Parses a key literal. This is similar to the `identifierSymbol` rule,
## except that literal strings mean literal strings, not symbols.
def parKeyLiteral = {:
    s = @string
    { makeLiteral(s::value) }
|
    parIdentifierSymbol
:};

## Parses a simple data literal, including literal booleans, ints, and
## strings.
##
## **Note:** This includes parsing a `-` prefix on ints, to allow for
## negative int literals in layer 0.
def parLiteral = {:
    @"-"
    i = @int
    { makeLiteral(i::value.neg()) }
|
    i = @int
    { makeLiteral(i::value) }
|
    s = @string
    { makeLiteral(s::value) }
|
    @false
    { LITS::false }
|
    @true
    { LITS::true }
|
    @null
    { LITS::null }
|
    @"@"
    symbol = parIdentifierSymbol
    ![@"(" @"{"]  ## Otherwise, records wouldn't be recognized.
    { symbol }
:};

## Parses a map or table key.
def parKey = {:
    key = parKeyLiteral
    @":"
    { key }
|
    key = parExpression
    @":"
    { key }
:};

## Parses a mapping (element of a map or table).
def parMapping = {:
    keys = parKey+
    value = parExpression

    ## `withoutIntermediates` here ensures that `value*` is treated as a
    ## `fetch` and not interpolation into an underlying function call.
    { @mapping{keys, value: withoutIntermediates(value)} }
|
    ## A plain expression is valid only if it's an interpolation, in which
    ## case we take the interpolation variant of the node.
    value = parExpression
    { value::interpolate }
|
    ## Otherwise, it's got to be a raw name, representing a binding of that
    ## name to its value as a variable.
    name = parNameSymbol
    { @mapping{keys: [makeLiteral(name)], value: makeVarFetch(name)} }
:};

## Parses a comma-delimited sequence of zero or more mappings (that is, a
## body of a map or map-like thing). Yields a list of mappings, including
## possibly `@mapping` elements.
def parMappings = {:
    one = parMapping
    rest = (@"," parMapping)*
    { [one, rest*] }
|
    { [] }
:};

## Parses a map literal.
def parMap = {:
    @"{"
    mappings = parMappings
    @"}"

    { makeMapExpression(mappings*) }
:};

## Parses a symbol table literal.
def parSymbolTable = {:
    @"@"
    @"{"
    mappings = parMappings
    @"}"

    { makeSymbolTableExpression(mappings*) }
:};

## Parses a record literal.
def parRecord = {:
    @"@"

    name = (
        parIdentifierSymbol
    |
        parParenExpression
    )

    @"{"
    mappings = parMappings
    @"}"

    {
        makeCall(LITS::Record, SYMS::new, name,
            makeSymbolTableExpression(mappings*))
    }
:};

## Parses a list item or function call argument. This handles all of:
##
## * accepting general expressions
## * rejecting expressions that look like `key:value` mappings. This is
##   effectively "reserved syntax" (for future expansion); rejecting this
##   here means that `x:y` won't be mistaken for other valid syntax.
def parListItem = {:
    expr = parExpression

    (
        @":"
        { die("Mapping syntax not valid as a list item or call argument.") }
    )?

    { expr }
:};

## Parses an "unadorned" (no bracketing) list. Yields a list (per se)
## of contents.
def parUnadornedList = {:
    one = parListItem
    rest = (@"," parListItem)*
    { [one, rest*] }
|
    { [] }
:};

## Parses a list literal.
def parList = {:
    @"["
    expressions = parUnadornedList
    @"]"
    {
        If.is { expressions.crossEq([]) }
            { LITS::EMPTY_LIST }
            {
                def result = makeCallGeneral(
                    LITS::List, SYMS::new, expressions*);
                ## If we end up with an `apply` node, then its `value` is
                ## going to be a list-yielding expression, in which case we
                ## can return that directly.
                If.is { result.hasName(@apply) }
                    { result::values }
                    { result }
            }
    }
:};

## Parses a closure, resulting in one that *always* has a `yield` binding.
def parFullClosure = {:
    ## The lookahead to makes it so we don't have to do any heavier-weight
    ## parsing in easy-failure cases.
    &@"{"
    raw = %parRawClosure

    { withoutTops(makeFullClosure(raw)) }
:};

## Parses a closure, resulting in one that does *not* necessarily have
## a `yield` binding.
def parBasicClosure = {:
    &@"{"  ## Lookahead for same reason as above.
    raw = %parRawClosure

    { withoutTops(makeBasicClosure(raw)) }
:};

## Parses a closure which must not define any formal arguments. This is done
## by parsing an arbitrary closure and then verifying that it does not
## declare formals. This is preferable to not-including formal argument
## syntax, because (a) no rule wants to differentiate these cases (rules
## either want an arbitrary closure or a specifically-constrained kind); (b)
## it reduces redundancy in the syntax, and (c) the error case on the former
## would be more obscure (as in just something like "unexpected token" on
## the would-be formal argument).
def parNullaryClosure = {:
    c = parFullClosure

    {
        If.not { c::formals.crossEq([]) }
            { die("Invalid formal argument in code block.") };
        c
    }
:};

## Like, `parNullaryClosure` (above), except returning a basic (no
## required `yield`) closure node.
def parBasicNullaryClosure = {:
    c = parBasicClosure

    {
        If.not { c::formals.crossEq([]) }
            { die("Invalid formal argument in code block.") };
        c
    }
:};

## Parses a term (basic expression unit).
def parTerm = {:
    parVarLvalue | parLiteral | parSymbolTable | parMap |
    parList | parRecord | parFullClosure | parParenExpression
|
    ## Defined by Samizdat Layer 1. The lookahead is just to make it clear
    ## that Layer 1 can only be "activated" with that one specific token.
    &@"{:" %parPexBlock
|
    ## Defined by Samizdat Layer 2.
    &[@interpolatedString @"(" @"["] %parTerm2
:};

## Parses a list of "actual" (as opposed to formal) arguments to a function.
## Yields a list of expression nodes.
def parActualsList = {:
    @"("
    normalActuals = parUnadornedList
    @")"
    closureActuals = parFullClosure*
    { [closureActuals*, normalActuals*] }
|
    parFullClosure+
:};

## Parses a unary postfix or tight-infix operator. This yields a function
## (per se) to call in order to construct a node that represents the
## appropriate ultimate function call.
def parPostfixOperator = {:
    actuals = parActualsList
    { { node -> makeFunCallGeneral(node, actuals*) } }
|
    @"::"
    key = parKeyLiteral
    { { node -> makeGet(node, key) } }
|
    ## The lookahead failure here is to make the grammar prefer `*` to be
    ## treated as a binary op. (`*` is only defined as postfix in Layer 0,
    ## but higher layers augment its meaning.)
    @"*" !parExpression
    { { node -> makeInterpolate(node) } }
|
    @"?"
    { { node -> makeMaybeValue(node) } }
|
    @"."
    name = (
        symbol = parNameSymbol
        { makeLiteral(symbol) }
    |
        parParenExpression
    )
    actuals = parActualsList
    { { node -> makeCallGeneral(node, name, actuals*) } }
|
    ## This translates as follows:
    ##
    ## ```
    ## target.memberName                =>  target.get_memberName()
    ## target.memberName := expression  =>  target.set_memberName(expression)
    ## ```
    ##
    ## The setter variant works via an `lvalue` binding added to a parsed
    ## getter expression.
    ##
    @"."
    name = parNameSymbol
    {
        def getterRef = makeLiteral(@get_.cat(name));
        { node ->
            def getterCall = makeCall(node, getterRef);
            getterCall.cat(@{
                lvalue: { expr ->
                    def setterRef = makeLiteral(@set_.cat(name));
                    makeCall(node, setterRef, expr)
                }})
        }
    }
|
    ## Note: Layer 2 adds additional rules here.
    %parPostfixOperator2
:};

## Parses a unary expression. This is a term, optionally surrounded on
## either side by any number of unary operators. Postfix operators
## take precedence over (are applied before) the prefix operators.
def parUnaryExpression = {:
    ## The rule is written this way in order to ensure that the `-`
    ## in front of a numeric constant gets parsed as a term and not as
    ## a unary expression.
    basePrefixes = (
        base = parTerm
        { {base, prefixes: []} }
    |
        ## Note: Layer 2 adds prefix operator parsing here.
        prefixes = (%parPrefixOperator2)*
        base = parTerm
        ## Reverse the `prefixes` list, so that prefixes are applied
        ## in outward order from the base term.
        { {base, prefixes: prefixes.reverse()} }
    )

    postfixes = parPostfixOperator*

    {
        def prefixes = basePrefixes::prefixes;
        var result = basePrefixes::base;

        postfixes.forEach { op -> result := op(result) };
        prefixes.forEach { op -> result := op(result) };
        result
    }
:};

## Parses an assignment expression, or passes through to parse a regular
## `opExpression`. An lvalue is parsed here by first parsing an arbitrary
## `opExpression` and then ensuring it's valid via a call to
## `makeAssignmentIfPossible`. This fails (gracefully) if the would-be lvalue
## isn't actually one.
parAssignExpression := {:
    target = %parOpExpression

    (
        @":="
        value = parExpression
        { makeAssignmentIfPossible(target, value) }
    |
        { target }
    )
:};

##
## Layer 0: Statements and yields
##

## Parses a yield or a nonlocal exit (named or implicit). All of the forms
## matched by this rule necessarily appear at the end of a block and are
## expected to be placed in a `yield` binding of a `closure` node. The
## nonlocal exit forms all end up being represented as a `noYield` node, so
## it's sorta funny that they hang off of `yield` (because they never actually
## yield, per se).
def parYieldOrNonlocal = {:
    op = [@break @continue @return @yield]
    optQuest = @"?"?

    name = (
        { op.hasName(@yield) }
        (
            @"/"
            parVarLvalue
        |
            ## Indicate that this is a regular (local) yield. Checked below.
            { @yield{} }
        )
    |
        { makeVarFetch(op.get_name()) }
    )

    ## A value expression is mandatory if there is a `?` after the
    ## operator. Otherwise, it's optional.
    value = (
        v = parExpression
        { If.is { optQuest* } { makeMaybe(v) } { v } }
    |
        { If.not { optQuest* } { @void{} } }
    )

    {
        If.is { name.hasName(@yield) }
            { value }
            { makeNonlocalExit(name, value) }
    }
:};

## Parses an immutable variable definition, or forward declaration of same.
## This includes lazy-evaluated variable definitions.
def parVarDef = {:
    @def
    name = parNameSymbol

    (
        @"="
        expr = parExpression
        { makeVarDef(name, @result, expr) }
    |
        c = parNullaryClosure
        { makeVarDef(name, @lazy, c) }
    |
        { makeVarDef(name, @promise) }
    )
:};

## Parses a mutable variable definition, or forward declaration of same.
def parVarDefMutable = {:
    @var
    name = parNameSymbol
    optExpr = (@"=" parExpression)?

    { makeVarDef(name, @cell, optExpr*) }
:};

## Parses a yield / nonlocal exit definition, yielding the def name.
def parYieldDef = {:
    @"/"
    parNameSymbol
:};

## Parses an optional yield / nonlocal exit definition, always yielding
## a map (an empty map if no yield def was present).
def parOptYieldDef = {:
    y = parYieldDef
    { {yieldDef: y} }
|
    { {} }
:};

## Parses a formal argument decalaration.
def parFormal = {:
    name = (
        n = parNameSymbol
        { {name: n} }
    |
        @"." { {} }
    )

    repeat = (
        r = [@"?" @"*" @"+"]
        { {repeat: r.get_name()} }
    |
        { {} }
    )

    { {name*, repeat*} }
:};

## Parses a list of formal arguments, with no surrounding parentheses.
def parFormalsList = {:
    one = parFormal
    rest = (@"," parFormal)*
    { [one, rest*] }
|
    { [] }
:};

## Parses program / function declarations.
def parClosureDeclarations = {:
    most = (
        name = (
            n = parNameSymbol
            { {name: n} }
        |
            { {} }
        )
        @"("
        formals = parFormalsList
        @")"
        { {name*, formals}}
    |
        formals = parFormalsList
        { {formals} }
    )

    yieldDef = parOptYieldDef

    @"->"

    { {most*, yieldDef*} }
|
    { {formals: []} }
:};

## Parses the common part of function definition and generic function binding.
## The result of this rule is a `@closure` node, translated along these lines:
##
## ```
## name(arg1, arg2) { /out -> stat1; stat2 }
## ```
## =>
## ```
## { name(arg1, arg2) /\"return" ->
##     def out = \"return";
##     stat1;
##     stat2
## }
## ```
##
## except without a yield def binding statement if an explicit yield def was
## not present.
def parFunctionCommon = {:
    name = parNameSymbol
    @"("
    formals = parFormalsList
    @")"
    code = parBasicNullaryClosure

    {
        def basic = withName(
            withFormals(
                withYieldDef(code, @return),
                formals),
            name);

        makeFullClosure(basic)
    }
:};

## Parses a `fn` definition statement. This wraps a `@closure` result of
## `parFunctionCommon` in a top-declaring `@varDef`.
def parFunctionDef = {:
    @fn
    closure = parFunctionCommon

    { withTop(makeVarDef(closure::name, @result, closure)) }
:};

## Parses a single class definition attribute.
def parAttribute = {:
    ## Note: `value` needs to be parsed as a `term` and not an `expression`,
    ## because the former would cause the class body `{...}` to be treated as
    ## a call argument as part of an attribute value in many cases.
    key = parNameSymbol
    @":"
    value = parTerm

    { @{(key): value} }
:};

## Parses a single method definition.
def parMethodDef = {:
    scope = (@class { @classMethod } | { @instanceMethod })
    @"."
    baseClosure = parFunctionCommon

    {
        def closure =
            withFormals(baseClosure, [@{name: @this}, baseClosure::formals*]);
        @(scope){closure*}
    }
:};

## Parses a class definition.
def parClassDef = {:
    @class
    name = parNameSymbol

    attributes = (
        first = parAttribute
        rest = (@"," parAttribute)*
        { [first, rest*] }
    |
        { [] }
    )

    @"{"
    @";"*

    methods = (
        first = parMethodDef
        rest = (@";"+ parMethodDef)*
        @";"*
        { [first, rest*] }
    |
        { [] }
    )

    @"}"

    { makeClassDef(name, attributes, methods) }
:};

## Parses an optional binding name or name prefix for an `import` statement.
## This rule never fails. The result is always a map, empty if there was no
## name / prefix, or binding one of `name` or `prefix`.
def parImportName = {:
    name = parNameSymbol

    key = (
        @"*" { @prefix }
    |
        { @name }
    )

    @"="
    { {(key): name} }
|
    { {} }
:};

## Parses an optional format name for an `import` statement. This rule never
## fails. The result is always a map, empty if there was no format name, or
## binding `format`.
def parImportFormat = {:
    @"@"
    f = parIdentifierSymbol
    { {format: f::value} }
|
    { {} }
:};

## Parses the source for an `import` statement. The result is either a name
## string as the payload of either an `@internal` or `@external` value.
def parImportSource = {:
    @"."
    @"/"

    first = parNameSymbol
    rest = (
        @"/"
        n = parNameSymbol
        { "/".cat(n) }
    )*
    optSuffix = (
        @"."
        n = parNameSymbol
        { ".".cat(n) }
    )?

    {
        def name = "".cat(first, rest*, optSuffix*);
        @internal{name}
    }
|
    first = parNameSymbol
    rest = (
        @"."
        n = parNameSymbol
        { ".".cat(n) }
    )*

    {
        def name = "".cat(first, rest*);
        @external{name}
    }
:};

## Parses a list of binding names for an `import` statement. The result is
## a list of symbols, or `@"*"` to indicate a wildcard of all names.
def parImportSelect = {:
    @"::"
    (
        @"*"
        { {select: @"*"} }
    |
        select = parNameSymbolList
        { {select} }
    )
|
    { {} }
:};

## Parses an `import` statement. This works by first parsing a very general
## version of the syntax which allows invalid combinations, and then checking
## in code that the actual combination is valid.
def parImportStatement = {:
    optExport = @export?
    @import
    nameOrPrefix = parImportName
    format = parImportFormat
    source = parImportSource
    select = parImportSelect

    {
        def data = {nameOrPrefix*, format*, select*, source};
        If.is { optExport* }
            { makeExport(makeImport(data)) }
            { makeImport(data) }
    }
:};

## Parses an executable statement form that is `export`able. This does *not*
## include `import` statements.
def parExportableStatement = {:
    parClassDef | parFunctionDef | parVarDef
:};

## Parses an executable statement form (direct closure / program element).
## This includes all the `export`able statements and a few additional forms.
def parStatement = {:
    parExportableStatement | parVarDefMutable | parExpression
:};

## Parses a program body statement form, including both regular executable
## statements and `export` statements (but not `import` statements).
def parProgramStatement = {:
    parStatement
|
    @export
    (
        select = parNameSymbolList
        { makeExportSelection(select*) }
    |
        stat = parExportableStatement
        { makeExport(stat) }
    )
:};

##
## Layer 0: Closures and programs
##

## Parses a closure body (statements plus optional yield).
def parClosureBody = {:
    @";"*

    most = (
        s = parStatement
        @";"+
        { s }
    )*

    last = (
        s = parStatement
        { {statements: [s]} }
    |
        y = parYieldOrNonlocal
        { {statements: [], yield: y} }
    |
        { {statements: []} }
    )

    @";"*

    { {last*, statements: [most*, last::statements*]} }
:};

## Parses a closure (in-line anonymous function, with no extra bindings).
## This results in a simple map of bindings.
parRawClosure := {:
    @"{"
    decls = parClosureDeclarations
    body = parClosureBody
    @"}"
    { {decls*, body*} }
:};

## Parses a program (list of statements, including imports and exports).
def parProgram = {:
    imports = (
        @";"*
        first = parImportStatement
        rest = (@";"+ parImportStatement)*
        { [first, rest*] }
    |
        { [] }
    )

    body = (
        (
            ## There was at least one import, so there needs to be at least
            ## one semicolon between the final import and first statement.
            { Cmp.ne(imports, []) }
            @";"+
        |
            @";"*
        )

        first = parProgramStatement
        rest = (@";"+ parProgramStatement)*
        { [first, rest*] }
    |
        { [] }
    )

    @";"*

    {
        def closure = makeFullClosure({
            statements: [imports*, body*],
            yield:      @void{}
        });
        withoutTops(closure)
    }
:};

##
## Layer 0: Entry points
##

## Top-level rule to parse an expression with possible error afterwards.
def parExpressionOrError = {:
    ex = parExpression
    (
        pending = .+
        { reportError(pending) }
    )?
    { ex }
:};

## Top-level rule to parse a program with possible error afterwards.
## Note that an empty string is a valid program.
def parProgramOrError = {:
    prog = parProgram
    (
        pending = .+
        { reportError(pending) }
    )?
    { prog }
:};


##
## Layer 1 Rules
##
## This section consists of the definitions required to implement Layer 1,
## above and beyond the preceding section.
##
## **Note:** The grammar uses the label "pex" to denote various
## "Parser EXpression" types and rules.
##

## Forward declaration.
def parPexChoice;

## Map from parser token types to record classes for pexes.
def PEX_TYPES = {
    @"&": @lookaheadSuccess,
    @"!": @lookaheadFailure,
    @"?": @opt,
    @"*": @star,
    @"+": @plus
};

## Parses a variable reference parsing expression.
def parPexVarRef = {:
    name = parNameSymbol
    { makeVarRef(name) }
:};

## Parses a string literal parsing expression.
def parPexString = {:
    @string
:};

## Parses a token literal parsing expression.
def parPexToken = {:
    @"@"
    type = parIdentifierSymbol
    { @token{value: type::value} }
:};

## Parses a string or character range parsing expression, used when defining
## sets. Yields a string per se (not a token).
def parPexSetString = {:
    s = @string

    (
        @".."
        end = @string

        { /out ->
            def startChar = s::value;
            def endChar = end::value;

            ## Reject non-single-character strings.
            If.is { Cmp.ne(1, startChar.get_size()) } { yield /out };
            If.is { Cmp.ne(1, endChar.get_size()) } { yield /out };

            yield "".cat(ClosedRange.newInclusive(startChar, endChar)*)
        }
    |
        { s::value }
    )
:};

## Parses a set (or set complement) parsing expression.
def parPexSet = {:
    @"["

    type = (
        @"!" { @tokenSetComplement }
    |
        { @tokenSet }
    )

    terminals = (
        strings = parPexSetString+
        { "".cat(strings*).collect { ch -> cast(Symbol, ch) } }
    |
        tokens = parPexToken+
        { tokens.collect { n -> n::value } }
    |
        { [] }
    )

    @"]"

    { @(type){values: terminals} }
:};

## Parses a code block parsing expression.
def parPexCode = {:
    closure = parNullaryClosure
    { @code{closure*} }
:};

## Parses a thunk parsing expression.
def parPexThunk = {:
    @"%"
    value = parTerm
    { @thunk{value} }
:};

## Parses a parsing expression term.
def parPexTerm = {:
    @"."
    { @any{} }
|
    @"("
    result = (%parPexChoice | { @empty{} })
    @")"
    { result }
|
    parPexVarRef | parPexString | parPexToken | parPexSet |
    parPexCode | parPexThunk
:};

## Parses a fully-qualified parsing item.
def parPexItem = {:
    optName = (
        name = parNameSymbol
        @"="
        { name }
    )?

    optLookahead = [@"&" @"!"]?
    base = parPexTerm
    optRepeat = [@"?" @"*" @"+"]?

    {
        var pex = base;

        If.value { optRepeat* }
            { repeat ->
                pex := @(PEX_TYPES.get(repeat.get_name())){pex}
            };
        If.value { optLookahead* }
            { lookahead ->
                pex := @(PEX_TYPES.get(lookahead.get_name())){pex}
            };
        If.value { optName* }
            { name ->
                pex := @varDef{name, value: pex}
            };

        pex
    }
:};

## Parses a sequence parsing expression. This includes sequences of length
## one, but it does *not* parse empty (zero-length) sequences.
def parPexSequence = {:
    pexes = parPexItem+
    { @sequence{pexes} }
:};

## Parses a choice parsing expression. This includes a single choice.
parPexChoice := {:
    one = parPexSequence
    rest = (@"|" parPexSequence)*
    { @choice{pexes: [one, rest*]} }
:};

## Parses a parser block. This results in a `@parser` value.
parPexBlock := {:
    @"{:"
    pex = parPexChoice
    @":}"
    { @parser{pex} }
:};


##
## Layer 2 Rule Stubs
##

## For more details, see comment where `parOpExpression` is declared, above.
parOpExpression := parUnaryExpression;

## In layer 2, these are all non-trivial, but here they are simply
## always-fail.

parExpression2      := {: !() :};
parPostfixOperator2 := {: !() :};
parPrefixOperator2  := {: !() :};
parTerm2            := {: !() :};
```
