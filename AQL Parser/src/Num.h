#ifndef NUM_H_
#define NUM_H_

#include "Token.h"

class Num: public Token {
public:
	const int value;
	Num(int v);
	virtual ~Num();
	virtual void print();
};

#endif /* NUM_H_ */
