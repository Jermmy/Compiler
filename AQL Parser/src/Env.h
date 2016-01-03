#ifndef ENV_H_
#define ENV_H_

#include <string>
#include <vector>
#include <map>
#include "Token.h"

using namespace std;

struct Extract_col {
	string view_id;
	string group;
	Extract_col(string view_id, string group) {
		this->view_id = view_id;
		this->group = group;
	}
};

struct Select_col {
	string view_id;
	string view_group;
	string new_group;
	Select_col(string view_id, string view_group, string new_group) {
		this->view_id = view_id;
		this->view_group = view_group;
		this->new_group = new_group;
	}
};

struct From_col {
	string view_id;       // 例如：Per P,其中Per是view_name,P是view_id
	string view_name;
	From_col(string view_id, string view_name) {
		this->view_id = view_id;
		this->view_name = view_name;
	}
};

class Env {
public:
	string view_name;   // 当前要创建的view的名字
	map<int, string> groups;    // 当前要创建的view的所有列的名字，从group0开始
	string reg;      // 如果是用正则表达式，这个变量会用到
	vector<Extract_col> extract_cols;   // 记录extract中查找的View的ID以及列名
	vector<Select_col> select_cols;      // 记录select中查找的View的ID以及列名
	map<string, From_col> from_cols;     // 记录from后面的view的名称和别名，string保存From_col的id
	vector<Token*> pattern_expr_tokens;    // 记录pattern_expr抓取的所有token序列
	int group_num;
	Env();
	virtual ~Env();
};

#endif /* ENV_H_ */
