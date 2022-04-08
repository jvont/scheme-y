CC := gcc
CFLAGS = -ansi -Wpedantic -std=c99 -g
# CFLAGS += -Wall -Wextra
LDFLAGS = -lm

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
TARGET := bin/scheme-y

TSRCS := $(wildcard tests/*.c)
TOBJS := $(TSRCS:.c=.o) $(filter-out %main.o,$(OBJS))
TESTS := $(TSRCS:tests/%.c=bin/%)

DEPENDS := $(SRCS:.c=.d) $(TSRCS:.c=.d)

.PHONY: all build rebuild tests clean

all: $(TARGET) tests
build: $(TARGET)
rebuild: clean $(TARGET)

$(TARGET): $(OBJS) | bin
	$(CC) $^ -o $@ $(LDFLAGS)

tests: $(TESTS)
	@for f in $^; do ./$$f; done

$(TESTS): $(TOBJS) | bin
	$(CC) $^ -o $@ $(LDFLAGS)

%.d: %.c
	$(CC) -MM $^ -MF $@

bin:
	@mkdir -p $@

clean:
	@rm -rf bin
	@rm -f src/*.o src/*.d
	@rm -f test/*.o test/*.d

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPENDS)
endif
