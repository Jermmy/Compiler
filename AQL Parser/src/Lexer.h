#ifndef LEXER_H_
#define LEXER_H_

#include <map>
#include <fstream>
#include "Token.h"

using namespace std;

class Lexer {
private:
	char peek;
	map<string, Token*> tokens;
	vector<Token*> garbages;
	bool isNum(char c);
	bool isLetter(char c);
	ifstream fin;
public:
	static int line;
	Lexer(string filename);
	void readch();
	bool can_scan();
	Token* scan();
	virtual ~Lexer();
};

#endif /* LEXER_H_ */
