/* this file contains the definition of the lexer class used by ConfigParser
* This lexer doesn't tokenize whitespace. Instead, it skips it.
*/

#include <istream>
#include <string>

struct Token {

	/* Tokens' equivalents in config file:
	 * SRV_NAME=server_name, ERR_PAGE=error_page, SM_COL= semi colon
	 * EOF = end of file (last token in input stream)
	 * CLIENT_MAX = client_body_size_max, LOC=location
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
		DEFLT,
		EXT,
		UPLOAD,
		LB,
		RB,
		NUM,
		SM_COL,
		OTHER,
		EOF
	};

	Type type;
	std::string value;

	// initializes type to EOF
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

};
