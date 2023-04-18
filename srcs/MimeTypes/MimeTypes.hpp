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
#include <string.h>

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
	
		void Print();
		
	private:
		/******* private member objects *******/
		MimeTypesContainer mData;

		//default mime type returned if an extension isn't found
		const static std::string defaultType ;

		/******* private member functions *******/

		//parse the mime file and store the result in the underlying map
		void ParseMimeData(std::ifstream& mimeFile);

		//read next token of a stream line and store it in the token
		bool NextToken(std::istringstream& streamLine, std::string &token);

		
		//check if the given token has a valid type format else throw excpt
		bool IsType(std::string &type);

		// read token  and check if it's a valid type
		void AddType(std::istringstream& streamLine, MimeType &type);


		//check if the given token has a valid Extension format else throw excpt
		bool IsExtension(Extension &extension);

		//check if the extention contain a special char
		bool IsSpecialCharacter(Extension &extension);

		// read token  and check if it's a valid extension
		void AddExtension(std::istringstream& streamLine, Extension &extension);


		// add a new entry of pair<extension , type> to the underlying map
		void AddPair(Extension &extension, MimeType& type);

		//throw expt of the msg passed as paramater
		void ThrowParsingExcpt(std::string tokenType, std::string token);

};
