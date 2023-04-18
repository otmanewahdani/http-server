/* This file contains the definition of the MimeTypes class. This is
 * a sort of container that holds (extensions, mime types) pairs. This
 * data comes from a mime types file that's passed to its constructor.
 * After construction, an extension can be passed to a public interface 
 * of this class to query its corresponding mime type
 */

#pragma once

#include <map>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

class MimeTypes {

	public:
		/******* alias types *******/
		typedef std::string Extension;
		typedef std::string MimeType;
		// underlying container type that holds
			// (extension, mime type) pairs
		typedef std::map<Extension, MimeType> MimeTypesContainer;

		/******* public member functions *******/
		MimeTypes(const char* fileNamePath);

		// returns mime type associated with the extension 
		//or the default type if it's not found
		const MimeType& getType(const Extension& extension) const ;
	
	private:
		/******* private member objects *******/
		MimeTypesContainer mData;

		//default mime type returned if an extension isn't found
		const static std::string defaultType ;

		/******* private member functions *******/

		//parse the mime file and fill in the underlying map
		void ParseMimeData(std::ifstream& mimeFile);

		//check if the file is empty
		bool IsEmptyFile(std::ifstream& mimeFile);

		// check and set the type , throw excpt if the type format is not valid
		void GetType(std::string& line, std::string &type);

		//check if the given token has a valid type format else throw excpt
		bool IsType(std::string &type);

		//check if the given token has a valid Extension format else throw excpt
		bool IsExtension();

		//peek the token from stream and store it in token (by ref)
		const std::string &NextToken(const std::string& line);
};
