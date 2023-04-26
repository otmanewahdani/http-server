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

// returns true if path is a directory
bool isDir(const std::string& path);

// throws a std::runtime_error an error message
	// that corresponds to errno
// prepends context with a ": " to the error msg
void throwErrnoException(const std::string& context);

// turns fd into a non-blocking file descriptor
// calls throwErrnoException() in case of error
void makeFDNonBlock(int fd);

// converts arithmetic type to string
template <class Num>
std::string toString(Num num) {

	std::ostringstream converter;
	converter << num;
	return (converter.str());

}
