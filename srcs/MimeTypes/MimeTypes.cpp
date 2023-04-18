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

    parseMimeData(MimeFile);

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

void MimeTypes::throwInvalidType(const MimeType &type) {

    std::string errorMsg ;
    errorMsg = "Invalid mime type : ";
    errorMsg += type;

    throw std::runtime_error(errorMsg);
}

void MimeTypes::throwInvalidExtention(const MimePair &mimePair) {

    std::string errorMsg ;

    errorMsg = "Invalid extension \" ";
    errorMsg += mimePair.first;
    errorMsg += " \" for type : ";
    errorMsg += mimePair.second;

    throw std::runtime_error(errorMsg);
}

void MimeTypes::addType(Tokenizer &tokenizer, MimePair &mimePair) {

    //read first token , empty if not found
    tokenizer.nextToken(mimePair.second);

    //throw excpt if type format is invalid
    isType(mimePair); 

}

//check if the token has a valid type format : "type/subtype"
bool MimeTypes::isType(MimePair &mimePair) {
  
    std::size_t found = mimePair.second.find("/");

    // check if the character '/' exist once and is in the middle
    if(mimePair.second.empty() || found == std::string::npos 
    || mimePair.second.find("/" , found+1) != std::string::npos
    || found == 0 || found == mimePair.second.size() - 1 )
        throwInvalidType(mimePair.second);

    return true;
}

void MimeTypes::addExtension(Tokenizer &tokenizer, MimePair &mimePair) {

    // read next token , empty if not found
    tokenizer.nextToken(mimePair.first);

    //throw excpt if extension format is invalid
    isExtension(mimePair);

}


bool MimeTypes::isSpecialCharacter(Extension &extension) {

    //check if extension has a special char
    if(extension.find_first_of(" :\\/?*\"<>[]{}%") != std::string::npos)
        return true;

    return (false);
}

//check if the token has a valid extension format
bool MimeTypes::isExtension(MimePair &mimePair) {

    if(mimePair.first.empty() || isSpecialCharacter(mimePair.first))
        throwInvalidExtention(mimePair);
    
    return (true);
}

//add entrey to our underlying map
void MimeTypes::addPair(MimePair &mimePair) {
    mData.insert(mimePair);
}


void MimeTypes::parseMimeData(std::ifstream& mimeFile) {

    std::string line;

    //reading line by line
    while(std::getline(mimeFile, line)) {

        //ignore empty line
        if(line.empty())
            continue;

        //creates an object of type std::istringstream named streamLine
            //and initialize it with the string line
        Tokenizer tokenizer(line);

        //create pair<extension,type>
        MimePair    mimePair;

        //parse the type and extension of the first entry of the line 
        addType(tokenizer, mimePair);
        addExtension(tokenizer, mimePair);

        addPair(mimePair);

        //parse and add entries with additional extensions who has same mimetype
        while(tokenizer.nextToken(mimePair.first) && isExtension(mimePair))
            addPair(mimePair);
    }
}


void MimeTypes::print() {

    for(MimeTypesContainer::iterator it = mData.begin() ; it != mData.end() ; it++)
    {
        std::cout << "Entry : < " <<  it->first << " , " << it->second << " >\n";
        std::cout << "      Extension : " << "\"" << it->first << "\";\n"; 
        std::cout << "      Type : " << "\"" << it->second << "\";\n\n"; 
    }
}
