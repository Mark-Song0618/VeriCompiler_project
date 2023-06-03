#include "MainWin.h"
#include "app.h"
#include "traversMgr.h"
#include "visitor.h"
#include "Netlistdumper.h"
#include <QGridLayout>
#include <QTextEdit>
#include <QTextBrowser>
#include <QTabBar>
#include <iostream>
#include <fstream>
void
MainWin::init()
{
	//basic appearence
	setWindowTitle("Mark ProtoType System");
	setFixedSize(800,800);

	// set toolbar
	initMenuBar();
	
	// init tabwidget
	m_browser = new QTabWidget();
	QTextBrowser* wcPage = new QTextBrowser();
	QString note;
	note = note +"Welcome to use Mark Advanced Prototype System." + "\r\n\n";
	note = note + "Usage:" + "\r\n";
	note = note + "	\"file\" menu to create or open a rtl design." + "\r\n";
	note = note + "	\"run\" menu to parse the rlt or compile to netlist." + "\r\n";
	wcPage->setText(note);

	// layout
	setCentralWidget(m_browser);
	m_browser->addTab(wcPage, "Welcome");
}

void
MainWin::initMenuBar()
{
	m_menu = menuBar();

	// init file menu
	m_file =	new QMenu("File");
	m_open =	new QAction("open file");
	m_new =		new QAction("new file");
	m_save =	new QAction("save current file");
	m_file->addActions({m_open, m_new, m_save});
	m_menu->addMenu(m_file);
;
	// init run menu
	m_run =		new QMenu("Run");
	m_parse =	new QAction("parse rtl");
	m_compile = new QAction("compile rtl");
	m_run->addActions({m_parse, m_compile});
	m_menu->addMenu(m_run);

	// connect signals and slots
	connect(m_open,		&QAction::triggered, this, &MainWin::openFileUI);
	connect(m_new,		&QAction::triggered, this, &MainWin::newFileUI);
	connect(m_save,		&QAction::triggered, this, &MainWin::saveFileUI);
	connect(m_parse,	&QAction::triggered, this, &MainWin::parseCurrFile);
	connect(m_compile,	&QAction::triggered, this, &MainWin::compileCurrFile);
}

void
MainWin::openFileUI()
{
	// read in file
	QString filePath = QFileDialog::getOpenFileName(this, "open rtl file", "../../../src/test", "*.v"	/* configurable path */);
	std::ifstream srcFile(filePath.toStdString());
	std::string line, context;
	while (std::getline(srcFile, line)) {
		context += line;
		context += "\r\n";
	}
	
	// display context
	QTextEdit* page = new QTextEdit();
	page->setText(context.c_str());
	auto it = filePath.rbegin();
	while (*it != '/' && it != filePath.rend())
		++it;
	QString str;

	for (--it; it >= filePath.rbegin(); --it) {
		str.push_back(*it);
	}

	m_browser->setCurrentIndex(m_browser->addTab(page, str));
}
void
MainWin::newFileUI()
{
	// create an empty page
	QTextEdit* newPage = new QTextEdit();
	m_browser->setCurrentIndex(m_browser->addTab(newPage, "new_design.v"));
}

void
MainWin::saveFileUI()
{
	QWidget* currWidget = m_browser->currentWidget();
	QTextEdit* currPage = dynamic_cast<QTextEdit*>(currWidget);
	QString context = currPage->toPlainText();
	QString title = m_browser->tabBar()->tabText(m_browser->currentIndex());
	std::ofstream saveFile(title.toStdString());
	saveFile << context.toStdString() << std::endl;
	saveFile.close();
}

void
MainWin::parseCurrFile()
{
	// get context
	QWidget* currWidget = m_browser->currentWidget();
	QTextEdit* currPage = dynamic_cast<QTextEdit*>(currWidget);
	QString context = currPage->toPlainText();
	QString title = m_browser->tabBar()->tabText(m_browser->currentIndex());
	//parse
	app::instance().parseRtlContext(title.toStdString(), context.toStdString());
	//display result

	PsTreeTraverser trs;
	std::remove("./tmp.txt");
	Dumper dumper("tmp.txt");
	trs.setVisitor(&dumper);
	trs.traverse(app::instance().getParseTree(), 0);

	std::ifstream rstFile("tmp.txt");
	std::string s1, s2;
	while (std::getline(rstFile, s1)) {
		s2 += s1;
		/*s2 += "\r\n";*/
	}
	QTextBrowser* newPage = new QTextBrowser();
	newPage->setText(s2.c_str());
	title += "_AST";
	m_browser->addTab(newPage, title);
}
void
MainWin::compileCurrFile()
{
	app &app = app::instance();
	app.elaborate();
	NetlistDumper dumper;
	
	std::remove("./tmpElab.txt");
	dumper.setOutput("./tmpElab.txt");
	for (auto n : app.getNLDBLib()->getAllNetlists()) {
		dumper.dump(n);
	}
	std::ifstream rstFile("tmpElab.txt");
	std::string s1, s2;
	while (std::getline(rstFile, s1)) {
		s2 += s1;
	}

	QTextBrowser* newPage = new QTextBrowser();
	newPage->setText(s2.c_str());
	QString title(app.getNLDBLib()->getName().c_str());
	title += "_NetList";
	m_browser->addTab(newPage, title);
}