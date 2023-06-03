#include "NetlistDumper.h"
#include <fstream>

void
NetlistDumper::dump(NLDB::Netlist* nl)
{
	m_currNetlist = nl;
	dump();
	std::ofstream of(m_output, std::ios_base::app);
	of << m_cache;
	of.close();
	m_cache.clear();
}

void
NetlistDumper::dump()
{
	m_cache += "--------------------------------------------Netlist--------------------------------------------\r\n";
	 m_cache += moduleStart();
	 m_cache += " (";
	 m_cache += dumpPorts();
	 m_cache += ");";
	 m_cache += "\r\n";
	
	 m_cache += "\r\n";
	 m_cache += dumpNets() + "\r\n";

	 m_cache += "\r\n";
	 m_cache += dumpInsts() + "\r\n";

	 m_cache += "\r\n";
	 m_cache += moduleEnd();
}

std::string
NetlistDumper::moduleStart()
{
	std::string str("module ");
	str += m_currNetlist->getName();
	return std::move(str);
}

std::string
NetlistDumper::moduleEnd()
{
	std::string str("endModule");
	return std::move(str);
}

std::string
NetlistDumper::dumpPorts()
{
	std::string ports;
	for (auto pair : m_currNetlist->getPorts()) {
		NLDB::Port* p = pair.second;
		NLDB::Net* n = p->getLowConn();
		ports += " ";
		ports += n->getName();
	}
	return ports;
}

std::string
NetlistDumper::dumpNets()
{
	std::string nets;
	for (auto n : m_currNetlist->getNets()) {
		std::string net("\r\nwire ");
		net += n.first;
		net += ";";
		nets += net;
	}
	return nets;
}

std::string
NetlistDumper::dumpInsts()
{
	std::string insts;
	for (auto prim : m_currNetlist->getInsts()) {
		std::string inst("\r\n");
		inst += prim.second->getPrimName();
		inst += " ";
		inst += prim.first;
		inst += "(";
		for (auto pin : prim.second->getPins()) {
			inst += ".";
			inst += pin->getName();
			inst += "(";
			if (pin->getHighConn()) {
				inst += pin->getHighConn()->getName();
			}
			inst += "), ";
		}
		inst += ");";
		inst += "\r\n";
		insts += inst;
	}

	return insts;
}