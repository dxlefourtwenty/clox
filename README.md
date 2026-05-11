# Chapter 30 Question 1 -- Summary of Changes

### vm.c
```c
#define BINARY_OP(valueType, op) \
  do { \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
      runtimeError("Operands must be numbers."); \
      return INTERPRET_RUNTIME_ERROR; \
    } \
    double b = AS_NUMBER(pop()); \
    vm.stackTop[-1] = valueType(AS_NUMBER(vm.stackTop[-1]) op b); \
  } while (false)

// ...

        } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
          double b = AS_NUMBER(pop());
          vm.stackTop[-1] = NUMBER_VAL(AS_NUMBER(vm.stackTop[-1]) + b);
        } else {
          runtimeError("Operands must be two numbers or two strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
```

```text
Performance comparison:
Unoptimized total runtime: 10.69s
Optimized total runtime: 9.55s
Performance gain: about 10.66%

Unoptimized stack ops:
_peek: 1.84s, 17.21%
_push: 1.32s, 12.35%
_pop: 1.30s, 12.16%

Optimized stack ops:
_peek: 1.87s, 19.58%
_pop: 1.03s, 10.79%
_push: 0.93s, 9.74%
```
