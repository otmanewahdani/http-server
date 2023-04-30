/* this file contains the implementation of RequestChecker class
 */

#include <RequestChecker.hpp>

RequestChecker::RequestChecker(Request& request)
	: mRequest(request)
	, mLocation()
	, mIsPath()
	, mIsDir() {}

bool RequestChecker::isValid() {

	// gets the location which contains the
		// config data about the request path
	mLocation = mRequest.getLocation();

	// saves if the file exists so that it's used
		// by the checking functions that are called next
	mIsPath = isPath(mRequest.getFullPath());
	mIsDir = isDir(mRequest.getFullPath());
	
	if ( isMethodAllowed() == false)
		return false;

	if ( isRedirect()
		|| isAutoIndex()
		|| isCGI()
		|| isUpload()
		|| isDefault()
		|| isContent() ) {

		return true;
	
	}

	// request doesn't match anything
	mRequest.setStatusCode(StatusCodeHandler::NOT_FOUND);
	return false;

}

bool RequestChecker::isMethodAllowed() {

	const Request::Method& method
		= mRequest.getMethod();

	if (method == Request::GET)
		if (mLocation->get)
			return true;

	if (method == Request::POST)
		if (mLocation->post)
			return true;

	if (method == Request::DELETE)
		if (mLocation->del)
			return true;
	
	// sets the status code to method not allowed
	mRequest.setStatusCode(StatusCodeHandler::METHOD_ALLOW);
	return false;

}

bool RequestChecker::isRedirect() {

	// no redirections are set if the
		// status code is 0
	if (mLocation->redirection.first == 0)
		return false;

	mRequest.setStatusCode
		(static_cast<StatusCodeType>
		 (mLocation->redirection.first));

	mRequest.setRequestType(Request::REDIRECT);

	return true;

}

bool RequestChecker::isCGI() {

	const Request::Method& method =
		mRequest.getMethod();

	// no CGI set for the location
		// or method incompatible with cgi
		// or CGI file doesn't exist
	if (mLocation->supportedCGIs.empty()
		|| (method != Request::GET
			&& method != Request::POST)
		|| mIsPath == false)
		return false;

	const std::string& path = mRequest.getPath();

	// searches for the last '.' in path
		// if present it could indicate that
		// there is an extension in the path
		// and it should be further investigated
		// to verify if it's a supported cgi extension
	const std::string::size_type
		extensionPos = path.rfind('.');

	// '.' not found
	if (extensionPos == std::string::npos)
		return false;

	// extracts the extension and checks if
		// it's a supported cgi extension
	const std::string extension
		= path.substr(extensionPos);

	// not a supported cgi extension
	if (mLocation->supportedCGIs.find(extension)
		== mLocation->supportedCGIs.end())
		return false;

	mRequest.setRequestType(Request::CGI);
	return true;

}

bool RequestChecker::isAutoIndex() {

	// if the method is get, the requested resource 
		// is a directory and the location
		// enables autoindexing
	if (mIsDir == false || 
		mRequest.getMethod() != Request::GET
		|| mLocation->autoindex == false) {
		return false;
	}

	mRequest.setRequestType(Request::AUTOINDEX);
	return true;
	
}

bool RequestChecker::isUpload() {

	// checks if the method is post,
		// full path of the requested 
		// resource is a directory and the 
		// location enables uploading 
	if (mIsDir == false || 
		mRequest.getMethod() != Request::POST
		|| mLocation->uploadRoute.empty()) {
		return false;
	}

	mRequest.setRequestType(Request::UPLOAD);
	return true;

}

bool RequestChecker::isDefault() {

	// checks if the location specifies a default 
		// file to serve in case of get method 
		// and the path of the request is
		// a directory otherwise returns false 
	if (mIsDir == false || 
		mRequest.getMethod() != Request::GET || 
		mLocation->defaultFile.empty()) {
		return false;
	}
	
	mRequest.setRequestType(Request::DEFAULT);
	return true;

}

bool RequestChecker::isContent() {

	// checks if the method is get, the path exists
		// and is not a directory in case of requesting
		// a static file
	if (mIsPath == false || mIsDir ||
		mRequest.getMethod() != Request::GET)
		return false;

	mRequest.setRequestType(Request::CONTENT);
	return true;

}
