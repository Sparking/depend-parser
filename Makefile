CC := gcc

CPPFLAGS := -Wall -Werror -std=gnu99 -MMD -Iinclude -D_GNU_SOURCE
CFLAGS   := -g -O0 -shared -fPIC
LDFLAGS  :=
LIBS     :=

EXEC_SUFFIX := elf
DYNL_SUFFIX := so
ifeq ($(OS), Windows_NT)
EXEC_SUFFIX := exe
DYNL_SUFFIX := dll
RM          := del
endif

src  := parser.c rbtree.c
objs := $(patsubst %.c,%.o,$(src))
deps := $(patsubst %.c,%.d,$(src))

out  := libdepend-parser.$(DYNL_SUFFIX)

.PHONY: all
all: run.$(EXEC_SUFFIX)

$(out): $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

-include $(deps)

$(objs): %.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	$(RM) $(objs)
	$(RM) $(deps)
	$(RM) $(out)
	@$(RM) run.d
	@$(RM) run.$(EXEC_SUFFIX)

run.$(EXEC_SUFFIX): demo.c $(out)
	$(CC) $(CPPFLAGS) -g -O0 -Wl,-rpath=. -o $@ $< -L. -ldepend-parser $(LIBS)
