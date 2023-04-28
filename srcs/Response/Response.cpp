/* this file contains the implementation of the Response class
 */

#include <Response.hpp>

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

	mLocation = location;
	mStart = true;

}
