#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
typedef unsigned char* bytepoint;
void show_bytes(bytepoint b, size_t size);
_Bool is_little_endian(void);
unsigned replace_byte(unsigned x, int i, unsigned char b);
int any_odd_one(unsigned x);
int lower_one_mask(int n);
unsigned rotate_left(unsigned x, int n);
int saturating_add(int x, int y);
int tsub_ok(int x, int y);
int divide_power2(int x, int k);
int mul3divi4(int x);
int main(void)
{
	//unsigned a = 0xffffffff;
	//unsigned b = replace_byte(a, 2, 0x56);
	//show_bytes((bytepoint)&b, sizeof a);
	//printf("%d\n", is_little_endian());
	//unsigned temp = rotate_left(0x12345678, 0);
	//show_bytes((bytepoint)&temp, sizeof temp);
	printf("%d\n", mul3divi4(1 << 30));
	system("pause");
	return 0;
}
void show_bytes(bytepoint b, size_t size)
{
	for (size_t i = 0; i < size; i++)
		printf("%.2x ", *(b + i));
	putchar('\n');
}

_Bool is_little_endian(void)
{
	int a = INT_MAX;
	bytepoint p = (bytepoint)&a;
	return !('0x7f' == *p);
	//int a = 1;
	//return *(char*)(&a);���˽�
}

unsigned replace_byte(unsigned x, int i, unsigned char b)
{
	bytepoint p = (bytepoint)&x;
	p = p + sizeof(unsigned) - i;
	*p = (*p) & 0x00;
	*p = (*p) | b;
	return x;
}

int any_odd_one(unsigned x)
{
	return !!(x&0x55555555);
}

int lower_one_mask(int n)
{
	int w = sizeof(int) << 3;
	return (unsigned)-1 >> (w - n);
}

unsigned rotate_left(unsigned x, int n)
{
	int w = sizeof(int) << 3;
	unsigned long long m = (unsigned long long)x << n;
	x = x << n;
	m = m >> w;
	return x | m;
}

int saturating_add(int x, int y)
{
	int sum = x + y;
	int pos_over = x > 0 && y > 0 && sum < 0;//!(x&INT_MAX)&&!(y&INT_MAX)&&(sum&INT_MAX);
	int neg_over = x < 0 && y < 0 && sum >= 0;//(x&INT_MAX)&&(y&INT_MAX)&&!(sum&INT_MAX);
	(pos_over && (sum = INT_MAX)) ||
		(neg_over && (sum = INT_MIN));
	return sum;
}

int tsub_ok(int x, int y)
{
	int sum = x - y;
	int pos_over = x ^ INT_MIN && y&INT_MIN && sum&INT_MIN;
	int neg_over = x & INT_MIN &&y^INT_MIN&&sum^INT_MIN;
	return !(pos_over || neg_over);
}

int divide_power2(int x, int k)
{
	return (x + (1 << k) - 1) >> k;
}

int mul3divi4(int x)
{
	x = x << 1 + x;
	return divide_power2(x, 2);
}