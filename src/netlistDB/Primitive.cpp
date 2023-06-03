#include "Netlist.h"
#include "Primitive.h"

using namespace NLDB;

void
PrimMux::ConnectEn(Net* n)
{
	if (!m_en)
		m_en = new PortRef(this, "en");

	n->addLoad(m_en);
	m_en->setHighConn(n);
}

void
PrimMux::ConnectInput(Net* n, bool b)
{
	PortRef* &pin = b ? m_true : m_false;
	if (!pin) {
		std::string name("input_");
		name += std::to_string(b);
		pin = new PortRef(this, name);
	}

	n->addLoad(pin);
	pin->setHighConn(n);
}

void
PrimMux::ConnectOutput(Net* n)
{
	if (!m_output)
		m_output = new PortRef(this, "output");

	m_output->setHighConn(n);
	n->setDriver(m_output);
}

PrimLatch::PrimLatch(std::string name, Netlist* nl)
	: Instance(name, nl)
{
	setBlackBox(true);
	m_d = new PortRef(this, "d");
	m_q = new PortRef(this, "q");
}

PrimLatch::~PrimLatch()
{
	if (m_d) delete m_d;
	if (m_q) delete m_q;
}

void
PrimLatch::connectInput(Net* n)
{
	n->addLoad(m_d);
	m_d->setHighConn(n);
}

void
PrimLatch::connectOutput(Net* n)
{
	n->setDriver(m_q);
	m_q->setHighConn(n);
}

PrimBuffer::PrimBuffer(std::string name, Netlist* nl, Net* input, Net* output)
	: Instance(name, nl)
{
	setBlackBox(true);
	m_input = new PortRef(this, "input");
	m_output = new PortRef(this, "output");
	connectInput(input);
	connectOutput(output);
}

PrimBuffer::PrimBuffer(std::string name, Netlist* nl)
	: Instance(name, nl)
{
	setBlackBox(true);
	m_input = new PortRef(this, "input");
	m_output = new PortRef(this, "output");
}

PrimBuffer::~PrimBuffer()
{
	if (m_input) delete m_input;
	if (m_output) delete m_output;
}

void
PrimBuffer::connectInput(Net* n)
{
	if (!m_input)
		m_input = new PortRef(this,"input");
	m_input->setHighConn(n);
	n->addLoad(m_input);
}

void
PrimBuffer::connectOutput(Net* n)
{
	if (!m_output)
		m_output = new PortRef(this, "output");

	m_output->setHighConn(n);
	n->addLoad(m_output);
}

PrimAdder::PrimAdder(std::string name, Netlist* nl)
	: Instance(name, nl)
{
	nl->addInst(this);
	m_input1 = new PortRef(this, "input1");
	m_input2 = new PortRef(this, "input2");
	m_output = new PortRef(this, "output");
}

PrimAdder::~PrimAdder()
{
	if (m_input1) delete m_input1;
	if (m_input2) delete m_input2;
	if (m_output) delete m_output;
}

void PrimAdder::connectInput(Net* n, unsigned idx)
{
	if (m_input1 == nullptr)
		m_input1 = new PortRef(this, "input1");
	if (m_input2 == nullptr)
		m_input2 = new PortRef(this, "input2");
	if (idx == 0) {
		m_input1->setHighConn(n);
		n->addLoad(m_input1);
	}
	else {
		m_input2->setHighConn(n);
		n->addLoad(m_input2);
	}
}


void PrimAdder::connectOutput(Net* n)
{
	if (m_output == nullptr)
		m_output = new PortRef(this, "output");

	m_output->setHighConn(n);
	n->setDriver(m_output);
}


PrimMiner::PrimMiner(std::string name, Netlist* nl)
	: Instance(name, nl)
{
	nl->addInst(this);
	m_input1 = new PortRef(this, "input1");
	m_input2 = new PortRef(this, "input2");
	m_output = new PortRef(this, "output");
}

PrimMiner::~PrimMiner()
{
	if (m_input1) delete m_input1;
	if (m_input2) delete m_input2;
	if (m_output) delete m_output;
}

void PrimMiner::connectInput(Net* n, unsigned idx)
{
	if (m_input1 == nullptr)
		m_input1 = new PortRef(this,"input1");
	if (m_input2 == nullptr)
		m_input2 = new PortRef(this, "input2");
	if (idx == 0) {
		m_input1->setHighConn(n);
		n->addLoad(m_input1);
	}
	else {
		m_input2->setHighConn(n);
		n->addLoad(m_input2);
	}
}


void PrimMiner::connectOutput(Net* n)
{
	if (m_output == nullptr)
		m_output = new PortRef(this,"output");

	m_output->setHighConn(n);
	n->setDriver(m_output);
}


PrimMultipler::PrimMultipler(std::string name, Netlist* nl)
	: Instance(name, nl)
{
	nl->addInst(this);
	m_input1 = new PortRef(this, "input1");
	m_input2 = new PortRef(this, "input2");
	m_output = new PortRef(this, "output");
}

PrimMultipler::~PrimMultipler()
{
	if (m_input1) delete m_input1;
	if (m_input2) delete m_input2;
	if (m_output) delete m_output;
}

void PrimMultipler::connectInput(Net* n, unsigned idx)
{
	if (m_input1 == nullptr)
		m_input1 = new PortRef(this, "input1");
	if (m_input2 == nullptr)
		m_input2 = new PortRef(this, "input2");
	if (idx == 0) {
		m_input1->setHighConn(n);
		n->addLoad(m_input1);
	}
	else {
		m_input2->setHighConn(n);
		n->addLoad(m_input2);
	}
}


void PrimMultipler::connectOutput(Net* n)
{
	if (m_output == nullptr)
		m_output = new PortRef(this, "output");

	m_output->setHighConn(n);
	n->setDriver(m_output);
}


PrimDivider::PrimDivider(std::string name, Netlist* nl)
	: Instance(name, nl)
{
	nl->addInst(this);
	m_input1 = new PortRef(this, "input1");
	m_input2 = new PortRef(this, "input2");
	m_output = new PortRef(this, "output");
}

PrimDivider::~PrimDivider()
{
	if (m_input1) delete m_input1;
	if (m_input2) delete m_input2;
	if (m_output) delete m_output;
}

void PrimDivider::connectInput(Net* n, unsigned idx)
{
	if (m_input1 == nullptr)
		m_input1 = new PortRef(this, "input1");
	if (m_input2 == nullptr)
		m_input2 = new PortRef(this, "input2");
	if (idx == 0) {
		m_input1->setHighConn(n);
		n->addLoad(m_input1);
	}
	else {
		m_input2->setHighConn(n);
		n->addLoad(m_input2);
	}
}


void PrimDivider::connectOutput(Net* n)
{
	if (m_output == nullptr)
		m_output = new PortRef(this, "output");

	m_output->setHighConn(n);
	n->setDriver(m_output);
}