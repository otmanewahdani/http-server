/*
	this type contain the definition the Tokenizer class , which is used by MimeTypes class
	to tokeniz each line passed to it constructor
*/

#pragma once

#include <iostream>
#include <sstream>

class Tokenizer {
	public :
		/******* public member functions *******/

		//initialize mstream with the string line given in parameter
		Tokenizer(std::string &line);

		//extract individual tokens from the streamLine 
    		//and return false to indicate extraction failure
		bool nextToken(std::string &token);

	private :

		/******* private member objects *******/
		//an object of type std::istringstream named streamLine
		std::istringstream streamLine;
};