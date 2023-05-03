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
		// set to -1 if the chunk size
			// not retrieved yet
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
		// takes the readBytes parameter and update
			// it by adding the full lenght
			// of the chunk size line if found
		// returns false in the following cases:
		// line note found
		// returns false on error and sets readBytes 
			// to std::string::npos
		// the chunk size is 0, which is the case of the
			// last chunk in the body entity,
			// sets mDone to true and add updates
			// the readBytes by adding the full
			// line size
		// returns true if the chunkSize was 
			// retrieved successfully 
			// and updates the readBbytes
		// throws exception on error
		bool parseChunkSize
			(std::string::size_type& readBytes);

		// returns true if the whole chunk data is 
			// parsed (chunk size == 0)
		// takes the readBytes parameter and 
			// updates it by adding the 
			// consumed chunk data size
		// updates the mChunkSize by subtracting
			// the read bytes from it
		bool parseChunkData
				(std::string::size_type& readBytes);

		// returns false if the chunk separator
		// (CRLF) not found, otherwise updates
		// the readBytes parameter by adding
		// the 2 bytes of the CRLF, and sets
		// the chunk size to -1 to indicate
		// that the whole chunk was parsed
		// successfully
		bool parseChunkSeparator
			(std::string::size_type& readBytes);

		// sets the status and code and marks
			// the parsing as done
		void setError(StatusCodeType code);	

};
