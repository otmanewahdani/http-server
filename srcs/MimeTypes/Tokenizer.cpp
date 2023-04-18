/* this file contains the implementation of the Tokenizer class */


#include <Tokenizer.hpp>

Tokenizer::Tokenizer(std::string &line)
:streamLine(line){}



bool Tokenizer::nextToken(std::string &token) {
	return bool(streamLine >> token);
}