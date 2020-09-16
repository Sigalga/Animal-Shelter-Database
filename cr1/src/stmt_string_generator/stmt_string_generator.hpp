#ifndef ASHS_STMT_STRING_GENERATOR_HPP
#define ASHS_STMT_STRING_GENERATOR_HPP

// Standard C++ includes
#include <string>
#include <map>
#include <vector>

#include <functional>		    // std::function
#include <boost/function.hpp>   // boost::function

namespace ashs
{

class StmtStringGenerator
{
public:
    using Key =         std::string;
    using StringFunc =  std::function<const std::string&()>;
    using FuncMap =     std::map<Key, StringFunc>;

    void AddStringFunc(const Key key, StringFunc func);
    void AddStringFunc(const char* cStr, StringFunc func);

    void RemoveStringFunc(const Key key);

    // Returns a vector of all stringFuncs keys
    std::vector<Key> GetKeys();

    const std::string& GenerateString(const Key key);

    void AtExit();

private:
    FuncMap stringFuncs;
};

} // namespace ashs

#endif // ASHS_STMT_STRING_GENERATOR_HPP