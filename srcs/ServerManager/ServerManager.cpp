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

void ServerManager::queryClientHandlers();

void ServerManager::addServersForMultiplexing();

void ServerManager::manageNewConnections();

void ServerManager::informClientHandlers();

void ServerManager::removeClientHandler(Socket ID);

void ServerManager::addClientHandler(Socket ID);
