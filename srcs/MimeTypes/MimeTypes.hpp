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

class MimeTypes {

	public:
		/******* public member variables *******/
		//const defaultType;
		/******* alias types *******/
		typedef std::string Extension;
		typedef std::string MimeType;
		// underlying container type that holds
			// (extension, mime type) pairs
		// One file extension maps to at most one MIME type. One MIME type maps to zero or more file extensions?
		typedef std::multimap<Extension, MimeType> MimeTypesContainer;

		/******* public member functions *******/
		MimeTypes(const char* fileNamePath);

		// returns mime type associated with the extension
		// if not found, returns empty value
	
		// Note : If a file extension is not recognized or specified in the server's configuration
		// most servers (ex : nginx) will typically send the file with a default MIME type of application/octet-stream.
		// This MIME type indicates that the file is a binary file and that the server does not know how to handle it.	
		const MimeType& getType(const Extension& extension) const ;

		//overload the operator [] to returns the Mimetype of a given key and the default type
		// if an extansion is invalid or doesn't exists
		const MimeType& operator[] (const Extension& extension) const;
	
	private:
		/******* private member objects *******/
		MimeTypesContainer MimeData;
		//BuiltinstMimeData

		/******* private member functions *******/
		void ParseMimeData(std::istream& MimeFile);
};
