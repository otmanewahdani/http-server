/* this file contains the definition of the StatusCodeHandler class
 * It is responsible for storing and serving different http status
 * codes through an interface that depends on a set of defined
 * enum values from the StatusCodeType enum
 */

#pragma once

#include <string>
#include <map>
#include <utils.hpp>

class StatusCodeHandler {

	public:
		/******* nested types *******/
		enum StatusCodeType {
			OK = 200,
			REDIRECT_MOVE = 301,
			REDIRECT_FOUND = 302,
			REDIRECT_PROXY = 305,
			REDIRECT_TEMPORARY = 307,
			BAD_REQUEST = 400,
			NOT_FOUND = 404,
			METHOD_ALLOW = 405,
			ENTITY_LARGE = 413,
			URI_LONG = 414,
			SERVER_ERROR = 500,
			NOT_IMPLEMENTED = 501
		};

		/******* alias types *******/
		typedef unsigned short StatusCode;
		// status code in string format
		typedef std::string StatusCodeStr;
		// phrase that explains status code
		typedef std::string ReasonPhrase;
		typedef std::pair<StatusCodeStr, ReasonPhrase>
			StatusCodePair;
		typedef std::map<StatusCodeType, StatusCodePair>
			StatusCodeMap;

		/******* public member functions *******/
		// returns pair containing status code in string
			// format and its reason phrase
		// throws std::runtime_error if not found
		static const StatusCodePair& getStatusCodeInfo
			(StatusCodeType code);

		static void initializeStaticData();
	
	private:
		static StatusCodeMap mStatusCodesData;

};
