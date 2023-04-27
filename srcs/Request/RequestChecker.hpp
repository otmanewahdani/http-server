/* this file contains the definition of RequestChecker class
 *  this class is responsible for checking the validity of a
 *  Request object after it has parsed its request headers, and
 *  determining the type of request 
 *  After determining the request type, it is set in the request object
 *  	along with the status code
 */

#pragma once

#include <Request.hpp>

class RequestChecker {

	public:
		/******* public member functions *******/
		// takes the request to be checked
		RequestChecker(Request& request);

		bool isValid();
	
	private:
		/******* private member objects *******/
		Request& mRequest;

};
