#include "Parser.h"
#include "regex.h"

Parser::Parser(Lexer* lexer, Tokenizer* to, const char *input) {
	ifstream fin(input);
	if (fin) {
		string line;
		while (!fin.eof()) {
			this->document += fin.get();
		}
	}
	fin.close();
	this->tokenizer = to;
	this->lexer = lexer;
	this->token = NULL;
	this->gen_type = -1;
	this->input_file = input;
	this->document_tokens = to->read_doc(this->document);
}

void Parser::move() {
	token = lexer->scan();
}

void Parser::error(string s, int line) {
	string l;
	while (line > 0) {
		l = (char) ((line % 10) + '0') + l;
		line /= 10;
	}
	throw string(s + l);
}

Token* Parser::match(int t) {
	if (token->tag == t) {
		Token* current = token;
		move();
		return current;
	} else {
		error("syntax error near line ", Lexer::line);
		return NULL;
	}
}

void Parser::program() {
	move();
	while (lexer->can_scan()) {
		envs.push_back(Env());
		aql_stmt();
		switch (gen_type) {
		case Parser::PATTERN_TYPE:
			gen_view_from_pattern_type();
			break;
		case Parser::REGEX_TYPE:
			gen_view_from_regex_type();
			break;
		case Parser::SELECT_TYPE:
			gen_view_from_select_type();
			break;
		}
	}
}

void Parser::gen_view_from_regex_type() {
	View* view = new View(envs[envs.size() - 1].view_name);
	const char* regex = envs[envs.size() - 1].reg.c_str();
	const char* content = document.c_str();
	vector<vector<int> > result = findall(regex, content);
	for (int i = 0, j = 0;
			i < envs[envs.size() - 1].groups.size() && i < result[0].size() / 2;
			i++, j = j + 2) {
		map<int, string>::iterator it = envs[envs.size() - 1].groups.find(i);
		if (it == envs[envs.size() - 1].groups.end()) {
			char s[2];
			s[0] = i + '0';
			error("group number " + string(s) + " not found. near ",
					Lexer::line);
		}
		view->insert_group(result, it->second, j, content);
	}

	views.insert(make_pair(view->name, view));
}

/**
 * 从select语句生成view
 * 需要判断三种非法情况：
 * 1、Select 的View ID在form语句中没找到
 * 2、Select 的View还没定义，即在views中找不到
 * 3、View ID和View都存在，但选取的group不存在
 * **/
void Parser::gen_view_from_select_type() {
	vector<Select_col> select_cols = envs[envs.size() - 1].select_cols;
	map<string, From_col> from_cols = envs[envs.size() - 1].from_cols;
	for (int i = 0; i < select_cols.size(); i++) {
		if (from_cols.find(select_cols[i].view_id) == from_cols.end()) {
			error("view name is not found: " + select_cols[i].view_id + " ",
					Lexer::line);
		} else {
			string view_name =
					from_cols.find(select_cols[i].view_id)->second.view_name;
			if (views.find(view_name) == views.end()) {
				error("view name is not found: " + view_name + " ",
						Lexer::line);
			} else {
				View* old_view = views.find(view_name)->second;
				if (old_view->groups.find(select_cols[i].view_group)
						== old_view->groups.end()) {
					error(
							"group name not found " + select_cols[i].view_group
									+ " ", Lexer::line);
				} else {
					map<string, View*>::iterator it;
					View* view;
					if ((it = views.find(envs[envs.size() - 1].view_name))
							== views.end()) {
						view = new View(envs[envs.size() - 1].view_name);
					} else {
						view = it->second;
					}
					view->insert_group(old_view, select_cols[i].view_group,
							select_cols[i].new_group);
					views.insert(make_pair(view->name, view));
				}
			}
		}
	}
}

void Parser::gen_view_from_pattern_type() {
	Temp temp;
	vector<Token*> tokens = this->envs[envs.size() - 1].pattern_expr_tokens;

	map<string, From_col> from_cols = envs[envs.size() - 1].from_cols;
	for (int i = 0; i < tokens.size(); i++) {
		Token* t = tokens[i];
		if (t->tag == Tag::ID) {
			string view_id = ((Word*) t)->lexeme;
			if (from_cols.find(view_id) == from_cols.end()) {
				error("view id " + view_id + " not found. near ", Lexer::line);
			} else {
				string view_name = from_cols.find(view_id)->second.view_name;
				if (views.find(view_name) == views.end()) {
					error("view name " + view_name + " not found. near ",
							Lexer::line);
				} else {
					++i;
					string group_name = ((Word*) tokens[i])->lexeme;
					View* old_view = views.find(view_name)->second;
					if (old_view->groups.find(group_name)
							== old_view->groups.end()) {
						error("group name" + group_name + " not found. near ",
								Lexer::line);
					} else {
						Group g = old_view->groups.find(group_name)->second;
						temp.groups.push_back(g);

					}
				}
			}
		} else if (t->tag == Tag::REG) {
			vector<vector<int> > results = findall(((Word*) t)->lexeme.c_str(),
					this->document.c_str());
			Group group;
			for (int i = 0; i < results.size(); i++) {
				Span span;
				for (int j = results[i][0]; j < results[i][1]; j++) {
					span.content += this->document[j];
				}
				span.begin = results[i][0];
				span.end = results[i][1];
				group.spans.insert(make_pair(span.begin, span));
			}
			temp.groups.push_back(group);
		} else if (t->tag == Tag::TOKEN) {
			temp.groups.push_back(document_tokens);
		} else {
			error("unexpected error ", Lexer::line);
		}
		if (t->grouped_num.size() > 0) {
			temp.grouped_nums.push_back(t->grouped_num);
		} else {
			temp.grouped_nums.push_back(vector<int>());
		}
		if ((i + 1) < tokens.size()
				&& ((Token*) tokens[i + 1])->tag == Tag::NUM) {
			Interval interval(((Num*) tokens[i + 1])->value,
					((Num*) tokens[i + 2])->value);
			i += 2;
			temp.intervals.push_back(interval);
			temp.has_intervals.push_back(true);
		} else {
			temp.intervals.push_back(Interval(0, 0));
			temp.has_intervals.push_back(false);
		}
	}
	match_pattern(temp, 0);
}

bool can_get_group_num(const vector<int> &grouped_num, int group_num) {
	for (int i = 0; i < grouped_num.size(); i++) {
		if (grouped_num[i] == group_num) {
			return true;
		}
	}
	return false;
}

string get_sub_string(string& origin, int begin, int end) {
	string s;
	for (int i = begin; i < end; i++) {
		s += origin[i];
	}
	return s;
}

void Parser::match_pattern(Temp &temp, int cur, int last_match_end_pos) {
	if (cur >= temp.groups.size()) {     // 递归结束，找到一组匹配

		View *view;
		if (views.find(envs[envs.size() - 1].view_name) == views.end()) {
			view = new View(envs[envs.size() - 1].view_name);
			views.insert(make_pair(envs[envs.size() - 1].view_name, view));
		} else {
			view = views.find(envs[envs.size() - 1].view_name)->second;
		}
		// 为group0插入Span
		Span span;
		span.begin = temp.match_begin_pos[0];
		Group group = temp.groups[temp.groups.size() - 1];
		multimap<int, Span>::iterator it = group.spans.find(
				temp.match_begin_pos[temp.match_begin_pos.size() - 1]);
		for (int j = 2; j <= temp.match_nums[temp.match_nums.size() - 1]; j++) {
			++it;
		}
		span.end = it->second.end;
		span.content = get_sub_string(this->document, span.begin, span.end);
		map<int, string>::iterator itt =
				this->envs[envs.size() - 1].groups.find(0);
		if ((itt = envs[envs.size() - 1].groups.find(0))
				== envs[envs.size() - 1].groups.end()) {
			error("group number 0 not found, near ", Lexer::line);
		} else {
			view->groups[itt->second].spans.insert(make_pair(span.begin, span));
			int max_length = Span::cal_span_length(span);
			if (max_length > view->groups[itt->second].max_size) {
				view->groups[itt->second].max_size = max_length;
			}
		}

		int max_group_num = 0;
		// 找出总共有多少组，因为temp.grouped_nums中，组号是按升序排列的，所以只需要比较最后一个元素
		for (int i = 0; i < temp.groups.size(); i++) {
			if (temp.grouped_nums[i].size() > 0
					&& temp.grouped_nums[i][temp.grouped_nums[i].size() - 1]
							> max_group_num) {
				max_group_num = temp.grouped_nums[i][temp.grouped_nums[i].size()
						- 1];
			}
		}
		if (max_group_num == 0) {
			// 没有分组
		} else {
			int begin_index = 0, end_index;
			for (int group_num = 1; group_num <= max_group_num; group_num++) {
				bool is_first_found = false;  // 判断是否找到第一个有group_num的项
				for (int i = 0; i < temp.groups.size(); i++) { // 遍历查找包含group_num的项
					if (can_get_group_num(temp.grouped_nums[i], group_num)) {
						if (is_first_found == false) {
							is_first_found = true;
							begin_index = temp.match_begin_pos[i];
						}
						if (is_first_found == true) {
							if (i + 1 >= temp.groups.size()
									|| can_get_group_num(
											temp.grouped_nums[i + 1], group_num)
											== false) {  // 该寻找end_index了
								Group group = temp.groups[i];
								multimap<int, Span>::iterator it =
										group.spans.find(
												temp.match_begin_pos[i]);
								for (int j = 2; j <= temp.match_nums[i]; j++) {
									++it;
								}
								end_index = it->second.end;
								break;
							}
						}
					}
				}
				if (envs[envs.size() - 1].groups.find(group_num)
						== envs[envs.size() - 1].groups.end()) {
					char s[2];
					s[0] = group_num + '0';
					error("group number " + string(s) + " not found. near ",
							Lexer::line);
				} else {
					string group_name = envs[envs.size() - 1].groups.find(
							group_num)->second;
					Span span;
					span.begin = begin_index;
					span.end = end_index;
					span.content = get_sub_string(this->document, span.begin,
							span.end);
					view->groups[group_name].spans.insert(
							make_pair(span.begin, span));
					int max_length = Span::cal_span_length(span);
					if (max_length > view->groups[group_name].max_size) {
						view->groups[group_name].max_size = max_length;
					}
				}
			}
		}
		return;
	}
	// 开始递归匹配
	Group group = temp.groups[cur];
	if (temp.has_intervals[cur] == false) {        // 这是没有{NUM, NUM}的情况
		multimap<int, Span>::iterator it;
		for (it = group.spans.begin(); it != group.spans.end(); ++it) {

			if (cur == 0) {      // 循环匹配第一项
				temp.match_begin_pos.push_back(it->second.begin);
				temp.match_nums.push_back(1);
				match_pattern(temp, cur + 1, it->second.end);
				temp.match_begin_pos.pop_back();
				temp.match_nums.pop_back();
			} else {          // 循环匹配之后的项
				if (it->second.begin >= last_match_end_pos) {
					if (it->second.begin == last_match_end_pos
							|| empty_between(last_match_end_pos,
									it->second.begin)) {
						temp.match_begin_pos.push_back(it->second.begin);
						temp.match_nums.push_back(1);
						match_pattern(temp, cur + 1, it->second.end);
						temp.match_begin_pos.pop_back();
						temp.match_nums.pop_back();
					} else {
						continue;
					}
				}
			}
		}
	} else {         // 存在{NUM, NUM}的情况
		multimap<int, Span>::iterator it;
		for (it = group.spans.begin(); it != group.spans.end(); ++it) {
			if (cur != 0) {
				if (it->second.begin < last_match_end_pos
						|| (it->second.begin != last_match_end_pos
								&& !empty_between(last_match_end_pos,
										it->second.begin))) { // 不是pattern里的第一个，先找到能与前一个token匹配的token
					continue;
				}
			}
			multimap<int, Span>::iterator it1 = it;
			multimap<int, Span>::iterator it2 = it;
			for (int i = 0; i < temp.intervals[cur].min - 1; i++) { // 先匹配完{min, max}中min-1的个数
				it2++;
				if (it2 == group.spans.end()) {
					return;
				} else {
					if (it2->second.begin == it1->second.end
							|| empty_between(it1->second.end,
									it2->second.begin)) {
						++it1;
						continue;
					} else {
						return;
					}
				}
			}
			// 开始匹配min到max的token个数
			for (int i = 0;
					i <= temp.intervals[cur].max - temp.intervals[cur].min
							&& it2 != group.spans.end(); i++, ++it2) {
				temp.match_begin_pos.push_back(it->second.begin);
				temp.match_nums.push_back(temp.intervals[cur].min + i);
				match_pattern(temp, cur + 1, it2->second.end);
				temp.match_begin_pos.pop_back();
				temp.match_nums.pop_back();
			}
		}
	}
}

bool Parser::empty_between(int start, int end) {
	for (int i = start; i < end; i++) {
		if (document[i] != ' ') {
			return false;
		}
	}
	return true;
}

void Parser::aql_stmt() {
	if (token->tag == Tag::CREATE) {
		create_stmt();
	} else if (token->tag == Tag::OUTPUT) {
		output_stmt();
	}
}

void Parser::create_stmt() {
	match(Tag::CREATE);
	match(Tag::VIEW);
	Word* view = (Word*) match(Tag::ID);
	if (views.find(view->lexeme) != views.end()) {
		error("View name has already existed ", Lexer::line);
	}
	envs[envs.size() - 1].view_name = view->lexeme;
	match(Tag::AS);
	view_stmt();
	match(';');
}

void Parser::view_stmt() {
	Token* cur = token;
	if (cur->tag == Tag::EXTRACT) {
		extract_stmt();
	} else if (cur->tag == Tag::SELECT) {
		gen_type = Parser::SELECT_TYPE;
		select_stmt();
	}
}

void Parser::extract_stmt() {
	match(Tag::EXTRACT);
	extract_spec();
	match(Tag::FROM);
	from_list();
}

void Parser::extract_spec() {
	if (token->tag == Tag::REGEX) {
		gen_type = Parser::REGEX_TYPE;
		regex_spec();
	} else if (token->tag == Tag::PATTERN) {
		gen_type = Parser::PATTERN_TYPE;
		pattern_spec();
	} else {
		error("syntax error near line ", Lexer::line);
	}
}

void Parser::regex_spec() {
	match(Tag::REGEX);
	envs[envs.size() - 1].reg = ((Word*) match(Tag::REG))->lexeme;
	match(Tag::ON);
	column();
	name_spec();
}

vector<Token*> Parser::column() {
	Word* view_id = (Word*) match(Tag::ID);
	match('.');
	Word* group = (Word*) match(Tag::ID);
	envs[envs.size() - 1].extract_cols.push_back(
			Extract_col(view_id->lexeme, group->lexeme));
	vector<Token*> temp;
	temp.push_back(view_id);
	temp.push_back(group);
	return temp;
}

// 例子：extract regex /..../ on ID.ID as ID
//      extract regex /..../ on ID.ID return group ....
void Parser::name_spec() {
	if (token->tag == Tag::AS) {
		move();
		Word* id = (Word*) match(Tag::ID);
		envs[envs.size() - 1].groups.insert(make_pair(0, id->lexeme));
	} else if (token->tag == Tag::RETURN) {
		move();
		group_spec();
	}
}

// 例子： return group 0 as PerLoc and group 1 as Per and group 2 as Loc
// 这里的PerLoc是用户定义的ID，PerLoc和Loc是pattern中出现的ID
void Parser::group_spec() {
	single_group();
	while (token->tag == Tag::AND) {
		move();
		single_group();
	}
}

void Parser::single_group() {
	match(Tag::GROUP);
	Num* num = (Num*) match(Tag::NUM);
	match(Tag::AS);
	Word* id = (Word*) match(Tag::ID);

	// group num已经存在，这也是groups用map的原因，容易判断重复
	if (envs[envs.size() - 1].groups.find(num->value)
			!= envs[envs.size() - 1].groups.end()) {
		error("group num has already existed ", Lexer::line);
	} else {
		envs[envs.size() - 1].groups.insert(make_pair(num->value, id->lexeme));
	}

}

void Parser::select_stmt() {
	match(Tag::SELECT);
	select_list();
	match(Tag::FROM);
	from_list();
}

// 例子：select PL.PerLoc as PerLoc, Col.c (Col.c的ID默认还是Col.c)
void Parser::select_list() {
	while (true) {
		Word* view_id = (Word*) match(Tag::ID);
		match('.');
		Word* view_group = (Word*) match(Tag::ID);
		if (token->tag == Tag::AS) {
			move();
			Word* new_group = (Word*) match(Tag::ID);
			envs[envs.size() - 1].select_cols.push_back(
					Select_col(view_id->lexeme, view_group->lexeme,
							new_group->lexeme));
		} else {
			move();
			envs[envs.size() - 1].select_cols.push_back(
					Select_col(view_id->lexeme, view_group->lexeme,
							view_group->lexeme));
		}
		if (token->tag == ',') {
			move();
			continue;
		} else {
			break;
		}
	}
}

void Parser::from_list() {
	string view_name = ((Word*) match(Tag::ID))->lexeme;
	string view_id = ((Word*) match(Tag::ID))->lexeme;
	envs[envs.size() - 1].from_cols.insert(
			make_pair(view_id, From_col(view_id, view_name)));
	while (token->tag == ',') {
		move();
		string view_name = ((Word*) match(Tag::ID))->lexeme;
		string view_id = ((Word*) match(Tag::ID))->lexeme;
		envs[envs.size() - 1].from_cols.insert(
				make_pair(view_id, From_col(view_id, view_name)));
	}
}
void Parser::pattern_spec() {
	match(Tag::PATTERN);
	this->envs[envs.size() - 1].pattern_expr_tokens = pattern_expr();
	name_spec();
}

vector<Token*> Parser::pattern_expr() {
	vector<Token*> tokens;
	while (true) {
		vector<Token*> temp = pattern_pkg();
		tokens.insert(tokens.end(), temp.begin(), temp.end());
		if (token->tag == '<' || token->tag == Tag::REG || token->tag == '(') {
			continue;
		} else {
			break;
		}
	}

	return tokens;
}

vector<Token*> Parser::pattern_pkg() {
	if (token->tag == '(') {
		return pattern_group();
	} else {
		vector<Token*> tokens;
		if (token->tag == '<') {
			vector<Token*> temp = atom();
			tokens.insert(tokens.end(), temp.begin(), temp.end());
			if (token->tag == '{') {           // 如果后面加了{NUM, NUM}，保存NUM NUM
				this->move();
				Token* t = match(Tag::NUM);
				int min = ((Num*) t)->value;
				if (min <= 0) {
					error("minimal value should be larger than 0 ",
							Lexer::line);
				}
				tokens.push_back(t);
				match(',');
				t = match(Tag::NUM);
				int max = ((Num*) t)->value;
				if (max <= min) {
					error("max value should be larger than minimal value ",
							Lexer::line);
				}
				match('}');
				tokens.push_back(t);
			}
		} else if (token->tag == Tag::REG) {
			vector<Token*> temp = atom();
			tokens.insert(tokens.end(), temp.begin(), temp.end());
		} else {
			error("synatx error ", Lexer::line);
		}
		return tokens;
	}
}

vector<Token*> Parser::atom() {
	vector<Token*> tokens;
	if (token->tag == '<') {
		move();
		if (token->tag == Tag::ID) {
			vector<Token*> temp = column();
			tokens.insert(tokens.end(), temp.begin(), temp.end());
		} else if (token->tag == Tag::TOKEN) {
			tokens.push_back(token);
			move();
		}
		match('>');
	} else if (token->tag == Tag::REG) {
		tokens.push_back(token);
		move();
	} else {
		error("synatx error ", Lexer::line);
	}
	return tokens;
}

vector<Token*> Parser::pattern_group() {
	match('(');
	int current_group_num = envs[envs.size() - 1].group_num;
	++envs[envs.size() - 1].group_num;
	vector<Token*> temp = pattern_expr();
	match(')');
	for (int i = 0; i < temp.size(); i++) {
		Token* t = temp[i];
		t->grouped_num.insert(t->grouped_num.begin(), current_group_num);
	}
	return temp;
}

void Parser::output_stmt() {
	this->gen_type = OUTPUT_TYPE;
	match(Tag::OUTPUT);
	match(Tag::VIEW);
	Word* w = ((Word*) match(Tag::ID));
    // 匹配之后的alias产生式
	string alias;   // alias产生式可能带有view的别名
	if (token->tag == Tag::AS) {
		move();
		Word* id = ((Word*) match(Tag::ID));
		alias = id->lexeme;
	}
	match(';');

	map<string, View*>::iterator it;
    // 要打印的view不存在
	if ((it = views.find(w->lexeme)) == views.end()) {
		error("view is not existed ", Lexer::line);
	} else {
		it->second->alias = alias;
		size_t dot_index = this->input_file.find(".txt");
		string new_file = this->input_file.substr(0, dot_index);
		new_file += ".output";

		ofstream fout(new_file, ofstream::app);
		it->second->print(fout);
		fout.close();
	}
}

Parser::~Parser() {
	this->document_tokens.spans.clear();
	delete this->lexer;
	delete this->tokenizer;
	lexer = NULL;
	tokenizer = NULL;
	views.clear();
	envs.clear();
}

