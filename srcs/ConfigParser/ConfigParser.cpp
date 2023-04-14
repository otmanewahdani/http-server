/* this file contains the implementation of the ConfigParser class */

#include <ConfigParser.hpp>

ConfigParser::ConfigParser(std::istream& input, Config& config)
	: mConfig(config)
	, mServers(config.getServers())
	, mLexer(input) {
	
	parseGlobal();

}

void ConfigParser::parseGlobal() {

	// searches for server blocks
	Token token = mLexer.next();
	while (token.type != Token::EOS) {
		if (token.type == Token::SRV_BLK)
			parseServer();
		else
			handleParsingError(token);
		token = mLexer.next();
	}

}

void ConfigParser::parseServer() {

	Token token = mLexer.next();
	// needs a left brace immediately after a server context directive
	isLeftBrace(token);

	// creates new server structure to contain the data within this
		// context
	addNewServer();
	updateServerRef();

	// enters server's context and parses directives within it
	token = mLexer.next();
	while (token->type != Token::EOS) {
		
		switch(toke->type) {
			case Token::SRV_NAME:
				break ;
		}
		token = mLexer.next();

	}

	defaultInitUnfilledServerFields();

}

void parseLocation();

void ConfigParser::handleParsingError(const Token& lastToken) {

	mServers.clear();

	std::string error = "Parsing error: unexpected token '";
	if (lastToken.value.empty() == false)
		error += lastToken.value;
	else
		error += "EOF";
	error += '\'';

	throw std::runtime_error(error);

}

void ConfigParser::updateServerRef() {

	if (!mServers.empty())
		mServerRef = --mServers.end();

}

void updateLocationRef();

void ConfigParser::addNewServer() {

	ServerContext tmp;
	mServers.push_back(tmp);

}

void addNewLocation();

void ConfigParser::isLeftBrace(const Token& token) {

	if (token->type != Token::LB)
		handleParsingError(token);

}

void isRightBrace(const Token& token);

void isSemiColon(const Token& token);

void defaultInitUnfilledServerFields();
