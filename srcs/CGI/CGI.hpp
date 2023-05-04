/* this file contains the definition of the CGI class.
 * It gets an input in case of post, runs a CGI script
 *  with a specified executable and writes the output to a file.
 * The input and output files should be set before
 *  the CGI is run.
 * All the other necessary information will be retrieved
 *  from the request object that's passed to its constructor
 */

#pragma once

#include <Request.hpp>
#include <StatusCodeHandler.hpp>

class CGI {

	public:
		/******* alias types *******/
		typedef Request::Method Method;
		typedef StatusCodeHandler::StatusCodeType
			StatusCodeType;

		/******* public member functions *******/
		CGI(const Request& request);

		setInputFilePath(const std::string& path);

		setOutputFilePath(const std::string& path);

		void run();

		// was the cgi run correctly
		bool isValid();

		StatusCodeType getStatusCode();
	
	private:
		/******* private member objects *******/
		// the file path from which the CGI
			// gets its input (request body)
		// if the method is not GET, then there
			// is no input
		std::string mInputFilePath;

		// the filepath where the cgi script
			// will save its output
		std::string mOutputFilePath;

		StatusCodeType mStatusCode;
		
		/******* private member functions *******/

};
