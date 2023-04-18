/* this file contains the defintion of the Network class
 */

#include <Network.hpp>

// gets protocol number
const int Network::mProtocol = getprotobyname("tcp")->p_proto;

void Network::initServersSockets(Servers& servers) {
	
	for (Servers::iterator server = servers.begin();
		server != servers.end(); ++server) {

		createSocket(server->socketID, servers);
		makeServerListen(server, servers);

	}

}

void Network::createSocket(Socket& socketID, Servers& servers) {

	socketID = socket(mAddrFamily, mSockType, mProtocol);
	if (socketID == -1) {
		clearServersSockets(servers);
		throwErrnoException("failed to create socket");
	}

	makeSocketReuseAddr(socketID, servers);

	makeSocketNonBlocking(socketID, servers);

}

void Network::makeSocketReuseAddr(const Socket& socketID, Servers& servers) {

	const int enable = 1;
	if (setsockopt(socketID, SOL_SOCKET,
		SO_REUSEADDR, &enable, sizeof(enable))) {

		clearServersSockets(servers);
		throwErrnoException("failed to make socket reuse address");

	}

}

void Network::makeSocketNonBlocking(const Socket& socketID, Servers& servers) {

	if (fcntl(socketID, F_SETFD, O_NONBLOCK) == -1) {

		clearServersSockets(servers);
		throwErrnoException("failed to make socket non-blocking");

	}

}

void Network::makeServerListen(const ServerRef& server, Servers& servers) {

	AddrInfo* serverAddr = getServerAddrInfo(server, servers);

	// binds address to server's socket
	if (bind(server->socketID, serverAddr->ai_addr,
		serverAddr->ai_addrlen) == -1) {

		std::string error = "failed to bind socket to ";
		error += server->hostname + ':' + server->port;
		clearServersSockets(servers);
		throwErrnoException(error);

	}
	
	// sets backlog to SOMAXCONN so the largest maximum number
		// possible of connections is queued
	if (listen(server->socketID, SOMAXCONN) == -1) {

		std::string error = "failed to listen on ";
		error += server->hostname + ':' + server->port;
		clearServersSockets(servers);
		throwErrnoException(error);

	}

	freeServerAddrInfo(serverAddr);

}

Network::AddrInfo* Network::getServerAddrInfo
	(const ServerRef& server, Servers& servers) {
	
	AddrInfo hints;
	// clears hints
	bzero(&hints, sizeof(hints));
	hints.ai_flags = mFlags;
	hints.ai_family = mAddrFamily;
	hints.ai_socktype = mSockType;
	hints.ai_protocol = mProtocol;

	// getaddrinfo will set it to the desired address
	AddrInfo* resultedAddr = NULL;
	int successCode = getaddrinfo(server->hostname.c_str(),
			server->port.c_str(), &hints, &resultedAddr);
	
	// failed
	if (successCode) {
		std::string error = "getServerAddrInfo failed for ";
		error += server->hostname + ':' + server->port + ": ";
		error += gai_strerror(successCode);
		clearServersSockets(servers);
		throw std::runtime_error(error);
	}
	
	// succeeded
	return resultedAddr;

}

void Network::freeServerAddrInfo(AddrInfo* addr) {
	freeaddrinfo(addr);
}

void Network::clearServersSockets(Servers& servers) {
	
	for (Servers::iterator server = servers.begin();
		server != servers.end(); ++server) {
		
		close(server->socketID);
		server->socketID = -1;

	}

}
