/* this file contains the implementation of RequestChecker class
 */

#include <RequestChecker.hpp>

RequestChecker::RequestChecker(Request& request)
	: mRequest(Request)
	, mLocation() {}

bool RequestChecker::isValid() {

	// gets the location which contains the
		// config data about the request path
	mLocation = mRequest.getLocation();
	
	if (isMethodAllowed() == false) {
		return false;
	}

	// path not found
	mRequest.setStatusCode();
	return false;

}
