# Chapter 15 Question 4 -- Summary of Changes

### vm.c
```
case OP_NEGATE: 
        vm.stackTop[-1] = -vm.stackTop[-1];
        break;

static InterpretResult run(void) {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
      vm.stackTop[-2] = vm.stackTop[-2] op vm.stackTop[-1]; \
      vm.stackTop--; \
    } while (false)
```

### Are there other instructions where you can do similar optimization?
```
Yes. The best examples are the arithmetic instructions 
inside vm.c / run()
```

### About the macro change
```
Before optimization, the binary arithmetic instructions used pop() twice and push() once. 
For Challenge Question 4, these instructions can be optimized in place by storing the result in vm.stackTop[-2] 
and then decrementing stackTop once. This works for OP_ADD, OP_SUBTRACT, OP_MULTIPLY, and OP_DIVIDE 
because they consume two stack values and produce one result.
```

