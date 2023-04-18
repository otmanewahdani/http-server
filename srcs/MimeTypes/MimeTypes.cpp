/* this file contains the implementation of the MimeTypes class */

#include <MimeTypes.hpp>

const std::string  MimeTypes::defaultType = "application/octet-stream";

MimeTypes::MimeTypes(const char* fileNamePath) {

    if (!fileNamePath)
		throw std::invalid_argument("Mime filename is NULL");

	std::ifstream MimeFile(fileNamePath);
	if (!MimeFile) {
        std::string ErroMsg = "Failed to open Mime file : "; 
        ErroMsg += fileNamePath;
        throw std::runtime_error(ErroMsg);
    }

    ParseMimeData(MimeFile);

    MimeFile.close();

}

const MimeTypes::MimeType& MimeTypes::getType(const Extension& extension) const {

    MimeTypesContainer::const_iterator it;

    //search for the given extension
    it = mData.find(extension);

    //if extension is not found 
    if(extension.empty() || it == mData.end())
        return defaultType;
    
    return it->second;

}

//throw a parsing excpt depending on the token type and value
void MimeTypes::ThrowParsingExcpt(std::string tokenType, std::string token) {

    std::string errorMsg = "Invalid ";
    errorMsg += tokenType ;
    errorMsg += " : ";
    if(token.empty())
        errorMsg += "empty token";
    else
        errorMsg += token;

    throw std::runtime_error(errorMsg);
}

//extract individual tokens from the streamLine 
    //and return false to indicate extraction failure
bool MimeTypes::NextToken(std::istringstream& streamLine, std::string &token) {
    return bool(streamLine >> token);
}

void MimeTypes::AddType(std::istringstream& streamLine, std::string &type) {

    //read first token , empty if not found
    NextToken(streamLine,type);

    //throw excpt if type format is invalid
    IsType(type);

}

void MimeTypes::AddExtension(std::istringstream& streamLine, Extension &extension) {

    // //clean extension string from previous value
    extension.erase();

    // read next token , empty if not found
    NextToken(streamLine, extension);

    //throw excpt if extension format is invalid
    IsExtension(extension);

}

//check if the token has a valid type format : "type/subtype"
bool MimeTypes::IsType(std::string &type) {
  
    std::size_t found = type.find("/");

    // check if the character '/' exist once and is in the middle
    if(type.empty() || found == std::string::npos || type.find("/" , found+1) != std::string::npos
    || found == 0 || found == type.size() - 1 )
        ThrowParsingExcpt("Mimetype", type);

    return true;
}

bool MimeTypes::IsSpecialCharacter(Extension &extension) {

    //check if extension has a special char
    if(extension.find_first_of(" :\\/?*\"<>[]{}%") != std::string::npos)
        return true;

    return (false);
}

//check if the token has a valid extension format
bool MimeTypes::IsExtension(Extension &extension) {

    if(extension.empty() || IsSpecialCharacter(extension))
        ThrowParsingExcpt("Extension", extension);
    
    return (true);
}

//add entrey to our underlying map
void MimeTypes::AddPair(Extension &extension, MimeType& type) {
    mData.insert(std::make_pair(extension, type));
}


void MimeTypes::ParseMimeData(std::ifstream& mimeFile) {

    std::string line;
    MimeType type;
    Extension extension;

    //reading line by line
    while(std::getline(mimeFile, line)) {

        //ignore empty line
        if(line.empty())
            continue;

        //creates an object of type std::istringstream named streamLine
            //and initialize it with the string line
        std::istringstream streamLine(line);

        //parse the type and extension of the first entry of the line 
        AddType(streamLine, type);
        AddExtension(streamLine, extension);

        AddPair(extension, type);

        //parse and add entries with additional extensions who has same mimetype
        while(NextToken(streamLine,extension) && IsExtension(extension))
            AddPair(extension, type);
    }
}


void MimeTypes::Print() {

    for(MimeTypesContainer::iterator it = mData.begin() ; it != mData.end() ; it++)
    {
        std::cout << "Entry : < " <<  it->first << " , " << it->second << " >\n";
        std::cout << "      Extension : " << "\"" << it->first << "\";\n"; 
        std::cout << "      Type : " << "\"" << it->second << "\";\n\n"; 
    }

    std::cout << "Parsing Status : OK\n";
}