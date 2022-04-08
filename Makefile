CC := gcc
CFLAGS = -ansi -Wpedantic -std=c99 -g
# CFLAGS += -Wall -Wextra
LDFLAGS = -lm

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
TESTS := $(wildcard tests/*.c)
DEPENDS := $(OBJS:.o=.d) $(TESTS:.c=.d)

.PHONY: all build rebuild tests clean
all: bin/scheme-y tests
build: bin/scheme-y
rebuild: clean bin/scheme-y

tests: $(TESTS:tests/%.c=bin/%)
	@for f in $^; do ./$$f; done

bin/%: tests/%.o $(filter-out %main.o,$(OBJS)) | bin
	gcc $^ -o $@ $(LDFLAGS)

bin/scheme-y: $(OBJS) | bin
	gcc $^ -o $@ $(LDFLAGS)

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
