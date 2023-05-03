/* this file contains the definition of the Response class
 * It is reponsible for taking a fully parsed http request,
 * 	generating an appropriate response and sending it over
 * 	the client's socket
 */

#pragma once

#include <StatusCodeHandler.hpp>
#include <Config.hpp>

// forward declaration of request
// it's included at the bottom of the file
// see the bottom for an explanation
class Request;

class Response {

	public:
		/******* alias types *******/
		typedef Config::Socket Socket;
		typedef Config::ConstServerRef ConstServerRef;
		typedef Config::ConstLocPtr ConstLocPtr;

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

		// signals that the socket is ready for writing
			// attempts to send response bytes over socket
		void proceedWithSocket();

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

		// sends the generated response over the
			// socket
		void sendRespone();

};

// this file was included here because it includes ClientHandler.hpp which in turn includes
// Response.hpp  and it  relies on its full definition of the Response class
#include <Request.hpp>
