/* this file contains the implementation of the Request class*/

#include <Request.hpp>

size_t Request::readSize = 1024;
size_t Request::requestLineSizeLimit = 2048;
size_t Request::headersSizeLimit = 8192;

Request::Request(Socket socket, ConstServerRef server)
	: mSocket(socket)
	, mServer(server)
	, mStage(REQUEST_LINE) // starts at request line stage
	, mMethod(UNSPECIFIED)
	, mLocation()
	, mHeaders(mBuffer)
	, mURL(server)
	, mStatusCode(StatusCodeHandler::OK)
	, mSocketOk(true) {}

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
}
