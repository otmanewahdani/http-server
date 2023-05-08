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
	
	// opens the output file that will contain
		// the generated html
	mListingStream.open(mListingFilePath.c_str(),  
		std::ofstream::out | std::ofstream::trunc);

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
	mListingStream << mListingHtmlHeader;

	generateDirListing();

	// writes the listing html footer which contains
		// the closing html tags into
		// the output file
	mListingStream << mListingHtmlFooter;

	mListingStream.close();

}

void AutoIndex::generateDirListing() {

	// gets all the elements within the director
	const std::vector<std::string> dirContent 
		= getDirContent(mDirPath);

	std::vector<std::string>::const_iterator dirElement;
	// the listing starts at the third element because
		// the first two elements are '.' and '..' and
		// they're not needed
	for(dirElement = dirContent.begin() + 2; 
		dirElement != dirContent.end(); ++dirElement) {
		
		mListingStream << generateDirElementRow(*dirElement);
		if (mListingStream.fail()) {
			throw std::runtime_error
				("generateDirListing(): "
				 "couldn't write to stream");
		}
	
	}

}

std::string AutoIndex::generateDirElementRow
	(const std::string& dirElement) {
	
	const std::string elementLinkCell = generateLinkCell(dirElement);
	const std::string elementSizeCell = generateSizeCell(dirElement);
	const std::string elementTimeCell = generateTimeCell(dirElement);

	std::string dirElementRow 
		= elementLinkCell + elementSizeCell + elementTimeCell;

	// joins all the table cells and encapsulates
		// them in a table row
	encapsulateTableRow(dirElementRow);

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

std::string AutoIndex::generateLinkCell
	(const std::string& dirElement) {

	// gets the URL of the element
	const std::string& relativePath
		= mLocation->replaceByLocRoute(dirElement);

	std::string dirElementHyperLinked(dirElement);

	// turns the URL into a hyperlink and 
		// encapsulates it in an anchor tag
	encapsulateInHyperLink(dirElementHyperLinked,
			relativePath);

	// finally turns it into a table cell
	encapsulateTableCell(dirElementHyperLinked);

	return dirElementHyperLinked;

}

void AutoIndex::encapsulateInHyperLink(std::string& content,
	const std::string& link) {

	const std::string openAnchorTag = std::string(
		"<a href='" + link + "'>";
	
	const std::string closeAnchorTag = "</a>";

	encapsulateInTag(content, openAnchorTag, closeAnchorTag);

}
