CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -g
LDFLAGS = -lm

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:%.c=%.o)
TESTS := $(wildcard tests/*.c)

# Linux-only readline library
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
CFLAGS += -DUSE_READLINE
LDFLAGS += -lreadline
endif

.PHONY: all tests clean

bin/scheme-y: $(OBJS) | bin
	gcc $^ -o $@ $(LDFLAGS)

tests: $(TESTS:tests/%.c=bin/%)
	$^

bin/%: tests/%.o $(filter-out %scheme-y.o, $(OBJS)) | bin
	gcc $^ -o $@

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

bin:
	mkdir -p $@

clean:
	rm -rf bin
	rm -f src/*.o
	rm -f tests/*.o
