/* this file contains the definition of the RequestBody class.
 * This class is responsible for parsing a request body either using
 * 	the content-length or transfer-encoding (chunked) to determine the
 * 	length of the body. The body will be appended to a specified file.
 * The body will be read from a string buffer that will need to get
 * 	updated from an external module until the full body is read.
 * 	as long as the full body isn't read yet, a call to parse() needs
 * 	to be made.
 * The body length will be a checked against a limit that it shouldn't
 *  be exceeded otherwise an error will be set and the parsing stops.
 * The bytes that were successfully read and consumed will be returned
 *  on each parsing iteration.
 */

#pragma once

#include <fstream>
#include <string>
#include <Config.hpp>
#include <StatusCodeHandler.hpp>
#include <cstring>

class RequestBody {

	public:
		/******* nested types *******/
		// format of the body to be parsed
		enum BodyType {
			CONTENT_LENGTH,
			CHUNKED
		};

		/******* alias types *******/
		typedef Config::Size Size;
		typedef StatusCodeHandler::StatusCodeType
			StatusCodeType;

		/******* public member functions *******/
		// takes the buffer from which the body
			// will be read
		RequestBody(const std::string& buffer,
			const Size maxBodySize);

		// returns true if parsing is over
		bool isDone();

		// returns the number of consumed bytes
			// from the buffer
		// returns std::string::npos on error
		std::string::size_type parse();

		/******* setters *******/
		/* these setters should be called once
		 *  before starting the iterative
		 *  calls to parse()
		 */
		void setBodyType(const BodyType type);

		// opens file to be used for body storage
		// if it couldn't be open, the status code is set
		void setBodyStore(const std::string& filePath);

		// converts contentLength to integral type and
		// checks if it's more than max body size
		// or if it's an invalid length
		// on these errors, status code is set
		void setContentLength
			(const std::string& contentLength);

		/******* getters *******/
		// returns OK if everything goes well otherwise
			// an error status code is returned
		 StatusCodeType getStatusCode();

	private:
		/******* private member objects *******/
		const std::string& mBuffer;

		// the read body shouldn't exceed this size
		const Size mMaxBodySize;

		// file where parsed body will be stored
			// It's necessary to use setBodyStore()
			// before starting the parsing process
		// the file is closed when parsing is done
			// (in case of error too)
		std::ofstream mBodyStore;

		BodyType mBodyType;

		// stores if the body parsing is done
		bool mDone;

		// keeps track of the chunk size
			// that needs to be read
		// -1 if the the whole chunk read
		int mChunkSize;

		// the full length of the body to be read
			// if the body type is CONTENT_LENGTH
		std::string::size_type mContentLength;

		StatusCodeType mStatusCode;

		// stores the total bytes of the body 
		// that have parsed and consumed so far
		std::string::size_type mTotalReadBytes;

		/******* private member functions *******/
		// parses body when body type is CONTENT_LENGTH
		// returns std::string::npos on error
		std::string::size_type parseFullLengthBody();

		// parses body when body type is CHUNKED
		// returns std::string::npos on ENTITY_LARGE error
		// throw exception on stream failure
		std::string::size_type parseChunkedBody();

		// parses the chunk size line and sets
			// the chunk size
		// sets the chunk size to -1 if line not found
		// returns false on error
		// takes the readBytes variable that stores the 
			// consumed bytes and updates it with the
			// line size if found (including CRLF)
		bool parseChunkSize
			(std::string::size_type& readBytes);

		// reads the chunk size from the buffer 
			// and write it into the storage file
		//reads the availble data if it's less that 
			//the chunk size, and updates the chunk
			// size to the remaining size of the chunk
		// takes the readBytes variable that stores the 
			// consumed bytes and updates it with the
			// the amount read
		void parseChunkData
				(std::string::size_type& readBytes);

		// sets the status and code and marks
			// the parsing as done
		void setError(StatusCodeType code);	

};
