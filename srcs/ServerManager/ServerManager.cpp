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
				// made the multiplexing request
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

void ServerManager::addServersForMultiplexing() {

	mListenFDs.clear();
	
	Servers::const_iterator server;
	for (server = mServers.begin();
		server != mServers.end(); ++server) {

		// adds server's socket to the end
			// of the FD collection
		mListenFDs.insert(mListenFDs.end(),
			server->socketID);

	}

}

void ServerManager::manageNewConnections() {

	FDCollection::const_iterator listenFD;
	for (listenFD = mListenFDs.begin();
		listenFD != mListenFDs.end(); ++listenFD) {

		try {
			Socket newSock = 
				getNewConnectionSock(*listenFD);

			addClientHandler(newSock, *listenFD);
		}
		catch (const std::exception& error) {
		}

	}

}

ServerManager::Socket
	ServerManager::getNewConnectionSock
	(Socket listenSock) {
	
	Socket newSock = accept(listenSock, NULL, NULL);
	if (newSock == -1)
		throwErrnoException
			("getNewConnectionSock() failed"
			 "to accept new connection");

	makeFDNonBlock(newSock);

	return newSock;

}

void ServerManager::informClientHandlers() {

	ClientHandler* clientHandler = NULL;

	// informs client handlers who made read
		// operation multiplex queries
	FDCollection::const_iterator readFDIt;
	for (readFDIt = mReadFDs.begin();
		readFDIt != mReadFDs.end(); ++readFDIt) {

		try {
			clientHandler =
				getMultiplexQueryClientHandler(*readFDIt);
		}
		catch (const std::exception& error) {
		}

	}

	// informs client handlers who made write
		// operation multiplex queries

}

void ServerManager::removeClientHandler(Socket ID) {
	mClientHandlers.erase(ID);
}

void ServerManager::addClientHandler(Socket clientID, Socket serverID) {
	
	ConstServerRef server = mConfig.getServerRef(serverID);

	// creates a new client handler and associates with client ID
	// and adds it to mClientHandlers
	std::pair<Socket, ClientHandler> newHandler( clientID,
		ClientHandler(clientID, server) );

	// if new client handler didn't get added because a client
		// handler with clientID exists already
	if (mClientHandlers.insert(newHandler).second == false) {
		std::string error = "couldn't create a new client handler"
			" with clientID: ";
		error += std::to_string(clientID);
		error += " because it exists already";
		throw std::invalid_argument(error);
	}

}

ClientHandler*
	ServerManager::getMultiplexQueryClientHandler
	(FD queriedFD) {

	std::map<FD, ClientHandler*>::iterator clientHandler;

	clientHandler = mFDMultiPlexQueries.find(queriedFD);
	// if client not found
	if (clientHandler == mFDMultiPlexQueries.end()) {
		std::string error = "couldn't find a client handler"
			" who made a multiplex query for this FD: ";
		error += std::to_string(queriedFD);
		throw std::invalid_argument(error);
	}

	// returns clientHandler pointer
	return clientHandler->second;

}
