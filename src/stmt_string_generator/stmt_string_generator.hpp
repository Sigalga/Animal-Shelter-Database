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
    typedef string                      Key;
    typedef boost::function<string&()>  StringFunc;
    typedef map<Key, StringFunc>        FuncMap;
    
    
    void AddStringFunc(const Key key, StringFunc func);
    void AddStringFunc(const char* cStr, StringFunc func);

    void RemoveStringFunc(const Key key);

    // Returns a vector of all stringFuncs keys
    vector<Key>* GetKeys();

    string& GenerateString(const Key key);

    void AtExit();

private:
    FuncMap stringFuncs;
};

} // namespace ashs

#endif // ASHS_STMT_STRING_GENERATOR_HPP