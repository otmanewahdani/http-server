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
		static void connectionEstablished(const Socket socket);

		// logs request coming from socket's client to socket's server
			// with the specified Method, URI and query string
		static void request(const Socket socket,
			const std::string& method, 
			const std::string& url);

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

		// both objects used as the notices that
			// come first at the beginning of all
			// log messages
		static const std::string mErrorNotice;
		static const std::string mInfoNotice;

		/******* private member functions *******/
		// writes date and time to the log file 
			// in format [YYYY-MM-DD HH:MM:SS]
		static void addTimeDate();

		// this is a general utility used by other methods that
			// log specific operations
		// it logs the operation (op) that happened between the
			// socket's server and the socket's client
		// the clientPrep and serverPrep are the prepositions
			// used before the client and the server
		// it handles all possible exceptions on behalf of the methods
		// if everything goes well mInfoNotice is preppended to the
			// log message, otherwise error() is called
		// example: when connectionEstablished calls this method and
			// passes it its socket, op = "connection established",
			// clientPrep = "from", serverPrep = "on", the output:
			// "[INFO] connection established from
			// client host:port on server host:port"
		static void logClientServerOperation(const Socket socket,
			const std::string& op, const std::string& clientPrep,
			const std::string& serverPrep);

};
