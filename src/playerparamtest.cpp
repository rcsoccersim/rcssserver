#include <iostream>
#include "playerparam.h"
int main()
{
    PlayerParam::instance();
    std::cout << "success\n";
}
