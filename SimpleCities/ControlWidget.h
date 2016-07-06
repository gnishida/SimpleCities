#pragma once

#include <QtWidgets/QDockWidget>
#include "ui_ControlWidget.h"

class MainWindow;

class ControlWidget : public QDockWidget {
	Q_OBJECT

public:
	MainWindow* mainWin;
	Ui::ControlWidget ui;

public:
	ControlWidget(MainWindow* mainWin);
};

