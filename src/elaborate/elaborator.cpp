#include "Netlist.h"
#include "Primitive.h"
#include "elaborator.h"

NLDB::Library*
Elaborator::elaborate(Library* lib)
{
	if (!lib)
		return nullptr;
	NLDB::Library*  rst = new NLDB::Library;
	rst->setName(lib->getName());
	for (auto m : lib->getModules()) {
		NLDB::Netlist* nl = new NLDB::Netlist();
		nl->setOwner(rst);
		nl->setName(m->getName());
		m_currMod = nl;		
		rst->addNetlist(nl);

		elaborate(m);
	}
	return rst;
}

void
Elaborator::elaborate(Module* mod)
{
	for (auto s : mod->getStatements()) {
		elaborate(s);
	}
}

void
Elaborator::elaborate(Statement* s)
{
	switch (s->getType())
	{
	case Statement::ALWAYS:
		elaborateAlways(dynamic_cast<AlwaysStatement*>(s));
		break;
	case Statement::IF:
		elaborateIf(dynamic_cast<IFStatement*>(s));
		break;
	case Statement::FOR:
		elaborateFor(dynamic_cast<ForStatement*>(s));
		break;
	case Statement::ASSIGN:
		elaborateAssign(dynamic_cast<AssignStatement*>(s));
		break;
	case Statement::DEFINITION:
		elaborateDef(dynamic_cast<DefineStatement*>(s));
		break;
	default:
		break;
	}
}

void
Elaborator::elaborateAlways(AlwaysStatement* as)
{
	for (auto s : as->getStatements()) {
		elaborate(s);
	}
}

void
Elaborator::elaborateIf(IFStatement* s)
{
	DataFlow* df = new DataFlow(m_currDataFlow);
	m_currDataFlow = df;

	NLDB::Net* en = elaborate(s->Condition());
	df->m_condition = en;
	m_currDataFlowBranch.push(true);
	for (auto sub : s->getBlock(true)) {
		elaborate(sub);
	}
	m_currDataFlowBranch.pop();
	m_currDataFlowBranch.push(false);
	for (auto sub : s->getBlock(false)) {
		elaborate(sub);
	}
	//post process
	m_currDataFlowBranch.pop();
	processElabIfResults();
	
	delete df;

	return;
}

void
Elaborator::elaborateFor(ForStatement* s)
{
	// todo
}

void
Elaborator::elaborateAssign(AssignStatement* s)
{
	// get right value
	NLDB::Net* result = elaborate(s->getRHS());
	IdDef* left = s->getLHS()->getExpression(true)->getIdDef();
	if (m_currDataFlow != nullptr) {
		// if in dataflow, save the {id : net} pair;
		m_currDataFlow->m_values_all.insert(left);
		if (m_currDataFlowBranch.top()) {
			m_currDataFlow->m_values_if.insert({left, result});
		}
		else {
			m_currDataFlow->m_values_else.insert({left, result});
		}
	}
	else {
		// not in control branch, assign value directly
		if (left->getType()->getTypeDef()->getType() == TypeDef::REG) {
			NLDB::PrimLatch* latch = dynamic_cast<NLDB::PrimLatch*>(m_currMod->getInst(left->getName()));
			if (latch == nullptr) {
				//error out
				return;
			}
			else {
				latch->connectInput(result);
			}
		}
		else if (left->getType()->getTypeDef()->getType() == TypeDef::WIRE) {
			NLDB::Net* id_net = m_currMod->getNet(left->getName());
			NLDB::PrimBuffer* buffer = new NLDB::PrimBuffer(getdefaultInstName(), m_currMod, result, id_net);
		}
		else {
			//error out
			return;
		}
	}
}

void
Elaborator::elaborateDef(DefineStatement* s)
{
	// create Ports¡¢instances¡¢ nets and regs¡¢for identifiers
	for (auto id : s->getIdDefs()) {
		TypeRef* typeRef = id->getType();
		if (typeRef->getTypeDir() == TypeRef::INPUT) {
			std::string portName = "input_";
			portName += id->getName();
			NLDB::Port* port = new NLDB::Port(portName, m_currMod);
			port->setDir(NLDB::Port::IN);

			TypeDef* typeDef = id->getType()->getTypeDef();
			if (typeDef->getType() == TypeDef::REG) {
				NLDB::PrimLatch* latch = new NLDB::PrimLatch(id->getName(), m_currMod);
				NLDB::Net* n1 = new NLDB::Net(getdefaultNetName(), m_currMod);
				port->setLowConn(n1);
				n1->setDriver(port);
				n1->addLoad(latch->getInput());
				latch->getInput()->setHighConn(n1);
			}
			else if (typeDef->getType() == TypeDef::WIRE) {
				NLDB::Net* net = new NLDB::Net(id->getName(), m_currMod);
				port->setLowConn(net);
				net->setDriver(port);
			}
		}
		else if (typeRef->getTypeDir() == TypeRef::OUTPUT) {
			std::string portName = "input_";
			portName += id->getName();
			NLDB::Port* port = new NLDB::Port(portName, m_currMod);
			port->setDir(NLDB::Port::OUT);

			TypeDef* typeDef = id->getType()->getTypeDef();
			if (typeDef->getType() == TypeDef::REG) {
				NLDB::PrimLatch* latch = new NLDB::PrimLatch(id->getName(), m_currMod);
				NLDB::Net* n1 = new NLDB::Net(getdefaultNetName(), m_currMod);
				port->setLowConn(n1);
				n1->addLoad(port);
				latch->connectOutput(n1);
			}
			else if (typeDef->getType() == TypeDef::WIRE) {
				NLDB::Net* net = new NLDB::Net(id->getName(), m_currMod);
				port->setLowConn(net);
				net->addLoad(port);
			}
		}
		else {
			//internal object
			TypeDef* typeDef = id->getType()->getTypeDef();
			if (typeDef->getType() == TypeDef::REG) {
				NLDB::PrimLatch* latch = new NLDB::PrimLatch(id->getName(), m_currMod);
			}
			else if (typeDef->getType() == TypeDef::WIRE) {
				NLDB::Net* net = new NLDB::Net(id->getName(), m_currMod);
			}
			else if (typeDef->getType() == TypeDef::UDM) {
				NLDB::Instance* inst = new NLDB::Instance(id->getName(), m_currMod);
			}
		}
	}

}

NLDB::Net*
Elaborator::elaborate(Expression* e)
{
	// todo: create different circuts for expressions
	NLDB::Net* result = nullptr;
	switch (e->getType()) {
	case Expression::ADD:
		result = elaborateBinaryOper<NLDB::PrimAdder>(e);
		break;
	case Expression::MIN:
		result = elaborateBinaryOper<NLDB::PrimMiner>(e);
		break;
	case Expression::MUL:
		result = elaborateBinaryOper<NLDB::PrimMultipler>(e);
		break;
	case Expression::DIV:
		result = elaborateBinaryOper<NLDB::PrimDivider>(e);
		break;
	case Expression::IDREF:
		result = elaborateId(e->getExpression(true));
		break;
	default:
		break;
	}
	return result;
}

NLDB::Net*
Elaborator::elaborateId(IdRef* id)
{
	// find id's net according to it's type in current netlist
	TypeDef::TYPE type = id->getIdDef()->getType()->getTypeDef()->getType();
	if (type == TypeDef::REG) {
		NLDB::Instance* inst = m_currMod->getInst(id->getName());
		NLDB::PrimLatch* latch = dynamic_cast<NLDB::PrimLatch*>(inst);
		if (latch == nullptr) {
			//error out
			return nullptr;
		}
		else {
			NLDB::Net* output = latch->getOutput()->getHighConn();
			if (output == nullptr) {
				output = new NLDB::Net(getdefaultNetName(), m_currMod);
				latch->connectOutput(output);
			}
			return output ;
		}
	}
	else if (type == TypeDef::WIRE) {
		return m_currMod->getNet(id->getName());
	}
	else {
		// error out
		return nullptr;
	}
}

void
Elaborator::processElabIfResults()
{
	DataFlow* df = m_currDataFlow;
	for (auto id : df->m_values_all) {
		//attention: only reg type can be assigned in dataflow
		if (id->getType()->getTypeDef()->getType() != TypeDef::REG &&
			id->getType()->getTypeDef()->getType() != TypeDef::UDM) {
			//error out
			return;
		}

		NLDB::Net* result = new NLDB::Net(getdefaultNetName(), m_currMod); // update this to identifier after processed
		NLDB::PrimMux* mux = new NLDB::PrimMux(getdefaultInstName(), m_currMod);
		NLDB::PrimLatch* latch = nullptr;
		mux->ConnectOutput(result);
		mux->ConnectEn(df->m_condition);
		// connect if branch
		if (df->m_values_if.find(id) != df->m_values_if.end()) {
			mux->ConnectInput(df->m_values_if[id], true);
		}
		else {
			latch = new NLDB::PrimLatch(getdefaultInstName(), m_currMod);
			NLDB::Net* latch_out = new NLDB::Net(getdefaultNetName(), m_currMod);
			latch->connectOutput(latch_out);
			latch->connectInput(result);
			result = latch_out;	//set latch's output as new result
			mux->ConnectInput(latch_out, true);
		}
		// connect else branch
		if (df->m_values_else.find(id) != df->m_values_else.end()) {
			mux->ConnectInput(df->m_values_else[id], false);
		}
		else {
			latch = new NLDB::PrimLatch(getdefaultInstName(), m_currMod);
			NLDB::Net* latch_out = new NLDB::Net(getdefaultNetName(), m_currMod);
			latch->connectOutput(latch_out);
			latch->connectInput(result);
			result = latch_out;	//set latch's output as 
			mux->ConnectInput(latch_out, false);
		}

		if (df->m_parent != nullptr) {
			//update id to parent dataflow
			if (m_currDataFlowBranch.top()) {
				df->m_parent->m_values_if[id] = result;
			}
			else {
				df->m_parent->m_values_else[id] = result;
			}
		}
		else {
			// the result is the final value of the identifier
			if (latch)
				m_currMod->updateReg(id->getName(), latch);
			else {
				m_currMod->updateReg(id->getName(), result);
			}
		}
	}
	m_currDataFlow = df->m_parent;
}

std::string
Elaborator::getdefaultNetName()
{
	const std::string prefix = "internal_net_";
	std::string name = prefix + std::to_string(m_cnts++);
	return name;
}

std::string Elaborator::getdefaultRegName()
{
	const std::string prefix = "internal_reg_";
	std::string name = prefix + std::to_string(m_cnts++);
	return name;
}

std::string Elaborator::getdefaultInstName()
{
	const std::string prefix = "internal_inst_";
	std::string name = prefix + std::to_string(m_cnts++);
	return name;
}

void
Elaborator::reset()
{
	while (m_currDataFlow) {
		DataFlow* todelete = m_currDataFlow;
		m_currDataFlow = m_currDataFlow->m_parent;
		delete todelete;
		
	}
	m_currDataFlow = nullptr;

	m_currDataFlowBranch = {};

	m_currMod = nullptr;
}