.PHONY : clean

CPP = g++
CPPFLAGS = -g -Wall -pedantic-errors
INCLUDE = -I../../include -I/usr/include/cppconn
LIBRARIES = -L/usr/lib -lmysqlcppconn

test.out : pet_book_test.cpp \
	pb_test_class.cpp \
	pet_book.cpp \
	../stmt_string_generator/stmt_string_generator.cpp
	${CPP} ${CPPFLAGS} ${INCLUDE} $^ -o $@  ${LIBRARIES}

clean :	
	rm -f *.o
	rm -f *.out
	rm -f *.d