#include <iostream>
#include "serverparam.h"
int main( int argc, char ** argv )
{
    ServerParam::init( argc, argv );
    ServerParam::instance();
    std::cout << "success\n";
}
