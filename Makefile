CC = gcc
CFLAGS = -ansi -Wpedantic -std=c99 -g
# CFLAGS += -Wall -Wextra
LDFLAGS = -lm

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
TESTS := $(wildcard tests/*.c)

.PHONY: all build rebuild tests clean

all: build tests
rebuild: clean build
build: bin/scheme-y

bin/scheme-y: $(OBJS) | bin
	$(CC) $^ -o $@ $(LDFLAGS)

tests: $(TESTS:tests/%.c=bin/%) | bin/scheme-y
	@for f in $^ ; do ./$$f ; done

# keep intermediate tests/%.o files
.SECONDARY: $(TESTS:.c=.o)
bin/%: tests/%.o $(filter-out %main.o, $(OBJS)) | bin
	$(CC) $^ -o $@ $(LDFLAGS)

%.d: %.c
	$(CC) -MM $^ -MF $@

bin:
	mkdir -p $@

clean:
	rm -rf bin
	rm -f src/*.o src/*.d tests/*.o tests/*.d

# do not include dependency files on clean
ifneq ($(MAKECMDGOALS), clean)
-include $(SRCS:.c=.d) $(TESTS:.c=.d)
endif
