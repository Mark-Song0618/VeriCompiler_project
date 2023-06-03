#include "Netlist.h"
#include "Primitive.h"
namespace NLDB {

Net::Net(std::string name, Netlist* owner)
	: DesignObj(name, owner)
{
	owner->addNet(this);
}

void
	Net::setDriver(Port* port)
{
	m_driven_port = port;
}

void
	Net::addLoad(Port* port)
{
	m_loadPorts.insert(port);
}

void
	Net::setDriver(PortRef* pin)
{
	m_driven_pin = pin;
}

void
	Net::addLoad(PortRef* pin)
{
	m_loadPins.insert(pin);
}

std::set<PortRef*>
	Net::getLoadPins()
{
	return m_loadPins;
}
std::set<Port*> Net::getLoadPorts()
{
	return m_loadPorts;
}

Port::Port(std::string name, Netlist* nl)
	: DesignObj(name, nl)
{
	nl->addPort(this);
}

void
	Port::setDir(DIR dir)
{
	m_dir = dir;
}

void
	Port::setLowConn(Net* n)
{
	m_net = n;
}

Port::DIR
	Port::getDir()
{
	return m_dir;
}

Net*
	Port::getLowConn()
{
	return m_net;
}

void Netlist::addNet(Net* n)
{
	m_nets.insert({ n->getName(), n });
}

void Netlist::addPort(Port* p) {
	m_ports.insert({ p->getName(), p });
}

void Netlist::addInst(Instance* i)
{
	m_insts.insert({ i->getName(), i });
}

Net*
	Netlist::getNet(std::string n)
{
	if (m_nets.find(n) != m_nets.end()) {
		return m_nets.find(n)->second;
	}
	return nullptr;
}

Port*
	Netlist::getPort(std::string n)
{
	if (m_ports.find(n) != m_ports.end()) {
		return m_ports.find(n)->second;
	}
	return nullptr;
}

Instance*
	Netlist::getInst(std::string n)
{
	if (m_insts.find(n) != m_insts.end()) {
		return m_insts.find(n)->second;
	}
	return nullptr;
}

void
Netlist::updateReg(std::string name, PrimLatch* latch)
{
	Instance* old = getInst(name);
	PrimLatch* old_latch = dynamic_cast<PrimLatch*>(old);
	if (!old_latch) {
		//error out
		return;
	}
	else {
		//update driver
		old_latch->connectInput(latch->getInput()->getHighConn());
		// update load
		Net* load_net_old = old_latch->getOutput()->getHighConn();
		Net* load_net_new = latch->getOutput()->getHighConn();
		if (load_net_old == nullptr) {
			old_latch->connectOutput(load_net_new);
			delete load_net_old;
		}
		else {
			for (auto p : load_net_new->getLoadPins())
				load_net_old->addLoad(p);
			for (auto p : load_net_new->getLoadPorts())
				load_net_old->addLoad(p);
		}
		delete latch;
	}
}

void
Netlist::updateReg(std::string name, Net* driver)
{
	Instance* old = getInst(name);
	PrimLatch* old_latch = dynamic_cast<PrimLatch*>(old);
	if (!old_latch) {
		//error out
		return;
	}
	else {
		//update driver
		old_latch->connectInput(driver);
	}
}

std::vector<Netlist*>
Library::getAllNetlists()
{
	std::vector<Netlist*> rsts;
	for (auto& p : m_netlists) {
		rsts.push_back(p.second);
	}
	return rsts;
}

}