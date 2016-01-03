#include <cstdio>
#include <iostream>
#include "Lexer.h"
#include "Tag.h"
#include "Num.h"
#include "Word.h"

using namespace std;

int Lexer::line = 1;

Lexer::Lexer(string filename) :
		fin(filename) {
	this->peek = ' ';
	tokens.insert(make_pair("create", new Word("create", Tag::CREATE)));
	tokens.insert(make_pair("view", new Word("view", Tag::VIEW)));
	tokens.insert(make_pair("as", new Word("as", Tag::AS)));
	tokens.insert(make_pair("output", new Word("output", Tag::OUTPUT)));
	tokens.insert(make_pair("select", new Word("select", Tag::SELECT)));
	tokens.insert(make_pair("from", new Word("from", Tag::FROM)));
	tokens.insert(make_pair("extract", new Word("extract", Tag::EXTRACT)));
	tokens.insert(make_pair("regex", new Word("regex", Tag::REGEX)));
	tokens.insert(make_pair("on", new Word("on", Tag::ON)));
	tokens.insert(make_pair("return", new Word("return", Tag::RETURN)));
	tokens.insert(make_pair("group", new Word("group", Tag::GROUP)));
	tokens.insert(make_pair("and", new Word("and", Tag::AND)));
	tokens.insert(make_pair("Token", new Word("Token", Tag::TOKEN)));
	tokens.insert(make_pair("pattern", new Word("pattern", Tag::PATTERN)));
	//tokens.insert(make_pair("Document", new Word("Document", Tag::DOCUMENT)));

}

void Lexer::readch() {
	//fin >> peek;   // fin默认忽略空白符
	//scanf("%c", &peek);
	peek = fin.get();
}

bool Lexer::can_scan() {
	return !fin.eof();
}

Token* Lexer::scan() {
	for (;; readch()) {
		if (peek == ' ' || peek == '\t' || peek == '\r') {
			continue;
		} else if (peek == '\n') {
			Lexer::line++;
		} else {
			break;
		}
	}
	if (isNum(peek)) {
		int v = 0;
		do {
			v = 10 * v + peek - '0';
			readch();
		} while (isNum(peek));
		Token* n = new Num(v);
		this->garbages.push_back(n);
		return n;
	}
	if (isLetter(peek)) {
		string lexeme;
		do {
			lexeme += peek;
			readch();
		} while (isLetter(peek) || isNum(peek));
		// Token要单独处理，不然之后分组会出错
		if (lexeme == "Token") {
			Token *w = new Token(Tag::TOKEN);
			this->garbages.push_back(w);
			return w;
		}
		if (tokens.find(lexeme) != tokens.end()) {
			return tokens[lexeme];
		}
		Token* w = new Word(lexeme, Tag::ID);
		this->garbages.push_back(w);
		return w;
	}
	if (peek == '/') {
		string lexeme;
		do {
			readch();
			if (peek == '/') {
				break;
			} else {
				lexeme += peek;
			}
		} while (true);
		peek = ' ';
		Token* w = new Word(lexeme, Tag::REG);
		this->garbages.push_back(w);
		return w;
	}
	// 如果所有情况都不匹配，就单独返回一个Token，这个Token可能是'(','<'这样的特殊字符，
	// 没有类型定义，可以直接通过peek的值来判断Token。
	Token* token = new Token(peek);
	peek = ' ';
	return token;
}

bool Lexer::isNum(char c) {
	return c >= '0' && c <= '9';
}

bool Lexer::isLetter(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

Lexer::~Lexer() {
	tokens.clear();
	for (int i = 0; i < this->garbages.size(); ++i) {
		if (garbages[i] != NULL) {
			delete garbages[i];
			garbages[i] = NULL;
		}
	}
}

