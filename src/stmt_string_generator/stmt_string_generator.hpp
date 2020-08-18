#ifndef ASHS_STMT_STRING_GENERATOR_HPP
#define ASHS_STMT_STRING_GENERATOR_HPP

// Standard C++ includes
#include <string>
#include <map>
#include <vector>

#include <boost/function.hpp>		// boost::function

using namespace std;

namespace ashs
{

class StmtStringGenerator
{
public:
    typedef string Key;
    // typedef string&(*StringFunc)(const string&, const string&);
    typedef boost::function<string& (const string&, const string&, const string&)> StringFunc; //
    typedef map<Key, StringFunc> FuncMap;

    void AddStringFunc(const Key key, StringFunc func);
    // void AddStringFunc(const string& str, StringFunc func);
    void AddStringFunc(const char* cStr, StringFunc func);

    void RemoveStringFunc(const Key key);

    // Returns a vector of all stringFuncs keys
    vector<Key>* GetKeys();

    string& GenerateString(const Key key, const string& col, const string& val1, const string& val2);

private:
    FuncMap stringFuncs;
};

} // namespace ashs

#endif // ASHS_STMT_STRING_GENERATOR_HPP