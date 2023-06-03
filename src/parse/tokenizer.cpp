#include <map>
#include "tokenizer.h"


static std::vector<std::pair<std::string, Token::TKTYPE>>
/* every element stands for a "pattern : tokenType" pair.
 * every time, we match the context with patterns, until we find the start of the context is matched with a pattern.
 * if didn't match, return error.
 * todo : change type to enum
 */
	m_patterns =
{
	/* -----------------------------------------------------space----------------------------------------------------- */
	{"^\\s+",	Token::SPACE },
	/* -----------------------------------------------------comments-------------------------------------------------- */
	// single line
	{"^//.*\\n", Token::COMMENT1},
	//multi line
	{"^/\\*[\\s\\w]?[\\s\\w]+\\*/",Token::COMMENT2},
	
	/* -----------------------------------------------------key words---------------------------------------------------- */
	{"^module",		Token::K_MODULE},
	{"^endmodule",	Token::K_ENDMODULE},
	{"^input",		Token::K_INPUT},
	{"^output",		Token::K_OUTPUT},
	{"^begin",		Token::K_BEGIN},
	{"^end",		Token::K_END},
	{"^for",		Token::K_FOR},
	{"^if",			Token::K_IF},
	{"^else",		Token::K_ELSE},
	{"^assign",		Token::K_ASSIGN},
	{"^reg",		Token::K_REG},
	{"^wire",		Token::K_WIRE},
	{"^always",		Token::K_ALWAYS},
	{"^@",			Token::K_AT},

	/* -----------------------------------------------------identifiers-------------------------------------------------- */
	{"^[a-zA-Z]+", Token::ID},

	/* -----------------------------------------------------punctuatios-------------------------------------------------- */
	{"^,",		Token::P_COMMAN},
	{"^\\(",	Token::P_PARENTHESIS_L},
	{"^\\)",	Token::P_PARENTHESIS_R},
	{"^;",		Token::P_SEMICOLON},
	/* -----------------------------------------------------operators---------------------------------------------------- */
	{"^=",		Token::O_ASSIGN_BLOCKING},
	{"^<=",		Token::O_ASSIGN_NONBLOCKING},
	{"^\\+",	Token::O_ADD},
	{"^-",		Token::O_MIN},
	{"^*",		Token::O_MUL},
	{"^/",		Token::O_DIV}
};

Token&
Token::operator =(const Token& rh) {
	m_type = rh.m_type;
	m_val = rh.m_val;
	return *this;
}

Token
Tokenizer::getNextToken(bool skipEmpty) {
	// todo: if not EOF, return error, if EOF, return empty token;
	// if no more context, return false;
	if (!hasMoretoken()) {
		return Token();
	}
	// try to match context with all patterns from the beginning
	std::string tgt(m_context.begin() + m_cursor, m_context.end());
	for (auto p : m_patterns) {
		std::regex expr(p.first);
		std::smatch rsts;
		if (!std::regex_search(tgt, rsts, expr))
			continue;
		else {
			acceptToken(rsts[0], p.second);
			if (skipEmpty && (m_tokens.back().getType() == Token::SPACE ||
							  /*m_tokens.back().getType() == Token::NEXT_LINE_QT ||
							  m_tokens.back().getType() == Token::NEXT_LINE_WIN ||*/
							  m_tokens.back().getType() == Token::COMMENT1 ||
							  m_tokens.back().getType() == Token::COMMENT2)) {
				return getNextToken(true);
			}
			return m_tokens.back();
		}
	}
	return Token();
}

void
Tokenizer::acceptToken(std::string value, Token::TKTYPE type) {
	Token token(value, type);
	m_tokens.push_back(token);
	m_cursor += value.size();
}

void
Tokenizer::reset()
{
	m_context = "";
	m_cursor = -1;
	m_tokens.clear();
}