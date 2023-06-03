#pragma once
namespace NLDB {

//class PrimMux : public Netlist
class PrimMux : public Instance
{
public:
	PrimMux(std::string name, Netlist* nl)
		: Instance(name, nl)
	{
		setBlackBox(true);
	}

	void ConnectEn(Net* n);
	void ConnectInput(Net* n, bool b);
	void ConnectOutput(Net* n);
	PortRef* getEn() { return m_en; }
	PortRef* getInput(bool b) { return b ? m_true : m_false; }
	PortRef* getOutput() { return m_output; }
	std::set<PortRef*> getPins() { return { m_en, m_true, m_false, m_output}; }

	std::string getPrimName() { return "PrimMux"; }
private:
	PortRef* m_en = nullptr;
	PortRef* m_true = nullptr;
	PortRef* m_false = nullptr;
	PortRef* m_output = nullptr;
};

//class PrimLatch : public Netlist
class PrimLatch : public Instance
{
public:
	PrimLatch(std::string name, Netlist* nl);
	~PrimLatch();

	void connectInput(Net* n);
	void connectOutput(Net* n);
	PortRef* getInput() { return m_d; }
	PortRef* getOutput() { return m_q; }

	std::string getPrimName() { return "PrimLatch"; }
	std::set<PortRef*> getPins() { return { m_d, m_q }; }
private:
	PortRef* m_d = nullptr;
	PortRef* m_q = nullptr;
};

class PrimBuffer : public Instance
{
public:
	PrimBuffer(std::string name, Netlist* nl, Net* input, Net* output);
	PrimBuffer(std::string name, Netlist* nl);
	~PrimBuffer();

	PortRef* getInput() { return m_input; }
	PortRef* getOutput() { return m_output; }
	void connectInput(Net* n);
	void connectOutput(Net* n);

	std::string getPrimName() { return "PrimBuffer"; }
	std::set<PortRef*> getPins() { return { m_input, m_output }; }
private:
	PortRef* m_input = nullptr;
	PortRef* m_output = nullptr;
};

// todo: use template class<TYPE> for adder¡¢ miner¡¢ multipler¡¢ divider
class PrimAdder : public Instance
{
public:
	PrimAdder(std::string name, Netlist* nl);
	~PrimAdder();
	void connectInput(Net*, unsigned);
	void connectOutput(Net*);

	std::string getPrimName() { return "PrimAdder"; }
	std::set<PortRef*> getPins() { return { m_input1, m_input2, m_output }; }
private:
	PortRef* m_input1 = nullptr;
	PortRef* m_input2 = nullptr;
	PortRef* m_output = nullptr;
};

class PrimMiner : public Instance
{
public:
	PrimMiner(std::string name, Netlist* nl);
	~PrimMiner();
	void connectInput(Net*, unsigned);
	void connectOutput(Net*);

	std::string getPrimName() { return "PrimMiner"; }
	std::set<PortRef*> getPins() { return { m_input1, m_input2, m_output }; }
private:
	PortRef* m_input1 = nullptr;
	PortRef* m_input2 = nullptr;
	PortRef* m_output = nullptr;
};

class PrimMultipler : public Instance
{
public:
	PrimMultipler(std::string name, Netlist* nl);
	~PrimMultipler();
	void connectInput(Net*, unsigned);
	void connectOutput(Net*);

	std::string getPrimName() { return "PrimMultipler"; }
	std::set<PortRef*> getPins() { return { m_input1, m_input2, m_output }; }
private:
	PortRef* m_input1 = nullptr;
	PortRef* m_input2 = nullptr;
	PortRef* m_output = nullptr;
};

class PrimDivider : public Instance
{
public:
	PrimDivider(std::string name, Netlist* nl);
	~PrimDivider();
	void connectInput(Net*, unsigned);
	void connectOutput(Net*);
	std::string getPrimName() { return "PrimDivider"; }
	std::set<PortRef*> getPins() { return { m_input1, m_input2, m_output }; }
private:
	PortRef* m_input1 = nullptr;
	PortRef* m_input2 = nullptr;
	PortRef* m_output = nullptr;
};
}