/* this file contains the deifnition of the ClientHandler class
 * This class represents a client handler for a specific
 * client identified by its socket number. It also represents
 * an http request-response cycle where a client is created
 * upon receiving of a request and it moves to the response stage
 * after that request is fully parsed and finaly it terminates
 * the cycle by closing its connection. you can also think of this class
 * as a mediator between a request module and a response module.
 * the Client can be asked if it wants to read or write to some FD
 * through calling isRead or isWrite and the FD in question can be
 * retrieved by a next call to getFD()
 */

#pragma once

class ClientHandler {

	public:
		/******* alias types *******/
		typedef int Socket;
		typedef int FD;

		/******* public member functions *******/
		ClientHandler(Socket ID);

		// returns true if it wants to read from an fd
		bool isRead() const ;

		// returns true if it wants to write to an fd
		bool isWrite() const ;

		// returns true if it wants to close its connection
		bool isClose() const ;

		// returns FD that's ready for I/O
		FD getFD();

		// signals to the Client Handler that the current FD
			// is ready for I/O
		void proceedWithFD();
	
	private:
		/******* public member functions *******/
		// socket identifier of the client
		Socket mID;

};
