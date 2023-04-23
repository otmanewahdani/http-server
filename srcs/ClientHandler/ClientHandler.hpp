/* this file contains the deifnition of the ClientHandler class
 * This class represents a client handler for a specific
 * client identified by its socket number. It also represents
 * an http request-response cycle where a client is created
 * upon receiving of a request and it moves to the response stage
 * after that request is fully parsed and finaly it terminates
 * the cycle by closing its connection. you can also think of this class
 * as a mediator between a request module and a response module.
 * the Client can be asked if it wants to read or write to his socket
 * through calling isRead or isWrite , to check if it wants to start an
 * I/O operation on it's socket. then it will go through multiplexing 
 * to check for its I/O readiness. 
 */

#pragma once

#include <Config.hpp>

class ClientHandler {

	public:
		/******* nested types *******/
		enum Stage {
			REQUEST,
			RESPONSE,
			CLOSE
		};

		/******* alias types *******/
		typedef int Socket;
		typedef Config::ServerRef ServerRef;
		typedef Config::ConstServerRef ConstServerRef;

		/******* public member functions *******/
		// ID : socket identifier of the new client
		// server: a reference to the server to which
			// the client is connected
		ClientHandler(Socket ID, ConstServerRef server);

		// returns true if it wants to read from a socket
		bool isRead() const ;

		// returns true if it wants to write to a socket
		bool isWrite() const ;

		// returns true if it closed its client connection
		bool isClosed() const ;

		// returns client handler's id
		Socket getID();

		// signals to the Client Handler that the socket
			// is ready for I/O
		void proceedWithSocket();
	
	private:
		/******* public member functions *******/
		// socket identifier of the client
		Socket mID;

		// server to which the client is connected
		ConstServerRef mServer;

		// represents the stage at which
			// client handler is at
		Stage mStage;

};
