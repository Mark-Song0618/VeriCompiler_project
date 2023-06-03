#include "app.h"
#include <fstream>
#include <algorithm>

void
app::parseRtlFile(std::string path)
{
	reset();
	for (auto it = path.rbegin(); it != path.rend(); ++it) {
		if (*it == '/') {
			path = std::string(path.rbegin(), it);
			break;
		}
	}
	std::reverse(path.begin(), path.end());
	m_parser.setLibName(path);

	std::ifstream file(path);
	std::string tmp;
	while (std::getline(file, tmp))
		m_src += tmp;

	parse();
}

void
app::parseRtlContext(std::string name, std::string context)
{
	reset();

	for (auto it = name.rbegin(); it != name.rend(); ++it) {
		if (*it == '/') {
			name = std::string(name.rbegin(), it);
			break;
		}
	}

	m_parser.setLibName(name);
	m_src = context;
	parse();
}

void app::reset()
{
	m_src.clear();
	m_parser.reset();
	m_elaborator.reset();

	if (m_NLDB_lib) {
		delete m_NLDB_lib;
		m_NLDB_lib = nullptr;
	}

	if (m_parse_lib) {
		delete m_parse_lib;
		m_parse_lib = nullptr;
	}
}

void
app::parse()
{
	m_parse_lib = m_parser.parse(m_src);
}

NLDB::Library*
app::elaborate()
{
	m_NLDB_lib = m_elaborator.elaborate(m_parse_lib);
	return m_NLDB_lib;
}