/* this file contains the implementation of the Response class */

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
	, mIsSeparator(true)
	, mIsDelBodyFile() {}

Response::~Response() {

	// deletes the file containing the body
		// if it should be deleted
	if (mIsDelBodyFile)
		removeFile(mBodyFileName);

}

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

	generateResponse();

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
		else if (isCGI())
			;
		else if (isAutoIndex())
			;
		else if (isDelete())
			;

		openBodyStream();

		// checks if an error happened while processing
			// and preparing the response so that an
			// error respone could be sent
		// opens a stream of the error file to be sent
			// if there is one
		if (isError())
			openBodyStream();

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

bool Response::isRedirect() {
	
	// the request type is not a redirection
	if (mRequest.getRequestType() != Request::REDIRECT)
		return false;

	// add The Location response-header field 
		// which will be used to redirect the recipient
    	// to the specified path
	// the location path is retrieved from the redirection 
		// directive withing mLocation
	mHeaders["Location"] = mLocation->redirection.second;

	return true;

}

bool Response::isError() {

	// checks that the type of status code
		// represents an error
	if (mStatusCode < 400 || mStatusCode > 500)
		return false;

	// checks if there is a page set to be sent
		// for that error status code
	std::map<StatusCode, Path>::const_iterator errorPage
		= mServer.errorPages.find(mStatusCode);
	
	// if an error page is found
	if (errorPage != mServer.errorPages.end()) {

		const Path& errorPagePath = errorPage->second;

		ConstLocPtr location
			= mServer.matchLocation(errorPagePath);

		// if the error page matches a location
			// gets the full path of the page and
			// sets the file path of the body to it
		if (location) {
			// replaces the location prefix by the
				// root prefix to get the full path
			mBodyFileName =
				location->replaceByRoot(errorPagePath);
		}

	}

	return true;

}

const Response::Mimetype& 
	Response::setMimeType() {
	
	// the extension of mBodyFileName
	Extension bodyFileExtension;

	// search for mBodyFileName extension pos
	const std::string::size_type extentionPos 
		= mBodyFileName.rfind(".");
	
	// sets the extension of mBodyFileName 
	if (extentionPos != std::string::npos)
		bodyFileExtension 
			= mBodyFileName.substr(extentionPos + 1);

	// get the mime type associated to
		// the bodyFileExtension in mMimeTypes
	// if not found, sets to the default
		// mime type (application/octet-stream)
	const Mimetype fileMimeType 
		= mMimeTypes.getType(bodyFileExtension);

	return fileMimeType;

}
