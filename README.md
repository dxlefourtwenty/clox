# Chapter 24 Question 1 -- Summary of Changes

### vm.c
```
  register uint8_t* ip = frame->ip;

// ...

#define READ_BYTE() (*ip++)
#define READ_SHORT() \
    (ip += 2, (uint16_t)((ip[-2] << 8) | ip[-1]))
#define RUNTIME_ERROR(...) \
    do { \
      frame->ip = ip; \
      runtimeError(__VA_ARGS__); \
      return INTERPRET_RUNTIME_ERROR; \
    } while (false)

// ...

        RUNTIME_ERROR("Operands must be numbers."); \

// ...

    disassembleInstruction(&frame->function->chunk,
        (int)(ip - frame->function->chunk.code));

// ...

          RUNTIME_ERROR("Undefined variable '%s'.", name->chars);

// ...

          RUNTIME_ERROR("Undefined variable '%s'.", name->chars);

// ...

          RUNTIME_ERROR(
              "Operands must be two numbers or two strings.");

// ...

          RUNTIME_ERROR("Operand must be a number.");

// ...

        ip += offset;
        if (isFalsey(peek(0))) ip += offset;
        ip -= offset;

// ...

        frame->ip = ip;
        frame = &vm.frames[vm.frameCount - 1];
        ip = frame->ip;

// ...

        frame->ip = ip;
        frame = &vm.frames[vm.frameCount - 1];
        ip = frame->ip;

// ...

#undef RUNTIME_ERROR
```
