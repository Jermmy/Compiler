#include <iostream>
#include "Word.h"

using namespace std;

Word::Word(string l, int tag): Token(tag), lexeme(l) {
}

Word::~Word() {

}

void Word::print() {
	cout << "Type: Word (" << this->lexeme << ", " << this->tag << ")" << endl;
}

