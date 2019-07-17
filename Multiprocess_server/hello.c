#include <stdio.h>
#include <string.h>

int main()
{
    FILE *fp = fopen("a.txt","r");
    char buff[1024];
    fgets(buff,1024,fp);
    
    
    printf("size:%d\n",strlen(buff));
    buff[6]='\0';
    printf("size:%d\n",strlen(buff));
    for(int i=0;i<10;i++)
        printf("%d ",buff[i]);
    return 0;
}

