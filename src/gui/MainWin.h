#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QTabWidget>
#include <QAction>
#include <QFileDialog>

class MainWin : public QMainWindow {
public:
	MainWin() { init(); }
	virtual ~MainWin() {}

	void		initMenuBar();

private:
	void			openFileUI();
	void			newFileUI();
	void			saveFileUI();
	void			parseCurrFile();
	void			compileCurrFile();

private:
	void init();
	QMenuBar		*m_menu = nullptr;
	QTabWidget		*m_browser = nullptr;
	QMenu			*m_file = nullptr;
	QMenu			*m_run = nullptr;

	QAction			*m_open = nullptr;
	QAction			*m_new = nullptr;
	QAction			*m_save = nullptr;
	QAction			*m_parse = nullptr;
	QAction			*m_compile = nullptr;
};