#include <stdio.h>
#include "fadd.h"

#define BIN_SIZE 33

/*******************************************************************************/
/**                   FOR C IMPLEMENTATION TEST                               **/
void num2bin(long f3);
void addff(float, float, float *);
unsigned char bin[BIN_SIZE];
/*******************************************************************************/
void add_c(float a, float b, float *x) {
	*x = a+b;
}

void evaluate_add(float a, float b) {
	float x;
	float y;

	add_c(a, b, &x);
	addf(a, b, &y);
	printf("add(%f, %f) returned %.8f", a, b, y);
	if (x == y)
		printf(" - correct!\n");
	else
		printf(" - incorrect, should return %.8f\n", x);
}

void addff(float a, float b, float *x)
{
	unsigned char ha[4], hb[4];
	long e1, e2, e3, f1, f2, f3, s3, full1, full2, i;

	/* Little-endian */
	if (a != 0) {
		ha[3] = ((unsigned char *)&a)[0];
		ha[2] = ((unsigned char *)&a)[1];
		ha[1] = ((unsigned char *)&a)[2];
		ha[0] = ((unsigned char *)&a)[3];
		full1 = (ha[0] << 24) | (ha[1] << 16) | (ha[2] << 8) | (ha[3]);
		e1 = ((full1 & 0x7F800000) >> 23) - 127;
		f1 = ((full1 & 0x7FFFFF)) + 8388608;  /* 2^13 */
	} else {
		e1 = 0;
		f1 = 0;
	}

	if (b != 0) {
		hb[3] = ((unsigned char *)&b)[0];
		hb[2] = ((unsigned char *)&b)[1];
		hb[1] = ((unsigned char *)&b)[2];
		hb[0] = ((unsigned char *)&b)[3];

		full2 = (hb[0] << 24) | (hb[1] << 16) | (hb[2] << 8) | (hb[3]);
		e2 = ((full2 & 0x7F800000) >> 23) - 127;
		f2 = ((full2 & 0x7FFFFF)) + 8388608;/* 2^13 */
	} else {
		e2 = 0;
		f2 = 0;
	}

	while (e1 < e2) {
		e1 = e1 + 1;
		f1 = f1 >> 1;
	}

	while (e2 < e1) {
		e2 = e2 + 1;
		f2 = f2 >> 1;
	}
	e3 = e1;

	f1 = (a < 0) ? -f1 : f1;
	f2 = (b < 0) ? -f2 : f2;
	f3 = f1 + f2;

	/* i will use 1 and -1 for + & - sign of the number instead of 0 & 1 */
	if (f3 < 0) {
		f3 = -f3;
		s3 = -1;
	} else {
		s3 = 1;
	}
	/* make a binary string for the fractional part */
	num2bin(f3);
	if (f3) {
		for (i = 0; i < 9; i++) {
			if (bin[i] == '1') {
				f3 = f3 >> 1;
				e3 = e3 + 1;
				num2bin(f3);
			}
		}
		while (bin[7] != '1') {
			f3 = f3 << 1;
			e3 = e3 - 1;
			num2bin(f3);
		}
	}
	*x = 0;
	for (i = 0; i < BIN_SIZE; i++) {
		*x = (bin[i] == '1') ? *x + 1.f/(1 << i) : *x;
	}
	*x = *x * (1 << (e3+8)) * s3;
}

void num2bin(long f3)
{
	unsigned long i, z;
	z = 0x80000000;
	for (i = 0; i < BIN_SIZE; i++) {
		bin[i] = ((f3 & z)) ? '1' : '0';
		z = z >> 1;
	}
	bin[i] = '\0';
}

int main()
{
	float a, b, x;
	unsigned int i;
	FILE *f;
	if ((f = fopen("../test/test_addf.txt", "r"))) {
		for(i = 0; i < HEIGHT; i++)  {
			fscanf(f,"%f",&a);
			fscanf(f,"%f",&b);
			addf(a, b, &x);
			printf("%.8f,%.8f,%.8f,%.8f\n", a, b, x, a + b);
		}
		fclose(f);
	}
	/*
	printf("***** Results for add *****\n");
	evaluate_add(0, 0);
	evaluate_add(1.0, 0);
	evaluate_add(0, 10);
	evaluate_add(-1.0, 1.0);
	evaluate_add(-1.0, -1.0);
	evaluate_add(3.7, 4.1);
	evaluate_add(1.5, 1.25);
	evaluate_add(10.6, 10.6);
	evaluate_add(5.321321, 54354);
	evaluate_add(-1, 5);
	evaluate_add(1.0, -2.0);
	evaluate_add(1.0, -2.5);
	evaluate_add(-1.0, 0.5);
	evaluate_add(20.0, -22.25);
	evaluate_add(133.6, 4.5);
	evaluate_add(1.6, 5.5);
	evaluate_add(1.1, 3.3);
	evaluate_add(12.1, 22.2323);
	evaluate_add(-133.6, 4.5);
	evaluate_add(-30.89, -40.21);
	evaluate_add(-12.984580, 102021.120);
	evaluate_add(-210.1245, -430.2112);
	evaluate_add(23.1050, 43.5694500);
	evaluate_add(100.115140, -232.21123);
	evaluate_add(123.367, 123.367);
	evaluate_add(0.1, 0.2);
	evaluate_add(123, 123.456);
	evaluate_add(0.456, 60.21);
	evaluate_add(1.34, 7.98);
	evaluate_add(0.35, 22.165);
	evaluate_add(-2323.54, -121.56);
	evaluate_add(3.14, -2.71);
	evaluate_add(27.12, 4.2017);
	evaluate_add(14.50, 18.2121);
	evaluate_add(54.120, -201.60609);
	evaluate_add(138.100006, 7.100000);
	evaluate_add(4.400000, 246.733994);
	evaluate_add(9.320000, -129.100006);
	evaluate_add(-71.099998, -640.335709);
	evaluate_add(0.1, -0.1);
	evaluate_add(0, 0);
	evaluate_add(1.0, 0);
	evaluate_add(-1.0, 1.0);
	evaluate_add(-1.0, -1.0);
	evaluate_add(3.7, 4.1);
	evaluate_add(1.5, 1.25);
	evaluate_add(10.6, 10.6);
	evaluate_add(5.321321, 54354);
	evaluate_add(1000, 10000);
	evaluate_add(5.321321, -5.321321);
	evaluate_add(5.321321, 5.321321);
	evaluate_add(-71.099998, 640.335709);
	evaluate_add(71.099998, 640.335709);
	evaluate_add(2323.54, 121.56);
	*/
	return EXIT_SUCCESS;
}
