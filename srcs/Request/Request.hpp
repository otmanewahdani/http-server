/* this file contains the definition of the Request class
 * 	This class is reponsible for receiving and parsing an
 * 	http request into a well-organized object that can be
 * 	used easily by a response module.
 */

#pragma once

#include <Config.hpp>

class Request {

	public:
		/******* nested types *******/
		// http methods tokens
		enum Method {
			GET,
			POST,
			DELETE,
			UNSPECIFIED
		};

		// represents the stages that
			// request parsing goes through
		enum Stage {
			STATUS_LINE,
			HEADERS,
			BODY,
			FINISH
		};

		// type of request to be executed
		enum RequestType {
			REDIRECT,
			CGI,
			UPLOAD,
			AUTOINDEX,
			CONTENT
		};

		/******* alias types *******/
		typedef int Socket;
		typedef Config::ConstServerRef ConstServerRef;

		/******* public member functions *******/
		// first parameter is the socket from which
			// the request is received
		// server refers to the server on which
			// the socket is connected and therefore
			// contains the needed configuration info
		Request(Socket socket, ConstServerRef server);

		// returns true if it still wants to read
			// request bytes from a socket
		// returns false if done reading and request
			// object ready for further processing
		bool isRead() const ;

		// signals that the socket is ready for reading
			// attempts to read from socket
			// and parse request bytes
		void proceedWithSocket();

		// returns true if parsed request is valid
		bool isValid();

	private:
		/******* private member objects *******/
		// socket from which the
			// request will be read
		Socket mSocket;

		// server on which the request arrives
		// It contains the necessary config
			// data used in parsing the request
		ConstServerRef mServer;

		// request parsing stage
		Stage mStage;

		// UNSPECIFIED in case method is not parsed
			// yet, or parsed method is unsupported
		Method mMethod;

		// contains the bytes read from socket
			// that still need to be processed
		std::string mBuffer;

		RequestHeaders mHeaders;

		// amount by which to read from socket
		static size_t readSize;

		/******* private member functions *******/
		// contains the whole logic that parses
			// the request
		void parseRequest();

		void parseStatusLine();

		void parseBody();

		void getHeaders();

};
