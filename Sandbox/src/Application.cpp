#include <iostream>
#include <stdlib.h>

namespace Paradox {

    __declspec(dllimport) void Print();

}

int main()
{
    Paradox::Print();
    return 0;
}