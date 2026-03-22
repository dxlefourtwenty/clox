# Chapter 17 Question 1 -- Summary of Changes

### No code changes here, but here is the order of 
### the functions that are called
```
expression()
    calls parsePrecedence(PREC_ASSIGNMENT)

CALL A: parsePrecedence(PREC_ASSIGNMENT)
    advance() consumes '('
    getRule(TOKEN_LEFT_PAREN) -> prefix = grouping
    calls grouping()

    grouping()
        calls expression() to parse inside the parentheses

        expression()
            calls parsePrecedence(PREC_ASSIGNMENT)

        CALL B: parsePrecedence(PREC_ASSIGNMENT)
            advance() consumes '-'
            getRule(TOKEN_MINUS) -> prefix = unary
            calls unary()

            unary()
                operatorType = TOKEN_MINUS
                calls parsePrecedence(PREC_UNARY)

                CALL C: parsePrecedence(PREC_UNARY)
                    advance() consumes '1'
                    getRule(TOKEN_NUMBER) -> prefix = number
                    calls number()

                    number()
                        emits constant 1

                    while PREC_UNARY <= precedence(current token '+')
                        compare PREC_UNARY <= PREC_TERM -> false

                unary()
                    emits NEGATE

            while PREC_ASSIGNMENT <= precedence(current token '+')
                compare PREC_ASSIGNMENT <= PREC_TERM -> true
                advance() consumes '+'
                getRule(TOKEN_PLUS) -> infix = binary
                calls binary()

                binary()
                    operatorType = TOKEN_PLUS
                    getRule(TOKEN_PLUS) -> precedence = PREC_TERM
                    calls parsePrecedence(PREC_TERM + 1)

                    CALL D: parsePrecedence(PREC_FACTOR)
                        advance() consumes '2'
                        getRule(TOKEN_NUMBER) -> prefix = number
                        calls number()

                        number()
                            emits constant 2

                        while PREC_FACTOR <= precedence(current token ')')
                            compare PREC_FACTOR <= PREC_NONE -> false

                    binary()
                        emits ADD

            while PREC_ASSIGNMENT <= precedence(current token ')')
                compare PREC_ASSIGNMENT <= PREC_NONE -> false

        grouping()
            consumes TOKEN_RIGHT_PAREN ')'

    while PREC_ASSIGNMENT <= precedence(current token '*')
        compare PREC_ASSIGNMENT <= PREC_FACTOR -> true
        advance() consumes '*'
        getRule(TOKEN_STAR) -> infix = binary
        calls binary()

        binary()
            operatorType = TOKEN_STAR
            getRule(TOKEN_STAR) -> precedence = PREC_FACTOR
            calls parsePrecedence(PREC_FACTOR + 1)

            CALL E: parsePrecedence(PREC_UNARY)
                advance() consumes '3'
                getRule(TOKEN_NUMBER) -> prefix = number
                calls number()

                number()
                    emits constant 3

                while PREC_UNARY <= precedence(current token '-')
                    compare PREC_UNARY <= PREC_TERM -> false

            binary()
                emits MULTIPLY

    while PREC_ASSIGNMENT <= precedence(current token '-')
        compare PREC_ASSIGNMENT <= PREC_TERM -> true
        advance() consumes '-'
        getRule(TOKEN_MINUS) -> infix = binary
        calls binary()

        binary()
            operatorType = TOKEN_MINUS
            getRule(TOKEN_MINUS) -> precedence = PREC_TERM
            calls parsePrecedence(PREC_TERM + 1)

            CALL F: parsePrecedence(PREC_FACTOR)
                advance() consumes '-'
                getRule(TOKEN_MINUS) -> prefix = unary
                calls unary()

                unary()
                    operatorType = TOKEN_MINUS
                    calls parsePrecedence(PREC_UNARY)

                    CALL G: parsePrecedence(PREC_UNARY)
                        advance() consumes '4'
                        getRule(TOKEN_NUMBER) -> prefix = number
                        calls number()

                        number()
                            emits constant 4

                        while PREC_UNARY <= precedence(current token EOF)
                            compare PREC_UNARY <= PREC_NONE -> false

                    unary()
                        emits NEGATE

                while PREC_FACTOR <= precedence(current token EOF)
                    compare PREC_FACTOR <= PREC_NONE -> false

            binary()
                emits SUBTRACT

    while PREC_ASSIGNMENT <= precedence(current token EOF)
        compare PREC_ASSIGNMENT <= PREC_NONE -> false
```
