#include <iostream>
#include "clientop.h"
using namespace std;
int main()
{
    clientop cli;
    cli.keyMngArrange();
    char*key = cli.getKey();
    cout<<key<<endl;
    //std::cout << "Hello world" << std::endl;
    return 0;
}

