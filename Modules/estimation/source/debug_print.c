#define PRECISION 7
#define DO_ROUNDING
// #define NO_TRAIL_NULL
/* PRECISION_IS_SIGNIFICANT whithout NO_TRAIL_NULL is not supported */
// #define PRECISION_IS_SIGNIFICANT
#include <string.h>
#include <limits.h>
#include "comlink.h"

typedef union {
	int L;
	float F;
} LF_t;
int itoa_uint64( char *s, uint64_t snum )
{
	char *ps = s;
	uint64_t num1 = snum, num2, num3, num4, num5, div;

	if (num1 < 10000) {
		if (num1 < 10) goto L1;
		if (num1 < 100) goto L2;
		if (num1 < 1000) goto L3;
	} else {
		num2 = num1 / 10000;
		num1 -= num2 * 10000;
		if (num2 < 10000) {
			if (num2 < 10) goto L5;
			if (num2 < 100) goto L6;
			if (num2 < 1000) goto L7;
		} else {
			num3 = num2 / 10000;
			num2 -= num3 * 10000;
			if (num3 < 10000) {
				if (num3 < 10) goto L9;
				if (num3 < 100) goto L10;
				if (num3 < 1000) goto L11;
			} else {
				num4 = num3 / 10000;
				num3 -= num4 * 10000;
				if (num4 < 10000) {
					if (num4 < 10) goto L13;
					if (num4 < 100) goto L14;
					if (num4 < 1000) goto L15;
				} else {
					num5 = num4 / 10000;
					num4 -= num5 * 10000;
					if (num5 < 10000) {
						if (num5 < 10) goto L17;
						if (num5 < 100) goto L18;
						if (num5 < 1000) goto L19;
					}

					*ps++ = '0' + (char)(div = (num5*8389UL)>>23);
					num5 -= div*1000;
L19:
					*ps++ = '0' + (char)(div = (num5*5243UL)>>19);
					num5 -= div*100;
L18:
					*ps++ = '0' + (char)(div = (num5*6554UL)>>16);
					num5 -= div*10;
L17:
					*ps++ = '0' + (char)(num5);
				}
				*ps++ = '0' + (char)(div = (num4*8389UL)>>23);
				num4 -= div*1000;
L15:
				*ps++ = '0' + (char)(div = (num4*5243UL)>>19);
				num4 -= div*100;
L14:
				*ps++ = '0' + (char)(div = (num4*6554UL)>>16);
				num4 -= div*10;
L13:
				*ps++ = '0' + (char)(num4);
			}
			*ps++ = '0' + (char)(div = (num3*8389UL)>>23);
			num3 -= div*1000;
L11:
			*ps++ = '0' + (char)(div = (num3*5243UL)>>19);
			num3 -= div*100;
L10:
			*ps++ = '0' + (char)(div = (num3*6554UL)>>16);
			num3 -= div*10;
L9:
			*ps++ = '0' + (char)(num3);
		}
		*ps++ = '0' + (char)(div = (num2*8389UL)>>23);
		num2 -= div*1000;
L7:
		*ps++ = '0' + (char)(div = (num2*5243UL)>>19);
		num2 -= div*100;
L6:
		*ps++ = '0' + (char)(div = (num2*6554UL)>>16);
		num2 -= div*10;
L5:
		*ps++ = '0' + (char)(num2);
    }
	*ps++ = '0' + (char)(div = (num1*8389UL)>>23);
	num1 -= div*1000;
L3:
	*ps++ = '0' + (char)(div = (num1*5243UL)>>19);
	num1 -= div*100;
L2:
	*ps++ = '0' + (char)(div = (num1*6554UL)>>16);
	num1 -= div*10;
L1:
	*ps++ = '0' + (char)(num1);

	return ps - s;

}

int ftoa( char *outbuf, float f )
{
	uint64_t mantissa, int_part, frac_part;
	int safe_shift;
	uint64_t safe_mask;
	short exp2;
	LF_t x;
	char *p;

	x.F = f;
	p = outbuf;

	exp2 = (unsigned char)(x.L >> 23) - 127;
	mantissa = (x.L & 0xFFFFFF) | 0x800000;
	frac_part = 0;
	int_part = 0;

	if( x.L < 0  ) {
		*p++ = '-';
	}

	/* Our algorithm works only on exponents >= -36 because safe_mask must
	   start with at least 4 zero bits. So we quickly print 0.0 here. (We could
	   do this even for bigger exponents dependently on PRECISION but would be
	   a useless optimization.) BTW the case f == 0.0 is also handled here. */
	if ( exp2 < -36 ) {
#if defined NO_TRAIL_NULL
		*p++ = '0';
#else
		// print 0.000... like "%._f" does
		memset( p, '0', PRECISION + 1 + 1);
		p[1] = '.';
		p += PRECISION + 1 + 1;
#endif
		goto END;
	}

	safe_shift = -(exp2 + 1);
	safe_mask = 0xFFFFFFFFFFFFFFFFULL >>(64 - 24 - safe_shift);

	if (exp2 >= 64) {
		/* |f| >= 2^64 > ULONG_MAX */
		/* NaNs and +-INF are also handled here*/
		int_part = ULONG_MAX;
	} else if (exp2 >= 23) {
		int_part = mantissa << (exp2 - 23);
	} else if (exp2 >= 0) {
		int_part = mantissa >> (23 - exp2);
		frac_part = (mantissa) & safe_mask;
	} else /* if (exp2 < 0) */ {
		frac_part = (mantissa & 0xFFFFFF);
	}

	if (int_part == 0) {
		*p++ = '0';
	} else {
		p += itoa_uint64(p, int_part);
	}

	if (frac_part != 0) {
		*p++ = '.';

		/* print BCD, calculating digits of frac_part (one more digit is needed
		   when rounding, less digits are needed when then precision should be
		   significant*/
		char max = PRECISION;
#ifdef PRECISION_IS_SIGNIFICANT
		int cnt_dig = ((p - outbuf) + (x.L < 0 ? 1 : 0) - 1);
		max -= (cnt_dig < max) ? cnt_dig : 0;
#endif
#if defined DO_ROUNDING
		max++;
#endif
		for (char m = 0; m < max; m++) {
			/* frac_part *= 10; */
			frac_part = (frac_part << 3) + (frac_part << 1);

			*p++ = (frac_part >> (24 + safe_shift)) + '0';
			frac_part &= safe_mask;
		}
#if defined DO_ROUNDING

		int round_int = 0;
		p--;

		if(  *p >= '5' ) {
			char *w = p-1;
			if( *w != '.' ) {
				if( frac_part != 0 || (*w & 1) ) {
					// round up
					while( *w == '9' ) {
						*w-- = '0';
					}
					if( *w != '.' ) {
						*w += 1;
					} else {
						round_int = 1;
					}
				}
			} else {
				if( frac_part != 0 || ( (int_part & 1)) ) {
					round_int = 1;
				}
			}

			if( round_int ) {
				// we have to round up int_part too
				w--;
				while( *w == '9' && w >= outbuf && *w != '-' ) {
					*w-- = '0';
				}
				if( w >= outbuf &&  *w != '-' ) {
					*w += 1;
				} else {
					// int_part has one digit more now
					w++;
					p++;
					memmove( w+1, w, p-w  );
					*w = '1';
				}
			}
		}
#endif
#if defined NO_TRAIL_NULL
		for (; p[-1] == '0'; --p) {
		}
		if( p[-1] == '.' ) {
			p--;
		}
#else
	} else {
		// print _.000... like "%._f" does
		*p++ = '.';
		memset( p, '0', PRECISION );
		p += PRECISION;
#endif
	} /*  if (frac_part != 0) */

END:
	*p = 0;
	return p - outbuf;
}

void print_float(float f)
{
	uint8_t buff[20];
	uint32_t i;

	i = ftoa(buff, f);

	xUSBSendData(buff, i);
	xUSBSendData("\n\r", 2);
}

void print_vector(vector3f_t *v)
{
    uint8_t buff[20];
	uint32_t i;

	i = ftoa(buff, v->x);
	xUSBSendData(buff, i);
	xUSBSendData("\n\r", 2);

	i = ftoa(buff, v->y);
	xUSBSendData(buff, i);
	xUSBSendData("\n\r", 2);

	i = ftoa(buff, v->z);
	xUSBSendData(buff, i);
	xUSBSendData("\n\r\n\r", 4);

}

void print_quaternion(quaternion_t *q)
{
    uint8_t buff[20];
	uint32_t i;

	i = ftoa(buff, q->q0);
	xUSBSendData(buff, i);
	xUSBSendData("\n\r", 2);

	i = ftoa(buff, q->q1);
	xUSBSendData(buff, i);
	xUSBSendData("\n\r", 2);

	i = ftoa(buff, q->q2);
	xUSBSendData(buff, i);
	xUSBSendData("\n\r", 2);

	i = ftoa(buff, q->q3);
	xUSBSendData(buff, i);
	xUSBSendData("\n\r\n\r", 4);
}

void print_matrix(float *a, uint32_t r, uint32_t c)
{
	int i, j, k;
	uint8_t buff[20];

	for (i = 0; i < r; i++)
	{
		for (j = 0; j < c; j++)
		{
			k = ftoa(buff, As(i, j, c));

			xUSBSendData(buff, k);
			xUSBSendData("    ", 4);
		}

		xUSBSendData("\n\r", 2);
	}

	xUSBSendData("\n\r", 2);
}