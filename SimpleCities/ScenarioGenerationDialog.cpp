#include "ScenarioGenerationDialog.h"
#include <QFileDialog>

ScenarioGenerationDialog::ScenarioGenerationDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	//ui.lineEditZone->setText("../3D/zone1.shp");
	ui.lineEditZone->setText("../3D/new_zone.shp");
	ui.lineEditTerrain->setText("../3D/dem1m_calb.tif");
	ui.lineEditNumScenarios->setText("10");
	ui.lineEditOutputDirectory->setText("scenarios");
	ui.lineEditAvenueSegmentLengthMin->setText("50");
	ui.lineEditAvenueSegmentLengthMax->setText("200");
	ui.lineEditRoadBaseOrientationMin->setText("0");
	ui.lineEditRoadBaseOrientationMax->setText("180");
	ui.lineEditRoadCurvatureMin->setText("0.0");
	ui.lineEditRoadCurvatureMax->setText("0.5");
	ui.lineEditParkRatioMin->setText("0.1");
	ui.lineEditParkRatioMax->setText("0.1");
	ui.lineEditParcelAreaMin->setText("1000");
	ui.lineEditParcelAreaMax->setText("1000");
	ui.lineEditParcelAreaDev->setText("200");
	ui.lineEditSetbackFrontMin->setText("5.0");
	ui.lineEditSetbackFrontMax->setText("5.0");
	ui.lineEditSetbackRearMin->setText("5.0");
	ui.lineEditSetbackRearMax->setText("5.0");
	ui.lineEditSetbackSideMin->setText("5.0");
	ui.lineEditSetbackSideMax->setText("5.0");
	ui.lineEditNumStoriesMin->setText("5");
	ui.lineEditNumStoriesMax->setText("5");
	ui.lineEditNumStoriesDev->setText("2");
	ui.lineEditMinBuildingDimMin->setText("10.0");
	ui.lineEditMinBuildingDimMax->setText("10.0");

	connect(ui.pushButtonZone, SIGNAL(clicked()), this, SLOT(onZone()));
	connect(ui.pushButtonTerrain, SIGNAL(clicked()), this, SLOT(onTerrain()));
	connect(ui.pushButtonOutputDirectory, SIGNAL(clicked()), this, SLOT(onOutputDirectory()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

void ScenarioGenerationDialog::onZone() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load zone..."), ui.lineEditZone->text(), tr("Shapefiles (*.shp)"));
	if (!filename.isEmpty()) {
		ui.lineEditZone->setText(filename);
	}
}

void ScenarioGenerationDialog::onTerrain() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load DEM file..."), ui.lineEditTerrain->text(), tr("DEM Files (*.tif)"));
	if (!filename.isEmpty()) {
		ui.lineEditTerrain->setText(filename);
	}
}

void ScenarioGenerationDialog::onOutputDirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditOutputDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditOutputDirectory->setText(dir);
	}
}

void ScenarioGenerationDialog::onOK() {
	accept();
}

void ScenarioGenerationDialog::onCancel() {
	reject();
}
