/* this file contains the implementaion of the ServerManager class
 */

#include <ServerManager.hpp>

ServerManager::ServerManager(const char* configFileName)
	: mConfig(configFileName)
	, mServers(mConfig.getServers())
	, mMimeTypes(NULL) {

		Network::initServersSockets(mConfig.getServers());

}

void ServerManager::printConfig() {
	mConfig.print();
}

void ServerManager::start() {
	manageClientHandlers();
}

void ServerManager::manageClientHandlers() {

	while (1) {

		addServersForMultiplexing();
		queryClientHandlers();
		Multiplexer::checkFDsForEvents(mListenFDs, mReadFDs, mWriteFDs);
		manageNewConnections();
		informClientHandlers();

	}

}

void ServerManager::queryClientHandlers() {

	mReadFDs.clear();
	mWriteFDs.clear();
	mFDMultiPlexQueries.clear();

	ClientHandlers::iterator clientHandlerIt;
	for (clientHandlerIt = mClientHandlers.begin();
		clientHandlerIt != mClientHandlers.end();
		++clientHandlerIt) {

		ClientHandler& handler =
			clientHandlerIt->second;

		// checks if it needs I/O multiplexing
		if (handler.isRead()
			|| handler.isWrite()) {

			// saves FD query and the handler that
				// made the request
			FD queriedFD = handler.getFD();
			mFDMultiPlexQueries[queriedFD] = &handler;

			// inserts queriedFD at the end of
				// the collection that corresponds
				// to the type of multiplexing query
				// it made (read or write)
			if (handler.isRead())
				mReadFDs.insert(mReadFDs.end(), queriedFD);
			else
				mWriteFDs.insert(mWriteFDs.end(), queriedFD);

		}

		// checks if it already closed its connection
			// so that it gets removed
		else if (handler.isClosed())
			removeClientHandler(handler.getID());

	} // end of for loop

}

void ServerManager::addServersForMultiplexing();

void ServerManager::manageNewConnections();

void ServerManager::informClientHandlers();

void ServerManager::removeClientHandler(Socket ID);

void ServerManager::addClientHandler(Socket ID);
