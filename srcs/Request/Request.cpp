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
	, mURL(server)
	, mStatusCode(StatusCodeHandler::OK)
	, mSocketOk(true) {}

void Request::initializeStaticData() {
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

	// not found
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

	// tries to parse the method and
		// the url and if either is invalid,
		// it stops here
	if (parseMethod() == false
		|| parseURL() == false)
		return;

	// moves to the headers stage
	mStage = HEADERS;

}

bool Request::parseMethod() {
	return true;
}

bool Request::parseURL() {
	return true;
}

void Request::moveFinStage
	(const StatusCodeType code) {

	mStage = FINISH;
	mStatusCode = code;

}

void Request::setSupportedMethods() {
}
