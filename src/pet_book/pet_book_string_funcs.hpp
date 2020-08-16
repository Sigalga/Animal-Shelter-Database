#ifndef ASHS_PET_BOOK_STRING_FUNCS_HPP
#define ASHS_PET_BOOK_STRING_FUNCS_HPP

// Standard C++ includes
#include <string>

using namespace std;

namespace ashs
{

// queries
string& FindBy(const string& col, const string& val, const string& val2="");

// TODO:

// string& OrderBy(const string&, const string&);

// edit operations
string& UpdateField(const string& col, const string& val, const string& id);
// string& AddEntry(const string&, const string&);
// string& RemoveEntry(const string&, const string&);

} // namespace ashs

#endif // ASHS_PET_BOOK_STRING_FUNCS_HPPs