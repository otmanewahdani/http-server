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

class AutoIndex {

	public:
		/******* public member functions *******/
		AutoIndex(const std::string& dirPath, 
			const std::string& outputFilePath);
		
		void generate();

	private:
		/******* private member objects *******/
		// the filepath where the generated html
			// listing will be saved
		const std::string mOutputFilePath;

		// the input directory that will be listed
		const std::string mDirPath;

		// contains the first lines of the listing 
			// html file
		static const std::string mHtmlListingHeader;

		// contains the ending lines of the listinfg
			// html file
		static const std::string mHtmlListingFooter;

		/******* private member functions *******/
		const std::string& getDirEntityInfo
			(const std::string& dirEntity);
};

