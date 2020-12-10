#include <stdio.h>

char hello[]={"hello gcc"};

int main(void)
{
    /*打印字符数组，显示字符串位置和大小*/
    printf("str:%s,ptr:%p,size:%d\n",hello,hello,sizeof(hello));

    return 0;
}