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
