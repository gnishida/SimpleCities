#include "ControlWidget.h"
#include "MainWindow.h"
#include "UrbanGeometry.h"
#include "GLWidget3D.h"
#include "global.h"

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);

	ui.lineEditAvenueSegmentLength->setText("100");
	ui.lineEditStreetSegmentLength->setText("20");
	ui.lineEditRoadCurvature->setText("0.2");
	ui.lineEditParkRatio->setText("0.1");
	ui.lineEditParcelArea->setText("1000");
	ui.lineEditSetbackFront->setText("5");
	ui.lineEditSetbackRear->setText("5");
	ui.lineEditSetbackSide->setText("5");
	ui.lineEditNumStoriesMin->setText("5");
	ui.lineEditNumStoriesMax->setText("5");
	
	connect(ui.pushButtonGenerateAll, SIGNAL(clicked()), mainWin, SLOT(onGenerateCity()));
	
	hide();	
}

