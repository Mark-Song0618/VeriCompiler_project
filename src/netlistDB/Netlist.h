#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>

/*
 *	netlist extraction��
 *  netlist�Ľṹ��Ӧ�þ������������Ҫ�˴˰�������������鷳��
 *	
 *  netlist : ��ʾһ��module���ڲ��ṹ��
 *		ports : �����ϲ�ṹ
 *		pins : ����insts��
 *		nets : ���е��ڲ����ӵ���·��
 *		insts : �����module������ʵ���������
 *
 *	Port : ����һ��netlistͨ�����Ĳ��������з���
 *		net:����һ��net��ʾ�������������ӵ�����net
 *	
 *  PortRef : ����һ��instance�������ڲ���port���ӡ�һ��netlist��port���ڲ�ͬ��instance���в�ͬ��PortRef��ӳ�䡣
 *		portref��port��ӳ���ϵ�����ڸ�instance�ͬʱ��netҲ���¼���PortRef.PortRef���к�Port��ͬ�ķ��򣨶�����instanceΪ��׼�ģ�
 * 
 *	instance : ��ʾһ����ʵ������netlist/module;��Ϊÿ��ʵ�����Ķ�����ڲ��ṹ���ǹ̶��ģ�netlist��,��ͬ��ֻ�����ӡ�����instance���Կ���
 *	�����ӹ�ϵ��ʵ�������������������Ϣ�������ӹ�ϵport:portRef
 *		Netlist:	m_view�������������netlist��
 *		������Ҫ����connect�Ĺ��ܣ���������netlist����port a����ô������Ҫά��pin a : port a��map�����ṩconnectA�ĺ�����ʹnet��������pin a;
 * 
 *	blackbox��blackbox�ǲ��ṩ�ڲ�netlist�ṹ��instance.����Ӧ���ṩpin�Ľӿڡ�
 */

namespace NLDB {

class PortRef;
class Port;
class Net;
class Instance;
class Netlist;
class Library;
class PrimLatch;

// basic class;
class DesignObj
{
public:
	DesignObj() = delete;
	DesignObj(std::string name, Netlist* owner) : m_name(name), m_owner(owner) {}

	std::string getName() { return m_name; }
	void setName(std::string name) { m_name = name; }
	Netlist* getOwner() { return m_owner; }
	void setOwner(Netlist* parent) { m_owner = parent; }
private:
	Netlist*	m_owner = nullptr;
	std::string m_name;
};


class Netlist
{
public:
	void setOwner(Library* lib) { m_owner = lib; }
	void addNet(Net* n);
	void addPort(Port* p);
	void addInst(Instance* i);
	Net* getNet(std::string n);
	const std::map<std::string, Net*>
		getNets() { return m_nets; }
	Port* getPort(std::string p);
	const std::map<std::string, Port*>
		getPorts() { return m_ports; }
	Instance* getInst(std::string i);
	const std::map<std::string, Instance*>&
			getInsts() { return m_insts; }
	void setName(std::string name) { m_name = name; }
	std::string getName() { return m_name; }

	void updateReg(std::string name, PrimLatch* latch);
	void updateReg(std::string name, Net* driver);

private:
	std::map<std::string, Port*>		m_ports;
	std::map<std::string, PortRef*>		m_pins;
	std::map<std::string, Net*>			m_nets;
	std::map<std::string, Instance*>	m_insts;
	std::set<Instance*>					m_refs;
	std::string							m_name;
	Library* m_owner = nullptr;
};

class Net : public DesignObj
{
	// net can only connect to port and portRef;
	// when conecting different nets, a buffer shoulb be inserted.
public:
	Net(std::string name, Netlist* owner);
	void setDriver(Port* port);
	void addLoad(Port* port);
	void setDriver(PortRef* pin);
	void addLoad(PortRef* pin);
	std::set<PortRef*> getLoadPins();
	std::set<Port*> getLoadPorts();
private:
	PortRef*			m_driven_pin = nullptr;		//could be driven by a port or portRef, multiDriven not supported yet
	Port*				m_driven_port = nullptr;	//could be driven by a port or portRef, multiDriven not supported yet
	std::set<PortRef*>	m_loadPins;
	std::set<Port*>		m_loadPorts;					
};

class Port : public DesignObj
{
public:
	enum DIR {IN, OUT};
	Port(std::string name, Netlist* nl);
	void setDir(DIR dir);
	void setLowConn(Net* n);
	DIR getDir();
	Net* getLowConn();
private:  
	Net*			m_net = nullptr;// low conn
	DIR				m_dir;
};

class Instance : public DesignObj
{
/* attention:
 * different instancs of a netlist, will have different portref, which points to the same port.
 * when a net connect a instance, the instance will create a PortRef��and save the PortRef:Port map
 */
public:
	Instance(std::string name, Netlist* nl) : DesignObj(name, nl) { nl->addInst(this); }
	virtual ~Instance() = default;
	bool isBlackBox() { return m_blackbox; }
	void setBlackBox(bool b) { m_blackbox = b; }

	virtual std::set<PortRef*> getPins() { return {}; }
	virtual std::string getPrimName() { return "Not A Primitive"; }
private:
	Netlist*					m_view = nullptr;	// the netlist instantialized from
	bool						m_blackbox = false;	//blackbox has no m_view available
	std::map<PortRef*, Port*>	m_pr2p;
	std::map<Port*, PortRef*>	m_p2pr;
};

class Library
{
public:
	void	addNetlist(Netlist* nl) { m_netlists.insert({ nl->getName(), nl }); }
	
	const std::map<std::string, Netlist*>&
			getNetlists() { return m_netlists; }
	
	std::vector<Netlist*>
			getAllNetlists();


	void setName(std::string name) { m_name = name; }
	std::string getName() { return m_name; }

private:
	std::map<std::string, Netlist*> m_netlists;
	std::string						m_name;
};

class PortRef
{
public:
	PortRef(Instance* owner, std::string name) : m_owner(owner), m_name(name) {}	// owner is the instance that owners this pin
	std::string getName() { return m_name; }
	void setHighConn(Net* n) { m_net = n; }
	Net* getHighConn() { return m_net; }
private:
	std::string		m_name;
	Port*			m_port = nullptr;	// low conn
	Net*			m_net = nullptr;	// high conn
	Instance*		m_owner = nullptr;
};

} // end of NLDB