CFLAGS = -ansi -Wpedantic -std=c99 -g
# CFLAGS += -Wall -Wextra
LDFLAGS = -lm

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:%.c=%.o)
TESTS := $(wildcard tests/*.c)

.PHONY: rebuild all tests clean

# rebuild: clean all
rebuild: clean bin/scheme-y

all: bin/scheme-y tests

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
