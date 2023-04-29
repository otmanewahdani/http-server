/* this file contains the implementation of the Request class*/

#include <Request.hpp>

size_t Request::mReadSize = 1024;
size_t Request::mRequestLineSizeLimit = 2048;
size_t Request::mHeadersSizeLimit = 8192;

std::map<std::string, Request::Method>
	Request::mSupportedMethods;

Request::Request(Socket socket, ConstServerRef server)
	: mSocket(socket)
	, mServer(server)
	, mStage(REQUEST_LINE) // starts at request line stage
	, mMethod(UNSPECIFIED)
	, mLocation()
	, mLastBuffSize()
	, mHeaders(mBuffer)
	, mURL(mServer)
	, mStatusCode(StatusCodeHandler::OK)
	, mRequestType(UNDETERMINED)
	, mRequestChecker(*this)
	, mSocketOk(true) {}

void Request::initializeStaticData() {
	setSupportedMethods();
}

bool Request::isRead() const {
	return (mStage != FINISH);
}

bool Request::isValid() const {

	if (mStage != FINISH) {
		const std::string errorMsg =
			"isValid(): hasn't finished parsing yet";
		throw std::runtime_error(errorMsg);
	}

	// it shouldn't be an error status code class
	return !(mStatusCode >= 400);

}

bool Request::isSocketOk() const {
	return mSocketOk;
}

Request::ConstLocPtr
	Request::getLocation() const {

	return mLocation;

}

const std::string& Request::getPath() const {
	return mURL.getPath();
}

const std::string& Request::getQueryString() const {
	return mURL.getQueryString();
}

const Request::HeaderValue*
	Request::getHeaderValue
	(const HeaderName& headerName) {

	return mHeaders.getHeaderValue
		(headerName);

}

const Request::Method& Request::getMethod() {
	return mMethod;
}

const std::string& Request::getFullPath() {
	return mURL.getFullPath();
}

const Request::StatusCodeType&
	Request::getStatusCode() {

	return mStatusCode;

}

const Request::RequestType&
	Request::getRequestType() {

	return mRequestType;

}

void Request::setStatusCode
	(const StatusCodeType& code) {

	mStatusCode = code;

}

void Request::setRequestType
	(const RequestType& requestType) {

	mRequestType = requestType;

}

void Request::proceedWithSocket() {

	// throws an error if the 
		// request was already done reading
	if (mStage == FINISH) {
		const std::string errorMsg = "Request::"
			"proceedWithSocket() : "
			"request is done reading";
		throw std::runtime_error(errorMsg);
	}

	// buffer on which read data
		// will be stored
	char readBuffer[mReadSize];

	// reads request data from socket
	ssize_t readAmount =
		read(mSocket, readBuffer, mReadSize);

	// failed to read from socket
	if (readAmount < 1) {
		mSocket = false;
		mStage = FINISH;
		return ;
	}

	// saves the current size of the buffer
		// before appending the new data to it
		// so that the next search operations start
		// from this position and therefore only
		// the new data is searched
	mLastBuffSize = mBuffer.size();

	// adds the read data to the
		// whole request buffer
	mBuffer.append(readBuffer, readAmount);
 
	parseRequest();

}

void Request::parseRequest() {

	switch (mStage) {

		case REQUEST_LINE:
			return parseRequestLine();
		case HEADERS:
			return parseHeaders();
		case BODY:
			return parseBody();
		case FINISH:
			return;

	}

}

void Request::parseRequestLine() {

	// starts searching from the last
		// data that was appened
	const std::string::size_type
		endOfLinePos = mBuffer.find
			("\r\n", mLastBuffSize);

	// end of line not found
	if (endOfLinePos == std::string::npos) {

		if (mBuffer.size() > mRequestLineSizeLimit)
			// Request-URI Too Long
			return moveFinStage
				(StatusCodeHandler::URI_LONG);
		// retries in the next call
		return;

	}

	// found but exceeds limit
	if (endOfLinePos > mRequestLineSizeLimit)
		// Request-URI Too Long
		return moveFinStage
			(StatusCodeHandler::URI_LONG);

	bool parseError = 
		(parseMethod(endOfLinePos) == false
		|| parseURL(endOfLinePos) == false);

	logRequest();

	// tries to parse the method and
		// the url and if either is invalid,
		// it stops here
	if (parseError)
		return;

	// moves to the headers stage
	mStage = HEADERS;

	// resets to 0 so that parseHeaders()
		// reads the remaining charcaters
	mLastBuffSize = 0;

	parseHeaders();

}

void Request::parseHeaders() {

	// searches for the new line that ends
		// the request header fields 
	const std::string::size_type 
		endOfHeadersPos = mBuffer.find
		("\r\n\r\n", mLastBuffSize);

	// end of header fields not found
	if (endOfHeadersPos == std::string::npos) {

		if (mBuffer.size() > mHeadersSizeLimit)
			// header fields entity Too Large
			return moveFinStage
				(StatusCodeHandler::ENTITY_LARGE);
		// retries in the next call
		return;

	}

	// found but exceeds limit
	if (endOfHeadersPos > mHeadersSizeLimit)
		//header fields entity Too Large
		return moveFinStage
			(StatusCodeHandler::ENTITY_LARGE);

	mHeaders.parse();

	// removes the headers bytes
		// after they were parsed
	mBuffer.erase(0, mHeaders.getSize());

	// resets to 0 so that the next
		// parsing function reads the
		// remaining charcaters
	mLastBuffSize = 0;

	// after finishing parsing the headers, it's time
		// to determine the request type
	determineRequestType();
	
}

void Request::determineRequestType() {

	// checks if the request is valid
	if (mRequestChecker.isValid() == false) {
		mStage = FINISH;
		return ;
	}

	// checks if the body needs to be read
		mStage = FINISH;

}

void Request::parseBody() {
}

bool Request::parseMethod
	(const std::string::size_type endOfLinePos) {
	
	// searches for the space that
		// ends the method token
	const std::string::size_type
		endOfMethodPos = mBuffer.find(' ');
	
	// if not found or it exceeds endOfLinePos
	if (endOfMethodPos == std::string::npos
		|| endOfMethodPos > endOfLinePos) {

		moveFinStage(StatusCodeHandler::BAD_REQUEST);
		return false;

	}

	// parses the method up to the terminating space
	const std::string parsedMethod =
		mBuffer.substr(0, endOfMethodPos);

	// searches for the parsed method
		// among the supported methods
	std::map<std::string, Method>::const_iterator
		method = mSupportedMethods.find(parsedMethod);

	// not a supported method
	if (method == mSupportedMethods.end()) {
		moveFinStage(StatusCodeHandler::BAD_REQUEST);
		return false;
	}

	// sets method to the found method
	mMethod = method->second;

	// deletes the parsed method from the buffer
	mBuffer.erase(0, endOfMethodPos);

	return true;

}

bool Request::parseURL(const std::string::size_type endOfLinePos) {

	// skips all the white spaces and 
		// returns the the start pos of the url
	const std::string::size_type urlStartPos 
		= mBuffer.find_first_not_of(" ");

	// search for the first space after the url in mBuffer
	const std::string::size_type foundSpacePos 
		= mBuffer.find(" ", urlStartPos);

	// set the end pos of the url to the space pos 
		// if it exists else set it to end of request line pos
	const std::string::size_type urlEndPos 
		= (foundSpacePos != std::string::npos && 
		foundSpacePos < endOfLinePos) ? foundSpacePos : endOfLinePos;

	// get the url substring from mBuffer
	const std::string url = mBuffer.substr
		(urlStartPos, urlEndPos - urlStartPos);
	mURL.parse(url);


	// if url is invalid move to finish stage
		// with the appropriate error status code
	if (mURL.isValid() == false) {
		moveFinStage(mURL.getStatusCode());
		return false;
	}

	// set the matched location
	mLocation = mURL.getLocation();
	
	// erase the request line from 
		// mBuffer ,including "\r\n"
	mBuffer.erase(0, endOfLinePos + 2);

	return true;

}

void Request::moveFinStage
	(const StatusCodeType code) {

	mStage = FINISH;
	mStatusCode = code;

}

void Request::logRequest() {

	// searches for the parsed method's
		// string format
	std::map<std::string, Method>::const_iterator
		method = mSupportedMethods.begin();
	for (; method != mSupportedMethods.end(); ++method) {
		if (method->second == mMethod)
			break;
	}

	// the method is invalid if isn't
		// set in the supported methods
	const std::string& methodStr =
		method == mSupportedMethods.end()
		? "INVALID METHOD" : method->first;

	Log::request(mSocket, methodStr,
		getPath(), getQueryString());

}

void Request::setSupportedMethods() {

	mSupportedMethods["GET"] = GET;
	mSupportedMethods["POST"] = POST;
	mSupportedMethods["DELETE"] = DELETE;

}
