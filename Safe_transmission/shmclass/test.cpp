#include <iostream>
#include "shmClass.h"
using namespace std;
int main()
{
    shmClass shm(1,1024);
    void* head = shm.getshm();
    shm.unshm();
    shm.deshm();

    cout << "Hello world" << endl;
    return 0;
}

