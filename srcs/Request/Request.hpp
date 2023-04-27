/* this file contains the definition of the Request class
 * 	This class is reponsible for receiving and parsing an
 * 	http request into a well-organized object that can be
 * 	used easily by a response module.
 * 	If the read request is invalid, the request parsing stops
 * 		and response status code is set appropriately
 * 		(the validity can be checked with isValid())
 */

#pragma once

#include <Config.hpp>
#include <RequestHeaders.hpp>
#include <URL.hpp>
#include <StatusCodeHandler.hpp>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdexcept>
#include <map>
#include <Log.hpp>

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
			REQUEST_LINE,
			HEADERS,
			BODY,
			FINISH
		};

		// type of request to be executed
		enum RequestType {
			UNDETERMINED,
			REDIRECT,
			CGI,
			UPLOAD,
			AUTOINDEX,
			CONTENT
		};

		/******* alias types *******/
		typedef int Socket;
		typedef Config::ConstServerRef ConstServerRef;
		typedef Config::ConstLocPtr ConstLocPtr;
		typedef StatusCodeHandler::StatusCodeType StatusCodeType;
		typedef RequestHeaders::HeaderName HeaderName;
		typedef RequestHeaders::HeaderValue HeaderValue;

		/******* public member functions *******/
		// first parameter is the socket from which
			// the request is received
		// server refers to the server on which
			// the socket is connected and therefore
			// contains the needed configuration info
		Request(Socket socket, ConstServerRef server);

		static void initializeStaticData();

		// returns true if it still wants to read
			// request bytes from a socket
		// returns false if done reading and request
			// object ready for further processing
		bool isRead() const ;

		// signals that the socket is ready for reading
			// attempts to read from socket
			// and parse request bytes
		// if it's called and Request is done reading,
			// throws std:runtime_error
		void proceedWithSocket();

		// returns true if parsed request is valid
			// if not finished parsing yet,
			// throws std::runtime_error
		bool isValid() const;

		// returns true if no issue happened when
			// reading from the socket
		bool isSocketOk() const;

		/******* getters *******/
		// returns pointer to location that contains the
			// configuration of the requested path
		ConstLocPtr getLocation() const ;

		// returns the path part of the parsed uri
		const std::string& getPath() const;

		// returns the query string part of the parsed uri
		const std::string& getQueryString() const;

		// gets pointer to header value
			// associated with a header name
		// if not found, return NULL
		const HeaderValue* getHeaderValue
			(const HeaderName& headerName);

		// returns the requested method
		const Method& getMethod();

		// returns full path of the requested path
		const std::string& getFullPath();

		const StatusCodeType& getStatusCode();

		const RequestType& getRequestType();

		/******* setters *******/
		void setStatusCode(const StatusCodeType& code);

		void setRequestType(const RequestType& requestType);

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

		// pointer to location that contains the
			// configuration of the requested path
		// it will be set when the request uri
			// path is known (if ever)
		ConstLocPtr mLocation;

		// contains the bytes read from socket
			// that still need to be processed
		std::string mBuffer;

		// tracks the previous size of the buffer
			// before new characters are
			// appended to it
		std::string::size_type mLastBuffSize;

		RequestHeaders mHeaders;

		// contains information about the request url
			// (path, query string, and the full path
			// after finding the corresponding location
			// context of the path)
		URL mURL;

		// it is initialized at first to OK, but changed
			// on error or redirection
		StatusCodeType mStatusCode;

		// type of parsed request
		RequestType mRequestType;

		// changed to false if cannot read from socket
		bool mSocketOk;

		// amount by which to read from socket
		static size_t mReadSize;
		// maximum size a request line can be
		static size_t mRequestLineSizeLimit;
		// mazimum size of the headers
		static size_t mHeadersSizeLimit;

		// supported http methods are stored as a
			// key of a stringifyied Method and
			// value of its enum
		static std::map<std::string, Method> mSupportedMethods;

		/******* private member functions *******/

		/* all the parse functions move prematurely to the
		 * 	finish stage and set the status code to an error
		 * 	class code in case of error
		 */
		// contains the whole logic that parses
			// the request
		void parseRequest();

		// checks if the whole request line has been read and that the request
			// uri doesn't exceed the request line size limit, and then parses
			// the method and uri
		// sets mLastBuffSize to 0 so that the next parsing function starts by
			// searching from the start of the buffer so no characters are
			// left behind
		void parseRequestLine();

		// checks if the whole headers (including the body separator)
			// are read and that they are not more the headers size
			// limit and then parses them
		// sets mLastBuffSize to 0 so that the next parsing function starts by
			// searching from the start of the buffer so no characters are
			// left behind
		void parseHeaders();

		// parses the body if needed, and keeps checking that the
			// read body doesn't exceed the maximum size limit
		// The body will be read only in these cases:
			// + no error is encountered
			// + post request if it's a cgi or upload
		// the body is unchunked if the 'transfer-encoding:
			// chunked' header is encountered
		void parseBody();

		// returns true and sets the request method if found,
			// otherwise it's left unchanged
		// the search for the method doesn't go further
			// than endOfLinePos
		// moves the buffer to next token
		bool parseMethod(const std::string::size_type endOfLinePos);

		// takes the end position of the request line 
			// then extract the url path and parses it
		// sets the matched location
		// returns true if the url is valid 
		// moves the buffer to the beginning of the headers
		bool parseURL(const std::string::size_type endOfLinePos);

		// determines the type of request made and either moves to
			// the finish stage if no request body is needed or to
			// the body stage and calls parseBody()
		void determineRequestType();

		// moves to the finish stage and
			// sets the status code
		void moveFinStage(const StatusCodeType code);

		static void setSupportedMethods();

		// logs the received request
		void logRequest();

};
