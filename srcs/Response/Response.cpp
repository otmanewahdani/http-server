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

void Response::generateResponse() {
	
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

void Response::sendResponse() {

	// there are still body bytes to
		// be sent
	if (mBodyStream.eof() == false) {

		char bodyBuf[mReadSize];

		// fill readBodyBytes buffer from body stream
		mBodyStream.read(bodyBuf, mReadSize);

		// if it failed before reaching end of file
			// stops sending the response
		if (mBodyStream.eof() == false && mBodyStream.fail())
			mDone = true;
		else {
			// appends the number of read bytes from the stream
				// to the send buffer
			mBuffer.append(bodyBuf, mBodyStream.gcount());
		}

	}
	// after checking the file stream,
		// checks if there are still
		// bytes in the buffer
	else if (mBuffer.empty())
			mDone = true;

	// if sending wasn't stopped before
		// because of some fatal error
		// or sending is complete
	if (mDone == false) {

		// if there are less bytes in the buffer
			// than the regular send size, use
			// the buffer size for sending
		const std::string::size_type sendSize
			= mBuffer.size() > mSendSize ?
			mSendSize : mBuffer.size();

		const ssize_t sentBytes = write
			(mSocket, mBuffer.c_str(), sendSize);

		// if sending failed
		if (sentBytes == -1)
			mDone = true;
		else {
			// removes the sent bytes
			mBuffer.erase(0, sentBytes);
		}

	}

}
