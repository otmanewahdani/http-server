/* this file contains the implementation of the Response class */

#include <Response.hpp>

const size_t Response::mSendSize = 1024;

const size_t Response::mReadSize = 1024;

Response::Response(Socket socket, const Request& request,
	ConstServerRef server, const MimeTypes& mimeTypes)
	: mSocket(socket)
	, mRequest(request)
	, mServer(server)
	, mLocation()
	, mDone()
	, mStart()
	, mIsSeparator(true)
	, mIsDelBodyFile()
	, mMimeTypes(mimeTypes) {}

Response::~Response() {

	// deletes the file containing the body
		// if it should be deleted
	if (mIsDelBodyFile)
		removeFile(mBodyFileName);

}

bool Response::isWrite() const {
	return (mDone == false);
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
	mHeaders["Connection"] = "close";
	mHeaders["Server"] = "Flouta-Otmane/1.0X";

	generateResponse();

}

const MimeTypes& Response::getMimeTypes() const {
	return mMimeTypes;
}

void Response::generateResponse() {
	
	// checks the reponse type and sets the
		// data needed for that response type
	// if else branching to stop checking when
		// a response type was determined
	if (isError())
		;
	else if (isDelete())
		;
	else if (isContent())
		;
	else if (isDefault())
		;
	else if (isRedirect())
		;
	else if (isCGI())
		;
	else if (isAutoIndex())
		;

	openBodyStream();

	// checks if an error happened while processing
		// and preparing the response so that an
		// error respone could be sent
	// opens a stream of the error file to be sent
		// if there is one
	if (isError())
		openBodyStream();

	generateStatusLine();

	generateHeaders();

	addHeadersBodySeparator();

}

void Response::sendResponse() {

	// there is a body and there are
		// still body bytes to be sent
	if (mBodyFileName.empty() == false && 
		mBodyStream.eof() == false) {

		char bodyBuf[mReadSize];

		// fill readBodyBytes buffer from body stream
		mBodyStream.read(bodyBuf, mReadSize);

		// if it failed before reaching eof,
			// stops sending the response
		if (mBodyStream.eof() == false
			&&  mBodyStream.fail()) {
			mDone = true;
		}
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

void Response::generateHeaders() {

	for (std::map<std::string, std::string>::const_iterator
			header = mHeaders.begin();
			header != mHeaders.end(); ++header) {

		// RFC format of the header field
		// HeaderName ":" SP HeaderValue CRLF
		// first = header name
		// second = header value
		// iterates over the headers and adds them to the
			// send buffer in an http format
		mBuffer += header->first + ": "
			+ header->second +"\r\n";

	}

}

void Response::addHeadersBodySeparator() {

	if (mIsSeparator)
		mBuffer += "\r\n";

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

			// if the error page doesn't exist in the filesystem
				// returns immediately so that another error
				// concerning the non-existence of this error
				// page could be returned
			if (isPath(mBodyFileName) == false)
				return true;

			setContentType();
			setContentLength();

		}

	}

	return true;

}

bool Response::isContent() {

	if (mRequest.getRequestType()
		!= Request::CONTENT)
		return false;

	// gets path of the file to be served
	mBodyFileName = mRequest.getFullPath();

	setContentType();
	setContentLength();

	return true;

}

bool Response::isDefault() {

	if (mRequest.getRequestType()
		!= Request::DEFAULT)
		return false;

	// get the default file name defined
		// in mLocation and prepend the
		// location route to it
	const std::string defaultFileName 
		= mLocation->route + mLocation->defaultFile;
	
	// construct the full path of the default file
		// by replacing the DefaultFileName
		// prefix that matches the location
		// route with the location root
	const std::string defaultFileFullPath
		= mLocation->replaceByRoot(defaultFileName);
	
	// set the mBodyFileName to the
		// full path of the default file
	mBodyFileName = defaultFileFullPath;

	// add the content-type response-header field 
	setContentType();

	// add the content-lenght response-header field
	setContentLength();

	return true;

}

bool Response::isCGI() {
	// to be completed
	return false;
}

bool Response::isAutoIndex() {
	// to be completed
	return false;
}

bool Response::isDelete() {

	if (mRequest.getMethod()
		!= Request::DELETE)
		return false;

	const std::string& pathToBeDeleted
		= mRequest.getFullPath();

	// if file couldn't be deleted
	if (removeFile(pathToBeDeleted)
		== false) {

		mStatusCode = StatusCodeHandler::SERVER_ERROR;

	}

	return true;

}

void Response::openBodyStream() {

	// there is no body to be sent
	if (mBodyFileName.empty())
		return;

	// if there is already a file associated
		// with the stream it closes it first
	if (mBodyStream.is_open())
		mBodyStream.close();

	mBodyStream.open(mBodyFileName.c_str(),
		std::ofstream::in | std::ofstream::binary);

	// stream was opened succesfully
	if (mBodyStream)
		return ;

	// otherwise clears the bodyfilename and
		// removes the headers that are associated
		// with the entity body
	mStatusCode = StatusCodeHandler::SERVER_ERROR;

	clearEntityBodyData();

}

void Response::setContentType() {
	
	// search for mBodyFileName extension pos
	const std::string::size_type& extentionPos
		= mBodyFileName.rfind(".");
	
	// sets the mbodyFileName extension if found
		// starting from the pos after
		// the extension dot
	// if not found the bodyFileExtension
		// will be left empty
	const Extension& bodyFileExtension 
		= extentionPos != std::string::npos ?
		mBodyFileName.substr(extentionPos + 1) : "";

	// get the mime type associated to
		// the bodyFileExtension in mMimeTypes
	// if not found, get the default
		// mime type (application/octet-stream)
	const Mimetype& fileMimeType 
		= mMimeTypes.getType(bodyFileExtension);

	// add the content-type response-header field
		// with the associated file type of mBodyFileName
	mHeaders["Content-Type"] = fileMimeType;

}

void Response::setContentLength() {

	try {

		// gets the size of the file containing
			// the response body
		const size_t bodyFileSize
			= getFileSize(mBodyFileName);
		mHeaders["Content-Length"] = toString(bodyFileSize);

	}
	catch (const std::exception& e) {
		mStatusCode = StatusCodeHandler::SERVER_ERROR;
	}

}

void Response::generateStatusLine() {

	// get mStatuscode info
	// the first element of the pair represents
		// the string format of mStatuscode
	// the second element is the reason phrase 
		// associated to mStatusCode
	const StatusCodeHandler::StatusCodePair 
		statusCodePair = StatusCodeHandler::
		getStatusCodeInfo(mStatusCode);
	
	// construct the response status line
		// that has the format:
		// [HTTP-Version SP Status-Code SP Reason-Phrase CRLF]
	const std::string statusLine 
		// the http version including the first space
		= "HTTP/1.1 "
		// string format of mStatusCode
		+ statusCodePair.first
		+ " "
		// textual reason phrase
		+ statusCodePair.second
		+ "\r\n";

	// add the response status line to mBuffer
	mBuffer.append(statusLine);

}

void Response::clearEntityBodyData() {

	mBodyFileName.clear();
	mHeaders.erase("Content-Type");
	mHeaders.erase("Content-Length");

}
