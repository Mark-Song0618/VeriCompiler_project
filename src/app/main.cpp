#include "tokenizer.h"
#include "traversMgr.h"
#include "parser.h"
#include "elaborator.h"
#include "NetlistDumper.h"
#include "MainWin.h"
#include "app.h"
#include <fstream>
#include <iostream>

#include <QApplication>

//#define TEST 
int main(int argc, char** argv) {
#ifdef TEST
	// do some test here
#else
	app &app = app::instance();

	if (argc < 2) {
		std::cout << "usage:" << std::endl;
		std::cout << "1: -gui" << std::endl;
		std::cout << "2: -rtl filename -output dir" << std::endl;
		return 1;
	}

	if (strcmp(argv[1], "-gui") == 0) {
		QApplication app(argc, argv);
		MainWin win;
		win.show();
		app.exec();
	}
	else {
		// -rtl fileName -output dir
		std::string file, output = "./output.txt";
		for (int i = 1; i < argc; ++i) {
			if (strcmp(argv[i], "-rtl") == 0) {
				file = argv[++i];
			}
			else if (strcmp(argv[i], "-output") == 0) {
				output = argv[++i];
			}
		}
		if (file.empty()) {
			std::cout << "Error: rtl file not provided." << std::endl;
			return 1;
		}

		app.parseRtlFile(file);
		app.elaborate();

		PsTreeTraverser trs;
		Dumper dumper(output);
		trs.setVisitor(&dumper);
		trs.traverse(app.getParseTree(), 0);

		NetlistDumper nldumper;
		nldumper.setOutput(output);
		for (auto nl : app.getNLDBLib()->getNetlists()) {
			nldumper.dump(nl.second);
		}
	}
#endif
	return 0;
}