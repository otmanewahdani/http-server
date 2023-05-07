/* this file contains the implementation  of the CGI class.
 */

#include <CGI.hpp>

const int CGI::mWaitTime = 1;

const size_t CGI::mMaxHeadersSize = 400;

CGI::CGI(const Request& request)
	: mStatusCode(StatusCodeHandler::OK)
	, mContentLength()
	, mRequest(request)
	, mScriptPath(mRequest.getFullPath()) {}

void CGI::run() {

	// executes the script
	manageExecution();

	// the execution was successful
	if (mStatusCode < 400)
		setContentLength();

}

void CGI::setInputFilePath
	(const std::string& path) {
	
	mInputFilePath = path;

}

void CGI::setOutputFilePath
	(const std::string& path) {
	
	mOutputFilePath = path;

}

bool CGI::isValid() {

	return (mStatusCode ==
		StatusCodeHandler::OK);

}

CGI::StatusCodeType 
	CGI::getStatusCode() {

	return mStatusCode;

}

size_t CGI::getContentLength() {
	return mContentLength;
}

void CGI::manageExecution() {

	// creates a new process for the
		// cgi to be run
	const pid_t pid = fork();

	// the start of the new process
		// where the environment will be
		// set for the cgi to be run
	if (pid == 0) {

		try {

			setEnv();

			setScriptIO();

			const std::string& executable 
				= findExecutable();
		
			// checks execution failure 
			if (execl(executable.c_str(), 
				executable.c_str(), mScriptPath.c_str(),
				NULL) == -1) {

				exit(EXIT_FAILURE);

			}

		}
		catch (const std::exception& e) {
				exit(EXIT_FAILURE);
		}

	}
	// couldn't create the new process
	else if (pid == -1) {
		mStatusCode = StatusCodeHandler::SERVER_ERROR;
		return ;
	}

	// back to the main server process
	waitForScript(pid);

}

void CGI::waitForScript(const pid_t pid) {

	// stops the execution for mWaitTime time
		// so that the script can finish its job
		// before its output is read
	sleep(mWaitTime);
	
	// gets the script's termination status
	pid_t status = waitpid(pid, NULL, WNOHANG);

	// checks if there was an error when getting
		// the termination status
	if (status < 1) {

		mStatusCode = StatusCodeHandler::SERVER_ERROR;
		// kills the script if it didn't terminate
			// on time
		if (status == 0)
			kill(pid, SIGKILL);

		return ;
	}

}

void CGI::setScriptIO() {

	// boilerplate exception message
		// to be used by different sections
		// of this function
	const std::string errorMsg = 
		"CGI::setScriptIO(): couldn't ";
	
	if (mRequest.getMethod() == Request::POST) {

		const int input = open(mInputFilePath.c_str(),  O_RDONLY);
		if (input == -1) {
			throw std::runtime_error(errorMsg
			+ "open " + mInputFilePath + " for input");
		}

		// made the standard input stream of the executed
			// script point to the input file instead so
			// it can automatically read from it
		if (dup2(input, STDIN_FILENO) == -1) {
			throw std::runtime_error(errorMsg
				+ "duplicate the input");
		}
	
	}

	// opens in write mode and clears any existing
		// data if the file exists already
	const int output = open(mOutputFilePath.c_str(),
		O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (output == -1) {
		throw std::runtime_error(errorMsg
		+ "open " + mOutputFilePath + " for output");
	}

	// made the standard output stream of the executed
		// script point to the output file instead so
		// it can automatically write to it
	if (dup2(output, STDOUT_FILENO) == -1) {
		throw std::runtime_error(errorMsg
			+ "duplicate the output");
	}
	
}

void CGI::setEnv() {

	// boilerplate exception message
		// to be used by different sections
		// of this function
	const std::string errorMsg
		= "CGI::setEnv(): missing "
		"necessary environment values";

	// overwrite environment variables if
		// they already exist
	const bool overwite
		= true;
	
	setenv("PATH_INFO", mScriptPath.c_str(), overwite);
	setenv("SCRIPT_FILENAME", mScriptPath.c_str(), overwite);
	setenv("SCRIPT_NAME", mScriptPath.c_str(), overwite);

	// This is a necessary environment variable
		// needed by php scripts
	setenv("REDIRECT_STATUS", "200", overwite);

	const std::string& queryString 
		= mRequest.getQueryString();
	setenv("QUERY_STRING", queryString.c_str(), overwite);

	const HeaderValue* header = 
		mRequest.getHeaderValue("cookie");
	if (header)
		setenv("HTTP_COOKIE", header->c_str(), overwite);
	
	const Request::Method method 
		= mRequest.getMethod();
	
	if (method == Request::GET) {
		setenv("REQUEST_METHOD", "GET", overwite);
	}
	else if (method == Request::POST) {

		setenv("REQUEST_METHOD", "POST", overwite);

		// searches for the content length of the input
			// body that the CGI will read
		// first it searches for the content-length header,
			// if found it is set as the length of the input file
		// else it looks for the transfer-encoding header
			// with the 'chunked' value, if it finds it,
			// then the size of the file containing the
			// is calculated unchunked body and set to the length
		// if neither option is available, then an exception since
			// the cgi cannot determine the length on its own
 		header = mRequest.getHeaderValue("content-length");
		if (header)
			setenv("CONTENT_LENGTH", header->c_str(), overwite);
		else {

			header = mRequest.getHeaderValue("transfer-encoding");
			if (header && *header == "chunked") {
				const size_t chunkedBodySize
					= getFileSize(mRequest.getPathToBodyFileName());
				setenv("CONTENT_LENGTH", 
					toString(chunkedBodySize).c_str(), overwite);
			}
			else {
				throw std::runtime_error(errorMsg);
			}

		}

		header = mRequest.getHeaderValue("content-type");
		if (!header)
			throw std::runtime_error(errorMsg);
			
		setenv("CONTENT_TYPE", header->c_str(), overwite);

	}
	else 
		throw std::runtime_error(errorMsg);
	
}

void CGI::setContentLength() {

	// boilerplate exception message
	// to be used by different sections
	// of this function
	const std::string errorMsg = 
		"CGI::setContentLength(): couldn't"
		" read the script's output";

	// stores the headers from the script's output.
	// and it's possible to have bytes
	// after the headers if mMaxHeadersSize
	// exceeds the total byte count of the headers
	// but tha's not an issue since we only care
	// about the CRLF body separator. However we
	// could also not reach all the headers, if
	// mMazHeadersSize is less than the actual size
	// of the headers, in that case this is an error
	// since the headers are too large
	std::string headers(mMaxHeadersSize, '\0');

	// opens the file stream containing script's output
	std::ifstream outputStream(mOutputFilePath.c_str(),
		std::ofstream::in | std::ofstream::binary);

	outputStream.read(&headers[0], mMaxHeadersSize);
	// if reading fails before reaching EOF
	if (outputStream.eof() == false
		&& outputStream.fail()) {
		
		mStatusCode = StatusCodeHandler::SERVER_ERROR;
		throw std::runtime_error(errorMsg);

	}

	// sets the headers to the actual size
		// that was read
	headers.resize(outputStream.gcount());

	const std::string::size_type bodySeparatorPos
		= headers.find("\r\n\r\n");

	// if the body separator wasn't found
	if (bodySeparatorPos == std::string::npos) {
		
		mStatusCode = StatusCodeHandler::SERVER_ERROR;
		throw std::runtime_error(errorMsg);

	}
	
	const size_t outpuFileSize =
		getFileSize(mOutputFilePath);
	
	// the size of the entity body without
		// the headers and the body separator
	mContentLength = outpuFileSize - 
		(bodySeparatorPos + 4);

}

const std::string& CGI::findExecutable() {

	// boilerplate exception message
		// to be used by different sections
		// of this function
	const std::string errorMsg 
		= "CGI::findExecutable(): "
		"couldn't find executable";
	
	// searches for the extension dot
	const std::string::size_type 
		extensionPos = mScriptPath.rfind('.');
	
	// no extension was found
	if (extensionPos == std::string::npos)
		throw std::runtime_error(errorMsg);

	// extracts the extension without the dot
	const std::string extension 
		= mScriptPath.substr(extensionPos + 1);

	// retrieves the supported CGI script extensions
	const CGISystems& supportedCGIs 
		= mRequest.getLocation()->supportedCGIs;
	
	// searches for a cgi executable that handles
		// the found extension
	CGISystems::const_iterator executable = 
		supportedCGIs.find(extension);

	// if no executable found
	if (executable == supportedCGIs.end())
		throw std::runtime_error(errorMsg);
	
	// return the executable's path
	return executable->second;

}

