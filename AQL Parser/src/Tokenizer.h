#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include "View.h"
#include <string>
using namespace std;

class Tokenizer {
public:
	Tokenizer();
	Group read_doc(string& doc);
	virtual ~Tokenizer();
};

#endif /* TOKENIZER_H_ */
