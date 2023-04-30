/* this file contains the implementation of the URL class */

#include <URL.hpp>

// init the unallowed characters in a url path
const std::string URL::mForbiddenChars = "<\">\\`{^|}";

URL::URL(ConstServerRef server)
	 :mServer(server),
	 mPath(""),
	 mFullPath(""),
	 mQuery(""),
	 mValid(true),
	 mParsed(false),
	 mLocation(NULL),
	 mStatusCode(StatusCodeHandler::OK) {} 

void URL::parse(const std::string& url) {

	// parses the string only if it's the 
		// first call of parse()
	if (!mParsed) {

		mUrl = url;
	
		// to prevent multiple calls
		mParsed = true;

		// parse the url path and query string
		parseUrl(url);
		
		// if url is valid , construct the full path
		addFullPath();
	}

}

void URL::parseUrl(const std::string& url) {

	// if the url is empty or doesn't starts with the root
	if (url.empty() || url.front() != '/') {
		setErrorStatusCode(StatusCodeHandler::BAD_REQUEST);
		return ;
	}
	
	// parse the path and get the position 
		// of the query string in the url
	const std::string::size_type
		queryPos = parsePath(url);

	// parse the query string if the url state still valid
	parseQueryString(url, queryPos);

}

std::string::size_type
	URL::parsePath(const std::string& url) {
	
	// to avoid string realloacation
	mPath.reserve(url.size() + 1);

	// parses the url path by reading and checking 
		// each character of the url and add it to
		//  mPath if it's valid until 
		// the end of the url or the beginning 
		// of the query string
	std::string::size_type i = 0;
	while( i < url.size() && url[i] != '?') {

		// sets error and invalid url if it's a bad char
		if (isForbiddenChar(url[i])) {
			setErrorStatusCode(StatusCodeHandler::BAD_REQUEST);
			return std::string::npos;
		}

		// add the valid character
		mPath.insert(mPath.end(), url[i]);
		++i;

	}

	// sets the most specific location 
		// that matches the url path within mServer
	mLocation = mServer.matchLocation(mPath);

	// if the url path doesn't match any location 
		// in mServer set NotFound error and invalid url
	if (!mLocation)
		setErrorStatusCode(StatusCodeHandler::NOT_FOUND);

	// returns pos of the query string
	return i;

}

bool URL::isForbiddenChar(const char c) {

	// if the char is unprintable or belongs to 
		// the forbidden characters return false
	return (c <= ' ' || c > '~' || mForbiddenChars.find(c) != std::string::npos);

}

void URL::parseQueryString(const std::string& url, const size_t queryPos) {

	// checks if there is a query string to be parsed
	if (mValid && queryPos != std::string::npos
		&& queryPos < url.size()) {
		// to avoid string reallocation
		mQuery.reserve(url.size() - mPath.size() + 1);

		// reads each character in the url starting 
			// from the start position of the query string
		for (std::string::size_type i = queryPos + 1;
				i != url.size(); ++i) {
			// sets error and invalid url if it's a bad char
			if (isForbiddenChar(url[i])) {
				setErrorStatusCode(StatusCodeHandler::BAD_REQUEST);
				return ;
			}

			// insert char in mQuery if it's valid
			mQuery.insert(mQuery.end(), url[i]);
		}
	}
	
}

void URL::addFullPath() {

	// if the url was parsed successfully and the 
		// matched location was found. construct the
		// full path of the requested resource by replacing 
		// the prefix subpath withing the url path
		// with the location root if it exists
		// or the current directory if not
	if (mLocation && mValid) {
		mFullPath = mLocation->replaceByRoot(mPath);
	}

}

bool URL::isValid() const {
	return mValid;
}

const URL::Path& URL::getPath() const {
	return mPath;
}

const URL::FullPath& URL::getFullPath() const {
	return mFullPath;
}

const URL::QueryString& URL::getQueryString() const {
	return mQuery;
}

const URL::StatusCodeType& URL::getStatusCode() const {
	return mStatusCode;
}

URL::ConstLocptr URL::getLocation() const {
	return mLocation;
}

const std::string& URL::getURLStr() const {
	return mUrl;
}

void URL::setErrorStatusCode(StatusCodeType statusCode) {

	mValid = false;
	mStatusCode = statusCode;
	mPath.clear();
	mQuery.clear();
	mFullPath.clear();

}

void URL::print() const {

	std::cout << "URL info :" << '\n';

	std::cout << "\tFull URL: " << mUrl << '\n';
	std::cout << "\tPath: " << mPath << '\n';
	std::cout << "\tQuery string: " << mQuery << '\n';
	std::cout << "\tFull Path: " << mFullPath << '\n';

	std::cout.flush();

}
