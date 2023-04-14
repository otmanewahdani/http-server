/* this file contains the implementation of the ConfigParser class */

#include <ConfigParser.hpp>

ConfigParser::ConfigParser(std::istream& input, Config& config)
	: mConfig(config)
	, mServers(config.getServers())
	, mServerContextPtr()
	, mLocationContextPtr()
	, mLexer(input) {
	
	parseGlobal();

}

void ConfigParser::parseGlobal() {

	Token token = mLexer.next();
	while (token.type != Token::EOS) {
		if (token.type == Token::SRV_BLK)
			parseServer();
		else
			handleParsingError();
		token = mLexer.next();
	}

}

void parseServer();

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

void updateServerIterator();

void updateLocationIterator();

void addNewServer();

void addNewLocation();
