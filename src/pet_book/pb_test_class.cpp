#include "pb_test_class.hpp"

namespace ashs
{

void PbTestClass::ExecutInputTest()
{
    instance->isRunning = true;
    instance->ExecuteInput();
}

void PbTestClass::MakeStringTest()
{
    instance->MakeString();
}

size_t PbTestClass::StringFuncsTest()
{
    size_t errors = 0;

    instance->currId = "2";
    if ("SELECT * FROM pets WHERE pet_id=2" != instance->FindByCurrId())
    {
        ++errors;
    }

    if ("SELECT * FROM pets WHERE pet_id = (SELECT MAX(pet_id) FROM pets)" !=
            instance->FindByMaxId())
    {
        ++errors;
    }

    cout << instance->FindJoined();
    // if ( != instance->FindJoined())
    // {
    //     ++errors;
    // }

    return errors;
}

} // namespace ashs