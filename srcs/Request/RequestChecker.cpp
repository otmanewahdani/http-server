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
	
	if ( isMethodAllowed() 
		|| isRedirect()
		|| isAutoIndex()
		|| isCGI()
		|| isUpload()
		|| isDefault()
		|| isContent() ) {

		return true;
	
	}

	return false;

}

bool RequestChecker::isAutoIndex() {

	// if the the requested resource 
		// is a directory and the location
		// enables autoindexing
	if(mIsDir == false || 
		mLocation->autoindex == false) {
		return false;
	}

	mRequest.setRequestType(Request::AUTOINDEX);
	return true;
	
}

bool RequestChecker::isUpload() {

	// checks if the full path of the requested 
		// ressource is a directory and the 
		// location enables uploading 
	if (mIsDir == false || 
		mLocation->uploadRoute.empty()) {
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
		mLocation.defaultFile.empty()) {
		return false;
	}
	
	mRequest.setRequestType(Request::DEFAULT);
	return true;

}

bool RequestChecker::isContent() {

	// checks if the path exists and is not a directory
		// in case of requesting a static file
	if (mIsPath == false || mIsDir)
		return false;

	mRequest.setRequestType(Request::CONTENT);
	return true;

}
