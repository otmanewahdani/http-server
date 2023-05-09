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
	errorMsg += std::strerror(errno);
	throw std::runtime_error(errorMsg);

}

void makeFDNonBlock(int fd) {

	if (fcntl(fd, F_SETFD, O_NONBLOCK) == -1)
		throwErrnoException("failed to make FD non-blocking");

}

void writeToStream(std::ostream& stream,
	const char* str, std::streamsize count) {

	stream.write(str, count);

	if (stream.fail())
		throw std::runtime_error(
		"writeToStream(): couldn't"
		" write to stream");

}

bool removeFile(const std::string& filePath) {
	return (unlink(filePath.c_str()) == 0);
}

std::string generateFileName
	(const std::string& pathPrefix) {
	
	// generates a file name with the format 
	// [file_pid_counter] identified by :
	// the current process id to distinguish 
		// between each session files
	// the counter value to identify each file
		// within the program session
	static unsigned long counter = 0;

	// retrieve the current pid
	// throws std::runtime_error on error
	pid_t pID = getpid();
	if(pID == -1) {
		throw std::runtime_error("generateFileName():"
		" Failed to retrieve the process id");
	}

	//constructs the file name using 
		// the pid and the counter value
	const std::string fileName = "file_" 
		+ toString(pID) 
		+ "_" + toString(counter);

	// generates the full path name
	const std::string fullPathName = 
		pathPrefix + "/" + fileName;

	//increment the counter for the next call
	++counter;
	
	return fullPathName;

}

size_t getFileSize(const std::string& path) {

	// info about path is set by stat()
		// in this structure
	struct stat pathInfo;

	// if an error occurs
	if (stat(path.c_str(), &pathInfo)) {

		const std::string errorMsg =
			std::string("getFileSize(): ")
			+ "couldn't get size of file: '"
			+ path + '\'';
		throw std::runtime_error(errorMsg);

	}

	// returns the file size
	return pathInfo.st_size;

}

std::vector<std::string>
	getDirContent(const std::string& dir) {

	DIR* directory = opendir(dir.c_str());

	if (directory == NULL)
		throw std::runtime_error(
		"getDirContent(): cannot open dir");

	std::vector<std::string> entries;

	// iterates over the entries in the directory
		// and adds them to vector of entries
	struct dirent* entry = readdir(directory);
	while (entry != NULL) {

		// adding the entry's name
		entries.push_back(std::string(entry->d_name));

		// gets next entry
		entry = readdir(directory);

	}

	closedir(directory);

	return entries;

}

const std::tm* getLastModifiedTime
	(const std::string& path) {

	// info about path is set by stat()
		// in this structure
	struct stat pathInfo;

	// if file doesn't exist
	if (stat(path.c_str(), &pathInfo)) {
		const std::string errorMsg =
			std::string("getLastModifiedTime(): ")
			+ "couldn't get info of path: '"
			+ path + '\'';
		throw std::runtime_error(errorMsg);
	}

	// gets the last modified time in local time
	const std::tm* lastModifiedTime =
		std::localtime(&pathInfo.st_mtime);

	if (lastModifiedTime == NULL) {
		const std::string errorMsg =
			std::string("getLastModifiedTime(): ")
			+ "couldn't convert to local time the "
			" last modified time of : '"
			+ path + '\'';
		throw std::runtime_error(errorMsg);
	}

	return lastModifiedTime;

}

std::string timeToStr(const std::tm* time) {

	const static size_t timeStrSize = 32;

	// allocates memory in advance to be filled
		// by time
	// the produced time won't be more than
		// timeStr
	std::string timeStr(timeStrSize, '\0');

	const size_t actualTimeStrSize
		= std::strftime(
		const_cast<char*>(timeStr.data()),
		timeStrSize, "%d-%b-%Y %H:%M", time);

	// reduces the string to the number
		// of bytes actuallly written
	timeStr.resize(actualTimeStrSize);

	return timeStr;

}

std::string getCurrentDir() {

	// creates a buffer that can hold
		// the maximum length possible of a path
	char dirNameBuff[MAXPATHLEN];

	if (getcwd(dirNameBuff, MAXPATHLEN) == NULL) {
			throwErrnoException("getCurrentDir()");
	}

	return dirNameBuff;

}
