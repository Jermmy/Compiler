#include "Env.h"

Env::Env() {
	this->group_num = 1;
}

Env::~Env() {
	groups.clear();
	extract_cols.clear();
	from_cols.clear();
	pattern_expr_tokens.clear();
}

