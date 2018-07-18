#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main()
{
    int cfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in seve;
    seve.sin_family = AF_INET;
    seve.sin_port = 8888;
    inet_aton("127.0.0.1",&seve.sin_addr);

    connect(cfd,(struct sockaddr*)&seve,sizeof(seve));
    send(cfd,"111",4,0);
    return 0;
}

