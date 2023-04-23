/* this file contains the definition of the URL class.
 * This class is reponsible for parsing a url string
 * and checking the validity of the url. A url is invalid
 * if the path does not match with any location within
 * the server that is passed to the constructor
 * The URL is organized then into a path, full path and
 * 	a query string
 */

#pragma once

class URL {

	public:
		/******* public member functions *******/
		// server containing location info about
			// the requested resource
		URL(ConstServerRef server);

		parse(const std::string& url);

	private:
		/******* private member objects *******/

};
