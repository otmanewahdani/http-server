/* this file contains the implementation of AutoIndex class */

#include <AutoIndex.hpp>

const std::string AutoIndex::mHtmlListingHeader 
	= "<!DOCTYPE html><html>"
	"<head><title>Directory Listing</title></head>"
	"<body><h1>Directory Listing</h1>"
	"<table style=\"font-size: 25px\">"
    "<thead><tr><th>Name</th>"
    "<th>Size</th><th>Last Modified</th>"
    "</tr></thead><tbody>";

const std::string AutoIndex::mHtmlListingFooter
	= "</tbody></table>"
	"</body></html>";

AutoIndex::AutoIndex(const std::string& dirPath, 
	const std::string& outputFilePath,
	ConstLocPtr location)
	:mOutputFilePath(outputFilePath) 
	, mDirPath(dirPath)
	, mLocation(location) {}

void AutoIndex::generate() {

	const std::vector<std::string>& dirContent 
		= getDirContent(mDirPath);
	
	std::ofstream listingStream;
	listingStream.open(mOutputFilePath.c_str(),  
		std::ofstream::in | std::ofstream::binary);
	
	listingStream << mHtmlListingHeader;

	std::vector<std::string>::const_iterator dirEntity;
	for(dirEntity = dirContent.begin(); dirEntity != dirContent.end(); ++dirEntity) {
		listingStream << getDirEntityInfo(*dirEntity);
	}

	listingStream << mHtmlListingFooter;

	listingStream.close();

}

const std::string& AutoIndex::
	getDirEntityInfo(const std::string& dirEntity) {
		//<tr>
		//<th><a href="replaceByRoute()">dirEntity</a></th>
		//<th>getSize()</th>
		// gte time
		//<th>timetostr</th>
		//</tr>
}