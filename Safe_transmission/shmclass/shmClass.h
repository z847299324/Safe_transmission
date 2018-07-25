#pragma once

class shmClass
{
public:
    shmClass(int key,int size);
    
    //映射到共享内存
    void* getshm();
    //离开映射区
    int unshm();
    //销毁共享内存
    int deshm();

    ~shmClass();

private:
    int shmid;
    void* shm;
};

