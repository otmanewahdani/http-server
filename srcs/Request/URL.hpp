/* this file contains the definition of the URL class.
 * This class is reponsible for parsing a url string
 * and checking the validity of the url. A url is invalid
 * if the path does not match with any location within
 * the server that is passed to the constructor
 * The URL is organized then into a path, full path and
 * a query string . in case of ivalid url a statusCode 
 * is set to indicate the type of the error
 */

#pragma once

#include <Config.hpp>
#include <StatusCodeHandler.hpp>

class URL {

	public:
		/******* alias types *******/
		typedef Config::ConstServerRef ConstServerRef;
		typedef Config::ConstLocPtr ConstLocptr;
		typedef std::sting  Path;
		typedef std::string FullPath;
		typedef std::string QueryString;

		/******* public member functions *******/
		// server containing location info about
			// the requested resource
		URL(ConstServerRef server);

		void parse(const std::string& url);

		// returns true if the url path is a valid path
			// means it matchs a valid location in 
			// the mServer and doesn't contain 
			// any bad characters
		bool isValid();

		// returns the url path
		const Path& getPath();

		// returns the full path of the requested resource
		const FullPath& getFullPath();

		// returns the query string part from the url
		const QueryString& getQueryString();
		

	private:
		/******* private member objects *******/
		// server from where will get the matching location 
			// of the requested resource
		ConstServerRef mServer;

		Path mPath;

		// stores the full path of the requested resource
		FullPath mFullPath;

		QueryString mQuery;

		// indicate if the url path is valid 
			// or an error occurred
		bool mValid;

		// is true if the parse() function was already 
			// used to parse the current request url 
		// is needed to prevent unexpected calls of 
			// the parse() function with the same request 
		bool mParsed;

		// the most specific matched location with the url path
		ConstLocptr mLocation;

		/******* private member functions *******/
		// checks if the url contains any bad character
		// parses the url to a valid path 
			// and query string if it exists
		void parseUrl(const std::string& url);

		// add the query string if it exists in the url
		void addQueryString(const std::string& url, size_t pos);

		// takes the lenght of the url path
			// add the url path if it matches with a location in mServer
		void addPath(const std::string& url, size_t len);

		// construct the full path of the requested resource 
			// from the url path and the matched location root
		void addFullPath();

		// returns the query string pos in the url if it exists
			// and std::string::npos if not
		size_t getQueryPos(const std::string& url);

		// search in mServer for the most specific
			// matched location with the url path
		// returns NULL if no match is found
		ConstLocPtr getMatchedLocation();

		// checks if the url contains any bad characters 
			// set bad request error if it's found
			// and mvalid to false
		void checkBadCharacters(const std::string& url);
};
