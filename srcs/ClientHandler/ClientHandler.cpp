/* this file contains the implementation of the ClientHandler class */

#include <ClientHandler.hpp>

ClientHandler::ClientHandler(Socket ID, ConstServerRef server)
	: mID(ID),
	, mServer(server)
	, mRequest(ID, server)
	, mResponse(ID, mRequest, mServer)
	, mStage(REQUEST) // starts at the request stage
	{}

bool ClientHandler::isRead() const {

	// doesn't need reading when
		// the request stage is over
	if (mStage != REQUEST)
		return false;

	if (mRequest.isRead() == false) {

		// if request is done, moves to the
			// response stage and starts the
			// response generation
		mStage = RESPONSE;
		mResponse.start(mRequest.getLocation());

	}
	
	return true;

}
