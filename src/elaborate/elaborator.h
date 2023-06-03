#pragma once
#include <set>
#include <map>
#include <stack>
#include "tree.h"

/*
	在elaborate时，一个identifier会实例化为一个器件:
		wire->Net
		reg->PrimLatch
		udm->instance
		input/output wire-> (port<-->wire)
		input/output reg-> (port<-->wire<-->latch<-->wire)
	实例化后会将这个器件的名字和句柄的map记录在netlist里。
	在其他的语句中，如果用到这个identifier,就可以从netlist里通过名字找到这个器件的句柄，比如一个reg或net
	这样，如果对这个id进行运算或赋值，就可以直接同这个net或reg的输出进行连接，以完成电路功能。
*/

/*************************************************************************************************************
	Elaborator:
		to generate netlist from parse tree for each module.
*************************************************************************************************************/
struct DataFlow {
	/* dataflow represent for the values of identifiers in different branch;

	   so a dataflow should be created when entering a module,or a if-else block,
	   it would record the updated value of identifiers in this block.
	   and an identifier's value in diffrent block would be determined by a mux or selector.

	   a dataflow would be deleted, left a circuit structure of all refered identifiers.
	   All these identifiers should be updated to parent dataflow.
	   the parent dataflow will use this results as the driven.
	 */
	DataFlow(DataFlow* parent) :m_parent(parent) {}
	DataFlow() = default;
	DataFlow* m_parent;
	NLDB::Net* m_condition = nullptr;
	std::set<IdDef*>				m_values_all;	// all Ids appeared in this control branch
	std::map<IdDef*, NLDB::Net*>	m_values_if;	// all Ids appeared in if branch
	std::map<IdDef*, NLDB::Net*>	m_values_else;	// all Ids appeared in else branch
};


class Elaborator
{
public:
	NLDB::Library* elaborate(Library* lib);
	void reset();
private:
	void elaborate(Module* mod);
	void elaborate(Statement* st);
	void elaborateAlways(AlwaysStatement* as);
	void elaborateIf(IFStatement* st);
	void elaborateFor(ForStatement* st);
	void elaborateAssign(AssignStatement* st);
	void elaborateDef(DefineStatement* st);

	void processElabIfResults();

	NLDB::Net* elaborate(Expression* ex);

	template <typename T>
	NLDB::Net* elaborateBinaryOper(Expression* e);

	NLDB::Net* elaborateId(IdRef* ex);

	std::string getdefaultNetName();
	std::string getdefaultRegName();
	std::string getdefaultInstName();

private:
	// dataflow control
	DataFlow*			m_currDataFlow;
	std::stack<bool>	m_currDataFlowBranch;
	NLDB::Netlist*		m_currMod = nullptr;
	
	// auto name;
	long				m_cnts = 0;
};

template<typename T>
NLDB::Net*
Elaborator::elaborateBinaryOper(Expression* ex)
{
	NLDB::Net* left = elaborateId(ex->getExpression(true));
	NLDB::Net* right = elaborateId(ex->getExpression(false));
	NLDB::Net* output = new NLDB::Net(getdefaultNetName(), m_currMod);
	T* prim = new T(getdefaultInstName(), m_currMod);
	prim->connectInput(left, 0);
	prim->connectInput(right, 1);
	prim->connectOutput(output);

	return output;
}