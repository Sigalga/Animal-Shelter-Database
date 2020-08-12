#ifndef ASHS_STMT_STRING_GENERATOR_HPP
#define ASHS_STMT_STRING_GENERATOR_HPP

// Standard C++ includes
#include <string>
#include <map>
#include <utility>

using namespace std;

namespace ashs
{

class StmtStringGenerator
{
public:
    typedef int Key;
    typedef string&(*StringFunc)(const string&, const string&);

    void AddStringFunc(const Key key, StringFunc func);
    void RemoveStringFunc(const Key key);

    string& GenerateString(const Key key, const string& col, const string& val);

private:
    map<Key, StringFunc> stringFuncs;
};

} // namespace ashs

#endif // ASHS_STMT_STRING_GENERATOR_HPP