#include "ControlWidget.h"
#include <QFileDialog>
#include "MainWindow.h"
#include "UrbanGeometry.h"
#include "GLWidget3D.h"
#include "global.h"
#include "GraphUtil.h"
#include "BBox.h"

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);

	ui.TabWidget->setCurrentIndex(0);
	
	ui.terrainPaint_changeEdit->setText("50");
	ui.terrainPaint_changeSlider->setMinimum(0);
	ui.terrainPaint_changeSlider->setMaximum(200);
	ui.terrainPaint_changeSlider->setValue(50);
	ui.terrainPaint_sizeSlider->setMinimum(1);
	ui.terrainPaint_sizeSlider->setMaximum(400);
	ui.terrainPaint_sizeSlider->setValue(120);
	
	// terrain
	connect(ui.terrainPaint_sizeSlider, SIGNAL(valueChanged(int)),this, SLOT(updateTerrainLabels(int)));
	connect(ui.terrainPaint_changeEdit, SIGNAL(textChanged(const QString &)),this, SLOT(updateTerrainEdit(const QString &)));
	connect(ui.terrainPaint_changeSlider, SIGNAL(valueChanged(int)),this, SLOT(updateTerrainLabels(int)));
	connect(ui.terrain_2DShader, SIGNAL(stateChanged(int)),this, SLOT(changeTerrainShader(int)));
	
	updateTerrainLabels(-1);

	hide();	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

void ControlWidget::generateRoadsPM() {
	/*
	if (mainWin->urbanGeometry->areas.selectedIndex == -1) return;

	G::global()["seaLevel"] = 60.0f;
	G::global()["numAvenueIterations"] = ui.lineEditNumAvenueIterations->text().toInt();
	G::global()["numStreetIterations"] = ui.lineEditNumStreetIterations->text().toInt();
	int numExamples = ui.lineEditNumExamples->text().toInt();
	G::global()["cleanAvenues"] = ui.checkBoxCleanAvenues->isChecked();
	G::global()["cleanStreets"] = ui.checkBoxCleanStreets->isChecked();
	G::global()["generateLocalStreets"] = ui.checkBoxLocalStreets->isChecked();
	G::global()["cropping"] = ui.checkBoxCropping->isChecked();
	G::global()["useLayer"] = ui.checkBoxUseLayer->isChecked();

	G::global()["houghScale"] = ui.lineEditHoughScale->text().toFloat();
	G::global()["avenuePatchDistance"] = ui.lineEditPatchDistance1->text().toFloat();
	G::global()["streetPatchDistance"] = ui.lineEditPatchDistance2->text().toFloat();
	G::global()["interpolationSigma1"] = ui.lineEditInterpolateSigma1->text().toFloat();
	G::global()["interpolationSigma2"] = ui.lineEditInterpolateSigma2->text().toFloat();
	G::global()["rotationAngle"] = ui.lineEditRotationAngle->text().toFloat() / 180.0f * M_PI;
	G::global()["roadAngleTolerance"] = ui.lineEditRoadAngleTolerance->text().toFloat() / 180.0f * M_PI;
	G::global()["acrossRiverTolerance"] = ui.lineEditAcrossRiverTolerance->text().toFloat();
	G::global()["slopeTolerance"] = ui.lineEditSlopeTolerance->text().toFloat() / 180.0f * M_PI;
	G::global()["saveRoadImages"] = ui.checkBoxSaveRoadImages->isChecked();
	G::global()["savePatchImages"] = ui.checkBoxSavePatchImages->isChecked();

	std::vector<ExFeature> features;
	features.resize(numExamples);
	for (int i = 0; i < numExamples; ++i) {
		QString filename = QFileDialog::getOpenFileName(this, tr("Open Feature file..."), "", tr("StreetMap Files (*.xml)"));
		if (filename.isEmpty()) return;
	
		features[i].load(filename, true);
	}

	mainWin->urbanGeometry->generateRoadsPM(features);
	
	mainWin->glWidget->updateGL();
	*/
}

void ControlWidget::clear() {

	mainWin->glWidget->updateGL();
}

void ControlWidget::updateTerrainLabels(int newValue) {
	int size = ui.terrainPaint_sizeSlider->value();
	ui.terrainPaint_sizeLabel->setText("Size: "+QString::number(size,'f',0)+"m");
	G::global()["2DterrainEditSize"] = size;

	float change = ui.terrainPaint_changeSlider->value();
	ui.terrainPaint_changeEdit->setText(QString::number(change, 'f', 0));
}

void ControlWidget::updateTerrainEdit(const QString &text) {
	ui.terrainPaint_changeSlider->setValue(ui.terrainPaint_changeEdit->text().toFloat());
}

void ControlWidget::changeTerrainShader(int) {
	bool shader2D = ui.terrain_2DShader->isChecked();
	G::global()["shader2D"] = shader2D;
	int terrainMode;
	if (shader2D) {
		terrainMode = 0;
	} else  {
		terrainMode=1;
	}

	mainWin->glWidget->vboRenderManager.changeTerrainShader(terrainMode);
	mainWin->urbanGeometry->update(mainWin->glWidget->vboRenderManager);
	mainWin->glWidget->shadow.makeShadowMap(mainWin->glWidget);
	mainWin->glWidget->updateGL();
}


