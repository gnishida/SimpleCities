#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glew.h"
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "ControlWidget.h"
#include "GLWidget3D.h"
#include "UrbanGeometry.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	Ui::MainWindow ui;
	ControlWidget* controlWidget;
	GLWidget3D* glWidget;
	UrbanGeometry* urbanGeometry;

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

public slots:
	void onLoadZone();
	void onNewTerrain();
	void onLoadTerrain();
	void onSaveTerrain();
	void onLoadRoads();
	void onSaveRoads();
	void onLoadParcels();
	void onSaveParcels();
	void onLoadBuildings();
	void onSaveBuildings();
	void onClear();

	void onSaveImage();
	void onSaveImageHD();
	void onLoadCamera();
	void onSaveCamera();
	void onResetCamera();

	void onGenerateRoads();
	void onGenerateBlocks();
	void onGenerateParcels();
	void onGenerateBuildings();
	void onGenerateVegetation();
	void onGenerateAll();

	void onGenerateScenarios();

	void onShowControlWidget();
};

#endif // MAINWINDOW_H
