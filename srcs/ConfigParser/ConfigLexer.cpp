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

bool ConfigLexer::isExtension() {

	// checks that it has 2 or more characters
	// and that the first character is a dot
	// and that it's the only dot in the string
	return (mCurrentTok.value.length() > 1
		&& mCurrentTok.value[0] == '.'
		&& mCurrentTok.value.find(1, '.') == std::string::npos);

}

bool ConfigLexer::isMethod() {

	return (mCurrentTok.value == "GET"
		|| mCurrentTok.value == "POST"
		|| mCurrentTok.value == "DELETE");

}

bool ConfigLexer::isNumber() {

	// checks if it starts with '0'
	if (mCurrentTok.value.length() == 0
		|| mCurrentTok.value[0] == '0')
		return false;
	
	for (std::string::const_iterator it = mCurrentTok.value.begin();
		it != mCurrentTok.value.end(); ++it)
		if (std::isdigit(*it) == false)
			return false;
	
	return true;

}

void ConfigLexer::setTypeForNewToken() {

	if (mCurrentTok.value.size() == 0)
		mCurrentTok.type = Token::EOS;
	else if (mCurrentTok.value == "server_name")
		mCurrentTok.type = Token::SRV_NAME;
	else if (mCurrentTok.value == "listen")
		mCurrentTok.type = Token::LISTEN;
	else if (mCurrentTok.value == "error_page")
		mCurrentTok.type = Token::ERR_PAGE;
	else if (mCurrentTok.value == "client_body_size_max")
		mCurrentTok.type = Token::CLIENT_MAX;
	else if (mCurrentTok.value == "location")
		mCurrentTok.type = Token::LOC;
	else if (mCurrentTok.value == "allow_methods")
		mCurrentTok.type = Token::ALLOW;
	else if (isMethod())
		mCurrentTok.type = Token::METHOD;
	else if (mCurrentTok.value == "redirect")
		mCurrentTok.type = Token::RDR;
	else if (mCurrentTok.value == "root")
		mCurrentTok.type = Token::ROOT;
	else if (mCurrentTok.value == "autoindex")
		mCurrentTok.type = Token::AUTOIN;
	else if (mCurrentTok.value == "on" || mCurrentTok.value == "off")
		mCurrentTok.type = Token::SWITCH;
	else if (mCurrentTok.value == "default")
		mCurrentTok.type = Token::DFLT;
	else if (isExtension())
		mCurrentTok.type = Token::EXT;
	else if (mCurrentTok.value == "cgi")
		mCurrentTok.type = Token::CGI;
	else if (mCurrentTok.value == "upload")
		mCurrentTok.type = Token::UPLOAD;
	else if (mCurrentTok.value == "{")
		mCurrentTok.type = Token::LB;
	else if (mCurrentTok.value == "}")
		mCurrentTok.type = Token::RB;
	else if (isNumber())
		mCurrentTok.type = Token::NUM;
	else if (mCurrentTok.value == ";")
		mCurrentTok.type = Token::SM_COL;
	else
		mCurrentTok.type = Token::OTHER;

}
