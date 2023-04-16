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
#include <sstream>
#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <limits>
#include <utils.hpp>

class ConfigParser {
	
	public:
		/*******  alias types *******/
		typedef Config::Servers Servers;
		typedef Config::LocationsCollection LocationsCollection;
		typedef Config::StatusCodesWithPaths StatusCodesWithPaths;
		typedef Config::StatusCode StatusCode;
		typedef Config::ServerContext ServerContext;
		typedef Config::LocationContext LocationContext;
		typedef Config::Size Size; 
		typedef Config::Path Path;
		typedef Config::Extension Extension;
		typedef Servers::iterator ServerReference;
		typedef LocationsCollection::mapped_type* LocationReference;
		// for example: the class of the status code 200 is 2
		typedef unsigned char StatusCodeClass;
 
		/******* public member functions *******/
		// configuration stream is parsed through this constructor
		// output is stored in config
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

		// creates a new server block and adds it to mServers
		void addNewServer();

		// updates  mLocationRef to the location in mServerRef
			// containing the Path argument
		// throws a std::runtime_error if path wasn't found
			// mServerRef's locations
		void updateLocationRef(const Path& path);

		// creates a new location in mServerRef
		// replaces any location with the same path
		void addNewLocation(const Path& path);

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

		// same as above
		void isMethod(const Token& token);

		// same as above
		void isExtension(const Token& token);

		// checks if token is of type
			// SWITCH (has value of 'on' or 'off)
		// same as above
		void isSwitch(const Token& token);

		// if token is a directive or a keyword like '{' , server or listen,
			// calls handleParsingError
		void isNotAKeyword(const Token& token);

		// if EOS, calls handleParsingError()
		void isNotEOS(const Token& token);

		// adds the token (that is presumably of type METHOD)
			// to the current location 
		void addMethod(const Token& token);

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
		// the statusCodeClasses vector parameter contains all status code
			// classes that are supported by the directive
		// statusCodeClass template parameter is a byte-sized integer that
			// indicates the class of the status code that will be parsed.
		// the saveStructure parameter is where the parsed pair (status
			// code, path) will be saved
		// if the parsed status code's class doesn't match any class in
			// statusCodeClasses vector, an error is printed to stderr and
			// handleParsingError() is called
		// if conversion of the status code failed, an error message
			// is printed to stderr and handleParsingError() is called
		void parseStatusCodeDirective
			(const std::vector<StatusCodeClass>& statusCodeClasses,
			StatusCodesWithPaths& saveStructure);

		// checks if status code string belong to any of the status code classes
		// read function above to understand these terms
		bool isStatusCodeMatchClasses
			(const std::vector<StatusCodeClass>& statusCodeClasses,
			const std::string& statusCodeStr);

		// prints a specific error message and returns
			// false if status code is not valid
		bool isStatusCodeValid
			(const std::vector<StatusCodeClass>& statusCodeClasses,
			const std::string& statusCodeStr);

		// converts string to integral type
		// string must be a positive number (0 included)
		// if number in str is too large to fit in template type
			// std::overflow_error is thrown.
		// std::runtime_error is thrown for other kinds of errors
		template<class Number>
		Number convertStrToNumber(const std::string& str);

		// adds a leading slash to path if it's missing
			// otherwise this function has no effect
		void checkPathLeadingSlash(std::string& path);

		/******* functions that parse specific directives *******/
		// all these functions call handleParsingError() in case of errors
		void parseServerName();

		void parseListen();
		
		// calls parseStatusCodeDirective
		void parseErrorPage();

		// prints error msg to stderr if a conversion of the
			// size argument fails
		void parseClientBodySizeMax();

		// parses a path token and ensures that the
			// token that follows it is a semi-colon
		// if a leading slash doesn't exist in the path
			// it appends it
		// it is used by directives that that take a path
			// argument as their one and only argument
		// example of directives that use this util:
			// root, default, uplod
		// the pathLoc argument indicates where the
			// parsed path should be stored
		void parsePath(Path& pathLoc);

		// parses method values that are appropriate for
			// the allow_methods directive
		void parseMethods();

		// calls parseStatusCodeDirective
		void parseRedirection();

		// parses path used as an alias path of the location path
		void parseRoot();

		// parses the switch status of autoindex directive (on or off)
		void parseAutoIndex();

		// parses path specified in the default directive
		void parseDefault();

		// parses cgi directive
		// if a file extension isn't supported,
			// an error msg is printed to stderr
		void parseCGI();

		// parses upload route
		// if route isn't a directory
			// clears mServers and 
			// throws a std::runtime_error
		void parseUpload();

};
