#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include <vector>
using namespace std;

class Token {
public:
	const int tag;   // 类型标示符，可看Tag.h
	vector<int> grouped_num;   // 表示捕获后属于哪些列
	Token(int t);
	virtual ~Token();
	virtual void print();
};

#endif /* TOKEN_H_ */
