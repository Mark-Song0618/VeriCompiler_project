#include <fstream>
#include <assert.h>
#include "parser.h"
#include "traversMgr.h"

#define ASSERT(T) assert(m_currToken.getType() == (T))

using namespace NS_PARSER;

Parser::Parser() {
	++m_cnt;
	m_LibName = "Lib_";
	m_LibName += std::to_string(m_cnt);
}

void
Parser::reset()
{
	m_tokenizer.reset();
	m_currToken = Token();
	m_context.clear();
	
	++m_cnt;
	m_LibName = "Lib_";
	m_LibName += std::to_string(m_cnt);
}

Library*
Parser::parse(const std::string& context) {
	// init
	for (auto c : context) {
		if (c != '\r' && c != '\n' && c != '\t')
			m_context.push_back(c);
	}
	m_tokenizer.init(m_context);
	m_currToken = m_tokenizer.getNextToken();
	if (m_currToken.getType() == Token::INVALID_TOKEN) {
		return nullptr;
	}

	Library* lib = library();
	lib->setName(m_LibName);

	IdResolver		ir;
	TypeResolver	tr;
	PsTreeTraverser trs;
	trs.setVisitor(&ir);
	trs.traverse(lib, 0);
	trs.setVisitor(&tr);
	trs.traverse(lib, 0);
	return lib;
}

Library*
Parser::library() {
	Library* lib = new Library();
	while (m_currToken.getType() != Token::INVALID_TOKEN) {
		lib->addModule(module());
	}
	return lib;
}

Module*
Parser::module() {
	// todo: move definition to statement
	Module* mod = new Module();
	m_currToken = m_tokenizer.getNextToken(true);
	ASSERT(Token::K_MODULE);

	m_currToken = m_tokenizer.getNextToken(true);
	ASSERT(Token::ID);
	mod->setName(m_currToken.getValue());

	m_currToken = m_tokenizer.getNextToken(true);
	ASSERT(Token::P_PARENTHESIS_L);

	// todo : process param list
	while (!checkToken(Token::P_PARENTHESIS_R) && m_tokenizer.hasMoretoken()) {
		m_currToken = m_tokenizer.getNextToken(true);
	}
	if (!(m_tokenizer.hasMoretoken())) {
		return mod;
	}
	m_currToken = m_tokenizer.getNextToken(true);
	ASSERT(Token::P_SEMICOLON);

	m_currToken = m_tokenizer.getNextToken(true);

	Statement* s;
	while (m_currToken.getType() != Token::K_ENDMODULE)
	{
		s = statement();
		mod->addStatement(s);
	}

	checkToken(Token::K_ENDMODULE);
	m_currToken = m_tokenizer.getNextToken(true);
	return mod;
}


bool Parser::checkToken(Token::TKTYPE type) {
	return m_currToken.getType() == type;
}

void Parser::skipUselessTk() {
	while(m_currToken.getType() == Token::SPACE ||
		  /*m_currToken.getType() == Token::NEXT_LINE_QT ||
		  m_currToken.getType() == Token::NEXT_LINE_WIN ||*/
		  m_currToken.getType() == Token::COMMENT1 ||
		  m_currToken.getType() == Token::COMMENT2
		)
	{
		if (m_tokenizer.hasMoretoken()) {
			m_currToken = m_tokenizer.getNextToken();
			continue;
		}
		else {
			return;
		}
	}
}

bool Parser::absorb(Token::TKTYPE type) {
	skipUselessTk();
	return checkToken(type);
}

Statement* Parser::statement() {
	Token::TKTYPE type = m_currToken.getType();
	if (type == Token::K_ALWAYS) {
		AlwaysStatement* as = new AlwaysStatement();
		// absorb format;
		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::K_AT);
		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::P_PARENTHESIS_L);
		m_currToken = m_tokenizer.getNextToken(true);
		while (m_currToken.getType() != Token::P_PARENTHESIS_R) {
			// ignore event_list for now
			m_currToken = m_tokenizer.getNextToken(true);
		}
		ASSERT(Token::P_PARENTHESIS_R);
		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::K_BEGIN);
		m_currToken = m_tokenizer.getNextToken(true);
		// process block;
		while (m_currToken.getType() != Token::K_END) {
			Statement* s = statement();
			as->addStatement(s);
		}
		// absorb END
		ASSERT(Token::K_END);
		m_currToken = m_tokenizer.getNextToken(true);
		return as;
	}else if (type == Token::K_IF) {
		/*
		 * if (cond) begin
		 *		statements;
		 * end
		 * else begin
		 *		statements;
		 * end
		 * todo: support and only support if-else, to simplify
		 */
		IFStatement* s = new IFStatement();
		ASSERT(Token::K_IF);

		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::P_PARENTHESIS_L);

		m_currToken = m_tokenizer.getNextToken(true);
		Expression* expr = expression();
		expr->setName("Condition");
		s->addCondition(expr);
		ASSERT(Token::P_PARENTHESIS_R);

		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::K_BEGIN);
		m_currToken = m_tokenizer.getNextToken(true);

		while (!checkToken(Token::K_END)) {
			Statement* sub = statement();
			s->addStatement(sub, true);
		}
		ASSERT(Token::K_END);

		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::K_ELSE);
		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::K_BEGIN);
		m_currToken = m_tokenizer.getNextToken(true);
		while (!checkToken(Token::K_END)) {
			Statement* sub = statement();
			s->addStatement(sub, false);
		}
		ASSERT(Token::K_END);
		m_currToken = m_tokenizer.getNextToken(true);
		return s;
	}
	else if (type == Token::K_FOR) {
		/*
		* for (statement; expression; statement) begin
		*		statements;
		* end
		*/
		ForStatement* s = new ForStatement();
		ASSERT(Token::K_FOR);
		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::P_PARENTHESIS_L);
		m_currToken = m_tokenizer.getNextToken(true);
		Expression* expr = expression();
		s->addCondition(expr);
		ASSERT(Token::P_PARENTHESIS_R);
		m_currToken = m_tokenizer.getNextToken(true);
		ASSERT(Token::K_BEGIN);
		skipUselessTk();
		while (!checkToken(Token::K_END)) {
			Statement* sub = statement();
			s->addStatement(sub);
		}
		ASSERT(Token::K_END);
		m_currToken = m_tokenizer.getNextToken(true);
		return s;
	}
	else if (type == Token::K_ASSIGN ||type == Token::ID) {
		/*
		* assign expr1 = expr2	|
		* expr1 = expr2			|
		* expr1 <= expr2
		*/
		AssignStatement* s = new AssignStatement();
		if (m_currToken.getType() == Token::K_ASSIGN) {
			// case 1;
			s->setType(AssignStatement::ASSIGN);
			m_currToken = m_tokenizer.getNextToken(true);
			Expression* left = expression();
			s->setLHS(left);
			ASSERT(Token::O_ASSIGN_BLOCKING);
			m_currToken = m_tokenizer.getNextToken(true);
			Expression* right = expression();
			s->setRHS(right);
			ASSERT(Token::P_SEMICOLON);
			m_currToken = m_tokenizer.getNextToken(true);
		}
		else {
			Expression* left = expression();
			if (m_currToken.getType() == Token::O_ASSIGN_BLOCKING) {
				s->setType(AssignStatement::BLOCKING);
			}
			else {
				s->setType(AssignStatement::NONBLOCKING);
			}
			m_currToken = m_tokenizer.getNextToken(true);
			Expression* right = expression();
			s->setLHS(left);
			s->setRHS(right);
			ASSERT(Token::P_SEMICOLON);
			m_currToken = m_tokenizer.getNextToken(true);
		}
		return s;
	}
	else if (type == Token::K_INPUT ||
			 type == Token::K_OUTPUT ||
			 type == Token::K_WIRE ||
			 type == Token::K_REG) {
		/*	definition statements£º
		 *		input a£¬b;
		 *		input reg a;
		 *		wire a;
		 *		...
		 */
		 DefineStatement* s = new DefineStatement();
		TypeRef* type = typeRef();
		while (!checkToken(Token::P_SEMICOLON)) {
			IdDef* Id = idDef();
			Id->setType(type);
			s->addIdDef(Id);
			if (checkToken(Token::P_COMMAN)) {
				m_currToken = m_tokenizer.getNextToken(true);
			}
		}
		m_currToken = m_tokenizer.getNextToken(true);
		return s;
		}
	else {
		Statement* s = new Statement();
		return s;
	}
}

Expression*
Parser::expression() {
	/* For now, only variablesand binary operators are supported.
	 * Examples:
	 *		wen
	 *		a + - * / b
	 */
	Expression* expr = new Expression();
	 // 1, create an variable
	absorb(Token::ID);
	IdRef* left = idRef();
	expr->setExpression(left, true);
	// 2. check if there is an oper, if so,create oper and the second operand
	skipUselessTk();
	if (!isOperator()) {
		expr->setOperator(Expression::IDREF);
		return expr;
	}

	switch (m_currToken.getType())
	{
	case Token::O_ADD:
		expr->setOperator(Expression::ADD);
		expr->setName("ADD");
		break;
	case Token::O_MIN:
		expr->setOperator(Expression::MIN);
		expr->setName("MIN");
		break;
	case Token::O_MUL:
		expr->setOperator(Expression::MUL);
		expr->setName("MUL");
		break;
	case Token::O_DIV:
		expr->setOperator(Expression::DIV);
		expr->setName("DIV");
		break;
	default:
		break;
	}
	m_currToken = m_tokenizer.getNextToken();
	skipUselessTk();
	IdRef* right = idRef();
	expr->setExpression(right, false);
	skipUselessTk();
	return expr;
}

IdRef* Parser::idRef() {
	skipUselessTk();
	IdRef* id = new IdRef();
	id->setName(m_currToken.getValue());
	m_currToken = m_tokenizer.getNextToken(true);
	return id;
}

bool Parser::isOperator() {
	bool rt = false;
	rt |= (m_currToken.getType() == Token::O_ADD);
	rt |= (m_currToken.getType() == Token::O_MIN);
	rt |= (m_currToken.getType() == Token::O_MUL);
	rt |= (m_currToken.getType() == Token::O_DIV);
	return rt;
}

TypeRef* Parser::typeRef()
{
	TypeRef* type = new TypeRef();
	bool typeSet = false;
	// absorb input/ouput
	if (checkToken(Token::K_INPUT)) {
		type->setTypeDir(TypeRef::INPUT);
		m_currToken = m_tokenizer.getNextToken(true);
		typeSet = true;
	}
	else if (checkToken(Token::K_OUTPUT)) {
		type->setTypeDir(TypeRef::OUTPUT);
		m_currToken = m_tokenizer.getNextToken(true);
		typeSet = true;
	}

	// absorb wire/reg/udm
	if (checkToken(Token::K_WIRE)) {
		type->setTypeName("wire");
		m_currToken = m_tokenizer.getNextToken(true);
		typeSet = true;
	}
	else if (checkToken(Token::K_REG)) {
		type->setTypeName("reg");
		m_currToken = m_tokenizer.getNextToken(true);
		typeSet = true;
	}
	else if (!typeSet){
		//udm
		type->setTypeName(m_currToken.getValue());
		m_currToken = m_tokenizer.getNextToken(true);
	}

	return type;
}

IdDef* Parser::idDef()
{
	IdDef* id = new IdDef();
	id->setName(m_currToken.getValue());
	m_currToken = m_tokenizer.getNextToken();
	return id;
}

int Parser::m_cnt = 0;