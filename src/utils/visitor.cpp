#include <iostream>
#include <fstream>
#include <assert.h>
#include "visitor.h"
#include "Primitive.h"
#include "tree.h"

/*************************************************************************************************************
	to dump parseTree
*************************************************************************************************************/
void Dumper::previsit(Library* lib, int level) {
	m_cache += "-------------------------------------------Parse Tree-------------------------------------------\r\n";
	std::string context = prefix(level);
	m_cache += context;
	m_cache += "Library: ";
	m_cache += lib->getName();
	m_cache += "\r\n";
}

void Dumper::previsit(Module* mod, int level) {
	std::string context = prefix(level);
	m_cache += context;
	m_cache += "Module: ";
	m_cache += mod->getName();
	m_cache += "\r\n";
}

void Dumper::previsit(Statement* s, int level) {
	std::string context = prefix(level);
	context += "Statement: ";
	context += s->getName() + "\r\n";
	m_cache += context;
}

void Dumper::previsit(Expression* e, int level) {
	std::string pre = prefix(level);
	/*switch (e->getType())
	{
	case Expression::ADD:
		oper = "ADD";
		break;
	case Expression::MIN:
		oper = "MIN";
		break;
	case Expression::MUL:
		oper = "MUL";
		break;
	case Expression::DIV:
		oper = "DIV";
		break;
	case Expression::IDREF:
		oper = "IDREF";
		break;
	case Expression::NONE:
	default:
		oper = "INVALID";
		break;
	}*/
	m_cache += (pre + "Expression: " + e->getName() + "\r\n");
}
 
void Dumper::previsit(IdRef* id, int level) {
	std::string pre = prefix(level);
	m_cache += (pre + "IdRef£º" + id->getName() + "\r\n");
}

void
Dumper::postvisit(Library* lib, int level)
{
	// move the context of cache to output
	std::ofstream of(m_path.c_str(), std::ios_base::app);
	of << m_cache;
	of.close();
	m_cache.clear();
}

std::string
Dumper::prefix(int level) {
	std::string rst;
	for (int i = 0; i < level; ++i)
		rst += "\t";
	return rst;
}

/*************************************************************************************************************
	IdResolver
*************************************************************************************************************/

IdDef* Scope::getIdDef(std::string IdRef) {
	if (m_IdDefs.find(IdRef) != m_IdDefs.end()) {
		return m_IdDefs[IdRef];
	}
	else if (m_parent != nullptr) {
		return m_parent->getIdDef(IdRef);
	}
	else {
		return nullptr;
	}
}

void IdResolver::previsit(Library* lib, int level)
{
	Scope* libScope = new Scope;
	m_scopeTree.push(libScope);
	m_currentScope = libScope;
}

void IdResolver::previsit(Module* mod, int level)
{
	Scope* modScope = new Scope;
	modScope->m_parent = m_scopeTree.top();
	m_scopeTree.top()->m_children.push_back(modScope);
	m_scopeTree.push(modScope);
	m_currentScope = modScope;
}

void IdResolver::previsit(Statement* s, int level)
{
	if (s->getType() == Statement::DEFINITION) {
		DefineStatement* ds = dynamic_cast<DefineStatement*>(s);
		for (auto id : ds->getIdDefs()) {
			m_currentScope->m_IdDefs.insert({id->getName(), id});
		}
	}
	else {
		// resolve every IdRef
		for (auto id : s->getIdRefs()) {
			IdDef* idDef = m_currentScope->getIdDef(id->getName());
			assert(idDef != nullptr);
			if (idDef) {
				id->setIdDef(idDef);
			}
		}
	}
}

void IdResolver::previsit(Expression* s, int level)
{
	if (s->m_left) {
		IdDef* idDef = m_currentScope->getIdDef(s->m_left->getName());
		assert(idDef != nullptr);
		if (idDef) {
			s->m_left->setIdDef(idDef);
		}
	}

	if (s->m_right) {
		IdDef* idDef = m_currentScope->getIdDef(s->m_right->getName());
		assert(idDef != nullptr);
		if (idDef) {
			s->m_right->setIdDef(idDef);
		}
	}
}

void
IdResolver::postvisit(Library* lib, int level)
{
	m_scopeTree.pop();
	m_currentScope = nullptr;
}
void
IdResolver::postvisit(Module* mod, int level)
{
	m_scopeTree.pop();
	m_currentScope = m_scopeTree.top();
}

void
TypeResolver::previsit(Library* lib, int level)
{
	for (auto mod : lib->getModules()) {
		m_typeDefs.insert({mod->getName(), mod});
	}
}
void
TypeResolver::previsit(Statement* s, int level)
{
	if (s->getType() == Statement::DEFINITION) {
		DefineStatement* ds = dynamic_cast<DefineStatement*>(s);
		for (auto def : ds->getIdDefs()) {
			TypeRef* typeRef = def->getType();
			std::string typeName = typeRef->getTypeName();
			TypeDef* typeDef = nullptr;
			if (typeName == "wire") {
				typeDef = new TypeDef(TypeDef::WIRE);
			}
			else if (typeName == "reg") {
				typeDef = new TypeDef(TypeDef::REG);
			}
			else if (m_typeDefs.find(typeName) != m_typeDefs.end()) {
				typeDef = new TypeDef(m_typeDefs[typeRef->getName()]);
			}
			else {
				assert(0);
			}
			typeRef->setTypeDef(typeDef);
		}
	}
}