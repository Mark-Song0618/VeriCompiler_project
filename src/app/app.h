#pragma once
#include "parser.h"
#include "elaborator.h"
class app {
public:
	static app &instance() {
		static app a;
		return a;
	}
	void parseRtlFile(std::string path);
	void parseRtlContext(std::string name, std::string context);
	void parse();
	NLDB::Library* elaborate();
	void reset();
	Library* getParseTree() { return m_parse_lib; }
	NLDB::Library* getNLDBLib() { return m_NLDB_lib; }
private:
	app() = default;
private:
	NS_PARSER::Parser m_parser;
	Elaborator m_elaborator;
	std::string m_src;
	NLDB::Library* m_NLDB_lib = nullptr;
	Library* m_parse_lib = nullptr;
};