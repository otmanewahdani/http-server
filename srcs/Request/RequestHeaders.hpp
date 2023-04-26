/* this file contains the definition of the RequestHeaders class.
 * As the name suggests this class is responsible for parsing
 * http headers until the entity body is found. It's given
 * a string buffer from which it reads the headers. After parsing
 * the headers, the number of bytes of the headers and the blank
 * line, separating the headers from the body, is returned
 */

#pragma once

#include <string>
#include <cctype>
#include <map>
#include <set>
#include <utils.hpp>

class RequestHeaders {

	public:
		/******* public alias types  *******/
		typedef std::string HeaderName;
		typedef std::string HeaderValue;

		/******* public member functions *******/
		// takes a string buffer from which
			// the headers will be read
		RequestHeaders(const std::string& buffer);

		// parses the request headers and
			// stores their size in mHeadersSize
			// and marks the parsing as done
		// if it is called again, it has no effect
		// throws std::runtime_error on error
		void parse();

		size_t getHeadersSize();

		// gets pointer to header value
			// associated with a header name
		// if not found, return NULL
		const HeaderValue* getHeaderValue
			(const HeaderName& headerName);

		static void initializeStaticData();

	private:
		/******* private alias types  *******/
		typedef std::string::size_type
			StrSizeType;

		/******* private member objects *******/
		// if headers are parsed already,
			// it is set true
		bool mDone;

		const std::string& mBuffer;

		// stores the number of bytes
			// in the headers
		size_t mHeadersSize;

		// parsed headers are saved here
		std::map<HeaderName, HeaderValue> mHeaders;

		// stores the headers names that are supported
		static std::set<HeaderName> mHeaderNames;

		// remembers if mHeaderNames were set
			// it is set to false at first but once
			// mHeaderNames is filled it will be
			// always set to true to avoid
			// setting mHeaderNames again
		static bool mHeaderNamesSet;

		/******* private member functions *******/
		// initializes mHeaderNames
		static void setHeaderNames();

		/* all these functions operate on mBuffer */
		// takes a begin position and returns a position
			// where the line ends (carriage return
			// + new line found). new pos points to '\r'
		// if there is no newline, std::runtime_error is thrown
		StrSizeType getLineEndPos(const StrSizeType begin);

		// returns a header name if found. It is transformed to lowercase
		// The header name should start at begin and end at a colon ':'
			// and be one of the header names that are looked for (mHeaderNames),
			// otherwise an empty str is returned.
		// nextPos is set to the character right after ':' if found, otherwise
			// it is unchanged
		// the search for the header name  will stop at endPos
		std::string getHeaderName(const StrSizeType begin,
			const StrSizeType endPos, StrSizeType& nextPos);

		// skips white space (SP/HT) starting from begin,
			// the reads the whole header name value until endPos
		// returns empty string if no value is found
		std::string getHeaderValue(const StrSizeType begin,
			const StrSizeType endPos);

};
