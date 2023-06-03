#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <string>
#include "Netlist.h"

class Netlist;
class Library;
class Module;
class Expression;
class Statement;
class IdRef;
class IdDef;

class Visitor
{
public:
	Visitor() = default;
	//todo : add common attributes of visitors
	virtual void previsit(Library* lib, int level) {}
	virtual void previsit(Module* mod, int level) {}
	virtual void previsit(Statement* s, int level) {}
	virtual void previsit(Expression* e, int level) {}
	virtual void previsit(IdDef* id, int level) {}
	virtual void previsit(IdRef* id, int level) {}

	virtual void postvisit(Library* lib, int level) {}
	virtual void postvisit(Module* mod, int level) {}
	virtual void postvisit(Statement* s, int level) {}
	virtual void postvisit(Expression* e, int level) {}
	virtual void postvisit(IdDef* id, int level) {}
	virtual void postvisit(IdRef* id, int level) {}
private:

};

/*************************************************************************************************************
	to dump parseTree
*************************************************************************************************************/
class Dumper : public Visitor
{
	//todo : add an ostream 
public:
	Dumper() = default;
	Dumper(std::string outputPath) { m_path = outputPath; }

	void setOutput(std::string outputPath) { m_path = outputPath; }

	void previsit(Library* lib, int level);
	void previsit(Module* mod, int level);
	void previsit(Statement* s, int level);
	void previsit(Expression* e, int level);
	void previsit(IdRef* id, int level);

	void postvisit(Library* lib, int level);
private:
	std::string prefix(int level);
	std::string m_cache;
	std::string m_path;
};

/*************************************************************************************************************
	to resolve IdRef to IdDef
		for now, a scope tree has only one level.
		all identifiers are defined in module level.
		but in the further, when function was supported, the scope tree will be deeper.
*************************************************************************************************************/

struct Scope {
	IdDef* getIdDef(std::string IdRef);
	Scope* m_parent = nullptr;
	std::vector<Scope*> m_children;
	std::map<std::string, IdDef*>	m_IdDefs;
};

class IdResolver : public Visitor
{
	/*
		when entering a namespace, create a scope to the scope tree;
		collect all IdDef to  the created scope;
		query all IdDefs for all Idrefs;
		pop up the scope when leaving the scope tree.
	*/
public:
	void previsit(Library* lib, int level);
	void previsit(Module* mod, int level);
	void previsit(Statement* e, int level);
	void previsit(Expression* e, int level);

	void postvisit(Library* lib, int level);
	void postvisit(Module* mod, int level);
private:
	std::stack<Scope*> m_scopeTree;
	Scope* m_currentScope = nullptr;
};

/*************************************************************************************************************
	TypeResolver:
		to resovle resolve typeRef to typedef
*************************************************************************************************************/

class TypeResolver : public Visitor
{
	/*
		when entering a library, it will collect module as a typeDef,
		when enterring a statement, the TypeRef of identifier will be resolved to the typeDef with same name
		todo: support impoted type
	*/
public:
	void previsit(Library* lib, int level);
	void previsit(Statement* e, int level);
private:
	std::map<std::string, Module*> m_typeDefs;	//typeName: typeDef;
};