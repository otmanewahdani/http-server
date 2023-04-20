/* this file contains the definition of the Log class.
 * It is responsible for opening a log file and logging
 * the following server operations:
 * + sockets binding
 * + established new connections
 * + closed connections
 * + request receipt
 * + response sending
 * The log functions mostly take a socket only and the hostname
 * and port of the peer is determined from that socket
*/

#pragma once

#include <fstream>
#include <string>
#include <Config.hpp>

class Log {

	public:
		/******** alias types ********/
		typedef int Socket;
		typedef Config::ServerContext& ServerRef;

		/******** public member functions ********/
		// logs that socket was bound to server's hostname and port
		static void socketBinding(Socket socket,
			const ServerRef server);

		// logs that peerSocket is a new connection to server's hostname
			// and port
		static void connectionEstablished(const ServerRef server,
			const Socket peerSocket);

		// logs request coming from peerSocket to server
		static void request(const ServerRef server, Socket peerSocket);

		// logs response thqt was sent to a peerSocket from server
		static void response(const ServerRef server, Socket peerSocket);

	private:
		/******* private member objects *******/
		static std::ofstream mLogfile;

		/******* private member functions *******/
		// writes date and time to the log file and aopends
			// a ": " to it
		static void addTimeDate();

};