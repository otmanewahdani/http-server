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
			+ mListingFilePath + '\'';
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

const std::string AutoIndex::generateDirListing() {

	const std::vector<std::string> dirContent 
		= getDirContent(mDirPath);
	
	std::string dirContentList;

	std::vector<std::string>::const_iterator dirElement;
	for(dirElement = dirContent.begin(); 
		dirElement != dirContent.end(); ++dirElement) {
		
		dirContentList += generateDirElementRow(*dirElement);
	
	}

	return dirContentList;

}

const std::string AutoIndex::generateDirElementRow
		(const std::string& dirElement) {
	
	const std::string elementLinkCell = generateLinkCell(dirElement);
	const std::string elementSizeCell = generateSizeCell(dirElement);
	const std::string elementTimeCell = generateTimeCell(dirElement);

	const std::string dirElementInfo 
		= elementLinkCell + elementSizeCell + elementTimeCell;

	const std::string dirElementRow =
		encapsulateInTag(dirElementInfo, "tr");

	return dirElementRow;

}

const std::string AutoIndex::generateSizeCell
	(const std::string& dirElement) {

	const std::string elementSize = 
		isDir(mDirPath + dirElement) == false ? 
		toString(getFileSize(dirElement))
		: "-";
	
	const std::string elementSizeCell 
		= encapsulateInTag(elementSize, "td");
	
	return elementSize;

}
