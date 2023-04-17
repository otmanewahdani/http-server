/* this file contains the implementation of the MimeTypes class */
/*possible errors in mime file :
    -> missing type or extension
    -> dup ?
    -> mime_type formt defined in RFC 2045 : top-level_type/subtype the top_level known types are 
     [text, image, application, audio, video, multipart, message]
    -> whitespaces
    ->empty file (nginex has its built-in MIME types)? define a default mime_type container?
*/

#include <MimeTypes.hpp>


MimeTypes::MimeTypes(const char* fileNamePath)
{
    if (!fileNamePath)
		throw std::invalid_argument("Mime filename is NULL");
	
	std::fstream MimeFile(fileNamePath);
	if (!MimeFile)
    {
        std::string ErroMsg = "Failed to open Mime file : "; 
        ErroMsg += fileNamePath;
         throw std::runtime_error(ErroMsg);
    }
    ParseMimeData(MimeFile);
    MimeFile.close();
}

const MimeTypes::MimeType& MimeTypes::operator[] (const Extension& extension) const
{
    MimeTypesContainer::const_iterator it ;
    it = MimeData.find(extension);
    if(extension.empty() || it == MimeData.end())
        return "application/octet-stream";
    return it->second;
}

const MimeTypes::MimeType& MimeTypes::getType(const Extension& extension) const
{
    return (*this)[extension];
}

void ParseMimeData(std::istream& MimeFile)
{
    // IfEmptyFile() ---->
    // addDefaultType ? ------>
    // read line 
    // isValidline()
    // split with space 
    // isType() && isValidType()
    // isExtension() && is validExtension
    // addPair()
}