.PHONY : clean

CPP = g++
CPPFLAGS = -g -Wall -pedantic-errors
INCLUDE = -I../../include
LIBRARIES =

test.out : stmt_string_generator_test.cpp\
	../../src/stmt_string_generator/stmt_string_generator.cpp
	${CPP} ${CPPFLAGS} ${INCLUDE} $^ -o $@  ${LIBRARIES}

clean :	
	rm -f *.o
	rm -f *.out
	rm -f *.d
