#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shmClass.h"
shmClass:: shmClass(int key,int size)
{
    this->shmid = shmget(key,size,IPC_CREAT|IPC_EXCL|0666);
    {
        if(this->shmid==-1)
        {
            this->shmid = shmget(key,0,0);
        }
    }
}
//映射到共享内存
void* shmClass:: getshm()
{
    this->shm = shmat(this->shmid,0,0);
    if(*(int*)this->shm==-1)
    {
        return (void*)-1;
    }
    return shm;
}
//离开映射区
int shmClass:: unshm()
{
    int ret = shmdt(this->shm);
    if(ret == -1)
    {
        return -1;
    }
    return 0;
}
//销毁共享内存
int shmClass:: deshm()
{
    int ret = shmctl(this->shmid,IPC_RMID,0);
    if(ret == -1)
    {
        return -1; 
    }
    return 0;
}



shmClass:: ~shmClass() {}


