/* this file contains the implementation of RequestChecker class
 */

#include <RequestChecker.hpp>

RequestChecker::RequestChecker(Request& request)
	: mRequest(Request)
	, mLocation()
	, mIsPath()
	, mIsDir() {}

bool RequestChecker::isValid() {

	// gets the location which contains the
		// config data about the request path
	mLocation = mRequest.getLocation();

	// saves if the file exists so that it's used
		// by the checking functions that are called next
	mIsPath = isPath(mRequest.getFullPath);
	mIsDir = isDir(mRequest.getFullPath);
	
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

	return false;

}

bool RequestChecker::isMethodAllowed() {

	const Method& method = mRequest.getMethod();

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

	mRequest.setStatusCode(mLocation->redirection.first);

	mRequest.setRequestType = REDIRECT;

	return true;

}

bool RequestChecker::isCGI() {

	Method method = mRequest.getMethod();

	// no CGI set for the location
		// or method incompatible with cgi
	if (mLocation->supportedCGIs.empty()
		|| (method != Request::GET
			&& method != Request::POST))
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

	return true;

}
