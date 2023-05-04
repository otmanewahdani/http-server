/* this file contains the implementation of the Response class
 */

#include <Response.hpp>

const size_t Response::mSendSize = 1024;

Response::Response(Socket socket,
	const Request& request, ConstServerRef server)
	: mSocket(socket)
	, mRequest(request)
	, mServer(server)
	, mLocation()
	, mDone()
	, mStart() {}

bool Response::isWrite() const {
	return false;
}

void Response::proceedWithSocket() {
}

void Response::start(ConstLocPtr location) {

	if (mStart) {
		throw std::runtime_error("Response::start(): "
			"the response process already started");
	}
	mStart = true;

	mLocation = location;

	mStatusCode = mRequest.getStatusCode();

	// This mandatory header is always
		// present in the response message
	mHeaders["connection"] = "close";

	generateReponse();

}

void generateResponse() {
}
