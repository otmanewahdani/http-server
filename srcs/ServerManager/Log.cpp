/* This file contains the implementation of the Log class 
 * resonsible on logging into mLogfile
 * The log message has the following format:
 * [date time] [level of severity] [message] where the
 * level of severity represents the type of msg (info , error, warning ...)
 */

#include <Log.hpp>

const std::string Log::mInfoNotice = "[INFO] ";
const std::string Log::mErrorNotice = "[ERROR] ";

// log file creation
std::ofstream Log::mLogfile("./log_files/basic_logfile",
	std::ofstream::out | std::ofstream::trunc);

// Print the date and time in the format [YYYY-MM-DD HH:MM:SS]
	// by using time() fuction to get the current time 
	// then localtime() to convert it to local time expression
void Log::addTimeDate() {

	// Get the current time
	time_t rawTime = time(0);

  	// Convert to local time expression
  	tm* timeInfo = localtime(&rawTime);

  	// retrieve the date and time from tm struct returned by localtime()
		// and print it in the format [YYYY-MM-DD HH:MM:SS]
		// by appending the characters "[ ]-:" to timeInfo struct members
  	mLogfile << "["
			<< timeInfo->tm_year + 1900 << '-'
            << timeInfo->tm_mon + 1 << '-'
            << timeInfo->tm_mday << ' '
            << timeInfo->tm_hour << ':'
            << timeInfo->tm_min << ':'
            << timeInfo->tm_sec << "] ";

}

void Log::socketBinding(const Socket socket) {

	const std::string socketLog =
		"socket id = " +
		std::to_string(socket);

	// current operation description
	const std::string op =
		"server listening on";

	try {
		// retrieve the host:port from the server socket
		const std::string serverName 
			= Network::getSocketServerName(socket);

		// adds time only after the exception-throwing
			// function so that it doesn't get
			// added again in catch statement
		addTimeDate();
		
		// logs that the server is listening
			// on the retrieved host:port 
		mLogfile << mInfoNotice << op << ' ' << serverName
			<< ", " << socketLog << '\n';
	}
	catch(const std::exception& e) {
		// logs that the server is listening on some
			// unknown host:port if the retrieval fails
		const std::string logMessage = op +
			" unknown host:port, " + socketLog
			+ ", reason: " + e.what();
		error(logMessage);
	}

	mLogfile << std::flush;
				
}

void Log::connectionEstablished(const Socket socket) {

	// new connection established between
		// client and server
	logClientServerOperation(socket,
		"new connection established",
		"between", "and");

}

void Log::request(const Socket socket,
	const std::string& method, 
	const std::string& url) {

	// operation description
	const std::string op =
		method + " request for url: "
		+ '\'' + url + '\''
		+ " received";

	// request for 'uri' received
		// from client by server
	logClientServerOperation(socket,
		op, "from", "by");

}

void Log::response(const Socket socket) {

	// response sent to client by server
	logClientServerOperation(socket,
		"response sent", "to", "by");

}

void Log::connectionClosed(const Socket socket) {

	// connection closed between client and server
	logClientServerOperation(socket,
		"connection closed", "between", "and");

}

void Log::error(const std::string& errorMsg) {

	// writes to stream the current time
		// in [YYYY-MM-DD HH:MM:SS] format
	addTimeDate();

	mLogfile << mErrorNotice << errorMsg
		<< "\n" << std::flush;

}

void Log::logClientServerOperation(const Socket socket,
	const std::string& op, const std::string& clientPrep,
	const std::string& serverPrep) {

	const std::string socketLog =
		"on socket " + std::to_string(socket)
		+ ", ";

	/* the "reason:" field contains the explanation
	 * why the server or client name couldn't
	 * be retrieved
	 */

	// tries to get both client and server names
	try {
		
		const std::string clientName 
			= Network::getSocketClientName(socket);

		// after successfully getting client name
			// it tries to get server name
		try {
			const std::string serverName 
				= Network::getSocketServerName(socket);

			addTimeDate();

			mLogfile << mInfoNotice << socketLog << op << " "
				<< clientPrep << " client " << clientName
				<< " " << serverPrep << " server " << serverName
				<< "\n" << std::flush;
		}
		// if it fails to retrieve the server name, it
			// logs the operation with the client name only
		catch(const std::exception& e) {
			
			const std::string logMessage = socketLog + op + " "
				+ clientPrep + " " + clientName + " " + serverPrep +
				" unknown server, " + "reason: " + e.what();
			error(logMessage);

		}

	}
	// couldn't get client name and hasn't
		// tried to get server name yet
	catch(const std::exception& e1) {

		// so it tries to get only server name
		try {

			const std::string serverName 
				= Network::getSocketServerName(socket);

			// logs the operation with server name only
			const std::string logMessage = socketLog + op + " "
				+ clientPrep + " unknown client " + serverPrep +
				" " + serverName + ", reason: " + e1.what();
			error(logMessage);

		}
		// couldn't get any names
		catch(const std::exception& e2) {

			// logs the operation with no names
			const std::string logMessage = socketLog + op + " "
				+ clientPrep + " unknown client " + serverPrep +
				" unknown server " + ", reason1: " + e1.what()
				+ ", reason2: " + e2.what();
			error(logMessage);

		}

	}

}
