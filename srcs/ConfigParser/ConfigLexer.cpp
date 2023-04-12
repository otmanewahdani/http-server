/* this file contains the implementation of ConfigLexer class */

#include <ConfigLexer.hpp>

Token::Token()
	: type(Token::EOF) {}

ConfigLexer::ConfigLexer(std::istream& input)
	: mInput(input) {
}
