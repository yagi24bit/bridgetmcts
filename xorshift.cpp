#include "xorshift.h"

unsigned long XorShift::x = 123456789;
unsigned long XorShift::y = 362436069;
unsigned long XorShift::z = 521288629;
unsigned long XorShift::w = 88675123;

unsigned long XorShift::xor128() {
	unsigned long t = x ^ (x << 11);
	x = y; y = z; z = w;
	return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}
