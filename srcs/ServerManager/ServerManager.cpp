/* this file contains the implementaion of the ServerManager class
 */

#include <ServerManager.hpp>

ServerManager::ServerManager(const char* configFileName)
	: mConfig(configFileName) {

		Network::initServersSockets(mConfig.getServers());

}

void ServerManager::printConfig() {
	mConfig.print();
}
