#include <fstream>
#include <iterator>
#include <iomanip>
#include "View.h"

using namespace std;

int cal_integer_length(int t) {
	if (t == 0) {
		return 1;
	}
	int length = 0;
	while (t > 0) {
		length++;
		t /= 10;
	}
	return length;
}

int Span::cal_span_length(Span& span) {
	int length = 0;
	length += span.content.length() + 4;
	length += cal_integer_length(span.begin);
	length += cal_integer_length(span.end);
	return length;
}

Group::Group() {
	this->max_size = 0;
}

//Group::Group(const Group &other) {
//	this->max_size = other.max_size;
//	this->name = other.name;
//	for (int i = 0; i < other.grouped_num.size(); i++) {
//		this->grouped_num.push_back(other.grouped_num[i]);
//	}
//	this->grouped_num = other.grouped_num;
//	this->spans = other.spans;
//}
//
//Group& Group::operator=(const Group &other) {
//	this->max_size = other.max_size;
//	this->name = other.name;
//	for (int i = 0; i < other.grouped_num.size(); i++) {
//		this->grouped_num.push_back(other.grouped_num[i]);
//	}
//	this->grouped_num = other.grouped_num;
//	this->spans = other.spans;
//	return *this;
//}

View::View(string name) {
	this->name = name;
}

void View::insert_group(const vector<vector<int> >& results,
		const string& group_name, int index, const char* doc) {
	Group group;
	group.name = group_name;
	for (int i = 0; i < results.size(); i++) {
		Span span;
		for (int j = results[i][index]; j < results[i][index+1]; j++) {
			span.content += *(doc + j);
		}
		span.begin = results[i][index];
		span.end = results[i][index+1];
		int max_length = Span::cal_span_length(span);
		if (max_length > group.max_size) {
			group.max_size = max_length;
		}
		group.spans.insert(make_pair(span.begin, span));
	}
	groups.insert(make_pair(group.name, group));
}

void View::insert_group(const View* view, const string &group_name,
		const string &new_group_name) {
	Group group;
	Group old_group = view->groups.find(group_name)->second;
	multimap<int, Span>::iterator it;
	for (it = old_group.spans.begin(); it != old_group.spans.end(); ++it) {
		group.spans.insert(make_pair(it->first, it->second));
	}
	group.max_size = old_group.max_size;
	groups.insert(make_pair(new_group_name, group));

}

void View::print(ofstream &out) {
	if (alias != "") {
		out << "View: " << alias << endl;
	} else {
		out << "View: " << name << endl;
	}
	int max_span_num = 0;
	map<string, Group>::iterator it;
	for (it = groups.begin(); it != groups.end(); ++it) {
		if (it->second.spans.size() > max_span_num) {
			max_span_num = it->second.spans.size();
		}
	}

	// +----------+-----------+-------------+
	for (it = groups.begin(); it != groups.end(); ++it) {
		out << setw(it->second.max_size+3) << setfill('-') << setiosflags(ios::left) << "+" ;
	}
	out << "+" << endl;

	// | Cap       | Loc          | Stt        |
	for (it = groups.begin(); it != groups.end(); ++it) {
		out << "| ";
		out << setiosflags(ios::left) << setw(it->second.max_size+1) << setfill(' ') << it->first;
	}
	out << "|" << endl;

	// +----------+-----------+-------------+
	for (it = groups.begin(); it != groups.end(); ++it) {
		out << setw(it->second.max_size+3) << setfill('-') << setiosflags(ios::left) << "+" ;
	}
	out << "+" << endl;

	for (int i = 0; i < max_span_num; i++) {
		for (it = groups.begin(); it != groups.end(); ++it) {
			multimap<int, Span>::iterator span_it = it->second.spans.begin();
			advance(span_it, i);
			out << "| " << span_it->second.content << ":(" << span_it->second.begin
					<< "," << span_it->second.end << ")";
			out << setw(it->second.max_size+2-Span::cal_span_length(span_it->second)-1) << setfill(' ') << " ";

			//cout << "Group: " << it->first << " span: " << span_it->second.content << endl;
		}
		out << "|" << endl;
	}

	// +----------+-----------+-------------+
	for (it = groups.begin(); it != groups.end(); ++it) {
		out << setw(it->second.max_size+3) << setfill('-') << setiosflags(ios::left) << "+" ;
	}
	out << "+" << endl;

	if (max_span_num > 0) {
		out << max_span_num << " rows in set" << endl << endl;
	} else {
		out << "Empty set" << endl << endl;
	}

}

View::~View() {
	map<string, Group>::iterator it;
	for (it = groups.begin(); it != groups.end(); it++) {
		it->second.spans.clear();
	}
}

