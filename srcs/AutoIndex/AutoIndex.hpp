/* this file contains the definition of the AutoIndex class.
 * This class generates an html listing of a directory that's
 * passed to it. The listing is in the following format:
 *  [File Size Last-Modified], and the generated html listing
 *  will be saved in the file path that is passed to it
 * A hyperlink will be embedded in the File name containing
 *  A URL to the file. The URL will be relative to the location
 *  parameter that's passed to this class
 */

#pragma once

#include <fstream>
#include <utils.hpp>
#include <vector>
#include <Config.hpp>
#include <stdexcept>

class AutoIndex {

	public:
		/******* alias types *******/
		typedef Config::ConstLocPtr ConstLocPtr;

		/******* public member functions *******/
		AutoIndex(const std::string& dirPath, 
			const std::string& listingFileOutputPath,
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
		std::string mListingFilePath;

		// the input directory that will be listed
		std::string mDirPath;

		// pointer to location that contains the
			// configuration of the requested path
		ConstLocPtr mLocation;

		// stream where the output of the autoindex
			// listing will be generated
		std::ofstream mListingStream;

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

		/* all generate*() functions call external functions
		 *  that may throw std::exception
		 */
		
		// extracts the input directory content
			// writes an html section of table rows for each element
			// to the listing to the listing stream member
		void generateDirListing();

		// takes the input directory element
		// returns the element info in the format
			// [File Size Last-Modified] encapsulated 
			// in a <tr> html tag
		std::string generateDirElementRow
			(const std::string& dirElement);
		
		// takes the input directory element
		// returns the link that will be used
			// to request the given element
			// encapsulated in a <td> html tag
		std::string generateLinkCell
			(const std::string& dirElement);

		// takes the input directory element
		// returns the given element size
			// if it's a file otherwise
			// a '-' character is returned
		// the element size will be encapsulated 
			// in a <td> html tag
		std::string generateSizeCell
			(const std::string& dirElement);

		// takes the input directory element
		// returns the last modification time
			// of the given element encapsulated in 
			// a <td> html tag
		std::string generateTimeCell
			(const std::string& dirElement);

		/* all encapsulte*() functions call encapsulateInTag()
		 *  for their encapsulation needs
		 */
		// takes content and inserts beginTag at its beginning
			// and endTage at its end
		// example : content = name, btag = <td>, etag = </td>
		//  content becomes <td>content</td>
		void encapsulateInTag(std::string& content,
			const std::string& beginTag, const std::string& endTag);

		// takes a link and puts it in an anchor tag like this:
			// <a href='link'> and encapsulates the content
			// between that tag and this one </a>
		void encapsulateInHyperLink(std::string& content,
			const std::string& link);

		// encapsultes content between <td> and </td>
		void encapsulateTableCell(std::string& content);

		// encapsultes content between <tr> and </tr>
		void encapsulateTableRow(std::string& content);
		
};

