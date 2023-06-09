/* this file contains the definition of the RequestBody class. */

#include <RequestBody.hpp>

RequestBody::RequestBody(const std::string& buffer,
	const Size maxBodySize)
	: mBuffer(buffer)
	, mMaxBodySize(maxBodySize)
	, mDone()
	, mContentLength()
	, mStatusCode(StatusCodeHandler::OK)
	, mTotalReadBytes()
	, mChunkSize(-1) {}

bool RequestBody::isDone() {
	return mDone;
}

bool RequestBody::isValid() {
	return (mStatusCode == StatusCodeHandler::OK);
}

std::string::size_type
	RequestBody::parse() {
	
	switch (mBodyType) {
		case CHUNKED:
			return parseChunkedBody();
		case CONTENT_LENGTH:
			return parseFullLengthBody();
	}

	// this statement is to silent gcc compiler
		// warnings since mBodyType will always
		// either be CHUNKED or CONTENT_LENGTH
	return 0;

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
		throw ;
	}

	// updates the total of the bytes that were read
	mTotalReadBytes += readBytes;

	// checks if the full body was read
	if (mContentLength == mTotalReadBytes) {
		mDone = true;
		// flushing the stream after reading everything
		mBodyStore << std::flush;
	}

	return readBytes;
	
}

void RequestBody::setError(StatusCodeType code) {

	mStatusCode = code;
	mDone = true;

}

std::string::size_type 
	RequestBody::parseChunkedBody() {

	// stores the bytes consumed from mBuffer
	std::string::size_type readBytes = 0;

	// keeps pasing mBuffer until no more
		// data is availble or an error occurs
	while (readBytes != mBuffer.size()) {

		// if the chunk size is not set 
			// extract the chunk size 
			// from the chunk size line
		if (mChunkSize == -1) {
			// on error or chunk size not found
			// if the last chunk size found (==0)
				// sets mDone and stops
			if (parseChunkSize(readBytes) == false)
				break;
		}

		if (parseChunkData(readBytes) == true) {
			// the whole chunk data is parsed
				// checks the chunk separator
			if (parseChunkSeparator(readBytes) == false)
				break;
		}
	
	}

	// add the consumed bytes to the total
		// bytes consumed from the body
	mTotalReadBytes += readBytes;

	// returns the consumed bytes after each call
	return readBytes;

}

bool RequestBody::parseChunkSize
	(std::string::size_type& readBytes) {
	
	// searches for the end of chunk
		// size line position starting
		// from the last position
		// of the consumed data
	const std::string::size_type
		chunkLineEndPos = mBuffer.find("\r\n", readBytes);
	
	// chunk size line not found
	if (chunkLineEndPos == std::string::npos) {
		// request body too large
		if (mMaxBodySize && mTotalReadBytes 
			+ mBuffer.size()  > mMaxBodySize) {

			setError(StatusCodeHandler::ENTITY_LARGE);
			readBytes = std::string::npos;
			return false;
		
		}
		// retries in next call
		return false;
	}


	// found but exceeds limit (including the 2 bytes of the CRLF)
	if (mMaxBodySize && mTotalReadBytes 
		+ chunkLineEndPos + 2 > mMaxBodySize) {

		setError(StatusCodeHandler::ENTITY_LARGE);
		readBytes = std::string::npos;
		return false;
	
	}

	// chunk size line found
	try {
		// extract the decimal value from the chunk size line
		mChunkSize = hexToDecimal<int>
			(mBuffer.substr(readBytes, chunkLineEndPos));

		// the size that need to be read inclding the 2 bytes of the chunk
			// separator (CRLF) exceeds the max body size
		if (mMaxBodySize && chunkLineEndPos + 2 + mChunkSize
			 + 2 + mTotalReadBytes > mMaxBodySize) {

			setError(StatusCodeHandler::ENTITY_LARGE);
			readBytes = std::string::npos;
			return false;
		
		}

		// the last chunk in the body entity
		if (mChunkSize == 0) {
			// flushes the whole body into the stream
			mBodyStore << std::flush;
			mDone = true;
			return false;
		}
	} 
	catch(const std::exception& error) {
		setError(StatusCodeHandler::SERVER_ERROR);
		throw ;
	}

	// add the line size and the 2 bytes
		// of the CRLF to the consumed bytes
	readBytes = chunkLineEndPos + 2;

	// returns true if the chunk size 
		// was retrieved successfully
	return true;

}


bool  RequestBody::parseChunkData
	(std::string::size_type& readBytes) {
	
	// stores the size of data not consumed yet
	const int remaining = mBuffer.size() - readBytes;

	// set to the size of bytes that will be parsed
		// to the body storage file
	// set to the available data size if it's less than 
		// the chunk size that still need to be read
	// otherwise , set to the chunk size
 	const std::string::size_type  chunkReadSize 
		= remaining < mChunkSize 
		? remaining: mChunkSize;

	// write the unconsumed data 
		// to the body storage file
	// rethrow the error on stream failure
	try {
		writeToStream(mBodyStore, mBuffer.c_str() 
			+ readBytes, chunkReadSize);
	}
	catch(const std::exception& error) {
		setError(StatusCodeHandler::SERVER_ERROR);
		throw ;
	}

	// updates the chunk size for next
		// calls
	mChunkSize -= chunkReadSize;

	// adds the bytes read from the chunk
		// to the number of bytes consumed
	readBytes += chunkReadSize ;

	// the chunk parsing not done
		// yet, more data needed
	if (mChunkSize != 0)
		return false;

	// the whole chunk parsing is done
	return true;
	
}

bool RequestBody::parseChunkSeparator
			(std::string::size_type& readBytes) {
	
	// stores the size of data not consumed yet
	const int remaining = mBuffer.size() - readBytes;

	// the chunk data not fully parsed
		// or the chunk separator
		// not read yet
	if (mChunkSize != 0 || remaining < 2)
		return false;

	// to indicates that the whole
		// chunk parsing is done
		// including the separator (CRLF)
	mChunkSize = -1;

	// add the separator size
		// to the consumed bytes
	readBytes += 2;

	return true;

}
