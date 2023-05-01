/* this file contains the prototypes of general utility
 	functions that are not associated to any classed
 * these functions usually have very small tasks and
 * can be used by different components regardless of their domain
 */

#pragma once

#include <string>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>
#include <iostream>

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
const std::string& generateFileName
	(const std::string& pathPrefix);

void closeFile(const std::string& filePath);

// converts arithmetic type to string
template <class Num>
std::string toString(Num num) {

	std::ostringstream converter;
	converter << num;
	return (converter.str());

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