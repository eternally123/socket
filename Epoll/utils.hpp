#include <stdio.h>
#include <unistd.h>
#include <errno.h>

ssize_t readn(int fd,void *buff,size_t n);
ssize_t writen(int fd,const void *buff,size_t n);
ssize_t readline(int fd,void *buff,size_t maxLength);


ssize_t readn(int fd,void *buff,size_t n){
    size_t nLeft;
    ssize_t nRead;
    char *ptr;
    
    ptr = (char *)buff;
    nLeft = n;
    while(nLeft>0){
        if((nRead = read(fd,ptr,nLeft))<0){
            if(errno == EINTR)
                nRead = 0;
            else 
                return -1;
        }else if(nRead == 0){
            break; //EOF
        }

        nLeft -= nRead;
        ptr += nRead;
    }
    return (n-nLeft);
}


ssize_t writen(int fd,const void *buff,size_t n){
    size_t nLeft;
    ssize_t nWritten;
    const char *ptr;

    ptr = (char *)buff;
    nLeft = n;
    while(nLeft>0){
        if((nWritten = write(fd,ptr,nLeft))<=0){
            if(nWritten<0 && errno==EINTR)
                nWritten = 0;
            else 
                return (-1);
        }

        nLeft -= nWritten;
        ptr += nWritten;
    }
    return (n);
}



ssize_t readline(int fd,void *buff,size_t maxLength){
    ssize_t n,readCount;
    char c,*ptr;
    ptr = (char*)buff;
    bool target = false;
    for (n=1;n<maxLength;n++){
        readCount = read(fd,&c,1);

        if(readCount == 1){
            *ptr++ = c;
            if(c == '\n'){
                target = true;
                break;//new line is stored
            }
        }else if (readCount == 0){//EOF n-1 bytes were read
            *ptr = 0;
            return (n-1);
        }else {
            if (errno == EINTR){
                n=n-1;
                continue;
            }
            return (-1);//error, errno set by read()
        }
        if(target == true)
            break;
    }
    
    *ptr = 0;
    return (n);

}



