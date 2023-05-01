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

	static unsigned int chunkSize;
	const std::string::size_type
		lineEndPos = mBuffer.find("\r\n");
	
}
