#include <iostream>
#include "serverparam.h"
int main()
{
    ServerParam::instance( "serverparamtest" );
    std::cout << "success\n";
}
