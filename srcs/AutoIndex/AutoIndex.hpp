/* this file contains the definition of the AutoIndex class.
 * This class generates an html listing of a directory that's
 * passed to it. The listing is in the following format:
 *  [File Size Last-Modified], and the generated html listing
 *  will be saved in the file path that is passed to it
 */

#pragma once

#include <fstream>
#include <utils.hpp>
#include <vector>
#include <Config.hpp>

class AutoIndex {

	public:
		/******* alias types *******/
		typedef Config::ConstLocPtr ConstLocPtr;

		/******* public member functions *******/
		AutoIndex(const std::string& dirPath, 
			const std::string& listingFilePath,
			ConstLocPtr location);
		
		// generates the listing html file
			// by extracting the input directory
			// elements and saves the generated 
			// html in the outputFile
		// throws std::runtime_error on failure
		void generate();

	private:
		/******* private member objects *******/
		// the file path where the generated html
			// listing will be saved
		const std::string mListingFilePath;

		// the input directory that will be listed
		const std::string mDirPath;

		// pointer to location that contains the
			// configuration of the requested path
		ConstLocPtr mLocation;

		// contains the first lines of the listing 
			// html file , this part of the html file
			// is independent of the input directory
			// content and only defines the first
			// structural lines
		static const std::string mListingHtmlHeader;

		// contains the ending lines of the listing
			// html file, this part is independent
			// of the input directory content,
			// and only defines the closing
			// tags of the listing html file
		static const std::string mListingHtmlFooter;

		/******* private member functions *******/
		
		// extracts the input directory content
			// and returns string format of
			// an html list that stores the info
			// of each element of the input dir
		// throws std::runtime_error on failure
		const std::string generateDirListing();

		// takes the input directory element
		// returns the element info in the format
			// [File Size Last-Modified] encapsulated 
			// in a <tr> html tag
		// throws std::runtime_error on failure
		const std::string generateDirElementRow
			(const std::string& dirElement);
		
		// takes the input directory element
		// returns the link that will be used
			// to request the given element
			// encapsulated in a <td> html tag
		// throws std::runtime_error on failure
		const std::string generateLinkCell
			(const std::string& dirElement);

		// takes the input directory element
		// returns the given element size
			// if it's a file otherwise
			// a '-' character is returned
		// the element size will be encapsulated 
			// in a <td> html tag
		// throws std::runtime_error on failure
		const std::string generateSizeCell
			(const std::string& dirElement);

		// takes the input directory element
		// returns the last modification time
			// of the given element encapsulated in 
			// a <td> html tag
		// throws std::runtime_error on failure
		const std::string generateTimeCell
			(const std::string& dirElement);

		const std::string encapsulateInTag
			(const std::string& content, const std::string tag);

		const std::string encapsulateInHyperLink
			(const std::string& content);
		
};

