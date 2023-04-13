/* this file contains the definition of the lexer class used by ConfigParser
* This lexer doesn't tokenize whitespace. Instead, it skips it.
*/

#include <istream>
#include <string>
#include <cctype>
#include <stdexcept>

struct Token {

	/* Tokens' equivalents in config file:
	 * SRV_NAME=server_name, ERR_PAGE=error_page, SM_COL= semi colon
	 * EOS = end of stream (last token in input stream)
	 * CLIENT_MAX = client_body_size_max, LOC=location, SWITCH= on/off
	 * ALLOW=allow_methods, METHOD= actual method value (GET, POST, ..)
	 * RDR=redirect, AUTOIN = autoindex, DEFLT=default, EXT=extension
	 * UPLOAD=upload, LB=left brace, RB=right brace OTHER= anything else
	 */
	enum Type {
		SRV_NAME,
		LISTEN,
		ERR_PAGE,
		CLIENT_MAX,
		LOC,
		ALLOW,
		METHOD,
		RDR,
		ROOT,
		AUTOIN,
		SWITCH,
		DFLT,
		EXT,
		CGI,
		UPLOAD,
		LB,
		RB,
		NUM,
		SM_COL,
		OTHER,
		EOS
	};

	Type type;
	std::string value;

	// initializes type to EOS
	Token();

};

class ConfigLexer {

	public:
		/******* public member functions *******/
		ConfigLexer(std::istream& input);

		// returns mCurrentTok and sets it to next extracted token
			// from mInput
		Token next();

		// returns mCurrentTok
		Token peek();
	
	private:
		/******* private member objects *******/
		std::istream& mInput;

		// current token that was extracted from mInput by a call
			// to next
		Token mCurrentTok;

		/******* private member functions *******/
		// all the functions that create or modify a token,
			// operate on mCurrentTok member
		// gets new token from input stream
		// skips all leading whitespace  by calling skipWhiteSpace()
		// creates a token value of either a special character if it was
			// the first character to be encountered , a string of
			// characters up to the first special or whitespace
			// character found or a an empty string if no characters
			// were found (after skipping whitespace of course)
		// calls checkInputHealth() at the end (throws)
		void extractNewToken();

		// checks token's value and assigns its type accordingly
		void setTypeForNewToken();

		// resets token to initial state (empty token)
			// and sets type to EOS
		void clearToken();

		bool isSpecialChar(char c);

		bool isWhiteSpace(char c);

		// calls checkInputHealth() at the end (throws)
		void skipWhiteSpace();

		// throws std::runtime_error if input stream fails
		void checkInputHealth();

		// checks if a token's value is a file extension (.c, .html, etc.)
			// i.e.: has Token::Type value of EXT
		bool isExtension();

		// checks if token's value has Token::Type value of METHOD
		bool isMethod();

		// checks if token's value is a number of a decimal base
			// Token::Type == NUM
		bool isNumber();

};
