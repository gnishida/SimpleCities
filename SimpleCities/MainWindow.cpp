#include "MainWindow.h"
#include <QFileDialog>
#include <QShortcut>
#include "TerrainSizeInputDialog.h"
#include "ScenarioGenerationDialog.h"
#include "GraphUtil.h"
#include "Util.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	QActionGroup* group = new QActionGroup(this);
	group->addAction(ui.actionView2D);
	group->addAction(ui.actionView3D);
	ui.actionView2D->setChecked(true);
	ui.actionView3D->setChecked(false);

	// setup the docking widgets
	controlWidget = new ControlWidget(this);

	// register the menu's action handlers
	connect(ui.actionLoadZone, SIGNAL(triggered()), this, SLOT(onLoadZone()));
	connect(ui.actionNewTerrain, SIGNAL(triggered()), this, SLOT(onNewTerrain()));
	connect(ui.actionOpenTerrain, SIGNAL(triggered()), this, SLOT(onLoadTerrain()));
	connect(ui.actionSaveTerrain, SIGNAL(triggered()), this, SLOT(onSaveTerrain()));
	connect(ui.actionLoadRoads, SIGNAL(triggered()), this, SLOT(onLoadRoads()));
	connect(ui.actionSaveRoads, SIGNAL(triggered()), this, SLOT(onSaveRoads()));
	connect(ui.actionLoadParcels, SIGNAL(triggered()), this, SLOT(onLoadParcels()));
	connect(ui.actionSaveParcels, SIGNAL(triggered()), this, SLOT(onSaveParcels()));
	connect(ui.actionLoadBuildings, SIGNAL(triggered()), this, SLOT(onLoadBuildings()));
	connect(ui.actionSaveBuildings, SIGNAL(triggered()), this, SLOT(onSaveBuildings()));
	connect(ui.actionClear, SIGNAL(triggered()), this, SLOT(onClear()));

	connect(ui.actionSaveImage, SIGNAL(triggered()), this, SLOT(onSaveImage()));
	connect(ui.actionSaveImageHD, SIGNAL(triggered()), this, SLOT(onSaveImageHD()));
	connect(ui.actionLoadCamera, SIGNAL(triggered()), this, SLOT(onLoadCamera()));
	connect(ui.actionSaveCamera, SIGNAL(triggered()), this, SLOT(onSaveCamera()));
	connect(ui.actionResetCamera, SIGNAL(triggered()), this, SLOT(onResetCamera()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	connect(ui.actionGenerateRoads, SIGNAL(triggered()), this, SLOT(onGenerateRoads()));
	connect(ui.actionGenerateBlocks, SIGNAL(triggered()), this, SLOT(onGenerateBlocks()));
	connect(ui.actionGenerateParcels, SIGNAL(triggered()), this, SLOT(onGenerateParcels()));
	connect(ui.actionGenerateBuildings, SIGNAL(triggered()), this, SLOT(onGenerateBuildings()));
	connect(ui.actionGenerateVegetation, SIGNAL(triggered()), this, SLOT(onGenerateVegetation()));
	connect(ui.actionGenerateAll, SIGNAL(triggered()), this, SLOT(onGenerateAll()));

	connect(ui.actionGenerateScenarios, SIGNAL(triggered()), this, SLOT(onGenerateScenarios()));

	connect(ui.actionView2D, SIGNAL(triggered()), this, SLOT(onViewChanged()));
	connect(ui.actionView3D, SIGNAL(triggered()), this, SLOT(onViewChanged()));
	connect(ui.actionControlWidget, SIGNAL(triggered()), this, SLOT(onShowControlWidget()));

	// setup the GL widget
	glWidget = new GLWidget3D(this);
	setCentralWidget(glWidget);

	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);

	//urbanGeometry = new UrbanGeometry(this);
}

MainWindow::~MainWindow() {
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
	glWidget->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	glWidget->keyReleaseEvent(e);
}

void MainWindow::onLoadZone() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load zone..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->loadZone(filename.toUtf8().constData());
	glWidget->updateGL();
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

void MainWindow::onLoadRoads() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load roads..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->loadRoads(filename.toUtf8().constData());
	glWidget->updateGL();
}

void MainWindow::onSaveRoads() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save roads..."), "", tr("Shapefiles Files (*.shp)"));
	if (filename.isEmpty()) return;

	QApplication::setOverrideCursor(Qt::WaitCursor);

	urbanGeometry->saveRoads(filename.toUtf8().constData());

	QApplication::restoreOverrideCursor();
}

void MainWindow::onLoadParcels() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load parcels..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->loadParcels(filename.toUtf8().constData());
	glWidget->updateGL();
}

void MainWindow::onSaveParcels() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save parcels..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->saveParcels(filename.toUtf8().constData());
	glWidget->updateGL();
}

void MainWindow::onLoadBuildings() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load buildings..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->loadBuildings(filename.toUtf8().constData());
	glWidget->updateGL();
}

void MainWindow::onSaveBuildings() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Load buildings..."), "", tr("Shapefiles (*.shp)"));
	if (filename.isEmpty()) return;

	urbanGeometry->saveBuildings(filename.toUtf8().constData());
	glWidget->updateGL();
}

void MainWindow::onClear() {
	urbanGeometry->clear();
	glWidget->updateGL();
}

void MainWindow::onSaveImage() {
	if (!QDir("screenshots").exists()) QDir().mkdir("screenshots");
	QString fileName = "screenshots/" + QDate::currentDate().toString("yyMMdd") + "_" + QTime::currentTime().toString("HHmmss") + ".png";
	glWidget->grabFrameBuffer().save(fileName);
}

void MainWindow::onSaveImageHD() {
	if (!QDir("screenshots").exists()) QDir().mkdir("screenshots");
	QString fileName = "screenshots/" + QDate::currentDate().toString("yyMMdd") + "_" + QTime::currentTime().toString("HHmmss") + "_HD.png";
	int cH = glWidget->height();
	int cW = glWidget->width();
	glWidget->resize(cW * 3, cH * 3);
	glWidget->updateGL();
	glWidget->grabFrameBuffer().save(fileName);
	glWidget->resize(cW,cH);
	glWidget->updateGL();
}

void MainWindow::onLoadCamera() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Camera file..."), "", tr("Camera files (*.cam)"));
	if (filename.isEmpty()) return;

	glWidget->camera.loadCameraPose(filename.toUtf8().constData());
	glWidget->updateCamera();
	glWidget->updateGL();
}

void MainWindow::onSaveCamera() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Camera file..."), "", tr("Camera files (*.cam)"));
	if (filename.isEmpty()) return;
	
	glWidget->camera.saveCameraPose(filename.toUtf8().constData());
}

void MainWindow::onResetCamera() {
	glWidget->camera.resetCamera();
	glWidget->updateCamera();
	glWidget->updateGL();
}

void MainWindow::onGenerateRoads() {
	urbanGeometry->generateRoads();
	glWidget->shadow.makeShadowMap(glWidget);
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

void MainWindow::onGenerateCity() {
	std::cout << "Generating a city...";

	urbanGeometry->clear();

	// set the parameter values
	G::global()["avenueAvgSegmentLength"] = controlWidget->ui.lineEditAvenueSegmentLength->text().toFloat();
	G::global()["streetAvgSegmentLength"] = controlWidget->ui.lineEditStreetSegmentLength->text().toFloat();
	G::global()["road_curvature"] = controlWidget->ui.lineEditRoadCurvature->text().toFloat();
	G::global()["parksRatio"] = controlWidget->ui.lineEditParkRatio->text().toFloat();
	G::global()["parcel_area_mean"] = controlWidget->ui.lineEditParcelArea->text().toFloat();
	G::global()["parcel_setback_front"] = controlWidget->ui.lineEditSetbackFront->text().toFloat();
	G::global()["parcel_setback_rear"] = controlWidget->ui.lineEditSetbackRear->text().toFloat();
	G::global()["parcel_setback_sides"] = controlWidget->ui.lineEditSetbackSide->text().toFloat();
	G::global()["building_stories_mean"] = Util::genRand(controlWidget->ui.lineEditNumStoriesMin->text().toInt(), controlWidget->ui.lineEditNumStoriesMax->text().toInt() + 1);

	urbanGeometry->generateAll();
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();

	std::cout << " Done." << std::endl;
}

void MainWindow::onGenerateScenarios() {
	ScenarioGenerationDialog dlg;
	if (!dlg.exec()) return;

	QString zone_file = dlg.ui.lineEditZone->text();
	QString terrain_file = dlg.ui.lineEditTerrain->text();
	int numScenarios = dlg.ui.lineEditNumScenarios->text().toInt();
	QString output_dir = dlg.ui.lineEditOutputDirectory->text();
	std::pair<float, float> avenueSegmentLengthRange = std::make_pair(dlg.ui.lineEditAvenueSegmentLengthMin->text().toFloat(), dlg.ui.lineEditAvenueSegmentLengthMax->text().toFloat());
	std::pair<float, float> streetSegmentLengthRange = std::make_pair(dlg.ui.lineEditStreetSegmentLengthMin->text().toFloat(), dlg.ui.lineEditStreetSegmentLengthMax->text().toFloat());
	std::pair<float, float> roadCurvatureRange = std::make_pair(dlg.ui.lineEditRoadCurvatureMin->text().toFloat(), dlg.ui.lineEditRoadCurvatureMax->text().toFloat());
	std::pair<float, float> parkRatioRange = std::make_pair(dlg.ui.lineEditParkRatioMin->text().toFloat(), dlg.ui.lineEditParkRatioMax->text().toFloat());
	std::pair<float, float> pacelAreaRange = std::make_pair(dlg.ui.lineEditParcelAreaMin->text().toFloat(), dlg.ui.lineEditParcelAreaMax->text().toFloat());
	std::pair<float, float> setbackFrontRange = std::make_pair(dlg.ui.lineEditSetbackFrontMin->text().toFloat(), dlg.ui.lineEditSetbackFrontMax->text().toFloat());
	std::pair<float, float> setbackRearRange = std::make_pair(dlg.ui.lineEditSetbackRearMin->text().toFloat(), dlg.ui.lineEditSetbackRearMax->text().toFloat());
	std::pair<float, float> setbackSideRange = std::make_pair(dlg.ui.lineEditSetbackSideMin->text().toFloat(), dlg.ui.lineEditSetbackSideMax->text().toFloat());
	std::pair<float, float> numStoriesRange = std::make_pair(dlg.ui.lineEditNumStoriesMin->text().toFloat(), dlg.ui.lineEditNumStoriesMax->text().toFloat());

	// load zone
	urbanGeometry->loadZone(zone_file.toUtf8().constData());

	// load terrain
	urbanGeometry->loadTerrain(terrain_file.toUtf8().constData());

	// generate scenarios
	urbanGeometry->generateScenarios(numScenarios, output_dir, avenueSegmentLengthRange, streetSegmentLengthRange, roadCurvatureRange, parkRatioRange, pacelAreaRange, setbackFrontRange, setbackRearRange, setbackSideRange, numStoriesRange);
}

void MainWindow::onViewChanged() {
	G::global()["shader2D"] = ui.actionView2D->isChecked();

	int terrainMode;
	if (ui.actionView2D->isChecked()) {
		terrainMode = 0;
	}
	else  {
		terrainMode = 1;
	}

	glWidget->vboRenderManager.changeTerrainShader(terrainMode);
	urbanGeometry->update(glWidget->vboRenderManager);
	glWidget->shadow.makeShadowMap(glWidget);
	glWidget->updateGL();
}

void MainWindow::onShowControlWidget() {
	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}

