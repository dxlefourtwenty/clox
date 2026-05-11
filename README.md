# Chapter 29 Question 1 -- Summary of Changes

### compiler.c
```c
typedef struct ClassCompiler {
  struct ClassCompiler* enclosing;
  Token name;
  bool hasSuperclass;
} ClassCompiler;

Parser parser;
Compiler* current = NULL;
ClassCompiler* currentClass = NULL;
bool thisAccess = false;

// ...

static uint8_t fieldConstant(Token* className, Token* field);

// ...

static void dot(bool canAssign) {
  bool wasThis = thisAccess;
  thisAccess = false;

  consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
  Token field = parser.previous;

  bool mangle = wasThis && currentClass != NULL;
  uint8_t plainName = 0;
  bool plainBuilt = false;

  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    uint8_t name = mangle ? fieldConstant(&currentClass->name, &field) : identifierConstant(&field);
    emitBytes(OP_SET_PROPERTY, name);
  } else if (match(TOKEN_LEFT_PAREN)) {
    if (!plainBuilt) {
      plainName = identifierConstant(&field);
      plainBuilt = true;
    }
    uint8_t argCount = 0;
    if (!check(TOKEN_RIGHT_PAREN)) {
      do {
        expression();
        if (argCount == 255) error("Can't have more than 255 arguments.");
        argCount++;
      } while (match(TOKEN_COMMA));
    }
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
    emitBytes(OP_INVOKE, plainName);
    emitByte(argCount);
  } else {
    uint8_t name = mangle ? fieldConstant(&currentClass->name, &field) : identifierConstant(&field);
    emitBytes(OP_GET_PROPERTY, name);
  }
}

// ...

static uint8_t fieldConstant(Token* className, Token* field) {
  int length = className->length + 1 + field->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, className->start, className->length);
  chars[className->length] = '$';
  memcpy(chars + className->length + 1, field->start, field->length);
  chars[length] = '\0';
  return makeConstant(OBJ_VAL(takeString(chars, length)));
}

// ...

static void this_(bool canAssign) {
  if (currentClass == NULL) {
    error("Can't use 'this' outside of a class.");
    return;
  }
  variable(canAssign);
  thisAccess = check(TOKEN_DOT);
}

// ...

static void classDeclaration() {
  consume(TOKEN_IDENTIFIER, "Expect class name.");
  Token className = parser.previous;
  uint8_t nameConstant = identifierConstant(&parser.previous);
  declareVariable();

  ClassCompiler classCompiler;
  classCompiler.enclosing = currentClass;
  classCompiler.name = className;
  classCompiler.hasSuperclass = false;
  currentClass = &classCompiler;
```
