#include "main.h"

void main(void)
{
	volatile float a = 1.0f;
	volatile float b = 0.0f;

	while(1)
	{
		b += a;
	}
}

void memset(int *dst, int c, int size)
{
	while(size--) *(dst++) = c;
}
void memcpy(int *dst, int *src, int size)
{
	while(size--) *(dst++) = *(src++);
}
