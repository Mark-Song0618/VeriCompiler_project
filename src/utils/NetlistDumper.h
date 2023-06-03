#pragma once

#include "Netlist.h"
#include <string>
//#include <fstream>

class NetlistDumper
{
public:
	NetlistDumper() = default;
	NetlistDumper(NLDB::Netlist* nl) : m_currNetlist(nl) {}

	void dump();
	void dump(NLDB::Netlist* nl);
	void setOutput(std::string path) { m_output = path; }
	void reset() { m_cache.clear(); m_output = ""; m_currNetlist = nullptr; }
private:
	std::string moduleStart();
	std::string moduleEnd();
	std::string dumpPorts();
	std::string dumpNets();
	std::string dumpInsts();

	NLDB::Netlist* m_currNetlist = nullptr;
	std::string m_cache;
	std::string m_output;

};