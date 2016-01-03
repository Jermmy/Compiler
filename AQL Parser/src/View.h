#ifndef VIEW_H_
#define VIEW_H_

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

using namespace std;

struct Span {
	int begin, end;
	string content;
	static int cal_span_length(Span& span);
};

class Group {
public:
	int max_size;
	vector<int> grouped_num;         // 表示捕获后属于哪些列
	string name;
	multimap<int, Span> spans;   // 第一个int是Span的begin
	Group();
};

class View {
public:
	string name;
	string alias;     //别名
	map<string, Group> groups;    // 第一个string是Group的name
	View(string name);
	// 用extract语句从文档中提取正则表达式的内容
	void insert_group(const vector<vector<int> >& results,
			const string& group_name, int index, const char* doc);
	// 用于select语句在view中插入group，第一个group_name是在原view中对应的group，第二个是该view中新的group
	void insert_group(const View* view, const string &group_name,
			const string &new_group_name);
	void print(ofstream &out);
	virtual ~View();
};

#endif /* VIEW_H_ */
