UNAME_S := $(shell uname -s)

CXX              := clang++
# Add -fsanitize=undefined for UBSAN.
COMPILER_OPTIONS := \
	-std=c++1z -stdlib=libc++ -O3 -g                           \
	-fPIC -fexceptions -ferror-limit=1 -fno-omit-frame-pointer \
	-Wall -Wpedantic                                           \
	-DGIPFELI_NO_UNALIGNED                                     \
	-DNDEBUG -Iinclude

TEST_TRANSLATION_UNITS    := $(wildcard */*_test.cc)
LIBRARY_TRANSLATION_UNITS := $(filter-out $(TEST_TRANSLATION_UNITS), $(wildcard src/*.cc))
TEST_OBJECTS              := $(TEST_TRANSLATION_UNITS:.cc=.o)
LIBRARY_OBJECTS           := $(LIBRARY_TRANSLATION_UNITS:.cc=.o)

ifeq ($(UNAME_S),Darwin)
    COMPILER_OPTIONS += -mmacosx-version-min=10.11 -arch x86_64
endif

all: library

library: $(LIBRARY_OBJECTS)
	ar -rcs libgipfeli.a $^

clean:
	rm -f src/*.o *.a gipfeli_test

test: $(TEST_OBJECTS) library
	$(CXX) -o gipfeli_test $(TEST_OBJECTS) $(COMPILER_OPTIONS) -L. -lgipfeli -lsupc++ -lc++


%.o: %.cc
	$(CXX) -c -o $@ $< $(COMPILER_OPTIONS)
