/* This file contains the definition of the MimeTypes class. This is
 * a sort of container that holds (extensions, mime types) pairs. This
 * data comes from a mime types file that's passed to its constructor.
 * After construction, an extension can be passed to a public interface 
 * of this class to query its corresponding mime type
 */

#pragma once

#include <map>
#include <string>

class MimeTypes {

	public:
		/******* alias types *******/
		typedef std::string Extension;
		typedef std::string MimeType;
		// underlying container type that holds
			// (extension, mime type) pairs
		typedef std::multimap<Extension, MimeType> MimeTypesContainer;

		/******* public member functions *******/
		MimeTypes(const char* fileNamePath);

		// returns mime type associated with the extension
		// if not found, returns empty value
		const MimeType& getType(const Extension& extension) const ;
	
	private:
		/******* private member objects *******/
		MimeTypesContainer mData;

};
