/* this file contains the definition of the Log class.
 * It is responsible for opening a log file and logging
 * the following server operations:
 * + sockets binding
 * + established new connections
 * + closed connections
 * + request receipt
 * + response sending
 * + error messages when some operation fails
 * The log functions mostly take a socket only and the hostname
 * and port of the peer or the server is determined from that socket
*/

#pragma once

#include <fstream>
#include <string>
#include <Network.hpp>

class Log {

	public:
		/******** alias types ********/
		typedef int Socket;

		/******** public member functions ********/
		// logs that socket was bound to a server's hostname and port
		static void socketBinding(const Socket socket);

		// logs that socket's server established
			// a new connection with socket's client
		static void connectionEstablished(const Socket clientSocket, const Socket serverSocket);

		// logs request coming from socket's client to socket's server
		static void request(const Socket socket, const std::string& uri);

		// logs request that was sent from socket's
			// server to socket's client
		static void response(const Socket socket);

		// logs that a connection the client specified
			// with the given socket is closed
		static void connectionClosed(const Socket socket);

		// logs error messages
		static void error(const std::string& errorMsg);

	private:
		/******* private member objects *******/
		static std::ofstream mLogfile;

		/******* private member functions *******/
		// writes date and time to the log file 
			// in format [YYYY-MM-DD HH:MM:SS]
		static void addTimeDate();

};
