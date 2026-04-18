# Chapter 23 Question 2 -- Summary of Changes

### scanner.h
```c
TOKEN_AND, TOKEN_CLASS, TOKEN_CONTINUE, TOKEN_ELSE, TOKEN_FALSE,
```

### scanner.c
```c
case 'c':
  if (scanner.current - scanner.start > 1) {
    switch (scanner.start[1]) {
      case 'l': return checkKeyword(2, 3, "ass", TOKEN_CLASS);
      case 'o': return checkKeyword(2, 6, "ntinue", TOKEN_CONTINUE);
    }
  }
  break;
```

### compiler.c
```c
typedef struct {
  Local locals[UINT8_COUNT];
  int localCount;
  int scopeDepth;
  int innermostLoopStart;
  int innermostLoopScopeDepth;
} Compiler;

[TOKEN_CONTINUE]      = {NULL,     NULL,   PREC_NONE},

static void continueStatement() {
  if (current->innermostLoopStart == -1) {
    error("Can't use 'continue' outside of a loop.");
    return;
  }

  consume(TOKEN_SEMICOLON, "Expect ';' after 'continue'.");

  for (int i = current->localCount - 1; i >= 0; i--) {
    if (current->locals[i].depth <= current->innermostLoopScopeDepth) {
      break;
    }
    emitByte(OP_POP);
  }

  emitLoop(current->innermostLoopStart);
}

static void forStatement() {
  beginScope();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
  if (match(TOKEN_SEMICOLON)) {
  } else if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    expressionStatement();
  }

  int surroundingLoopStart = current->innermostLoopStart;
  int surroundingLoopScopeDepth = current->innermostLoopScopeDepth;

  int loopStart = currentChunk()->count;
  int exitJump = -1;
  current->innermostLoopStart = loopStart;
  current->innermostLoopScopeDepth = current->scopeDepth;

  if (!match(TOKEN_SEMICOLON)) {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");
    exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP);
  }

  if (!match(TOKEN_RIGHT_PAREN)) {
    int bodyJump = emitJump(OP_JUMP);
    int incrementStart = currentChunk()->count;
    expression();
    emitByte(OP_POP);
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

    emitLoop(loopStart);
    loopStart = incrementStart;
    current->innermostLoopStart = incrementStart;
    patchJump(bodyJump);
  }

  statement();
  emitLoop(loopStart);

  current->innermostLoopStart = surroundingLoopStart;
  current->innermostLoopScopeDepth = surroundingLoopScopeDepth;

  if (exitJump != -1) {
    patchJump(exitJump);
    emitByte(OP_POP);
  }

  endScope();
}

static void whileStatement() {
  int loopStart = currentChunk()->count;
  int surroundingLoopStart = current->innermostLoopStart;
  int surroundingLoopScopeDepth = current->innermostLoopScopeDepth;

  current->innermostLoopStart = loopStart;
  current->innermostLoopScopeDepth = current->scopeDepth;

  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

  int exitJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();
  emitLoop(loopStart);

  current->innermostLoopStart = surroundingLoopStart;
  current->innermostLoopScopeDepth = surroundingLoopScopeDepth;

  patchJump(exitJump);
  emitByte(OP_POP);
}

case TOKEN_CONTINUE:
  return;

} else if (match(TOKEN_CONTINUE)) {
  continueStatement();

compiler->innermostLoopStart = -1;
compiler->innermostLoopScopeDepth = 0;

advance();
while (!match(TOKEN_EOF)) {
  declaration();
}
```
