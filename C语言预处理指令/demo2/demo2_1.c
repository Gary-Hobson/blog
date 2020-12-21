#include <stdio.h>
#define LOG_INT(x) printf(#x " is :%d\n",x)

int main(void)
{
    int a=10;
    
    LOG_INT(a);
}