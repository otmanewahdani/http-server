/* this file includes the definition of ConfigParser class which represents the complete logic
 * 	that parses the configuration file (given as an input stream) of the web server
 * This implementation is based on the recursive descent concept
 * The ConfigParser parses the configuration text by passing an input
	stream to its constructor and a Config class that gets populated
	by the stream's content (obviously :p). An exception is thrown if
	a parsing error was encountered.
 * It relies on the ConfigLexer class for lexical analysis
 */

#pragma once

#include <Config.hpp>
#include <ConfigLexer.hpp>
#include <istream>
#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>

class ConfigParser {
	
	public:
		/*******  alias types *******/
		typedef Config::Servers Servers;
		typedef Config::LocationsCollection LocationsCollection;
		typedef Config::StatusCodesWithPaths StatusCodesWithPaths;
		typedef Servers::iterator ServerReference;
		typedef LocationsCollection::iterator LocationReference;
		// for example: the class of the status code 200 is 1
		typedef unsigned char StatusCodeClass;
 
		/******* public member functions *******/
		// configuration stream is parsed through this constructor
		ConfigParser(std::istream& input, Config& config);
	
	private:
		/******* private member objects *******/
		Config& mConfig;
		
		Servers& mServers;

		ServerReference mServerRef;

		LocationReference mLocationRef;

		ConfigLexer mLexer;

		/******* private member functions *******/
		/* the parse*() member functions parse directives in their scope.
		 * if a directive or token doesn't belong to that scope, a call
		 *	to handleParsingError() with the unexpected token is made
		 * For example, parseGlobal() looks for directive
		 *	that reside in the global context like 'server'.
		 *	if a 'location' directive is encountered (which resides
		 *	in the server scope), then it calls handleParsingError()
		 *	with the token 'location'.
		 */

		// looks for any directives or keywords that exist
		// 	in the global space ( like server)
		void parseGlobal();

		// looks for any directives or keywords that exist
		// 	in the server context ( like location)
		// also creates a new sever
		void parseServer();

		// looks for any directives or keywords that exist
		// 	in the location context ( like root)
		// also creates a new location within a server
		void parseLocation();

		// clears the mServers structure
		// and throws std::runtime_error with an error string composed
			// of the unexpected token 'lastToken'
		void handleParsingError(const Token& lastToken);

		// updates mServerRef to point to the latest server block created
		void updateServerRef();

		void updateLocationRef();

		// creates a new server block and adds it to mServers
		void addNewServer();

		void addNewLocation();

		// calls handleParsingError if token's type value doesn't match
			// the second argument
		// e.g.: if token->type == LB and second argument is LB as well
			// then it matches and handleParsingError() is not called
		void isToken(const Token& token, Token::Type type);

		// calls isToken
		void isLeftBrace(const Token& token);

		// same as above
		void isRightBrace(const Token& token);

		// same as above
		void isSemiColon(const Token& token);

		// same as above
		void isNum(const Token& token);

		// if token is a directive or a keyword like '{' , server or listen,
			// calls handleParsingError
		void isNotAKeyword(const Token& token);

		// if EOS, calls handleParsingError()
		void isNotEOS(const Token& token);

		// initializes mServerRef's fields that were not supplied by
			// the user to their default values
		void defaultInitUnfilledServerFields();

		// initializes mLocationRef's fields that were not supplied by
			// the user to their default values
		void defaultInitUnfilledLocationFields();

		// returns true if str is only composed of digits
		bool isStrNumerical(const std::string& str);

		// it's called by functions like parseErrorPage() that parse
			// directives which consists of a status code and a path
			// arguments
		// statusCodeClass parameter is a byte-sized integer that
			// indicates the class of the status code that will be
			// parsed. if a status code with a different class is found 
			// then, an error is printed to stderr and
			// handleParsingError() is called
		// the saveStructure parameter is where the parsed pair (status
			// code, path) will be saved
		void parseStatusCodesDirectives(StatusCodeClass statusCodeClass,
			StatusCodesWithPaths& saveStructure);

		/******* functions that parse specific directives *******/
		void parseServerName();

		void parseListen();
		
		void parseErrorPage();

};
