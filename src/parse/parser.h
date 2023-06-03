#pragma once

#include "tree.h"
#include "tokenizer.h"

namespace NS_PARSER {

class Parser
{
public:
	Parser();

	Library* parse(const std::string& context);

	void reset();

	void setLibName(std::string& name) { m_LibName = name; }

private:
	/* process current token.
	 * expect the type of current token is "type".
	 * if type not match, return false.else return true
	 */
	bool checkToken(Token::TKTYPE type);

	bool absorb(Token::TKTYPE type);
	
	bool isOperator();

	// skip comments and space, until m_currToken is useful.
	void skipUselessTk();

	// rdp methods
	Library*	library();
	Module*		module();
	Statement*	statement();
	Expression* expression();
	IdRef*		idRef();
	IdDef*		idDef();
	TypeRef*	typeRef();
private:
	static int		m_cnt;
	Tokenizer		m_tokenizer;
	Token			m_currToken;
	std::string		m_context;
	std::string		m_LibName;
};

} // namespace PARSER
