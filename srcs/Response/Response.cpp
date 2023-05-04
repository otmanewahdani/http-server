/* this file contains the implementation of the Response class
 */

#include <Response.hpp>

const size_t Response::mSendSize = 1024;

const size_t Response::mReadSize = 1024;

Response::Response(Socket socket,
	const Request& request, ConstServerRef server)
	: mSocket(socket)
	, mRequest(request)
	, mServer(server)
	, mLocation()
	, mDone()
	, mStart()
	, mIsSeparator(true) {}

bool Response::isWrite() const {
	return mDone;
}

void Response::proceedWithSocket() {
		sendResponse();
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
	
	// if there is no error page
		// to be generated
	// generates the appropriate response
	if (isError() == false) {

		// checks the reponse type and sets the
			// data needed for that response type
		// if else branching to stop checking when
			// a response type was determined
		if (isContent())
			;
		else if (isDefault())
			;
		else if (isRedirect())
			;
		else if isCGI()
			;
		else if (isAutoIndex())
			;
		else if (isDelete())
			;

		openBodyStream();

		// checks if an error happened while processing
			// and preparing the response so that an
			// error respone could be sent
		isError();

	}

	generateStatusLine();

	generateHeaders();

	addHeadersBodySeparator();

}
