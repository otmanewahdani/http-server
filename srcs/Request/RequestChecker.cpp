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
	
	if (isMethodAllowed() == false) {
		return false;
	}

	if (isRedirect())

	// path not found
	mRequest.setStatusCode();
	return false;

}
