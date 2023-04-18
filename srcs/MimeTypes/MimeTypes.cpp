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

    it = mData.find(extension);
    //if extension is not found 
    if(extension.empty() || it == mData.end())
        return defaultType;
    
    return it->second;

}

bool MimeTypes::IsEmptyFile(std::ifstream& mimeFile) {
    //if the first char is EOF then the file is empty 
    return mimeFile.peek() == mimeFile.eof();
}

void MimeTypes::GetType(std::string& line, std::string &type) {

    //read first token
    type = strtok(&line[0] , " ");

    IsType(type);
}

bool MimeTypes::IsType(std::string &type) {

    //check if there's a '/' in the middle of the string 
    std::size_t found = type.find("/");
    if(type.empty() || found == std::string::npos || found == 0 || found == type.size() - 1)
    {
        std::string errorMsg = "Invalid Mime type : ";
        errorMsg += type;
        throw std::runtime_error(errorMsg);
    }

}

void MimeTypes::ParseMimeData(std::ifstream& mimeFile) {

    std::string line;
    MimeType type;
    Extension extension;

    //read line by line
    while(std::getline(mimeFile, line)) {
        if(line.empty())
            continue;
        GetType(line, type);
        //GetExtension();
        //AddPair();
        // while()
        // {
        //     //GetExtension();
        //     //addpair();
        // }
    }
}
