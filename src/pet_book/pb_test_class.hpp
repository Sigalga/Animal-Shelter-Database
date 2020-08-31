#ifndef ASHS_PB_TEST_CLASS_HPP
#define ASHS_PB_TEST_CLASS_HPP

#include "pet_book.hpp"

namespace ashs
{

class PbTestClass
{
public:
    PbTestClass(PetBook* petBook) : instance(petBook) {}

    void ExecutInputTest();
    void MakeStringTest();
    size_t StringFuncsTest();

private:
    PetBook* instance;

    // stringFuncs
    size_t FindByCurrIdTest();
    size_t FindByMaxIdTest();
    size_t GetAllTest();
    size_t FindByTest();
    size_t FilterByTest();
};

} // namespace ashs

#endif // ASHS_PB_TEST_CLASS_HPP