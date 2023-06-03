#include "traversMgr.h"

void
PsTreeTraverser::traverse(Library* lib, int level)
{
	m_visitor->previsit(lib, level);
	for (auto mod : lib->getModules()) {
		traverse(mod, level + 1);
	}
	m_visitor->postvisit(lib, level);
}

void PsTreeTraverser::traverse(Module* mod, int level) {
	m_visitor->previsit(mod, level);
	for (auto statement : mod->getStatements()) {
		traverse(statement, level + 1);
	}
	m_visitor->postvisit(mod, level);
}

void
PsTreeTraverser::traverse(Statement* s, int level) {
	m_visitor->previsit(s, level);
	if (s->m_type == Statement::ALWAYS) {
		AlwaysStatement* as = dynamic_cast<AlwaysStatement*>(s);
		for (auto s : as->m_Block) {
			traverse(s, level + 1);
		}
	}
	else if (s->m_type == Statement::IF) {
		IFStatement* ifs = dynamic_cast<IFStatement*>(s);
		traverse(ifs->m_condition, level + 1);
		for (auto s : ifs->m_Block_True) {
			traverse(s, level + 1);
		}
		for (auto s : ifs->m_Block_False) {
			traverse(s, level + 1);
		}
	}
	else if (s->m_type == Statement::FOR) {
		ForStatement* ifs = dynamic_cast<ForStatement*>(s);
		traverse(ifs->m_condition, level + 1);
		for (auto s : ifs->m_Block) {
			traverse(s, level + 1);
		}
	}
	else if (s->m_type == Statement::ASSIGN) {
		AssignStatement* ifs = dynamic_cast<AssignStatement*>(s);
		traverse(ifs->m_left, level + 1);
		traverse(ifs->m_right, level + 1);
	}
	else if (s->m_type == Statement::DEFINITION) {
		DefineStatement* ifs = dynamic_cast<DefineStatement*>(s);
		for (auto id : ifs->getIdDefs()) {
			traverse(id, level + 1);
		}
	}
	m_visitor->postvisit(s, level);
}

void
PsTreeTraverser::traverse(Expression* e, int level) {
	m_visitor->previsit(e, level);
	if (e->m_left != nullptr)
	traverse(e->m_left, level + 1);
	if (e->m_right != nullptr)
	traverse(e->m_right, level + 1);
	m_visitor->postvisit(e, level);
}

void
PsTreeTraverser::traverse(IdRef* id, int level) {
	m_visitor->previsit(id, level);
}

void
PsTreeTraverser::traverse(IdDef* id, int level) {
	m_visitor->previsit(id, level);
}