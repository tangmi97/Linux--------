#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include<fcntl.h>

#define err(cmd,temp)\
    if (temp<0)      \
    {                \
        perror(cmd); \
        return -1;   \
    }                \

#define SOCK_IP "0.0.0.0"
#define SOCK_DC 8888

typedef struct 
{
    int type;       // 1 上传  2 下载
    int len;        // 文件大小
    char filename[64];
}tools;


void *pthread_fwd(void *arg)
{
    int fd=*(int *)arg;
    ssize_t count,n;
    tools h;
    char buf[1024];
    count=recv(fd,&h,sizeof(tools),0);
    printf("[线程] 客户端请求：type=%d, file=%s, size=%d\n",h.type, h.filename, h.len);
    if(h.type==1)
    {
        int file_fd=open(h.filename,O_WRONLY|O_CREAT,0666);
        int shu=0;
        while(shu<h.len)
        {
            n=recv(fd,buf,1024,0);
            write(file_fd,buf,n);
            shu+=n;
        }
        close(file_fd);
    }
    else if(h.type==2)
    {
        int file_fd=open(h.filename,O_RDONLY,0755);
        if(fd>0)
        {
            send(fd,&file_fd,sizeof(int),0);   
            while((n=read(file_fd,buf,1024))>0)
            {
                send(fd,buf,n,0);
            }
            close(file_fd);
        }
        else
        {
            send(fd,&file_fd,sizeof(int),0); 
            printf("要下载的文件不存在\n");
        }
    }
    else
    {
        printf("输入指令有误！\n");
    }
    close(fd);
    return NULL;
}

int main(int argc,char *argv)
{
    struct sockaddr_in fwd;
    struct sockaddr_in khd;
    memset(&fwd,0,sizeof(fwd));
    memset(&khd,0,sizeof(khd));
    inet_aton(SOCK_IP,&fwd.sin_addr);
    fwd.sin_port=htons(SOCK_DC);
    fwd.sin_family=AF_INET;

    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    err("socket",sockfd);
    int temp_bind=bind(sockfd,(struct sockaddr *)&fwd,sizeof(fwd));
    err("bind",temp_bind);
    int temp_listen=listen(sockfd,1024);
    err("listen",temp_listen);
    printf("等待连接中～\n");
    while(1)
    {
        socklen_t khd_len=sizeof(khd);
        int fd=accept(sockfd,(struct sockaddr *)&khd,&khd_len);
        err("accept",fd);
        printf("连接客户端%s %d,标识符%d\n",inet_ntoa(khd.sin_addr),ntohs(khd.sin_port),fd);
        pthread_t pthread_pid;
        pthread_create(&pthread_pid,NULL,pthread_fwd,(void *)&fd);
        // err("pthread_create",temp_pthread);
        pthread_detach(pthread_pid);
    }
    close(sockfd);
    return 0;
}
    