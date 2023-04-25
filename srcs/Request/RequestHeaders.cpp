/* this file contains the implementation of the RequestHeaders class.
 */

#include <RequestHeaders.hpp>

std::set<std::string> RequestHeaders::mHeaderNames;

bool RequestHeaders::mHeaderNamesSet = false;

RequestHeaders::RequestHeaders(const std::string& buffer)
	: mDone()
	, mBuffer(buffer)
	, mHeadersSize() {

	// set header names

}

void RequestHeaders::parse() {

	// don't parse if already parsed
	if (mDone)
		return ;

	// tracks current position in the buffer
	std::string::size_type
		pos = 0;

	// used to get the end position of
		// the next line on each iteration
	StrSizeType lineEndPos = Npos;

	while (pos < mBuffer.size()) {

		lineEndPos = mBuffer.getLineEndPos(pos);

		// checks if the line found is
			// the headers-body separator
		// if the end of the the line is the same
			// as pos then it means it's a "\r\n"
		if (lineEndPos == pos) {
			// finished parsing the headers
			mDone = true;
			// the size of the parsed headers
				// up to the body
			mHeadersSize = lineEndPos + 2;
			break ;
		}

	}

}

RequestHeaders::StrSizeType
	RequestHeaders::getLineEndPos
	(const StrSizeType begin) {

void RequestHeaders::setHeaderNames() {

std::string RequestHeaders::getHeaderName
	(const StrSizeType begin, const StrSizeType endPos,
	 StrSizeType& nextPos) {

	StrSizeType colonPos = mBuffer.find(':');
	// not found
	if (colonPos == std::string::npos) {
		nextPos = std::string::npos;
		return "";
	}

	// copies found header name
	std::string headerName(mBuffer, begin, colonPos - begin);
	// converts it to lower case
	for (StrSizeType i = 0; i < headerName.size(); ++i)
		headerName[i] = std::tolower(headerName[i]);

	// if found header name is supported
	if (mHeaderNames.find(headerName) != mHeaderNames.end())
		return headerName;

	// found header name wasn't supported
	nextPos = std::string::npos;
	return "";

}
