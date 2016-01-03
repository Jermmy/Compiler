#include "Tokenizer.h"

Tokenizer::Tokenizer() {

}

bool is_character(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool is_number(char c) {
	return c >= '0' && c <= '9';
}

Group Tokenizer::read_doc(string &doc) {
	Group g;
	for (int i = 0; i < doc.length(); i++) {
		if (doc[i] == ' ' || doc[i] == '\t' || doc[i] == '\r' || doc[i] == '\n') {
			continue;
		}
		string content;
		if (is_character(doc[i])) {
			while (i < doc.length() && doc[i] != ' ' && doc[i] != '\t'
					&& is_character(doc[i])) {
				content += doc[i];
				++i;
			}
		} else if (is_number(doc[i])) {
			while (i < doc.length() && doc[i] != ' ' && doc[i] != '\t'
					&& is_number(doc[i])) {
				content += doc[i];
				++i;
			}
		} else {
			while (i < doc.length() && doc[i] != ' ' && doc[i] != '\t'
					&& !is_number(doc[i]) && !is_character(doc[i])) {
				content += doc[i];
				++i;
			}
		}
		Span span;
		span.content = content;
		span.end = i;
		span.begin = i - content.length();
		g.spans.insert(make_pair(span.begin, span));
		--i;
	}
	return g;
}

Tokenizer::~Tokenizer() {

}

