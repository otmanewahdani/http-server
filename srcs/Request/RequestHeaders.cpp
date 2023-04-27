/* this file contains the implementation of the RequestHeaders class.
 */

#include <RequestHeaders.hpp>

std::set<std::string> RequestHeaders::mHeaderNames;

bool RequestHeaders::mHeaderNamesSet = false;

RequestHeaders::RequestHeaders(const std::string& buffer)
	: mDone()
	, mBuffer(buffer)
	, mHeadersSize() {

}

void RequestHeaders::parse() {

	// don't parse if already parsed
	if (mDone)
		return ;

	// pos: tracks current position in the buffer
	// headerValPos: points at the position of a
		// header value at a header field
	StrSizeType pos = 0, headerValPos = 0,
	// used to get the end position of
		// the next line on each iteration
		lineEndPos = std::string::npos;

	std::string headerName;

	while (pos < mBuffer.size()) {

		lineEndPos = getLineEndPos(pos);

		// checks if the line found is
			// the headers-body separator
		// if the end of the line is the same
			// as pos then it means it's a "\r\n"
		if (lineEndPos == pos)
			break ;

		// gets header name if found and stores
			// the start position of the header value
		headerName = getHeaderName
			(pos, lineEndPos, headerValPos);

		// if found, it is added to the headers
			// along with the header value
		if (headerName.empty() == false) {
			
			// adds headerName and header value 
			mHeaders[headerName] = getHeaderValue
				(headerValPos, lineEndPos);

		}

		// moves to position after
			// end of line ("\r\n")
		pos = lineEndPos + 2;

	}

	// the size of the parsed headers
		// up to the body
	mHeadersSize = lineEndPos + 2;

	// finished parsing the headers
	mDone = true;

}

const RequestHeaders::HeaderValue*
	RequestHeaders::getHeaderValue
	(const HeaderName& headerName) {

	// finds whole header field by its header name
	std::map<HeaderName, HeaderValue>::const_iterator
		header = mHeaders.find(headerName);

	// not found
	if (header == mHeaders.end())
		return NULL;

	// returns the header value part
	return &header->second;

}

size_t RequestHeaders::getSize() {
	return mHeadersSize;
}

RequestHeaders::StrSizeType
	RequestHeaders::getLineEndPos
	(const StrSizeType begin) {
	
	StrSizeType lineEndPos =
		mBuffer.find("\r\n", begin);
	// not found
	if (lineEndPos == std::string::npos) {

		const std::string errorMsg = 
			std::string("RequestHeaders: couldn't "
				"find the end of line "
				"starting from position: "
				+ toString(begin));

		throw std::runtime_error(errorMsg);

	}

	return lineEndPos;

}

void RequestHeaders::setHeaderNames() {

	// if the header names are already set
	if (mHeaderNamesSet)
		return ;

	mHeaderNames.insert("content-type");
	mHeaderNames.insert("content-length");
	mHeaderNames.insert("host");
	mHeaderNames.insert("cookie");

	mHeaderNamesSet = true;

}

std::string RequestHeaders::getHeaderName
	(const StrSizeType begin, const StrSizeType endPos,
	 StrSizeType& nextPos) {

	// searches for ':'
	const StrSizeType colonPos = mBuffer.find(':', begin);
	// if not found
	if (colonPos == std::string::npos
		// or found after endPos
		|| colonPos >= endPos)
		return "";

	// copies found header name
	std::string headerName(mBuffer, begin, colonPos - begin);
	// converts it to lower case
	for (StrSizeType i = 0; i < headerName.size(); ++i)
		headerName[i] = std::tolower(headerName[i]);

	// if the found header name is supported
	if (mHeaderNames.find(headerName) != mHeaderNames.end()) {
		nextPos = colonPos + 1;
		return headerName;
	}

	// the found header name isn't supported
	return "";

}

std::string RequestHeaders::getHeaderValue
	(const StrSizeType begin, const StrSizeType endPos) {

	// skips white space
	const StrSizeType headerValPos =
			mBuffer.find_first_not_of(" \t", begin);

	if (headerValPos >= endPos)
		return "";

	// copies a string of (endPos - headerValPos) characters
	return std::string(mBuffer, headerValPos,
		endPos - headerValPos);

}

void RequestHeaders::initializeStaticData() {
	setHeaderNames();
}
