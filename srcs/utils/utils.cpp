/* this file contains the implementation of the functions in utils.hpp */

#include <utils.hpp>

bool isDir(const std::string& path) {

	// info about path is set by stat()
		// in this structure
	struct stat pathInfo;

	// if file doesn't exist
	if (stat(path.c_str(), &pathInfo))
		return false;
	// if file exists but isn't a directory
	if ( !(pathInfo.st_mode & S_IFDIR)  )
		return false;

	return true;

}

bool isPath(const std::string& path) {

	// if file doesn't exist
	if (access(path.c_str(), F_OK))
		return false;

	return true;

}

bool isPathRead(const std::string& path) {

	// if file isn't readble
	if (access(path.c_str(), R_OK))
		return false;

	return true;

}

bool isPathExec(const std::string& path) {

	// if file isn't executable
	if (access(path.c_str(), X_OK))
		return false;

	return true;

}

void throwErrnoException(const std::string& context) {

	std::string errorMsg = context + ": ";
	errorMsg += strerror(errno);
	throw std::runtime_error(errorMsg);

}

void makeFDNonBlock(int fd) {

	if (fcntl(fd, F_SETFD, O_NONBLOCK) == -1)
		throwErrnoException("failed to make FD non-blocking");

}
