#ifndef WORD_H_
#define WORD_H_

#include "Token.h"

class Word: public Token {
public:
	const string lexeme;   // 存放字符串的值
	Word(string l, int tag);
	virtual ~Word();
	virtual void print();
};

#endif /* WORD_H_ */
