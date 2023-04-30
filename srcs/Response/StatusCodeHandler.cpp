#include <StatusCodeHandler.hpp>

// static member object declaration
StatusCodeHandler::StatusCodeMap
	StatusCodeHandler::mStatusCodesData;

void StatusCodeHandler::initializeStaticData() {
	
	StatusCodePair statusCodePair("200", "OK");
	mStatusCodesData[OK] = statusCodePair;
	
	statusCodePair = std::make_pair("301", "Moved Permanently");
	mStatusCodesData[REDIRECT_MOVE] = statusCodePair;
	
	statusCodePair = std::make_pair("400", "Bad Request");
	mStatusCodesData[BAD_REQUEST] = statusCodePair;

	statusCodePair = std::make_pair("404", "Not Found");
	mStatusCodesData[NOT_FOUND] = statusCodePair;

	statusCodePair = std::make_pair("405", "Method Not Allowed");
	mStatusCodesData[METHOD_ALLOW] = statusCodePair;

	statusCodePair = std::make_pair("414", "Request-URI Too Long");
	mStatusCodesData[URI_LONG] = statusCodePair;

	statusCodePair = std::make_pair("413", "Request Entity Too Large");
	mStatusCodesData[ENTITY_LARGE] = statusCodePair;

	statusCodePair = std::make_pair("501", "Not Implemented");
	mStatusCodesData[NOT_IMPLEMENTED] = statusCodePair;

}

const StatusCodeHandler::StatusCodePair&
	StatusCodeHandler::getStatusCodeInfo
	(StatusCodeType code) {

	StatusCodeMap::const_iterator codeInfo
		= mStatusCodesData.find(code);

	// if code wasn't found
	if (codeInfo == mStatusCodesData.end()) {

		const std::string errorMsg =
			std::string("getStatusCodeInfo(): "
			"no such status code '")
			+ toString(code) + '\'';

		throw std::runtime_error(errorMsg);

	}

	// returns status code pair
	return codeInfo->second;

}
