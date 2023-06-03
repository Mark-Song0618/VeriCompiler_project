#pragma once
#include "visitor.h"
#include "tree.h"
class PsTreeTraverser
{
public:
	PsTreeTraverser() = default;
	PsTreeTraverser(Visitor* v) : m_visitor(v) {}

	void setVisitor(Visitor* v) { m_visitor = v; }

	void traverse(Library* lib, int level);
	void traverse(Module* mod, int level);
	void traverse(Statement* s, int level);
	void traverse(Expression* e, int level);
	void traverse(IdRef* id, int level);
	void traverse(IdDef* id, int level);
private:
	Visitor* m_visitor;
};