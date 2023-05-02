/* this file contains the definition of the RequestBody class. */

#include <RequestBody.hpp>

std::string::size_type
	RequestBody::parse() {
	
	switch (mBodyType) {
		case CHUNKED:
			return parseChunkedBody();
		case CONTENT_LENGTH:
			return parseFullLengthBody();
	}

}

std::string::size_type RequestBody::parseChunkedBody() {

	// stores the bytes consumed from mBuffer
	std::string::size_type readBytes = 0;

	// keeps processing the mBuffer data
		// untill an error occured or more
		// data is needed
	while (readBytes != mBuffer.size() && mChunkSize != 0) {

		// if the chunk size is not set 
			// extract the chunk size 
			// from the chunk size line
		if (mChunkSize == -1) {
			
			// if an error occured while parsing the
				// chunk size
			if (parseChunkSize(readBytes) == false)
				return std::string::npos;
			// if the chunk line size not found
				// return the conusumed bytes
				// and wait for the next incoming data
			if(mChunkSize == -1)
				return readBytes;
			// reads the end of the body entity
				// set done and returns the consumed 
				// bytes 
			if(mChunkSize == 0) {
				mDone = true;
				// flushes the whole body entity
					// into the stream
				mBodyStore << std::flush;
				return readBytes;
			}

		}

		// reads mChunkSize bytes from the buffer
			// and write to the storage file
		parseChunkData(readBytes);
	
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
		// size line position
	const std::string::size_type
		chunkLineEndPos = mBuffer.find("\r\n", readBytes);
	
	// chunk size line not found
	if (chunkLineEndPos == std::string::npos) {
		// request body too large
		if (mMaxBodySize && mTotalReadBytes + mBuffer.size()  > mMaxBodySize) {
			setError(StatusCodeHandler::ENTITY_LARGE);
			return false;
		}
		// retries in next call
		return true;
	}

	// found but exceeds limit
	if (mMaxBodySize && mTotalReadBytes + chunkLineEndPos + 2 > mMaxBodySize) {
		setError(StatusCodeHandler::ENTITY_LARGE);
		return false;
	}

	// chunk size line found
	try {
		// extract the decimal value of chunk size
		mChunkSize = hexToDecimal<unsigned int>(mBuffer.substr(readBytes, chunkLineEndPos));

		// the size that need to be read exceeds the max body size
		if (mMaxBodySize && mChunkSize + 2 + mTotalReadBytes > mMaxBodySize) {
			setError(StatusCodeHandler::ENTITY_LARGE);
			return false;
		}

	} 
	catch(const std::exception& error) {
		setError(StatusCodeHandler::SERVER_ERROR);
		throw error;
	}

	// add the line size and the CRLF 
		// to the consumed bytes
	readBytes = chunkLineEndPos + 2;

	return true;

}


void RequestBody::parseChunkData
	(std::string::size_type& readBytes) {
	
	// stores the number of bytes not consumed++
	const int remaining = mBuffer.size() - readBytes;

	// if remaining bytes are bigger than the chunk size need to be read
 	const std::string::size_type  chunkReadSize = remaining >= mChunkSize ? mChunkSize: remaining;

	try {
		writeToStream(mBodyStore, mBuffer.c_str() + readBytes, chunkReadSize);
	}
	catch(const std::exception& error) {
		setError(StatusCodeHandler::SERVER_ERROR);
		throw error;
	}

	// updates the size of the chunk 
		//size that still need to be read
	mChunkSize -= chunkReadSize;

	// adds the bytes read from the chunk
		// to the number of bytes consumed
	readBytes += chunkReadSize ;

	// if the whole chunk data was consumed
		// and the chunk separator exists 
		// add it to the number of consumed
		// bytes and mark the chunk as done
		// by assigning -1 to it's size 
	if (mChunkSize ==  0 && remaining >= 2) {
		mChunkSize = -1;
		readBytes += 2 ;
	}
	
}