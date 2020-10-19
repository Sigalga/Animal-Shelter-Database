.PHONY : clean

CPP = g++
CPPFLAGS = -g -Wall -pedantic-errors
INCLUDE = -I../../include -I/usr/include/cppconn
LIBRARIES = -L/usr/lib -lmysqlcppconn

test.out : animal_shelter_system_test.cpp\
	../../src/animal_shelter_system/animal_shelter_system.cpp\
	../../src/pet_book/pet_book.cpp\
	../../src/stmt_string_generator/stmt_string_generator.cpp
	${CPP} ${CPPFLAGS} ${INCLUDE} $^ -o $@  ${LIBRARIES}

clean :	
	rm -f *.o
	rm -f *.out
	rm -f *.d
