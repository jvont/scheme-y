CC = gcc
CFLAGS = -ansi -Wpedantic -std=c99 -g
# CFLAGS += -Wall -Wextra
LDFLAGS = -lm

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)

TESTS := $(wildcard tests/*.c)
TEST_OBJS := $(TESTS:.c=.o)
TEST_DEPS := $(TESTS:.o=.d)
# TEST_EXES := $(TESTS:tests/%.c=bin/%.c)

# tests: $(TEST_EXES)

# bin/%: tests/%.o $(TEST_OBJS) $(filter-out %main.o, $(OBJS)) bin
# 	gcc $^ -o $@ $(LDFLAGS)

bin/scheme-y: $(OBJS) | bin
	gcc $^ -o $@ $(LDFLAGS)

%.d: %.c
	$(CC) -MM $^ -MF $@

bin obj:
	@mkdir -p $@

clean:
	@rm -rf bin
	@rm -f src/*.o src/*.d
	@rm -f test/*.o test/*.d

-include $(DEPS)

# -include $(TEST_DEPS)

# CFLAGS = -ansi -Wpedantic -std=c99 -g
# # CFLAGS += -Wall -Wextra
# LDFLAGS = -lm

# SRCS := $(wildcard src/*.c)
# OBJS := $(SRCS:%.c=%.o)
# TESTS := $(wildcard tests/*.c)

# .PHONY: build rebuild tests clean

# build: bin/scheme-y
# rebuild: clean bin/scheme-y

# bin/scheme-y: $(OBJS) | bin
# 	gcc $^ -o $@ $(LDFLAGS)

# tests: $(TESTS:tests/%.c=bin/%)
# 	for f in $^; do ./$$f; done

# bin/%: tests/%.o $(filter-out %main.o, $(OBJS)) | bin
# 	gcc $^ -o $@

# %.o: %.c
# 	gcc -c $(CFLAGS) $< -o $@

# bin:
# 	mkdir -p $@

# clean:
# 	rm -rf bin
# 	rm -f src/*.o
# 	rm -f tests/*.o
