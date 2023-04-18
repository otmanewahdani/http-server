/* this file contains the defintion of the Network class
 */

#include <Network.hpp>

// gets protocol number
const int Network::mProtocol = getprotobyname("tcp")->p_proto;

void Network::initServersSockets(Servers& servers) {
	
	for (Servers::iterator server = servers.begin();
		server != servers.end(); ++server) {

		createSocket(server->socketID, servers);

	}

}

void Network::createSocket(Socket& socketID, Servers& servers) {

	socketID = socket(mAddrFamily, mSockType, mProtocol);
	if (socketID == -1) {
		throwErrnoException("failed to create socket");
		clearServersSockets(servers);
	}

	makeSocketReuseAddr(socketID, servers);

	makeSocketNonBlocking(socketID, servers);

}

void Network::makeSocketReuseAddr(const Socket& socketID, Servers& servers) {

	const int enable = 1;
	if (setsockopt(socketID, SOL_SOCKET,
		SO_REUSEADDR, &enable, sizeof(enable)) {

		throwErrnoException("failed to make socket reuse address");
		clearServersSockets(servers);

	}

}

void Network::makeSocketNonBlocking(const Socket& socketID, Servers& servers) {

	if (fcntl(socketID, F_SETFD, O_NONBLOCK) == -1) {

		throwErrnoException("failed to make socket reuse address");
		clearServersSockets(servers);

	}

}

void Network::clearServersSockets(Servers& servers) {
