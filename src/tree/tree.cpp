#include "tree.h"

void DefineStatement::addIdDef(IdDef* id)
{
	if (m_IdDefs.find(id) != m_IdDefs.end()) {
		// warning
	}
	else {
		m_IdDefs.insert(id);
	}
}

void IFStatement::addStatement(Statement* s, bool b) {
	if (b)
		m_Block_True.push_back(s);
	else
		m_Block_False.push_back(s);
}

void Expression::setExpression(IdRef* id, bool left) {
	if (left)
		m_left = id;
	else
		m_right = id;
}

IdRef* Expression::getExpression(bool left) {
	if (left)
		return m_left;
	else
		return m_right;
}