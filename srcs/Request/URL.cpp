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

	// parse the string only if it's the first call of parse()
	if (!mParsed) {
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
	size_t queryPos;
	queryPos = addPath(url);

	// parse the query string if the url state still valid
	addQueryString(url, queryPos);

}

size_t URL::addPath(const std::string& url) {
	
	// to avoid string realloacation
	mPath.reserve(url.size() + 1);

	// read from the url till the end or 
		// start of query string
	size_t i = 0;
	while(url[i] && url[i] != '?') {

		if (isForbiddenChar(url[i])) {
			setErrorStatusCode(StatusCodeHandler::BAD_REQUEST);
			return i;
		}

		mPath.insert(mPath.end(), url[i]);
		++i;
	}

	// get the mServer location that matches the mPath
	getMatchedLocation();

	// if the url path doesn't match any location 
		// in mServer set NotFound error
	if (!mLocation)
		setErrorStatusCode(StatusCodeHandler::NOT_FOUND);

	// returns pos of query string
	return i;
}

bool URL::isForbiddenChar(const char c) {

	// if the char is unprintable or belongs to 
		// the forbidden characters return false
	return (c <= ' ' || c > '~' || mForbiddenChars.find(c) != std::string::npos);

}

void URL::addQueryString(const std::string& url, size_t queryPos) {

	if (mValid && queryPos != std::string::npos) {
		// to avoid string reallocation
		mQuery.reserve(url.size() - mPath.size() + 1);

		// check each character withing the url 
			// and add it if it's not forbidden
		while(url[++queryPos]) {
			// if char is forbidden set error and stop
			if (isForbiddenChar(url[queryPos])) {
				setErrorStatusCode(StatusCodeHandler::BAD_REQUEST);
				return ;
			}

			// insert char in mQuery if it's valid
			mQuery.insert(mQuery.end(), url[queryPos]);
		}
	}
	
}

void URL::getMatchedLocation() {

	// copy the mpath to a tmp string and append "/" 
		// to it in case of location rout ending with "/"
	Path subPath;
	subPath = mPath + "/";

	// check each sub path of the url path if it 
		// matches a location in mServer starting 
		// from the most specific one
	size_t pos;
	while (!subPath.empty()) {
		// find the pos of the last "/"
		pos = subPath.rfind("/");

		// search sub path with the / char
		subPath.erase(pos + 1);
		mLocation = mServer.getLocation(subPath);
		if(mLocation)
			break;

		// search sub path without the / char
		subPath.erase(pos);
		mLocation = mServer.getLocation(subPath);
		if(mLocation)
			break;
	}

}

void URL::addFullPath() {

	// if no error occurred
	if (mValid) {
		// if the matched loacation has no root
			// take the current path as root
		if (mLocation->root.empty())
			mFullPath = "." + mPath;
		else
			mFullPath = mLocation->root + mPath;
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

void URL::setErrorStatusCode(StatusCodeType statusCode) {

	mValid = false;
	mStatusCode = statusCode;
	mPath.clear();
	mQuery.clear();
	mFullPath.clear();

}
