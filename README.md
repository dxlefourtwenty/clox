# Chapter 22 Question 3 -- Summary of Changes

# scanner.h (token added)
```
add:
  TOKEN_CONST,
```

# scanner.c (keyword recognition for const)
```
update:
  identifierType() case 'c'

before:
  "class" only

after:
  supports both:
    "class" -> TOKEN_CLASS
    "const" -> TOKEN_CONST
```

# compiler.c (handling const and declaration logic)
```
Local struct update:
  add bool isConst;

Compiler struct update:
  add Token constGlobals[UINT8_COUNT];
  add int constGlobalCount;

Declaration flow:
  declaration():
    if (match(TOKEN_VAR))   varDeclaration(false);
    else if (match(TOKEN_CONST)) varDeclaration(true);
    else statement();

Variable declaration updates:
  parseVariable(const char* errorMessage, bool isConst)
  declareVariable(bool isConst)
  varDeclaration(bool isConst)

Const rules enforced:
  const must be initialized:
    error("Const variables must be initialized.");

  assignment restriction (compile-time):
    local const:
      error("Can't assign to const variable.");
    global const:
      error("Can't assign to const variable.");

Global const tracking:
  markGlobalConst(Token name)
  isConstGlobal(Token name)

Parse rules:
  add [TOKEN_CONST] = {NULL, NULL, PREC_NONE}

Synchronization:
  add TOKEN_CONST as statement boundary token

Top-level compile loop:
  before:
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");

  after:
    while (!match(TOKEN_EOF)) {
      declaration();
    }
```
