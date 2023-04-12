/* this file contains the implementation of ConfigLexer class */

#include <ConfigLexer.hpp>

Token::Token()
	: type(Token::EOS) {}

ConfigLexer::ConfigLexer(std::istream& input)
	: mInput(input) {

		// extracts first token
		next();

}

Token ConfigLexer::next() {

	// saves current token to be returned
	Token tmpTok = mCurrentTok;

	extractNewToken();

	setTypeForNewToken();

	return tmpTok;

}

Token ConfigLexer::peek() { return mCurrentTok; }

void ConfigLexer::extractNewToken() {

	// clears token so it's used to store the next token
	clearToken();

	skipWhiteSpace();

	char streamChar;
	while (!mInput.eof() && !mInput.fail()) {

		// peeks current character, checks that stream is healthy and
		// if any whitespace or special character was encountered.
		// In the case of the latter, checks if it's not the first
		// character to be extracted from stream
		streamChar = mInput.peek();
		if (mInput.eof() || mInput.fail() || isWhiteSpace(streamChar)
			|| (!mCurrentTok.value.empty() && isSpecialChar(streamChar)))
			break ;

		mCurrentTok.value += streamChar;

		// removes current character and gets the next one
		mInput.get();

		// if a special character was found then it's a full token
		if (isSpecialChar(streamChar))
			break ;

	}

	checkInputHealth();

}

void ConfigLexer::setTypeForNewToken() {

	/*if (mCurrentTok.value == "listen")*/
	mCurrentTok.type = Token::LISTEN;

}

void ConfigLexer::clearToken() {

	// empties string
	mCurrentTok.value.clear();
	mCurrentTok.type = Token::EOS;

}

bool ConfigLexer::isSpecialChar(char c) {

	switch (c) {
		case ';':
		case '{':
		case '}':
			return true;
	}
	return false;

}

bool ConfigLexer::isWhiteSpace(char c) {

	switch (c) {
		case ' ':
		case '\n':
		case '\t':
			return true;
	}
	return false;

}

void ConfigLexer::skipWhiteSpace() {

	if (mInput.eof())
		return ;

	char streamChar = mInput.peek();
	while (!mInput.eof() && !mInput.fail() && isWhiteSpace(streamChar)) {
		// extracts whitespace character and ignores it
		mInput.get();
		streamChar = mInput.peek();
	}

	checkInputHealth();

}

void ConfigLexer::checkInputHealth() {

	if (mInput.fail())
		throw std::runtime_error("ConfigLexer failed to extract"
		" character from input stream");

}
