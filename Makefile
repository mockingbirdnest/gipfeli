CXX              := clang++
COMPILER_OPTIONS := \
	-std=c++1z -stdlib=libc++ -O3 -g                           \
	-fPIC -fexceptions -ferror-limit=1 -fno-omit-frame-pointer \
	-Wall -Wpedantic                                           \
	-DNDEBUG -Iinclude
	
TEST_TRANSLATION_UNITS    := $(wildcard */*_test.cc)
LIBRARY_TRANSLATION_UNITS := $(filter-out $(TEST_TRANSLATION_UNITS), $(wildcard src/*.cc))
TEST_OBJECTS              := $(TEST_TRANSLATION_UNITS:.cc=.o)
LIBRARY_OBJECTS           := $(LIBRARY_TRANSLATION_UNITS:.cc=.o)

all: $(LIBRARY_OBJECTS)
	ar -rcs libgipfeli.a $^

clean:
	rm -f src/*.o

%.o: %.cc
	$(CXX) -c -o $@ $< $(COMPILER_OPTIONS)
