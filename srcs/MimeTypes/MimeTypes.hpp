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
#include <Tokenizer.hpp>

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
	
		void print();

	private:
		/******* private nested types *********/
		typedef std::pair<MimeType,Extension> MimePair;
		
		/******* private member objects *******/
		MimeTypesContainer mData;

		//default mime type returned if an extension isn't found
		const static std::string defaultType ;

		/******* private member functions *******/

		//parse the mime file and store the result in the underlying map
		void parseMimeData(std::ifstream& mimeFile);

		// read token  and check if it's a valid type
		void addType(Tokenizer &tokenizer, MimePair &mimePair);

		//check if the given token has a valid type format else throw excpt
		bool isType(MimePair &mimePair);

		// read token  and check if it's a valid extension
		void addExtension(Tokenizer &tokenizer, MimePair &mimePair);

		//check if the given token has a valid Extension format else throw excpt
		bool isExtension(MimePair &mimePair);

		//check if the extention contain a special char
		bool isSpecialCharacter(Extension &extension);

		// add a new entry of pair<extension , type> to the underlying map
		void addPair(MimePair &mimePair);

		//throw a parsing excpt depending on the token type(type or extension) 
		void throwParsingExcpt(std::string tokneType, const MimePair &mimePair);


};
