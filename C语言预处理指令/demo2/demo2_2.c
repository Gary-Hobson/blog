#include <stdio.h>

#define VAR(x)  var##x 
#define LOG_INT(x) printf("var"#x " is :%d\n",var##x)

int main(void)
{
    int VAR(a1) =1;
    int VAR(a2) =10;

    LOG_INT(a1);    
}
