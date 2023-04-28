/* this file contains the definition of RequestChecker class
 *  this helper class is responsible for checking the validity of a
 *  Request object after it has parsed its request headers, and
 *  determining the type of request 
 *  After determining the request type, it is set in the request object
 *  	along with the status code
 */

#pragma once

// the include of this class is at the bottom
	// of this file with the explanation
class Request;

#include <Config.hpp>
#include <utils.hpp>
#include <StatusCodeHandler.hpp>

class RequestChecker {

	public:
		/******* alias types *******/
		typedef Config::StatusCode StatusCode;
		typedef Config::Path Path;
		typedef Config::ConstLocPtr ConstLocPtr;
		typedef StatusCodeHandler::StatusCodeType
			StatusCodeType;

		/******* public member functions *******/
		// takes the request to be checked
		RequestChecker(Request& request);

		// returns true if the request is valid
			// sets the response status code
			// and the request type according
			// to the nature of the request
		bool isValid();
	
	private:
		/******* private member functions *******/
		// checks if the requested method
			// is allowed for the requested path
		// sets request's status code appropriately if the
			// requested method isn't allowed
		bool isMethodAllowed();

		/******* checkers *******/
		/* all these checkers check the type of request
		 * and set request type of the request object
		 * to that type if it's true and its associated
		 * status code
		 */
		// checks if there is redirection associated
			// with the path
		bool isRedirect();

		// checks if it's a cgi request
		bool isCGI();

		// checks if it's an
			// autoindex request
		bool isAutoIndex();

		// checks if it's an upload request
		bool isUpload();

		// checks if a default file should be served
			// and if there is one that was set
		bool isDefault();

		// checks if a static file should be served
		bool isContent();

		/******* private member objects *******/
		Request& mRequest;

		// stores the location containing the
			// config data of the requested path
		ConstLocPtr mLocation;

		// stores if the requested path exists
		bool mIsPath;

		// stores if the requested path is a directory
		bool mIsDir;

};

// this include directive was brought down here because the included file
// includes this file (RequestChecker.hpp) as well and relies on the full definition
// of the RequestChecker class
#include <Request.hpp>
