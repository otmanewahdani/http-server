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

	static unsigned int chunkSize = -1;
	std::string::size_type AmountRead = 0;

	if (chunkSize == -1) {
		// searches for the end of chunk 
			// size line position
		const std::string::size_type
			ChunkLineEndPos = mBuffer.find("\r\n");
		
		if (ChunkLineEndPos == std::string::npos) {
			if (mBuffer.size() > mMaxBodySize) {
				mStatusCode = StatusCodeHandler::ENTITY_LARGE;
				return std::string::npos;
			}
		}
	}
		// chunkSize = parseChunkSize(AmountRead);
		// \r\n if > max
		//get teh size
		// from hex
		// check if it's valid if total + size > max
	
	// chunkSize = parseChunkData(AmountRead);
	// if (mBuffer.size() - AmountRead < chunkSize)
	// 	// write all the buffer
	// 	// chunkSize - buffer.size
	// else
		// write the chunkSize and check if == \r\n
		// chunksize = -1;

	return AmountRead;

}
