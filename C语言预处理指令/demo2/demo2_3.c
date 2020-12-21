#include <stdio.h>

#define LOG(format,...) printf("::log > " format"\n",__VA_ARGS__)

int main(void)
{
    for(int i=0; i<3;i++)
    {
        LOG("run cnt :%d",i);
    }
}
