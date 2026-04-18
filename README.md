# Chapter 23 Question 1 -- Direct Code Changes

### scanner.h
Before:
```c
TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
...
TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,
```

After:
```c
TOKEN_COMMA, TOKEN_DOT, TOKEN_COLON, TOKEN_MINUS, TOKEN_PLUS,
...
TOKEN_AND, TOKEN_CASE, TOKEN_CLASS, TOKEN_DEFAULT, TOKEN_ELSE, TOKEN_FALSE,
TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
TOKEN_TRUE, TOKEN_VAR, TOKEN_SWITCH, TOKEN_WHILE,
```

### scanner.c
Before:
```c
case '.': return makeToken(TOKEN_DOT);
case '-': return makeToken(TOKEN_MINUS);

case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
...
case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
```

After:
```c
case '.': return makeToken(TOKEN_DOT);
case ':': return makeToken(TOKEN_COLON);
case '-': return makeToken(TOKEN_MINUS);

case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
case 'c':
  if (scanner.current - scanner.start > 1) {
    switch (scanner.start[1]) {
      case 'a': return checkKeyword(2, 2, "se", TOKEN_CASE);
      case 'l': return checkKeyword(2, 3, "ass", TOKEN_CLASS);
    }
  }
  break;
case 'd': return checkKeyword(1, 6, "efault", TOKEN_DEFAULT);
...
case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
case 's':
  if (scanner.current - scanner.start > 1) {
    switch (scanner.start[1]) {
      case 'u': return checkKeyword(2, 3, "per", TOKEN_SUPER);
      case 'w': return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
    }
  }
  break;
```

### chunk.h
Before:
```c
OP_NIL,
OP_TRUE,
OP_FALSE,
OP_POP,
```

After:
```c
OP_NIL,
OP_TRUE,
OP_FALSE,
OP_DUP,
OP_POP,
```

### compiler.c
Before:
```c
Parser parser;
Compiler* current = NULL;
Chunk* compilingChunk;

ParseRule rules[] = {
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  ...
  [TOKEN_AND]           = {NULL,     and_,   PREC_AND},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  ...
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
};

static void statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else if (match(TOKEN_FOR)) {
    forStatement();
  } else if (match(TOKEN_IF)) {
    ifStatement();
  } else if (match(TOKEN_WHILE)) {
    whileStatement();
  } else if (match(TOKEN_LEFT_BRACE)) {
    beginScope();
    block();
    endScope();
  } else {
    expressionStatement();
  }
}

bool compile(const char* source, Chunk* chunk) {
  ...
  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");
  endCompiler();
  return !parser.hadError;
}
```

After:
```c
Parser parser;
Compiler* current = NULL;
Chunk* compilingChunk;

#define MAX_SWITCH_CASES 256

ParseRule rules[] = {
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COLON]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  ...
  [TOKEN_AND]           = {NULL,     and_,   PREC_AND},
  [TOKEN_CASE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DEFAULT]       = {NULL,     NULL,   PREC_NONE},
  ...
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SWITCH]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
};

static void switchStatement() {
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'switch'.");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after value.");
  consume(TOKEN_LEFT_BRACE, "Expect '{' before switch cases.");

  int state = 0;
  int caseEnds[MAX_SWITCH_CASES];
  int caseCount = 0;
  int previousCaseSkip = -1;

  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    if (match(TOKEN_CASE) || match(TOKEN_DEFAULT)) {
      TokenType caseType = parser.previous.type;
      if (state == 2) {
        error("Can't have another case or default after the default case.");
      }

      if (state == 1) {
        if (caseCount == MAX_SWITCH_CASES) {
          error("Too many case clauses in switch statement.");
        } else {
          caseEnds[caseCount++] = emitJump(OP_JUMP);
        }
        patchJump(previousCaseSkip);
        emitByte(OP_POP);
      }

      if (caseType == TOKEN_CASE) {
        state = 1;
        emitByte(OP_DUP);
        expression();
        consume(TOKEN_COLON, "Expect ':' after case value.");
        emitByte(OP_EQUAL);
        previousCaseSkip = emitJump(OP_JUMP_IF_FALSE);
        emitByte(OP_POP);
      } else {
        state = 2;
        consume(TOKEN_COLON, "Expect ':' after default.");
        if (previousCaseSkip != -1) {
          patchJump(previousCaseSkip);
          emitByte(OP_POP);
        }
        previousCaseSkip = -1;
      }
    } else {
      if (state == 0) {
        error("Can't have statements before any case.");
      }
      statement();
    }
  }

  consume(TOKEN_RIGHT_BRACE, "Expect '}' after switch cases.");

  if (state == 1) {
    if (caseCount == MAX_SWITCH_CASES) {
      error("Too many case clauses in switch statement.");
    } else {
      caseEnds[caseCount++] = emitJump(OP_JUMP);
    }
    patchJump(previousCaseSkip);
    emitByte(OP_POP);
  }

  emitByte(OP_POP);
  for (int i = 0; i < caseCount; i++) {
    patchJump(caseEnds[i]);
  }
}

static void statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else if (match(TOKEN_FOR)) {
    forStatement();
  } else if (match(TOKEN_IF)) {
    ifStatement();
  } else if (match(TOKEN_SWITCH)) {
    switchStatement();
  } else if (match(TOKEN_WHILE)) {
    whileStatement();
  } else if (match(TOKEN_LEFT_BRACE)) {
    beginScope();
    block();
    endScope();
  } else {
    expressionStatement();
  }
}

bool compile(const char* source, Chunk* chunk) {
  ...
  advance();
  while (!match(TOKEN_EOF)) {
    declaration();
  }
  endCompiler();
  return !parser.hadError;
}
```

### debug.c
Before:
```c
case OP_FALSE:
  return simpleInstruction("OP_FALSE", offset);
case OP_POP:
  return simpleInstruction("OP_POP", offset);
```

After:
```c
case OP_FALSE:
  return simpleInstruction("OP_FALSE", offset);
case OP_DUP:
  return simpleInstruction("OP_DUP", offset);
case OP_POP:
  return simpleInstruction("OP_POP", offset);
```

### vm.c
Before:
```c
case OP_FALSE: push(BOOL_VAL(false)); break;
case OP_POP: pop(); break;
```

After:
```c
case OP_FALSE: push(BOOL_VAL(false)); break;
case OP_DUP:
  push(peek(0));
  break;
case OP_POP: pop(); break;
```
