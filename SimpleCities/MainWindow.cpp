#include "MainWindow.h"
#include <QFileDialog>
#include <QShortcut>
#include "TerrainSizeInputDialog.h"
#include "GraphUtil.h"
#include "Util.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	// setup the docking widgets
	controlWidget = new ControlWidget(this);
	propertyWidget = new PropertyWidget(this);

	// setup the toolbar
	ui.fileToolBar->addAction(ui.actionNewTerrain);
	ui.fileToolBar->addAction(ui.actionOpenTerrain);

	// register the menu's action handlers
	connect(ui.actionNewTerrain, SIGNAL(triggered()), this, SLOT(onNewTerrain()));
	connect(ui.actionOpenTerrain, SIGNAL(triggered()), this, SLOT(onLoadTerrain()));
	connect(ui.actionSaveTerrain, SIGNAL(triggered()), this, SLOT(onSaveTerrain()));
	connect(ui.actionLoadParcels, SIGNAL(triggered()), this, SLOT(onLoadParcels()));
	connect(ui.actionLoadBuildings, SIGNAL(triggered()), this, SLOT(onLoadBuildings()));
	connect(ui.actionLoadRoads, SIGNAL(triggered()), this, SLOT(onLoadRoads()));
	connect(ui.actionSaveRoads, SIGNAL(triggered()), this, SLOT(onSaveRoads()));
	connect(ui.actionClearRoads, SIGNAL(triggered()), this, SLOT(onClearRoads()));
	connect(ui.actionSaveImage, SIGNAL(triggered()), this, SLOT(onSaveImage()));
	connect(ui.actionSaveImageHD, SIGNAL(triggered()), this, SLOT(onSaveImageHD()));
	connect(ui.actionLoadCamera, SIGNAL(triggered()), this, SLOT(onLoadCamera()));
	connect(ui.actionSaveCamera, SIGNAL(triggered()), this, SLOT(onSaveCamera()));
	connect(ui.actionResetCamera, SIGNAL(triggered()), this, SLOT(onResetCamera()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	connect(ui.actionGenerateBlocks, SIGNAL(triggered()), this, SLOT(onGenerateBlocks()));
	connect(ui.actionGenerateParcels, SIGNAL(triggered()), this, SLOT(onGenerateParcels()));
	connect(ui.actionGenerateBuildings, SIGNAL(triggered()), this, SLOT(onGenerateBuildings()));
	connect(ui.actionGenerateVegetation, SIGNAL(triggered()), this, SLOT(onGenerateVegetation()));
	connect(ui.actionGenerateAll, SIGNAL(triggered()), this, SLOT(onGenerateAll()));

	connect(ui.actionControlWidget, SIGNAL(triggered()), this, SLOT(onShowControlWidget()));
	connect(ui.actionPropertyWidget, SIGNAL(triggered()), this, SLOT(onShowPropertyWidget()));

	// setup the GL widget
	glWidget = new GLWidget3D(this);
	setCentralWidget(glWidget);

	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);

	urbanGeometry = new UrbanGeometry(this);
}

MainWindow::~MainWindow() {
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	glWidget->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	glWidget->keyReleaseEvent(e);
}

void MainWindow::onNewTerrain() {
	TerrainSizeInputDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted) {
		glWidget->vboRenderManager.changeTerrainDimensions(glm::vec2(dlg.side, dlg.side));
		glWidget->shadow.makeShadowMap(glWidget);
		glWidget->updateGL();
	}
}

void MainWindow::onLoadTerrain() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load DEM file..."), "", tr("DEM Files (*.tif)"));
	if (filename.isEmpty()) return;

	urbanGeometry->loadTerrain(filename.toUtf8().constData());
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onSaveTerrain() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Terrain file..."), "", tr("Terrain Files (*.png)"));
	if (filename.isEmpty()) return;
	glWidget->vboRenderManager.vboTerrain.saveTerrain(filename);
}

void MainWindow::onLoadBuildings() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load buildings..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->loadBuildings(filename.toUtf8().constData());
	glWidget->updateGL();
}

void MainWindow::onLoadParcels() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load parcels..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->loadParcels(filename.toUtf8().constData());
	glWidget->updateGL();
}

void MainWindow::onLoadRoads() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load roads..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->loadRoads(filename.toUtf8().constData());
	glWidget->updateGL();
}

void MainWindow::onSaveRoads() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Street Map file..."), "", tr("StreetMap Files (*.gsm)"));
	if (filename.isEmpty()) return;

	QApplication::setOverrideCursor(Qt::WaitCursor);

	urbanGeometry->saveRoads(filename);

	QApplication::restoreOverrideCursor();
}

void MainWindow::onClearRoads() {
	urbanGeometry->clearRoads();
	glWidget->updateGL();
}

void MainWindow::onSaveImage() {
	if(QDir("screenshots").exists()==false) QDir().mkdir("screenshots");
	QString fileName="screenshots/"+QDate::currentDate().toString("yyMMdd")+"_"+QTime::currentTime().toString("HHmmss")+".png";
	glWidget->grabFrameBuffer().save(fileName);
	printf("Save %s\n", fileName.toUtf8().constData());
}//

void MainWindow::onSaveImageHD() {
	if(QDir("screenshots").exists()==false) QDir().mkdir("screenshots");
	QString fileName="screenshots/"+QDate::currentDate().toString("yyMMdd")+"_"+QTime::currentTime().toString("HHmmss")+".png";
	glWidget->grabFrameBuffer().save(fileName);

	QString fileNameHD="screenshots/"+QDate::currentDate().toString("yyMMdd")+"_"+QTime::currentTime().toString("HHmmss")+"_HD.png";
	int cH=glWidget->height();
	int cW=glWidget->width();
	glWidget->resize(cW*3,cH*3);
	glWidget->updateGL();
	glWidget->grabFrameBuffer().save(fileNameHD);
	glWidget->resize(cW,cH);
	glWidget->updateGL();

	printf("Save HD %s\n", fileName.toUtf8().constData());
}//

void MainWindow::onLoadCamera() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Camera file..."), "", tr("Area Files (*.cam)"));
	if (filename.isEmpty()) return;

	glWidget->camera.loadCameraPose(filename);
	glWidget->updateCamera();

	glWidget->updateGL();
}

void MainWindow::onSaveCamera() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Camera file..."), "", tr("Area Files (*.cam)"));
	if (filename.isEmpty()) return;
	
	glWidget->camera.saveCameraPose(filename);
}

void MainWindow::onResetCamera() {
	glWidget->camera.resetCamera();
	glWidget->updateCamera();
	glWidget->updateGL();
}

void MainWindow::onGenerateBlocks() {
	urbanGeometry->generateBlocks();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateParcels() {
	urbanGeometry->generateParcels();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateBuildings() {
	urbanGeometry->generateBuildings();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateVegetation() {
	urbanGeometry->generateVegetation();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onGenerateAll() {
	urbanGeometry->generateAll();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onShowControlWidget() {
	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}

void MainWindow::onShowPropertyWidget() {
	propertyWidget->show();
	addDockWidget(Qt::RightDockWidgetArea, propertyWidget);
}
