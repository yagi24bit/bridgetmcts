#ifndef __xorshift_h__
#define __xorshift_h__

class XorShift {
protected:
	static unsigned long x;
	static unsigned long y;
	static unsigned long z;
	static unsigned long w;

public:
	static unsigned long xor128();
};

#endif
