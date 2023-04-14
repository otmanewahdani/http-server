/* this file includes the definition of ConfigParser class which represents the complete logic
 * 	that parses the configuration file (given as an input stream) of the web server
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

class ConfigParser {
	
	public:
		/*******  alias types *******/
		typedef Config::Servers Servers;
		typedef Config::ServerContext ServerContext;
		typedef Config::LocationContext LocationContext;
 
		/******* public member functions *******/
		// configuration stream is parsed through this constructor
		ConfigParser(std::istream& input, Config& config);
	
	private:
		/******* private member objects *******/
		Config& mConfig;
		
		Servers& mServers;

		Servers::iterator mServerIter;

		LocationsCollection::iterator mLocationIter;

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

		void handleParsingError();

		void updateServerIterator();

		void updateLocationIterator();

		void addNewServer();

		void addNewLocation();

};
