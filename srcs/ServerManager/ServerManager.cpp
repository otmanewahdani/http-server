/* this file contains the implementaion of the ServerManager class
 */

#include <ServerManager.hpp>
#include <Network.hpp>

ServerManager::ServerManager(const char* configFileName)
	: mConfig(configFileName) {

		Network::initServersSockets(mConfig.getServers());

}
