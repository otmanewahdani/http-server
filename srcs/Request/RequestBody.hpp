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
#include <stdexcept>
#include <utils.hpp>

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
		// also the maximum size that the body
			// shoudln't exceed
		RequestBody(const std::string& buffer,
			const Size maxBodySize);

		// returns true if parsing is over
		bool isDone();

		// returns the number of consumed bytes
			// from the buffer
		// throws std::exception on error
		// qlso returns std::string::npos on error
		std::string::size_type parse();

		/******* setters *******/
		/* these setters should be called once
		 *  before starting the iterative
		 *  calls to parse()
		 */
		void setBodyType(const BodyType type);

		// opens file to be used for body storage
		// if it couldn't be opened, the status code is set
		// throws std::runtime_error on error
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
		// a value of 0 means there is no size limit
		const Size mMaxBodySize;

		// file where parsed body will be stored
			// It's necessary to use setBodyStore()
			// before starting the parsing process
		std::ofstream mBodyStore;

		BodyType mBodyType;

		// stores if the body parsing is done
		bool mDone;

		// the full length of the body to be read
			// if the body type is CONTENT_LENGTH
		std::string::size_type mContentLength;

		StatusCodeType mStatusCode;

		// stores the total bytes of the body 
		// that have parsed and consumed so far
		std::string::size_type mTotalReadBytes;

		/******* private member functions *******/
		// parses body when body type is CONTENT_LENGTH
		// throws std::exception on error
		// returns the number of bytes that were consumed
			// from the buffer
		std::string::size_type parseFullLengthBody();

		// parses body when body type is CHUNKED
		// throws std::exception on error
		// returns the number of bytes that were consumed
			// from the buffer
		// also returns std::string::npos on error
		std::string::size_type parseChunkedBody();

		// sets the status and code and marks
			// the parsing as done
		// also closes body store file
		void setError(StatusCodeType code);

};
