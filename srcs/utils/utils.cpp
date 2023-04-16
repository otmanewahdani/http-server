/* this file contains the implementation of the functions in utils.hpp */

#include <utils.hpp>

bool isDir(const std::string& path) {

	// info about path is set by stat()
		// in this structure
	struct stat pathInfo;

	// if file doesn't exist
	if (stat(path.c_str()))
		return false;
	// if file exists but isn't a directory
	if ( !(pathInfo.st_mode & S_IFDIR)  )
		return false;

	return true;

}
