/* this file contains the definition of the Response class
 * It is reponsible for taking a fully parsed http request,
 * 	generating an appropriate response and sending it over
 * 	the client's socket
 */

#pragma once

#include <Request.hpp>
#include <StatusCodeHandler.hpp>

class Response {

	public:
		/******* alias types *******/
		typedef Request::Socket Socket;
		typedef Request::ConstServerRef ConstServerRef;
		typedef Request::ConstLocPtr ConstLocPtr;

		/******* public member functions *******/
		// first parameter is the socket on which
			// the response is sent
		// server refers to the server on which
			// the socket is connected and therefore
			// contains the needed configuration info
		Response(Socket socket, const Request& request,
			ConstServerRef server);

		// returns true if it still wants to write
			// response bytes on the socket
		// returns false if fully done sending
			// the response
		bool isWrite() const ;

		// starts the reponse generating process
		// sets mLocation to location
		// should only be called once, otherwise
			// std::runtime_error is thrown
		void start(ConstLocPtr location);
	
	private:
		/******* private member objects *******/
		// socket over which the
			// reponse will be sent
		Socket mSocket;

		// the whole parsed request info
			// and data
		const Request& mRequest;

		// server on which the request arrives
			// It contains the necessary config
			// data used in generating the response
		ConstServerRef mServer;

		// pointer to location that contains the
			// configuration of the requested path
		ConstLocPtr mLocation;

		// is response done
		bool mDone;

		// did response already start
		bool mStart;

		/******* private member functions *******/
		// contains the main logic that generates
			// the reponse
		void generateReponse();

};
