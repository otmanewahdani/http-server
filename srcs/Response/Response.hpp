/* this file contains the definition of the Response class
 * It is reponsible for taking a fully parsed http request,
 * 	generating an appropriate response and sending it over
 * 	the client's socket
 */

#pragma once

#include <StatusCodeHandler.hpp>
#include <Config.hpp>
#include <stdexcept>
#include <unistd.h>
#include <utils.hpp>
#include <MimeTypes.hpp>

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
		typedef Config::StatusCode StatusCode;
		typedef Config::Path Path;
		typedef StatusCodeHandler::StatusCodeType StatusCodeType;
		typedef MimeTypes::MimeType Mimetype;
		typedef std::string Extension;

		/******* public member functions *******/
		// first parameter is the socket on which
			// the response is sent
		// server refers to the server on which
			// the socket is connected and therefore
			// contains the needed configuration info
		Response(Socket socket, const Request& request,
			ConstServerRef server, const MimeTypes& mimeTypes);

		~Response();

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
		// gets all the necessary info it needs from
			// the request member such us status code
		// should only be called once in the lifetime of
			// a Response's object, otherwise
			// std::runtime_error is thrown
		void start(ConstLocPtr location);

		// returns the mimetypes member
		const MimeTypes& getMimeTypes() const;
	
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

		// status code to be sent
		StatusCodeType mStatusCode;

		// is response done
		bool mDone;

		// did response already start
		bool mStart;

		// buffer containing the response
			// bytes to be sent
		// bytes will keep being added to
			// this buffer until there is no
			// more bytes to be sent or an
			// error occurs
		std::string mBuffer;

		// file where the body to be sent is stored
			// if the response will send one
		// if mIsDelBodyFile is set, this file will
			// be removed
		std::string mBodyFileName;

		// the file stream where the response body is
			// stored will be turned into a
			// file stream before sending it
		std::ifstream mBodyStream;

		// response headers
		// connection: close header pair is always present
		std::map<std::string, std::string> mHeaders;

		// stores if the headers-body separator is needed
		// by default, there is a body separator unless
			// it is provided by another response type
			// like CGI
		bool mIsSeparator;

		// stores whether the file containing the sent
			// body should be deleted
		// it's deleted in the destructor
		bool mIsDelBodyFile;

		// contains the types needed for content-type
		const MimeTypes& mMimeTypes;

		// amount of bytes sent on each send attempt
		const static size_t mSendSize;

		// amount of bytes read from the body stream
			// on each attempt
		// these are the bytes that are sent in the
			// response body message
		const static size_t mReadSize;

		/******* private member functions *******/
		// contains the main logic that generates
			// the reponse
		void generateResponse();

		// sends the generated response over the
			// socket
		void sendResponse();

		// appends the approriate status line
			// to the sending buffer
		void generateStatusLine();

		// appends the appropriate headers
			// to the sending buffer
		void generateHeaders();

		// appends a CRLF separator to the sending
		// buffer if needed (if mIsSeparator is set)
		void addHeadersBodySeparator();

		// if the response requires a body, opens the stream
			// of the file where the message body exists
		// sets status code to an error code if the file
			// couldn't be opened
		void openBodyStream();

		/* these functions check the type of response to be made,
		 *  generare it and set its headers in the headers members
		 * If there is an entity body to be sent, its full path
		 *  is set in mBodyFileName
		 * Errors may happen during these functions so in that case
		 *  the status code is set accordingly
		 */
		// checks if the reponse contains an error status and looks
			// for the error entity to be sent if there is one
		bool isError();

		// sets the location header
		bool isRedirect();

		// checks if the response will return a regular file
		bool isContent();

		// checks if the response will return a default file
			// which is located in the location
			// config of the requested path (mLocation)
		bool isDefault();

		// checks if it's a cgi request and runs the CGI script
		bool isCGI();

		// checks if it's an autoindex request and generate
			// a directory listing to be sent in the entity body
		bool isAutoIndex();

		// checks if it's a delete request and deletes the path
			// if it can (maybe it doesn't exist or has no permissions)
		bool isDelete();

		// logs the response in the Log module
		void logResponse();

		// search the mime type associated to mBodyFileName
			// and add the content-type response-header field
			// with the retrieved type to mHeaders
		void setContentType();

};

// this file was included here because it includes ClientHandler.hpp which in turn includes
// Response.hpp  and it  relies on its full definition of the Response class
#include <Request.hpp>
