#pragma once

#include <QtWidgets/QDockWidget>
#include "ui_ControlWidget.h"

class MainWindow;

class ControlWidget : public QDockWidget {
Q_OBJECT

private:
	
	MainWindow* mainWin;

public:
	Ui::ControlWidget ui;
	ControlWidget(MainWindow* mainWin);


public slots:
	void generateRoadsPM();
	void clear();
	void updateTerrainLabels(int newValue);
	void updateTerrainEdit(const QString &text);
	void changeTerrainShader(int);

};

