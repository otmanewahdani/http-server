/* this file contains the implementation of the URL class */

#include <URL.hpp>


URL::URL(ConstServerRef server)
	:mServer(server)
	:mValid(true)
	:mParsed(false) {}


void URL::parse(const std::string& url) {

	if(!mParsed) {
		parseUrl();
		addFullPath();
	}

}

void URL::parseUrl(const std::string& url) {

	checkBadCharacter(url);

	// get the position of the query string in the url
	size_t pos;
	pos = getQueryPos(url);

	// add the query string starting from pos if it exists
	addQueryString(url, pos);

	// add the path if it matches a mSeerver location
	addPath(url, pos);

}

size_t URL::getQueryPos(const std::string& url) {
	return (url.find("?"));
}

void URL::addQueryString(const std::string& url, size_t pos) {
	
	if(pos != std::string::npos) {
		// construct the mQuery string starting 
			//from pos to the end of the url
		mQuery = url.substr(pos);
	}

}

void URL::addPath(const std::string& url, size_t len) {

	// construct the path string from the beginning 
		// of the url until the position of query part
	mPath = substr(0, len);

	// get the mServer location that matches the mPath
	mLocation = getMatchedLocation();

	// if the url path doesn't match any location in mServer set NotFound 

}