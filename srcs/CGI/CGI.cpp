/* this file contains the implementation  of the CGI class.
 */

#include <CGI.hpp>

const int CGI::mWaitTime = 1;

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

void CGI::manageExecution() {

	// creates a new process for the
		// cgi to be run
	const pid_t pid = fork();

	// the start of the new process
		// where the environment will be
		// set for the cgi to be run
	if (pid == 0) {

		setEnv();

		setScriptIO();

		const std::string& executable 
			= findExecutable();
		
		// checks execution failure 
		if (execl(executable.c_str(), 
			mScriptPath.c_str(), NULL) == -1) {
			
			mStatusCode = StatusCodeHandler::SERVER_ERROR;
			return ;

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

	const int output = open(mOutputFilePath.c_str(),  O_WRONLY);
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

	const std::string errorMsg
		= "CGI::setEnv(): missing "
		"necessary environment values";

	const bool overwite
		= true;
	
	setenv("PATH_INFO", mScriptPath.c_str(), overwite);
	setenv("SCRIPT_FILENAME", mScriptPath.c_str(), overwite);
	setenv("SCRIPT_NAME", mScriptPath.c_str(), overwite);
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