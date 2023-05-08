/* this file contains the prototypes of general utility
 	functions that are not associated to any classes
 * these functions usually have very small tasks and
 * can be used by different components regardless of their domain
 * However most of the functions revolve around getting file properties
 *  and conveting integral values to strings and vice-versa
 */

#pragma once

#include <string>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <vector>
#include <ctime>
#include <sys/param.h>

// returns true if path is a directory
bool isDir(const std::string& path);

// returns true if path exists
bool isPath(const std::string& path);

// returns true if path readable
bool isPathRead(const std::string& path);

// returns true if path executable
bool isPathExec(const std::string& path);

// throws a std::runtime_error an error message
	// that corresponds to errno
// prepends context with a ": " to the error msg
void throwErrnoException(const std::string& context);

// turns fd into a non-blocking file descriptor
// calls throwErrnoException() in case of error
void makeFDNonBlock(int fd);

// writes count bytes from str to stream
// throws std::runtime_error on error
void writeToStream(std::ostream& stream,
	const char* str, std::streamsize count);

// generates a unique temporary file name
	// and appends it to pathPrefix
// throws std::runtime_error on error
std::string generateFileName
	(const std::string& pathPrefix);

// returns true if file was removed
bool removeFile(const std::string& filePath);

// returns the size of the file
// throws std::runtime_error on error
size_t getFileSize(const std::string& path);

// gets the files and directories within
	// a specified directory
// throws std::runtime_error on failure
std::vector<std::string>
	getDirContent(const std::string& dir);

// gets the last modified time of a file
	// in local time tm format
// throws std::runtime_error on failure
const std::tm* getLastModifiedTime
	(const std::string& path);

// converts time to the following stringified 
	// format [day-mon-year hour:min]
	// example [10-Apr-2020 10:20]
std::string timeToStr(const std::tm* time);

// gets the full path of the current
	// working directory
// std::runtime_error is thrown on error
std::string getCurrentDir();

// converts arithmetic type to string
template <class Num>
std::string toString(Num num) {

	std::ostringstream converter;
	converter << num;
	return (converter.str());

}

// converts string to integral type
// std::runtime_error is thrown on error
template<class Num>
Num strToNum(const std::string& numStr) {

	std::istringstream converter(numStr);
	Num result;
	converter >> result;

	if (converter.fail()) {

		const std::string errorMsg =
			std::string("strToNum(): couldn't convert '")
			+ numStr + '\'';
		throw std::runtime_error(errorMsg);

	}

	return result;

}

// converts a hexadecimal string 
	// to an arithmetic type
// throws std::runtime_error on error
template <class Num>
Num hexToDecimal(const std::string &hex) {

	Num result;

	std::stringstream converter(hex);
	converter >> std::hex >> result;

	if(converter.fail()) {
		throw std::runtime_error("hexToDecimal():"
			" not a valid hexadecimal string");
	}

	return result;

}
