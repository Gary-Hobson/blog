#include "application.h"
#include "stdio.h"

void setup(void)
{
	printf("system init...\n");

#if defined(__MICROLIB)
	printf("armcc version :%d\n", __ARMCC_VERSION);
#elif defined(__GNUC__)
	printf("arm gcc version :%s\n", __VERSION__);
#endif
}

void loop(void)
{
	static int cnt = 0;
	printf("run cnt%d\n", cnt++);
	delay(1000);
}
