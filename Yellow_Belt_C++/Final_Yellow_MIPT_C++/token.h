#pragma once

#include <sstream>
#include <vector>
#include <stdexcept>

enum class TokenType {
	DATE,
	EVENT,
	COLUMN,
	LOGICAL_OP,
	COMPARE_OP,
	PAREN_LEFT,
	PAREN_RIGHT,
};

enum class Comparison {
	Less,
	LessOrEqual,
	Greater,
	GreaterOrEqual,
	Equal,
	NotEqual
};

enum class LogicalOperation {
	Or,
	And
};

struct Token {
	const std::string value;
	const TokenType type;
};

std::vector<Token> Tokenize(std::istream& cl);