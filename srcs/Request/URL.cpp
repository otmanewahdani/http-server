/* this file contains the implementation of the URL class */

#include <URL.hpp>

// init the allowed characters in a url path
const std::string URL::mAllowedChars =	"0123456789"
										"abcdefghijklmnopqrstuvwxyz"
										"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"-_.~:/?#[]@!$&'()*+,;=";

URL::URL(ConstServerRef server)
	:mServer(server),
	 mValid(true),
	 mParsed(false),
	 mLocation(NULL) {}


void URL::parse(const std::string& url) {

	if (!mParsed) {
		parseUrl(url);
		addFullPath();
	}

}

void URL::parseUrl(const std::string& url) {

	checkBadCharacters(url);

	if (mValid) {
		// get the position of the query string in the url
		size_t pos;
		pos = getQueryPos(url);

		// add the query string starting from pos if it exists
		addQueryString(url, pos);

		// add the path if it matches a mSeerver location
		addPath(url, pos);
	}

}

size_t URL::getQueryPos(const std::string& url) {
	return (url.find("?"));
}

void URL::addQueryString(const std::string& url, size_t pos) {
	
	if (pos != std::string::npos) {
		// construct the mQuery string starting 
			//from pos to the end of the url
		mQuery = url.substr(pos);
	}

}

void URL::addPath(const std::string& url, size_t len) {

	// construct the path string from the beginning 
		// of the url until the position of query part
	mPath = url.substr(0, len);

	// get the mServer location that matches the mPath
	getMatchedLocation();

	// if the url path doesn't match any location 
		// in mServer set NotFound error
	if (!mLocation) {
		mValid = false;
		mStatusCode = StatusCodeHandler::NOT_FOUND;
	}

}

void URL::checkBadCharacters(const std::string& url) {

	// if a bad request 
	if (url.find_first_not_of(mAllowedChars) != std::string::npos) {
		mValid = false;
		mStatusCode = StatusCodeHandler::BAD_REQUEST;
	}
}

void URL::getMatchedLocation() {

	Path TmpPath;
	TmpPath = mPath + "/";
	size_t pos;

	while (!TmpPath.empty()) {
		pos = TmpPath.rfind("/");
		TmpPath.erase(pos+1);
		mLocation = mServer.getLocation(TmpPath);
		TmpPath.erase(pos);
		mLocation = mServer.getLocation(TmpPath);
	}

}

void URL::addFullPath() {

	// appends the mPath to the root of mLocation
		// to get the full path of the requested resource
		// if no error occurred
	if (mValid)
		mFullPath = mLocation->root + mPath;

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

const URL::StatusCode& URL::getStatusCode() const {
	return mStatusCode;
}