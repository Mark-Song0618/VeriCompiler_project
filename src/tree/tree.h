#pragma once
#include <vector>
#include <set>
#include <ostream>

#include "visitor.h"

//todo: destructor

class Statement;
class Expression;
class IdDef;
class IdRef;
class TypeDef;
class TypeRef;

class TreeNode
{
	friend class PsTreeTraverser;
public:
	TreeNode() = default;
	TreeNode(std::string name): m_name(name) {}
	virtual ~TreeNode() {}

	void setName(const std::string& n) { m_name = n; }
	std::string getName() { return m_name; }

private:
	std::string m_type;	//todo: use enum
	std::string m_name = "";
	Scope* m_scope;
};

class Module : virtual public TreeNode
{
	friend class PsTreeTraverser;
public:
	Module() : TreeNode("Module") {}
	void addStatement(Statement* s) { m_statements.push_back(s); }
	std::vector<Statement*>& getStatements() { return m_statements; }

	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
private:
	std::vector<Statement*>		m_statements;
};

class Library : virtual public TreeNode
{
	friend class PsTreeTraverser;
public:
	Library() : TreeNode("Library") {}
	void addModule(Module* s) { m_modules.push_back(s); }
	std::vector<Module*> getModules() { return m_modules; }

	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
private:
	std::vector<Module*> m_modules;
};

class Statement : virtual public TreeNode
{
	friend class PsTreeTraverser;
public:
	enum TYPE {INVALID, IF, FOR, ASSIGN, DEFINITION, ALWAYS};
	Statement() : TreeNode("Invalid Statement") {}
	Statement(TYPE t) :m_type(t) {}
	void addIdRef(IdRef* id) { m_IdRefs.push_back(id); }
	std::vector<IdRef*>	getIdRefs() { return m_IdRefs; }
	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
	TYPE getType() { return m_type; }
private:
	TYPE m_type = INVALID;
	std::vector<IdRef*>	m_IdRefs;	// store all variables used in this sattement
};

class AlwaysStatement : virtual public Statement
{
	friend class PsTreeTraverser;
public:
	AlwaysStatement() : TreeNode("Always"), Statement(ALWAYS) {}
	void addStatement(Statement* s) { m_Block.push_back(s); }
	std::vector<Statement*>& getStatements() { return m_Block; }
	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
private:
	std::vector<Statement*>	m_Block;
};

class DefineStatement : virtual public Statement
{
	friend class PsTreeTraverser;
public:
	DefineStatement() : TreeNode("Definition"), Statement(DEFINITION) {}
	void addIdDef(IdDef* id);
	std::set<IdDef*> getIdDefs() { return m_IdDefs; }
private:
	std::set<IdDef*>	m_IdDefs;	// store all IdDefs defined in this sattement
};

class ForStatement : virtual public Statement
{
	friend class PsTreeTraverser;
public:
	ForStatement() : TreeNode("For"), Statement(FOR){}
	void addCondition(Expression* e) { m_condition = e; }
	void addStatement(Statement*s) { m_Block.push_back(s); }
	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
private:
	Expression*				m_condition = nullptr;
	std::vector<Statement*>	m_Block;
};

class IFStatement : virtual public Statement
{
	friend class PsTreeTraverser;
public:
	IFStatement() : TreeNode("IF"), Statement(IF) {}
	void addCondition(Expression* e) { m_condition = e; }
	Expression* Condition() { return m_condition; }
	void addStatement(Statement*, bool b);
	std::vector<Statement*>& getBlock(bool b) { return b ? m_Block_True : m_Block_False; }
	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
private:
	Expression* m_condition = nullptr;
	std::vector<Statement*>	m_Block_True;
	std::vector<Statement*>	m_Block_False;
};

class AssignStatement : virtual public Statement
{
	friend class PsTreeTraverser;
public:
	enum TYPE { NONE, BLOCKING, NONBLOCKING, ASSIGN};
	AssignStatement() : TreeNode("Assign"), Statement(Statement::ASSIGN) {}
	void setType(TYPE type) { m_type = type; }
	void setRHS(Expression* e) { m_right = e; }
	void setLHS(Expression* id) { m_left = id; }
	Expression* getRHS() { return m_right; }
	Expression* getLHS() { return m_left; }
	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
private:
	Expression*	m_left = nullptr;
	Expression* m_right = nullptr;
	TYPE		m_type = NONE;
};

class Expression : public TreeNode {
	friend class PsTreeTraverser;
	friend class IdResolver;
public:
	enum TYPE { NONE, ADD, MIN, MUL, DIV, IDREF};
	Expression() : TreeNode("Expression") {}
	void setExpression(IdRef* id, bool left);
	void setOperator(TYPE t) { m_oper = t; }
	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
	IdRef* getExpression(bool left);
	TYPE getType() { return m_oper; }
private:
	IdRef* m_left = nullptr;
	IdRef* m_right = nullptr;
	TYPE m_oper = NONE;
};

class IdRef : public TreeNode
{
	friend class PsTreeTraverser;
public:
	IdRef() : TreeNode("Id Ref") {}
	void setIdDef(IdDef* id) { m_IdDef = id; }
	IdDef* getIdDef() { return m_IdDef; }
	void accept(Visitor* visitor, int level) { visitor->previsit(this, level); }
private:
	IdDef* m_IdDef = nullptr;
};

class IdDef : public TreeNode
{
	friend class PsTreeTraverser;
public:
	IdDef() : TreeNode("Id Def") {}
	void accept(Visitor* visitor, int level) {} //todo
	void setType(TypeRef* type) { m_type = type; }
	TypeRef* getType() { return m_type; }
private:
	TypeRef* m_type = nullptr;
};

class TypeRef : public TreeNode
{
	friend class PsTreeTraverser;
public:
	enum DIR {INPUT, OUTPUT, NON};
	TypeRef(std::string name = "wire") : TreeNode("Type Ref"), m_typeName(name) {}
	void setTypeName(std::string type) { m_typeName = type; }
	std::string getTypeName() { return m_typeName; }
	void setTypeDir(DIR dir) { m_dir = dir; }
	DIR getTypeDir() { return m_dir; }
	void setTypeDef(TypeDef* def) { m_TypeDef = def; }
	TypeDef* getTypeDef() { return m_TypeDef; }
	void accept(Visitor* visitor, int level) {} 
private:
	std::string m_typeName;	// use typeName to find typeDef
	DIR			m_dir = NON;
	TypeDef*	m_TypeDef = nullptr;
};

class TypeDef : public TreeNode
{
	friend class PsTreeTraverser;
public:
	/* an component of design could be a user defined module, a wire or a reg. */
	enum TYPE { UDM, REG, WIRE };
	TypeDef() : TreeNode("Type Def") {}
	TypeDef(TYPE t) : TreeNode("Type Def"), m_type(t) {}
	TypeDef(Module* type) : TreeNode("Type Def"), m_type(UDM), m_module(type) {}
	void accept(Visitor* visitor, int level) {} //todo
	TYPE getType() { return m_type; }
private:
	Module* m_module = nullptr;
	TYPE m_type = WIRE;
};
