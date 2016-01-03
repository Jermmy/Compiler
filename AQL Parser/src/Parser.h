#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "Env.h"
#include "Tokenizer.h"
#include "Lexer.h"
#include "View.h"
#include "Tag.h"
#include "Word.h"
#include "Token.h"
#include "Num.h"
using namespace std;

struct Interval {
	int min, max;
	Interval(int min, int max) {
		this->min = min;
		this->max = max;
	}
};

class Temp {
public:
	vector<Group> groups;   // pattern中待处理的组
	vector<Interval> intervals;
	vector<bool> has_intervals;
	vector<int> match_begin_pos;    // 匹配的起始位置
	vector<int> match_nums;    // 匹配了几个，<P.Pol>{1,3}可能匹配了3个，如果这一项刚好是最后一项，需要通过3来获取结束的位置
	vector<vector<int> > grouped_nums;         // 表示捕获后属于哪些组
	//vector<bool> is_grouped;          // 表示是否需要捕获
	Temp() {}
};

class Parser {
private:
	Lexer* lexer;
	Tokenizer* tokenizer;
	Token* token;
	string input_file;
	string document;
	int gen_type;       // 生成view的方法，有三种：select，regex(从文档提取), pattern(几个view结合)
	map<string, View*> views;     // string是View的名称
	vector<Env> envs;
	Group document_tokens;
	void move();
	Token* match(int t);
	void error(string s, int line);
	// 三种生成view的方法
	void gen_view_from_regex_type();  // 默认从Document里面提取view
	void gen_view_from_select_type();
	void gen_view_from_pattern_type();

	void match_pattern(Temp &temp, int cur, int last_match_end_pos = 0);
	bool empty_between(int start, int end);
	// 以下是根据文法定义的函数
	void aql_stmt();
	void create_stmt();
	void output_stmt();
	void view_stmt();
	void select_stmt();
	void select_list();
	void from_list();
	void extract_stmt();
	void extract_spec();
	void regex_spec();
	void pattern_spec();
	vector<Token*> pattern_expr();
	vector<Token*> pattern_pkg();
	vector<Token*> atom();
	vector<Token*> pattern_group();
	vector<Token*> column();
	void name_spec();
	void group_spec();
	void single_group();
public:
	Parser(Lexer* lexer, Tokenizer *to, const char* input);
	void program();
	virtual ~Parser();
	static const int SELECT_TYPE = 0, REGEX_TYPE = 1, PATTERN_TYPE = 2, OUTPUT_TYPE = 3;
};

#endif /* PARSER_H_ */
