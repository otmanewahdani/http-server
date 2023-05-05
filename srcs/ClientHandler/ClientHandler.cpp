/* this file contains the implementation of the ClientHandler class */

#include <ClientHandler.hpp>

ClientHandler::ClientHandler(Socket ID, ConstServerRef server,
	const MimeTypes& mimeTypes)
	: mID(ID)
	, mServer(server)
	, mRequest(ID, server)
	, mResponse(ID, mRequest, mServer, mimeTypes)
	, mStage(REQUEST) // starts at the request stage
	{}

ClientHandler::ClientHandler(const ClientHandler& handler)
	: mID(handler.mID)
	, mServer(handler.mServer)
	, mRequest(mID, mServer)
	, mResponse(mID, mRequest, mServer,
		handler.mResponse.getMimeTypes())
	, mStage(REQUEST) // starts at the request stage
	{}

bool ClientHandler::isRead() {

	// doesn't need reading when
		// the request stage is over
	if (mStage != REQUEST)
		return false;

	// checks the validity of the socket
	if (mRequest.isSocketOk() == false) {
		closeClientConnection();
		return false;
	}

	if (mRequest.isRead() == false) {

		// if request is done, moves to the
			// response stage and starts the
			// response generation
		mStage = RESPONSE;
		mResponse.start(mRequest.getLocation());
		return false;

	}
	
	return true;

}

bool ClientHandler::isWrite() {

	// doesn't need writing if not in
		// the respone stage
	if (mStage != RESPONSE)
		return false;

	// if respone is done, moves to the
		// closing stage
	if (mResponse.isWrite() == false) {
		closeClientConnection();
		return false;
	}

	return true;

}

bool ClientHandler::isClosed() const {
	return (mStage == CLOSE);
}

void ClientHandler::proceedWithSocket() {

	switch (mStage) {

		case REQUEST:
			return (mRequest.proceedWithSocket());
		case RESPONSE:
			return (mResponse.proceedWithSocket());
		default:
			const std::string errorMsg = "ClientHandler::"
				"proceedWithSocket(): no multiplexing "
				"query was made before";
			throw std::runtime_error(errorMsg);

	}

}

ClientHandler::Socket ClientHandler::getID() const {
	return mID;
}

void ClientHandler::closeClientConnection() {

	Log::connectionClosed(mID);
	mStage = CLOSE;
	// closes the client connection
	close(mID);
	mID = -1;

}
