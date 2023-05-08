/* this file contains the implementation of AutoIndex class */

#include <AutoIndex.hpp>

const std::string AutoIndex::mListingHtmlHeader
	= "<!DOCTYPE html><html>"
	"<head><title>Directory Listing</title></head>"
	"<body><h1>Directory Listing</h1>"
	"<table style=\"font-size: 25px\">"
    "<thead><tr><th>Name</th>"
    "<th>Size</th><th>Last Modified</th>"
    "</tr></thead><tbody>";

const std::string AutoIndex::mListingHtmlFooter
	= "</tbody></table>"
	"</body></html>";

AutoIndex::AutoIndex(const std::string& dirPath, 
	const std::string& listingFilePath,
	ConstLocPtr location)
	: mListingFilePath(listingFilePath) 
	, mDirPath(dirPath)
	, mLocation(location) {}

void AutoIndex::generate() {
	
	// opens the output file that will contains
		// the generated html
	std::ofstream listingStream;
	listingStream.open(mListingFilePath.c_str(),  
		std::ofstream::in | std::ofstream::binary);

	// file couldn't be opened
	if (!listingStream) {

		const std::string errorMsg =
			std::string("generate(): couldn't open file: '")
			+ mOutputFilePath + '\'';
		throw std::runtime_error(errorMsg);

	}
	
	// writes the listing html header which contains
		// the first structural html lines into
		// the output file
	listingStream << mListingHtmlHeader;

	// generates string format of the html directory
		// listing and write it into the output file
	listingStream << generateDirListing();

	// writes the listing html footer which contains
		// the closing html tags into
		// the output file
	listingStream << mListingHtmlFooter;

	listingStream.close();

}
