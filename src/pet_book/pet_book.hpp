#ifndef ASHS_PET_BOOK_HPP
#define ASHS_PET_BOOK_HPP

// MySQL/C++ Connector includes
#include <cppconn/connection.h>     // sql::Connection

// ASHS includes
#include "stmt_string_generator.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

class PetBook
{
public:
    PetBook() : con(NULL), stringGen(NULL) {}
    PetBook(Connection* con, StmtStringGenerator* stringGen);

    void Start();

private:
    Connection* con;
    StmtStringGenerator* stringGen;

    void DisplayMenu();
    void ExecuteRequests();
    void DisplayResults();
};

} // namespace ashs

#endif // ASHS_PET_BOOK_HPP