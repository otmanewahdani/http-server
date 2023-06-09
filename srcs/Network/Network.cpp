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

	try {
		makeFDNonBlock(socketID);
	}
	catch (const std::exception& error) {

		clearServersSockets(servers);
		throw error;

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

	Log::socketBinding(server->socketID);

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
		clearServersSockets(servers);
		std::string error = "getServerAddrInfo failed for ";
		error += server->hostname + ':' + server->port;
		throwAddrInfoError(successCode, error);
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

std::string Network::sockAddrToName
	(const SockAddr* addr, SockLen addrLen) {
	
	// creates 2 buffers that are large enough
		// to hold any hostname and port
	// the hostname and port of the addr parameter
		// are stored by getnameinfo in
		// these 2 buffers
	char hostname[NI_MAXHOST];
	char port[NI_MAXSERV];

	int successCode = getnameinfo(addr, addrLen, hostname,
		sizeof(hostname), port, sizeof(port),
		// gets hostname and port in numeric format
		NI_NUMERICHOST | NI_NUMERICSERV);

	if (successCode)
		throwAddrInfoError(successCode,
			"failed to convert socket to name");

	return (std::string(hostname) + ':' + port);

}

std::string Network::getSocketServerName(const Socket sock) {

	// server's socket address will be stored here
		// by getsockname
	SockAddrStore serverAddr;
	SockLen serverAddrLen = sizeof(serverAddr);

	if (getsockname(sock,
		reinterpret_cast<SockAddr*>(&serverAddr),
		&serverAddrLen) == -1) {

		throwErrnoException("failed to get socket's server name");

	}

	std::string serverName = sockAddrToName
		(reinterpret_cast<SockAddr*>(&serverAddr), serverAddrLen);
	
	return serverName;

}

std::string Network::getSocketClientName(const Socket sock) {

	// client's socket address will be stored here
		// by getpeername
	SockAddrStore clientAddr;
	SockLen clientAddrLen = sizeof(clientAddr);

	if (getpeername(sock,
		reinterpret_cast<SockAddr*>(&clientAddr),
		&clientAddrLen) == -1) {

		throwErrnoException("failed to get socket's client name");

	}

	std::string clientName = sockAddrToName
		(reinterpret_cast<SockAddr*>(&clientAddr), clientAddrLen);
	
	return clientName;

}

void Network::throwAddrInfoError
	(int errorCode, const std::string &errorMsg) {

	std::string exceptionStr = errorMsg + ": ";
	exceptionStr += gai_strerror(errorCode);
	
	throw std::runtime_error(exceptionStr);

}
