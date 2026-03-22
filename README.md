# Chapter 17 Question 3 -- Summary of Changes 

I went with a different approach, since the group implemented the
code with the full compiler already written

### scanner.h
```
typedef enum {
  // Single-character tokens.
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
  TOKEN_QUESTION, TOKEN_COLON,
  TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
  // One or two character tokens.
  TOKEN_BANG, TOKEN_BANG_EQUAL,
  TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,
  // Literals.
  TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
  // Keywords.
  TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
  TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
  TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
  TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

  TOKEN_ERROR, TOKEN_EOF
} TokenType;
```

### scanner.c
```
// adds TOKEN_QUESTION and TOKEN_COLON
switch (c) {
  case '(': return makeToken(TOKEN_LEFT_PAREN);
  case ')': return makeToken(TOKEN_RIGHT_PAREN);
  case '{': return makeToken(TOKEN_LEFT_BRACE);
  case '}': return makeToken(TOKEN_RIGHT_BRACE);
  case ';': return makeToken(TOKEN_SEMICOLON);
  case ',': return makeToken(TOKEN_COMMA);
  case '.': return makeToken(TOKEN_DOT);
  case '-': return makeToken(TOKEN_MINUS);
  case '+': return makeToken(TOKEN_PLUS);
  case '?': return makeToken(TOKEN_QUESTION);
  case ':': return makeToken(TOKEN_COLON);
  case '/': return makeToken(TOKEN_SLASH);
  case '*': return makeToken(TOKEN_STAR);
  case '!':
    return makeToken(
        match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=':
    return makeToken(
        match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '<':
    return makeToken(
        match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>':
    return makeToken(
        match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  case '"': return string();
} 
```

### compiler.c
```

// adds PREC_CONDITIONAL
typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_CONDITIONAL,
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Precedence;

// ...

static void conditional() {
  // Parse both operands to demonstrate the Pratt parser wiring for `?:`.
  // This intentionally does not emit any ternary-specific bytecode yet.
  parsePrecedence(PREC_ASSIGNMENT);
  consume(TOKEN_COLON,
      "Expect ':' after then branch of conditional expression.");
  parsePrecedence(PREC_CONDITIONAL);
}

// ..

// added TOKEN_QUESTION and TOKEN_COLON
ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,        PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,        PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,        PREC_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,        PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,        PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary,      PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary,      PREC_TERM},
  [TOKEN_QUESTION]      = {NULL,     conditional, PREC_CONDITIONAL},
  [TOKEN_COLON]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,        PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary,      PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary,      PREC_FACTOR},
  [TOKEN_BANG]          = {NULL,     NULL,        PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     NULL,        PREC_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,        PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,        PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,        PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,        PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL,     NULL,        PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,        PREC_NONE},
  [TOKEN_STRING]        = {NULL,     NULL,        PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,        PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,        PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,        PREC_NONE},
  [TOKEN_FALSE]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,        PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,        PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,        PREC_NONE},
  [TOKEN_NIL]           = {NULL,     NULL,        PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,        PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,        PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,        PREC_NONE},
  [TOKEN_TRUE]          = {NULL,     NULL,        PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,        PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,        PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,        PREC_NONE},
};
```
