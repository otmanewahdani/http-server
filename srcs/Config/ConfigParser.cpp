/* this file contains the implementation of the ConfigParser class */

#include <ConfigParser.hpp>

ConfigParser::ConfigParser(std::istream& input, Config& config)
	: mConfig(config)
	, mServers(config.getServers())
	, mLocationRef()
	, mLexer(input) {
	
	parseGlobal();

}

void ConfigParser::parseGlobal() {

	// searches for server blocks
	Token token = mLexer.next();
	while (token.type != Token::EOS) {
		isToken(token, Token::SRV_BLK);
		parseServer();
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
	// while there are more tokens and a right curly brace hasn't been found
	while (token.type != Token::EOS
		&& token.type != Token::RB) {
		
		switch(token.type) {
			case Token::SRV_NAME:
				parseServerName();
				break;
			case Token::LISTEN:
				parseListen();
				break;
			case Token::ERR_PAGE:
				parseErrorPage();
				break;
			case Token::CLIENT_MAX:
				parseClientBodySizeMax();
				break;
			case Token::LOC:
				parseLocation();
				break;
			default:
				handleParsingError(token);
		}
		token = mLexer.next();

	}

	// makes sure server context is closed with a right brace
	isRightBrace(token);

	defaultInitUnfilledServerFields();

}

void ConfigParser::parseLocation() {

	Token token = mLexer.next();

	// next token should be suitable for a route argument of
		// the location directive
	isNotEOS(token);
	isNotAKeyword(token);

	// adds leading directory slash to route if missing
	checkPathLeadingSlash(token.value);

	// saves path
	const Path route = token.value;

	token = mLexer.next();
	// makes sure that a scope was actually opened
	isLeftBrace(token);

	addNewLocation(route);
	updateLocationRef(route);

	mLocationRef->route = route;

	// enters location's context and parses directives within it
	token = mLexer.next();
	// while there are more tokens and a right curly brace hasn't been found
	while (token.type != Token::EOS
		&& token.type != Token::RB) {
		
		switch(token.type) {
			case Token::ALLOW:
				parseMethods();
				break;
			case Token::RDR:
				parseRedirection();
				break;
			case Token::ROOT:
				parseRoot();
				break;
			case Token::AUTOIN:
				parseAutoIndex();
				break;
			case Token::DFLT:
				parseDefault();
				break;
			case Token::CGI:
				parseCGI();
				break;
			case Token::UPLOAD:
				parseUpload();
				break;
			default:
				handleParsingError(token);
		}
		token = mLexer.next();

	}

	// makes sure location context is closed with a right brace
	isRightBrace(token);

	defaultInitUnfilledLocationFields();

}

void ConfigParser::addNewLocation(const Path& path) {

	LocationContext newLocation;
	mServerRef->locations[path] = newLocation;

}

void ConfigParser::updateLocationRef(const Path& path) {

	LocationsCollection::iterator search =
		mServerRef->locations.find(path);
	// if location wasn't found
	if (search == mServerRef->locations.end()) {
		std::string error(path);
		error += " couldn't be found in locations";
		throw std::runtime_error(error);
	}

	mLocationRef = &search->second;

}

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

void ConfigParser::addNewServer() {

	ServerContext tmp;
	mServers.push_back(tmp);

}

void ConfigParser::parseMethods() {

	Token token = mLexer.next();
	// there should be at least one method supplied to the allow_methods
		// directive, after that we can parse more methods if they
		// were supplied
	isMethod(token);
	addMethod(token);

	token = mLexer.next();
	// parses more methods
	while (token.type == Token::METHOD) {
		addMethod(token);
		token = mLexer.next();
	}

	isSemiColon(token);

}

void ConfigParser::addMethod(const Token& token) {

	if (token.value == "GET")
		mLocationRef->get = true;
	else if (token.value == "POST")
		mLocationRef->post = true;
	else if (token.value == "DELETE")
		mLocationRef->del = true;
	
}

void ConfigParser::parseRedirection() {

	// sets status code classes that are supported by the redirect directive
	std::vector<StatusCodeClass> supportedClasses(1);
	supportedClasses[0] = 3;
	
	Token token = mLexer.next();
	// this token must be a status code,
		// so it makes sense to have a NUM type
	isNum(token);

	if (isStatusCodeValid(supportedClasses, token.value) == false)
		handleParsingError(token);

	// converts token's value to StatusCode type value
	// checks for conversion exceptions
	StatusCode code = 0;
	try {
		code = strToNum<StatusCode>(token.value);
	}
	catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		handleParsingError(token);
	}

	// checks if the redirect status code is supported
		// (even if it's a 3xx class it might not be
		// supported or might be just some meaningless
		// status code)
	if (StatusCodeHandler::isRedirection
			(static_cast<StatusCodeType>(code)) == false) {

		std::cerr << code << " is either an unsupported or meaningless"
			" or meaningless redirection code\n";
		handleParsingError(token);

	}

	Path redirectPath;
	// expects next token to be a path, parses it
	// and saves it in redirectPath;
	parsePath(redirectPath);

	// The path always contains a leading slash. so to not remove the only
		// character available  this operatiorn happens only if the path
		// has more than one character
	// So in the case of redirection, the leading slash will be removed so
		// that the browser matches that path with the same location of the
		// URL that returned the redirection.
	// Another reason for removing that slash, is to enable redirections to
		// other urls outside the server (full urls that begin with http://)
	if (redirectPath.size() > 1) {
		redirectPath.erase(0, 1);
	}

	// saves redirectPath:code pair in current location context
	mLocationRef->redirection.first = code;
	mLocationRef->redirection.second = redirectPath;

}

void ConfigParser::parsePath(Path& pathLoc) {

	// checks that next token is appropriate for a path/route value
	Token token = mLexer.next();
	isNotEOS(token);
	isNotAKeyword(token);

	// adds leading slash if missing
	checkPathLeadingSlash(token.value);

	pathLoc = token.value;

	token = mLexer.next();
	isSemiColon(token);

}

void ConfigParser::parseRoot() {

	parsePath(mLocationRef->root);

	// if the root path is a directory appends a slash to it because in case
		// there was no slash there will be issues when creating full paths
		// from the root.
	// For example, root = /path, location = /, the request path = /file.
		// it will match with loccation and then the location route will
		// be replaced by the root which will lead to the following path:
		// /pathfile. As you can see this is clearly wrong
	if (isDir(mLocationRef->root))
		mLocationRef->root += '/';

}

void ConfigParser::parseDefault() {
	parsePath(mLocationRef->defaultFile);
}

void ConfigParser::parseCGI() {
	
	// first argument to cgi directive is a file extension
	Token token = mLexer.next();
	isExtension(token);

	if (!mConfig.isCGIExtensionSupported(token.value)) {
		std::cerr << token.value << " extension";
		std::cerr << " isn't a supported CGI extension\n";
		handleParsingError(token);
	}

	// extracts the extension without the dot
	const Extension extension = token.value.substr(1);

	// next argument is a path to an executable that runs
		// program files with the extension found above
	Path CGIExecutableLocation;
	parsePath(CGIExecutableLocation);

	// checks if the provided cgi executable
		// is readable and executable
	if (isPathRead(CGIExecutableLocation) == false
		|| isPathExec(CGIExecutableLocation) == false) {

		std::cerr << "parseCGI(): executable path '"
			<< CGIExecutableLocation << "' should "
			<< "exist and be readable and executable\n";
		handleParsingError(token);

	}

	// finally stores the parsed data
	mLocationRef->supportedCGIs[extension] = CGIExecutableLocation;

	// no need to check for semi-colon since it was
		// already checked in the call to parsePath()

}

void ConfigParser::parseUpload() {

	parsePath(mLocationRef->uploadRoute);

	// checks if path is a directory
	if (!isDir(mLocationRef->uploadRoute)) {
		std::string error = mLocationRef->uploadRoute;
		error += " is not a valid directory";
		mServers.clear();
		throw std::runtime_error(error);
	}

}

void ConfigParser::parseAutoIndex() {

	Token token = mLexer.next();
	isSwitch(token);

	if (token.value == "on")
		mLocationRef->autoindex = true;
	else
		mLocationRef->autoindex = false;

	token = mLexer.next();
	isSemiColon(token);

}

void ConfigParser::isToken(const Token& token, Token::Type type) {

	if (token.type != type)
		handleParsingError(token);

}

void ConfigParser::isExtension(const Token& token) {
	isToken(token, Token::EXT);
}
void ConfigParser::isMethod(const Token& token) { isToken(token, Token::METHOD);
}

void ConfigParser::isLeftBrace(const Token& token) {
	isToken(token, Token::LB);
}

void ConfigParser::isRightBrace(const Token& token) {
	isToken(token, Token::RB);
}

void ConfigParser::isSemiColon(const Token& token) {
	isToken(token, Token::SM_COL);
}

void ConfigParser::isNum(const Token& token) {
	isToken(token, Token::NUM);
}

void ConfigParser::isSwitch(const Token& token) {
	isToken(token, Token::SWITCH);
}

void ConfigParser::isNotAKeyword(const Token& token) {

	switch (token.type) {
		case Token::SRV_BLK:
		case Token::SRV_NAME:
		case Token::LISTEN:
		case Token::ERR_PAGE:
		case Token::CLIENT_MAX:
		case Token::LOC:
		case Token::ALLOW:
		case Token::RDR:
		case Token::ROOT:
		case Token::AUTOIN:
		case Token::DFLT:
		case Token::CGI:
		case Token::UPLOAD:
		case Token::LB:
		case Token::RB:
		case Token::SM_COL:
			handleParsingError(token);
		default:
			return;
	}

}

void ConfigParser::isNotEOS(const Token& token) {

	if (token.type == Token::EOS)
		handleParsingError(token);

}

void ConfigParser::defaultInitUnfilledServerFields() {

	if (mServerRef->hostname.empty())
		mServerRef->hostname = ServerContext::defaultHostname;
	
	if (mServerRef->port.empty())
		mServerRef->port = ServerContext::defaultPort;

}

void ConfigParser::defaultInitUnfilledLocationFields() {

	// if no method was allowed for location, then the
		// the default is to enable get method
	if (!mLocationRef->get && !mLocationRef->post
		&& !mLocationRef->del)
		mLocationRef->get = true;

	// if no root path was specified,
		// initiliazes it to current diretory
	if (mLocationRef->root.empty()) {
		mLocationRef->root = getCurrentDir();
		// see parseRoot() for explanation
		mLocationRef->root += '/';
	}

}

bool ConfigParser::isStrNumerical(const std::string& str) {
	
	for (std::string::const_iterator it = str.begin();
		it != str.end(); ++it)
		if (std::isdigit(*it) == false)
			return false;
	
	return true;

}

void ConfigParser::parseServerName() {

	Token token = mLexer.next();

	// checks if token is a valid name
	isNotEOS(token);
	isNotAKeyword(token);

	mServerRef->server_name = token.value;

	token = mLexer.next();

	// makes sure directive ends in a semi-colon
	// this applies to all directives
	isSemiColon(token);

}

void ConfigParser::parseListen() {

	Token token = mLexer.next();

	// checks if token is a valid string
	isNotEOS(token);
	isNotAKeyword(token);

	// makes sure target fields are empty
	mServerRef->hostname.clear();
	mServerRef->port.clear();

	// position of the ':' that separate hostname and port
	std::string::size_type colonPos = token.value.find(':');
	// if a colon was found
	if (colonPos != std::string::npos) {
		// makes sure that colon is in the middle
		if (colonPos == 0 || colonPos == token.value.size() - 1)
			handleParsingError(token);
		// sets hostname to string before colon
		mServerRef->hostname = token.value.substr(0, colonPos);
		// sets port to string after colon
		mServerRef->port = token.value.substr
			(colonPos + 1, token.value.size() - (colonPos + 1));
	}
	// it's a port value then
	else if (isStrNumerical(token.value))
		mServerRef->port = token.value;
	// otherwise it's a hostname
	else
		mServerRef->hostname = token.value;

	token = mLexer.next();
	isSemiColon(token);

}
		
void ConfigParser::parseErrorPage() {

	// set status code classes that are supported by the error_page directive
	std::vector<StatusCodeClass> supportedClasses(2);
	supportedClasses[0] = 4;
	supportedClasses[1] = 5;
	
	Token token = mLexer.next();
	// this token must be a status code,
		// so it makes sense to have a NUM type
	isNum(token);

	if (isStatusCodeValid(supportedClasses, token.value) == false)
		handleParsingError(token);

	// converts token's value to StatusCode type value
	// checks for conversion exceptions
	StatusCode code = 0;
	try {
		code = strToNum<StatusCode>(token.value);
	}
	catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		handleParsingError(token);
	}

	// parses path and creates a pair of status code and a path as
		// the corresponding error page in current server context
	parsePath(mServerRef->errorPages[code]);

}

void ConfigParser::parseClientBodySizeMax() {

	Token token = mLexer.next();
	// size must be expressed as a positive number
	isNum(token);

	// converts token's value to number of type Size
	try {
		mServerRef->clientBodySizeMax =
			strToNum<Size>(token.value);
	}
	catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		handleParsingError(token);
	}

	token = mLexer.next();
	isSemiColon(token);

}

bool ConfigParser::isStatusCodeValid
	(const std::vector<StatusCodeClass>& statusCodeClasses,
	const std::string& statusCodeStr) {

	// checks if the token's value matches any of the status code classes
		// and that the status code supplied (token's value) has exactly 3 digits
	if (!isStatusCodeMatchClasses(statusCodeClasses, statusCodeStr)
		|| statusCodeStr.length() != 3) {

		if (statusCodeStr.length() != 3)
			std::cerr << "status code needs to have exactly 3 digits\n";
		else {
			std::cerr << "status code can only be one of these classes: ";
			for (size_t i = 0; i < statusCodeClasses.size(); ++i) {
				// convert form  StatusCodeClass type to char type
				std::cerr << static_cast<char>('0' + statusCodeClasses[i])
					<< "xx";
				// if there are more elements
				if (i + 1 < statusCodeClasses.size())
					std::cerr << ", ";
			}
			std::cerr << '\n';
		}
		return false;

	}
	return true;

}

bool ConfigParser::isStatusCodeMatchClasses
	(const std::vector<StatusCodeClass>& statusCodeClasses,
	const std::string& statusCodeStr) {

	if (statusCodeStr.empty())
		return false;

	bool classMatched = false;
	for (std::vector<StatusCodeClass>::const_iterator it = statusCodeClasses.begin();
		it != statusCodeClasses.end(); ++it) {
		// status code class is given as a single digit int,
			// so it is converted to its digit char equivalent so
			// that it's compared against the first character of the
			// status code string
		if ((*it + '0') == statusCodeStr[0]) {
			classMatched = true;
			break;
		}

	}
	return classMatched;

}

void ConfigParser::checkPathLeadingSlash(std::string& path) {
	
	if (path.empty()) {
		path += '/';
		return ;
	}

	if (path[0] != '/')
		// prepends one slash to path
		path.insert(0, 1, '/');

}
