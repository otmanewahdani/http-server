/* this file contains the definition of the RequestHeaders class.
 * As the name suggests this class is responsible for parsing
 * http headers until the entity body is found. It's a given
 * a string buffer from which it reads the headers. After parsing
 * the headers, the number of bytes of the headers and the blank
 * line, separating the headers from the body, is returned
 */

#pragma once

class RequestHeaders {

	public:
		/******* public member functions *******/
		RequestHeaders(const std::string& buffer);

		// parses the request headers and
			// stores their size in mHeadersSize
			// and marks the parsing as done
		// if it is called again, it has no effect
		void parse();

		size_t getHeadersSize();

	private:
		/******* private member objects *******/
		// if headers are parsed already,
			// it is set true
		bool mDone;

		const std::string& mBuffer;

		// stores the number of bytes
		// in the headers
		size_t mHeadersSize;

};
