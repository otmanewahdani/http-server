/* this file contains the definition of the RequestBody class. */

#include <RequestBody.hpp>

RequestBody::RequestBody(const std::string& buffer,
	const Size maxBodySize)
	: mBuffer(buffer)
	, mMaxBodySize(maxBodySize)
	, mDone()
	, mContentLength()
	, mStatusCode(StatusCodeHandler::OK)
	, mTotalReadBytes() {}

bool RequestBody::isDone() {
	return mDone;
}

std::string::size_type
	RequestBody::parse() {
	
	switch (mBodyType) {
		case CHUNKED:
			return parseChunkedBody();
		case CONTENT_LENGTH:
			return parseFullLengthBody();
	}

}

void RequestBody::setBodyType(const BodyType type) {
	mBodyType = type;
}

void RequestBody::setBodyStore
	(const std::string& filePath) {
	
	// opens file in binary mode and clears its content
	mBodyStore.open(filePath.c_str(), std::ofstream::out
		| std::ofstream::binary | std::ofstream::trunc);

	// file couldn't be opened
	if (!mBodyStore) {

		mStatusCode = StatusCodeHandler::SERVER_ERROR;

		const std::string errorMsg =
			std::string("setBodyStore(): couldn't open file: '")
			+ filePath + '\'';
		throw std::runtime_error(errorMsg);

	}

}

void RequestBody::setContentLength
	(const std::string& contentLength) {
	
	try {

		mContentLength = strToNum<std::string::size_type>
						(contentLength);

		// if max body limit is set and the content length
			// is larger than that
		if (mMaxBodySize && mContentLength > mMaxBodySize)
			setError(StatusCodeHandler::ENTITY_LARGE);

	}
	catch (const std::exception& e) {
		setError(StatusCodeHandler::BAD_REQUEST);
	}

}

RequestBody::StatusCodeType
	RequestBody::getStatusCode() {

	return mStatusCode;

}

std::string::size_type
	RequestBody::parseFullLengthBody() {
	
	// remaining bytes to be read
	const std::string::size_type remaining
		= mContentLength - mTotalReadBytes;

	// if there are more bytes in the buffer
		// than the remaining bytes to be read,
		// only reads the remaining bytes, otherwise
		// it reads the whole buffer
	const std::string::size_type readBytes =
		mBuffer.size() >= remaining ?
		remaining : mBuffer.size();

	try {
		// appends the read bytes to the stored request body
		writeToStream(mBodyStore, mBuffer.c_str(), readBytes);
	}
	catch (const std::exception& error) {
		// sets the appropriate status code and rethrows
		setError(StatusCodeHandler::SERVER_ERROR);
		throw error;
	}

	// updates the total of the bytes that were read
	mTotalReadBytes += readBytes;

	// checks if the full body was read
	if (mContentLength == mTotalReadBytes)
		mDone = true;

	return readBytes;
	
}

void RequestBody::setError(StatusCodeType code) {

	mStatusCode = code;
	mDone = true;

}
