/* this file contains the implementaion of the ServerManager class
 */

#include <ServerManager.hpp>

ServerManager::ServerManager(const char* configFileName)
	: mConfig(configFileName)
	, mServers(mConfig.getServers())
	, mMimeTypes(NULL) {

		Network::initServersSockets(mConfig.getServers());
		initializeStaticData();

}

void ServerManager::initializeStaticData() {

	RequestHeaders::initializeStaticData();

	StatusCodeHandler::initializeStaticData();

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

		try {
			Multiplexer::checkFDsForEvents
				(mListenFDs, mReadFDs, mWriteFDs);
		}
		catch (const std::exception& error) {
			Log::error(error.what());
		}

		manageNewConnections();
		informClientHandlers();

	}

}

void ServerManager::queryClientHandlers() {

	mReadFDs.clear();
	mWriteFDs.clear();

	ClientHandlers::iterator clientHandlerIt;
	for (clientHandlerIt = mClientHandlers.begin();
		clientHandlerIt != mClientHandlers.end();
		++clientHandlerIt) {

		ClientHandler& handler =
			clientHandlerIt->second;

		Socket handlerID = clientHandlerIt->first;

		// checks if it needs I/O multiplexing
		if (handler.isRead()
			|| handler.isWrite()) {

			// inserts handlerID at the end of
				// the collection that corresponds
				// to the type of multiplexing query
				// it made (read or write)
			if (handler.isRead())
				mReadFDs.insert(mReadFDs.end(), handlerID);
			else
				mWriteFDs.insert(mWriteFDs.end(), handlerID);

		}

		// checks if it already closed its connection
			// so that it gets removed
		else if (handler.isClosed())
			removeClientHandler(handlerID);

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
			Log::error(error.what());
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

	Log::connectionEstablished(newSock);

	makeFDNonBlock(newSock);

	return newSock;

}

void ServerManager::informClientHandlers() {

	// informs client handlers who made read
		// operation multiplex queries
	informClientHandlers(mReadFDs);

	// informs client handlers who made write
		// operation multiplex queries
	informClientHandlers(mWriteFDs);

}

void ServerManager::informClientHandlers
	(FDCollection& FDs) {

	FDCollection::const_iterator FDIt;
	for (FDIt = FDs.begin();
		FDIt != FDs.end(); ++FDIt) {

		try {
			// finds client handler by socket ID
				// and informs it that it can use
				// its socket for I/O
			ClientHandler& handler =
				getClientHandler(*FDIt);
			handler.proceedWithSocket();
		}
		catch (const std::exception& error) {
			Log::error(error.what());
		}

	}

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

ClientHandler& ServerManager::getClientHandler
	(Socket ID) {
	
	ClientHandlers::iterator handler
		= mClientHandlers.find(ID);

	if (handler == mClientHandlers.end()) {
		const std::string error = std::string
			("couldn't find client handler"
			" with socket id: ")
			+ std::to_string(ID);
		throw std::invalid_argument(error);
	}

	// returns the handler associated
		// with ID
	return handler->second;

}
