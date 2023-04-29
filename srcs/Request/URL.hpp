/* this file contains the definition of the URL class.
 * This class is reponsible for parsing a url string
 * and checking the validity of the url. A url is invalid
 * if it conatins any bad characters or if the path does 
 * not match with any location within the server that is
 *  passed to the constructor .
 * The URL is organized then into a path, full path and
 * a query string . in case of ivalid url a statusCode 
 * is set to indicate the type of the error
 * a bad charachter means that it's either unprintable
 * or belongs to mForbiddenChars collection
 */

#pragma once

#include <Config.hpp> 
#include <StatusCodeHandler.hpp>

class URL {

	public:
		/******* alias types *******/
		typedef Config::ConstServerRef ConstServerRef;
		typedef Config::ConstLocPtr ConstLocptr;
		typedef std::string  Path;
		typedef std::string FullPath;
		typedef std::string QueryString;
		typedef StatusCodeHandler::StatusCodeType StatusCodeType;

		/******* public member functions *******/
		// server containing location info about
			// the requested resource
		URL(ConstServerRef server);

		// it takes a url and parse it to a 
			//valid path , query string and full path
		void parse(const std::string& url);

		// returns true if the url is valid 
			// means it doesn't contain any bad characters
			// and it matches a valid location in 
			// the mServer 
		bool isValid() const;

		// returns the url path
		const Path& getPath() const;

		// returns the full path of the requested resource
		const FullPath& getFullPath() const;

		// returns the query string part from the url
		const QueryString& getQueryString() const;

		// returns a status code that indicate the url validity
		const StatusCodeType& getStatusCode() const;

		// returns the most specific matched 
			// location with the url path
		ConstLocptr getLocation() const;
		

	private:
		/******* private member objects *******/
		// server from where will get the matching location 
			// of the requested resource
		ConstServerRef mServer;

		// url path
		Path mPath;

		// stores the full path of the requested resource
		FullPath mFullPath;

		// url query string
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

		// contains the unallowed characters in a url
		static const std::string mForbiddenChars;

		// state the validitiy of the url
		StatusCodeType mStatusCode;

		/******* private member functions *******/
		// parse the url to a valid path and 
			// a valid query string if it exists
		void parseUrl(const std::string& url);

		// parse the url path and check if it has any bad char
			// or didn't match a location within mServer
		// it returns the position of the query 
			// string in the url if it exists
		// sets BAD_REQUEST error and invalid url if a 
			// bad character was found
		// sets NOT_FOUND error and invalid url
			// if it didn't match any location
		// on error, std::string::npos is returned
		std::string::size_type
			parsePath(const std::string& url);

		// parse the query string if it exists in the url
		// sets BAD_REQUEST error and invalid url if a 
			// bad character was found
		void parseQueryString(const std::string& url,
			const size_t queryPos);
		

		// construct the full path of the requested resource 
		void addFullPath();

		// sets the status code if an error occured
			// and set mValid to false and clear the 
			// mvariables to prevent getting wrong data
		void setErrorStatusCode(StatusCodeType statusCode);

		// checks if a char is a bad character
		bool isForbiddenChar(const char c);
};
