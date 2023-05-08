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
			const std::string& outputFilePath,
			ConstLocPtr location);
		
		// generates the listing
		void generate();

	private:
		/******* private member objects *******/
		// the file path where the generated html
			// listing will be saved
		const std::string mOutputFilePath;

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
		static const std::string mHtmlListingHeader;

		// contains the ending lines of the listing
			// html file, this part is independent
			// of the input directory content,
			// and only defines the closing
			// tags of the listing html file
		static const std::string mHtmlListingFooter;

		/******* private member functions *******/
		const std::string& getDirEntityInfo
			(const std::string& dirEntity);
};

