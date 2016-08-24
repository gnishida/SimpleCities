#pragma once

#include <QDialog>
#include "ui_ScenarioGenerationDialog.h"

class ScenarioGenerationDialog : public QDialog
{
	Q_OBJECT

public:
	ScenarioGenerationDialog(QWidget *parent = 0);

public slots:
	void onZone();
	void onTerrain();
	void onOutputDirectory();
	void onUseSimulator();
	void onSimulatorDirectory();
	void onOK();
	void onCancel();

public:
	Ui::ScenarioGenerationDialog ui;
};

