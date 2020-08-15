.PHONY : clean

CPP = g++
CPPFLAGS = -g -Wall -pedantic-errors
INCLUDE = -I../../include -I/usr/include/cppconn
LIBRARIES = -L/usr/lib -lmysqlcppconn

test.out : animal_shelter_system_test.cpp animal_shelter_system.cpp\
	../pet_book/pet_book_string_funcs.cpp\
	../pet_book/pet_book.cpp\
	../stmt_string_generator/stmt_string_generator.cpp
	${CPP} ${CPPFLAGS} ${INCLUDE} $^ -o $@  ${LIBRARIES}

clean :	
	rm -f *.o
	rm -f *.out
	rm -f *.d