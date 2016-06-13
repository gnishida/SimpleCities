#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "glew.h"
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "ControlWidget.h"
#include "PropertyWidget.h"
#include "GLWidget3D.h"
#include "UrbanGeometry.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	Ui::MainWindow ui;
	ControlWidget* controlWidget;
	PropertyWidget* propertyWidget;
	GLWidget3D* glWidget;
	UrbanGeometry* urbanGeometry;

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

public slots:
	void onNewTerrain();
	void onLoadTerrain();
	void onSaveTerrain();
	void onLoadBuildings();
	void onLoadParcels();
	void onLoadRoads();
	void onSaveRoads();
	void onClearRoads();

	void onSaveImage();
	void onSaveImageHD();
	void onLoadCamera();
	void onSaveCamera();
	void onResetCamera();

	void onGenerateBlocks();
	void onGenerateParcels();
	void onGenerateBuildings();
	void onGenerateVegetation();
	void onGenerateAll();

	void onShowControlWidget();
	void onShowPropertyWidget();
};

#endif // MAINWINDOW_H
