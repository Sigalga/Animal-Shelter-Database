# animal-shelter-system

This system is designed to run on Ubuntu 18.0.

To compile the program:
1. Make sure to have the mysqlcppconn library installed.
2. Type the command ‘make’ in the terminal, from within current directory.
    To reverse the build, type ‘make clean’.

To run the animal shelter system:
1. On first run, create the PetBook database, by running the sql script at ./src/sql/create-database.sql. Currently, the database should be placed in a local SQL server with the address tcp://127.0.0.1:3306 and password ‘root’.
For testing, a PetBook database creating script with pre-filled fake entries is located at ./test/sql/create-database.sql.
2. Execute the file ./ashes.out
