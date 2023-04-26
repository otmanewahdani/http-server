/* this file contains the implementation of the URL class */

#include <URL.hpp>

// init the allowed characters in a url path
const std::string URL::mAllowedChars =	"0123456789"
										"abcdefghijklmnopqrstuvwxyz"
										"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"-_.~:/?#[]@!$&'()*+,;=";

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

		addFullPath();
	}

}

void URL::parseUrl(const std::string& url) {

	checkBadCharacters(url);

	// parse if url has no bad chars
	if (mValid) {
		// get the position of the query string in the url
		size_t pos;
		pos = getQueryPos(url);

		// add the query string starting from pos if it exists
		addQueryString(url, pos);

		// add the path if it matches a mServer location
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
		mQuery = url.substr(pos + 1);
	}

}

void URL::addPath(const std::string& url, size_t len) {
	
	// construct the path string from the beginning 
		// of the url until the position of query part
	mPath = url.substr(0, len);

	// get the mServer location that matches the mPath
	getMatchedLocation();

	// if the url path doesn't match any location 
		// in mServer set NotFound error and 
		// url validity to false
	if (!mLocation) {
		setStatusCode(StatusCodeHandler::NOT_FOUND);
		// clear the path and query string 
			// to prevent getting wrong data
		mPath.clear();
		mQuery.clear();
	}

}

void URL::checkBadCharacters(const std::string& url) {

	// if a bad character is found indicate 
		//bad request error and invalid url 
	if (url.empty() || url.front() != '/' ||
		url.find_first_not_of(mAllowedChars) 
		!= std::string::npos)
		setStatusCode(StatusCodeHandler::BAD_REQUEST);

}

void URL::getMatchedLocation() {

	// copy the mpath to a tmp string and append "/" 
		// to it in case of location rout ending with "/"
	Path subPath;
	subPath = mPath;
	// append "/" if it doesn't exists at the end
	if(mPath.back() != '/')
		subPath += "/";

	// check each sub path of the url path if it 
		// matches a location in mServer starting 
		// from the most specific one
	size_t pos;
	while (!subPath.empty()) {
		// find the pos of the last "/"
		pos = subPath.rfind("/");

		// if no sub path is found
		if(pos == std::string::npos)
			break;
		// search sub path with the / char
		subPath.erase(pos + 1);
		mLocation = mServer.getLocation(subPath);

		// search sub path without the / char
		subPath.erase(pos);
		mLocation = mServer.getLocation(subPath);
	}

}

void URL::addFullPath() {

	// if no error occurred
	if (mValid) {
		// takes the current location as root
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

const URL::StatusCode& URL::getStatusCode() const {
	return mStatusCode;
}

void URL::setStatusCode(StatusCode statusCode) {

	mValid = false;
	mStatusCode = statusCode;

}
