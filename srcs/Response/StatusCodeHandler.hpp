/* this file contains the definition of the StatusCodeHandler class
 * It is responsible for storing and serving different http status
 * codes through an interface thta depends on a set of defined
 * enum values from the StatusCodeType enum
 */

#pragma once

#include <string>

class StatusCodeHandler {

	public:
		/******* nested types *******/
		enum StatusCodeType {
			OK = 200,
			NOT_FOUND = 404,
			BAD_REQUEST = 400,
		};

		/******* alias types *******/
		typedef unsigned short StatusCode;
		// status code in string format
		typedef std::string StatusCodeStr;
		// phrase that explains status code
		typedef std::string ReasonPhrase;
		typedef std::pair<StatusCodeStr, ReasonPhrase>
			StatusCodePair;
		typedef std::map<StatusCode, StatusCodePair>
			StatusCodeMap;

		/******* public member functions *******/
		// returns pair containing status code in string
			// format and its reason phrase
		static const StatusCodePair& getStatusCodeInfo
			(StatusCodeType code);
	
	private:
		static const StatusCodeMap statusCodesData;

};
