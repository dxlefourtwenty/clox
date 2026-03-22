CC := gcc

TARGET := main
SRCS := main.c chunk.c memory.c debug.c value.c vm.c
HARDCORE_TEST := main_hardcore_test
HARDCORE_TEST_SRCS := main_hardcore.c memory_hardcore.c
OBJDIR := obj
OBJS := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

# Max diagnostics + debug info + runtime checks.
CFLAGS := -std=c17 -g3 -O0 \
	-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wformat=2 \
	-fdiagnostics-color=always -fdiagnostics-show-option \
	-fno-omit-frame-pointer -fsanitize=address,undefined
LDFLAGS := -fsanitize=address,undefined

.PHONY: all hardcore-test hardcore-run clean

all: $(TARGET)

hardcore-test: $(HARDCORE_TEST)

hardcore-run: $(HARDCORE_TEST)
	ASAN_OPTIONS=detect_leaks=0 ./$(HARDCORE_TEST)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(HARDCORE_TEST): $(HARDCORE_TEST_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET) $(HARDCORE_TEST)
