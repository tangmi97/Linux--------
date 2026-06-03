// 运行该客户端.c文件生成的可执行文件，并可以将所处文件夹里的文件上传到服务端，也可以从服务端下载文件。
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

void  shangchuan(int *sockfd,char *filename);//上传函数
void  xiazai(int *sockfd,char *filename);    //下载函数

#define err(cmd,temp)\
    if (temp<0)      \
    {                \
        perror(cmd); \
        return -1;   \
    }                \

#define SOCK_IP "0.0.0.0"//服务端ip地址
#define SOCK_DC 8888     //服务端端口号

typedef struct 
{
    int type;       //模式： 1 上传  2 下载
    int len;        // 文件大小
    char filename[64];//文件名
}tools;

int main(int atgc,char *argv[])
{
    struct sockaddr_in fwd;
    struct sockaddr_in khd;
    memset(&fwd,0,sizeof(fwd));
    memset(&khd,0,sizeof(khd));
    fwd.sin_family=AF_INET;
    inet_aton(SOCK_IP,&fwd.sin_addr);
    fwd.sin_port=htons(SOCK_DC);

    khd.sin_family=AF_INET;
    khd.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    khd.sin_port=htons(6666);

    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    err("socket",sockfd);
    int temp_connect=connect(sockfd,(struct sockaddr *)&fwd,sizeof(fwd));
    err("connect",temp_connect);
    printf("===== 文件传输客户端 =====\n");
    printf("连接服务端%s %d,标识符%d\n",inet_ntoa(fwd.sin_addr),ntohs(fwd.sin_port),sockfd);
    printf("命令格式：1 文件名(上传)  或  2 文件名(下载)\n");
    printf("示例：1 test.txt  或  2 test.txt\n");
    tools h;
    fh:scanf("%d %s",&h.type,h.filename);
    if(h.type==1)
        shangchuan(&sockfd,h.filename);
    else if(h.type==2)
        xiazai(&sockfd,h.filename);
    else
    {
        printf("选项有误\n");
    }
    close(sockfd);
    return 0;
}

void shangchuan(int *sockfd,char *filename)
{

    int fd=open(filename,O_RDONLY,0755);
    if(fd<0)
    {
        perror("open");
        return;
    }
    tools f;
    f.type=1;
    int size = lseek(fd, 0, SEEK_END),n,file_size;
    lseek(fd, 0, SEEK_SET);
    f.len=size;
    strcpy(f.filename,filename);
    send(*sockfd,&f,sizeof(f),0);
    char buf[1024];
    while ((n = read(fd,buf,1024)) > 0)
    {
        send(*sockfd, buf, n, 0);
        file_size+=n;
    }
    close(fd);
    printf("[上传成功！]%s(%dbytes)\n",f.filename,file_size);
}
void xiazai(int *sockfd,char *filename)
{
    tools f;
    int n,file_size,flag;
    f.len=0;
    f.type = 2;
    strcpy(f.filename, filename);
    send(*sockfd,&f,sizeof(f),0);
    recv(*sockfd,&flag,sizeof(int),0);
    if(flag<0)
    {
        printf("%s文件不存在！\n",f.filename);
        return;
    }
    int fd=open(filename,O_WRONLY|O_CREAT,0666);
    char buf[1024];
    while((n=recv(*sockfd,buf,1024,0))>0)
    {
        write(fd,buf,n);
        file_size+=n;
    }
    close(fd);
    printf("[下载成功！]%s(%dbytes)\n",f.filename,file_size);
}
