#include <iostream>
#include "Token.h"
#include "Tag.h"

using namespace std;


Token::Token(int t): tag(t) {
}

Token::~Token() {
}

void Token::print() {
	cout << "Type: Token" << endl;
	switch (this->tag) {
	case Tag::ID:
		cout << "ID" << endl;
		break;
	case Tag::NUM:
		cout << "Num" << endl;
		break;
	case Tag::REG:
		cout << "REG" << endl;
		break;
	case Tag::TOKEN:
		cout << "Token" << endl;
		break;
	default:
		cout << "other" << endl;
		break;
	}
}
