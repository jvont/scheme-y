CFLAGS = -ansi -Wpedantic -std=c99 -g
# CFLAGS += -Wall -Wextra
LDFLAGS = -lm

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
TESTS := $(wildcard test/*.c)

.PHONY: all build rebuild tests clean

all: bin/scheme-y tests
build: bin/scheme-y
rebuild: clean bin/scheme-y

bin/scheme-y: $(OBJS) | bin
	gcc $^ -o $@ $(LDFLAGS)

tests: $(TESTS:test/%.c=bin/%)
	@for f in $^; do ./$$f;	done

# keep intermediate test/%.o files
.SECONDARY: $(TESTS:.c=.o)
bin/%: test/%.o $(filter-out %main.o, $(OBJS)) | bin
	gcc $^ -o $@ $(LDFLAGS)

%.d: %.c
	gcc -MM $^ -MF $@

bin:
	@mkdir -p $@

clean:
	@rm -rf bin
	@rm -f src/*.o src/*.d
	@rm -f test/*.o test/*.d

# do not include dependency files on clean
ifneq ($(MAKECMDGOALS), clean)
-include $(SRCS:.c=.d) $(TESTS:.c=.d)
endif
