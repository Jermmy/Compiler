#include <iostream>
#include "Num.h"
#include "Tag.h"

using namespace std;

Num::Num(int v): Token(Tag::NUM), value(v) {
}

Num::~Num() {
	// TODO Auto-generated destructor stub
}

void Num::print() {
	cout << "Type: Num (" << this->value << ", " << this->tag << ")" << endl;
}

