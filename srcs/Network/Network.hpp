/* this file contains the defintion of the Network class
 * this class has static functions that mainly sets up
 * 	listening sockets for a Config::Servers collection.
 * It can also clean up those sockets
 */

#pragma once

#include <Config.hpp>
#include <fcntl.h>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

class Network {

	public:
		/******* public alias types *******/
		typedef Config::Servers Servers;
		typedef Config::Socket Socket;
		// datatype that refers to a specific server
		typedef Servers::iterator ServerRef;
		typedef struct addrinfo AddrInfo;

		/******* public member functions *******/
		// traverses the servers and creates listening
			// sockets for each one in socketID
		// the sockets are created depending on the
			// values of hostname and port in each server
		// throws std::runtime_error if an error was encountered
		static void initServersSockets(Servers& servers);

		static void clearServersSockets(Servers& servers);
	
	private:
		/******* private member objects *******/
		/* the static objects below represent default
		 * values used to create a specific type of sockets
		 * for example: mSockType is the default value
		 * used to specify the type of sockets (whether
		 * a stream socket or datagram socket..)
		 */
		const static int mSockType = SOCK_STREAM;
		// the socket address family is ipv4
		const static int mAddrFamily = AF_INET;
		// AI_PASSIVE specifies an address that will
		// be suitable for listening (aka acting as a server)
		const static int mFlags = AI_PASSIVE;
		const static int mProtocol;

		/******* private member functions *******/
		// creates a non-blocking socket with the SO_REUSEADDR option
		// in case of error calls clearServerSockets and 
		// throws std::exception with the cause of error msg
		static void createSocket(Socket& socketID, Servers& servers);

		// enables address reuse for socket
		// in case of error calls clearServerSockets and 
		// throws std::exception with the cause of error msg
		static void makeSocketReuseAddr(const Socket& socketID, Servers& servers);

		// same as above in case of error
		static void makeSocketNonBlocking(const Socket& socketID, Servers& servers);

		// binds server's socket to their corresponding hostname and port
			// and makes it listen for connections on it
		// same as above in case of error
		static void makeServerListen(const ServerRef& server, Servers& servers);

		// gets AddrInfo structure for a specific server's host and port
		// The structure is suitable for socket binding and listening
		// same as above in case of error
		static AddrInfo* getServerAddrInfo(const ServerRef& server, Servers& servers);

		// frees the structure passed by getServerAddrInfo
		static void freeServerAddrInfo(AddrInfo* addr);

};