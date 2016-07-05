/********************************************************************************
** Form generated from reading UI file 'ScenarioGenerationDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENARIOGENERATIONDIALOG_H
#define UI_SCENARIOGENERATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_ScenarioGenerationDialog
{
public:
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QLabel *label;
    QLineEdit *lineEditNumScenarios;
    QLabel *label_2;
    QLineEdit *lineEditOutputDirectory;
    QPushButton *pushButtonOutputDirectory;
    QLabel *label_13;
    QLabel *label_14;
    QLineEdit *lineEditZone;
    QLineEdit *lineEditTerrain;
    QPushButton *pushButtonZone;
    QPushButton *pushButtonTerrain;
    QGroupBox *groupBox;
    QLabel *label_9;
    QLineEdit *lineEditAvenueSegmentLengthMin;
    QLineEdit *lineEditRoadCurvatureMin;
    QLineEdit *lineEditStreetSegmentLengthMax;
    QLabel *label_6;
    QLineEdit *lineEditRoadCurvatureMax;
    QLabel *label_3;
    QLineEdit *lineEditAvenueSegmentLengthMax;
    QLineEdit *lineEditStreetSegmentLengthMin;
    QLabel *label_4;
    QLabel *label_7;
    QLabel *label_10;
    QLabel *label_5;
    QLabel *label_8;
    QGroupBox *groupBox_2;
    QLabel *label_11;
    QLineEdit *lineEditParkRatioMin;
    QLineEdit *lineEditParkRatioMax;
    QLabel *label_12;
    QLabel *label_15;
    QLineEdit *lineEditParcelAreaMin;
    QLineEdit *lineEditParcelAreaMax;
    QLabel *label_16;
    QLabel *label_17;
    QLineEdit *lineEditSetbackFrontMin;
    QLineEdit *lineEditSetbackFrontMax;
    QLabel *label_18;
    QLabel *label_19;
    QLineEdit *lineEditSetbackRearMin;
    QLineEdit *lineEditSetbackRearMax;
    QLabel *label_20;
    QLabel *label_21;
    QLineEdit *lineEditSetbackSideMin;
    QLineEdit *lineEditSetbackSideMax;
    QLabel *label_22;
    QGroupBox *groupBox_3;
    QLabel *label_33;
    QLineEdit *lineEditNumStoriesMin;
    QLineEdit *lineEditNumStoriesMax;
    QLabel *label_34;

    void setupUi(QDialog *ScenarioGenerationDialog)
    {
        if (ScenarioGenerationDialog->objectName().isEmpty())
            ScenarioGenerationDialog->setObjectName(QStringLiteral("ScenarioGenerationDialog"));
        ScenarioGenerationDialog->resize(544, 544);
        pushButtonOK = new QPushButton(ScenarioGenerationDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(330, 500, 91, 31));
        pushButtonCancel = new QPushButton(ScenarioGenerationDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(440, 500, 91, 31));
        label = new QLabel(ScenarioGenerationDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 70, 61, 21));
        lineEditNumScenarios = new QLineEdit(ScenarioGenerationDialog);
        lineEditNumScenarios->setObjectName(QStringLiteral("lineEditNumScenarios"));
        lineEditNumScenarios->setGeometry(QRect(130, 70, 71, 20));
        label_2 = new QLabel(ScenarioGenerationDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 100, 91, 21));
        lineEditOutputDirectory = new QLineEdit(ScenarioGenerationDialog);
        lineEditOutputDirectory->setObjectName(QStringLiteral("lineEditOutputDirectory"));
        lineEditOutputDirectory->setGeometry(QRect(130, 100, 361, 20));
        pushButtonOutputDirectory = new QPushButton(ScenarioGenerationDialog);
        pushButtonOutputDirectory->setObjectName(QStringLiteral("pushButtonOutputDirectory"));
        pushButtonOutputDirectory->setGeometry(QRect(500, 100, 31, 23));
        label_13 = new QLabel(ScenarioGenerationDialog);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(20, 10, 91, 21));
        label_14 = new QLabel(ScenarioGenerationDialog);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(20, 40, 91, 21));
        lineEditZone = new QLineEdit(ScenarioGenerationDialog);
        lineEditZone->setObjectName(QStringLiteral("lineEditZone"));
        lineEditZone->setGeometry(QRect(130, 10, 361, 20));
        lineEditTerrain = new QLineEdit(ScenarioGenerationDialog);
        lineEditTerrain->setObjectName(QStringLiteral("lineEditTerrain"));
        lineEditTerrain->setGeometry(QRect(130, 40, 361, 20));
        pushButtonZone = new QPushButton(ScenarioGenerationDialog);
        pushButtonZone->setObjectName(QStringLiteral("pushButtonZone"));
        pushButtonZone->setGeometry(QRect(500, 10, 31, 23));
        pushButtonTerrain = new QPushButton(ScenarioGenerationDialog);
        pushButtonTerrain->setObjectName(QStringLiteral("pushButtonTerrain"));
        pushButtonTerrain->setGeometry(QRect(500, 40, 31, 23));
        groupBox = new QGroupBox(ScenarioGenerationDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 130, 521, 111));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(20, 80, 121, 21));
        lineEditAvenueSegmentLengthMin = new QLineEdit(groupBox);
        lineEditAvenueSegmentLengthMin->setObjectName(QStringLiteral("lineEditAvenueSegmentLengthMin"));
        lineEditAvenueSegmentLengthMin->setGeometry(QRect(150, 20, 71, 20));
        lineEditRoadCurvatureMin = new QLineEdit(groupBox);
        lineEditRoadCurvatureMin->setObjectName(QStringLiteral("lineEditRoadCurvatureMin"));
        lineEditRoadCurvatureMin->setGeometry(QRect(150, 80, 71, 20));
        lineEditStreetSegmentLengthMax = new QLineEdit(groupBox);
        lineEditStreetSegmentLengthMax->setObjectName(QStringLiteral("lineEditStreetSegmentLengthMax"));
        lineEditStreetSegmentLengthMax->setGeometry(QRect(240, 50, 71, 20));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 50, 121, 21));
        lineEditRoadCurvatureMax = new QLineEdit(groupBox);
        lineEditRoadCurvatureMax->setObjectName(QStringLiteral("lineEditRoadCurvatureMax"));
        lineEditRoadCurvatureMax->setGeometry(QRect(240, 80, 71, 20));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 20, 121, 21));
        lineEditAvenueSegmentLengthMax = new QLineEdit(groupBox);
        lineEditAvenueSegmentLengthMax->setObjectName(QStringLiteral("lineEditAvenueSegmentLengthMax"));
        lineEditAvenueSegmentLengthMax->setGeometry(QRect(240, 20, 71, 20));
        lineEditStreetSegmentLengthMin = new QLineEdit(groupBox);
        lineEditStreetSegmentLengthMin->setObjectName(QStringLiteral("lineEditStreetSegmentLengthMin"));
        lineEditStreetSegmentLengthMin->setGeometry(QRect(150, 50, 71, 20));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(226, 20, 16, 21));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(226, 50, 16, 21));
        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(226, 80, 16, 21));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(320, 20, 16, 21));
        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(320, 50, 16, 21));
        groupBox_2 = new QGroupBox(ScenarioGenerationDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 250, 521, 171));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(20, 20, 121, 21));
        lineEditParkRatioMin = new QLineEdit(groupBox_2);
        lineEditParkRatioMin->setObjectName(QStringLiteral("lineEditParkRatioMin"));
        lineEditParkRatioMin->setGeometry(QRect(150, 20, 71, 20));
        lineEditParkRatioMax = new QLineEdit(groupBox_2);
        lineEditParkRatioMax->setObjectName(QStringLiteral("lineEditParkRatioMax"));
        lineEditParkRatioMax->setGeometry(QRect(240, 20, 71, 20));
        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(226, 20, 16, 21));
        label_15 = new QLabel(groupBox_2);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(20, 50, 121, 21));
        lineEditParcelAreaMin = new QLineEdit(groupBox_2);
        lineEditParcelAreaMin->setObjectName(QStringLiteral("lineEditParcelAreaMin"));
        lineEditParcelAreaMin->setGeometry(QRect(150, 50, 71, 20));
        lineEditParcelAreaMax = new QLineEdit(groupBox_2);
        lineEditParcelAreaMax->setObjectName(QStringLiteral("lineEditParcelAreaMax"));
        lineEditParcelAreaMax->setGeometry(QRect(240, 50, 71, 20));
        label_16 = new QLabel(groupBox_2);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(226, 50, 16, 21));
        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(20, 80, 121, 21));
        lineEditSetbackFrontMin = new QLineEdit(groupBox_2);
        lineEditSetbackFrontMin->setObjectName(QStringLiteral("lineEditSetbackFrontMin"));
        lineEditSetbackFrontMin->setGeometry(QRect(150, 80, 71, 20));
        lineEditSetbackFrontMax = new QLineEdit(groupBox_2);
        lineEditSetbackFrontMax->setObjectName(QStringLiteral("lineEditSetbackFrontMax"));
        lineEditSetbackFrontMax->setGeometry(QRect(240, 80, 71, 20));
        label_18 = new QLabel(groupBox_2);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(226, 80, 16, 21));
        label_19 = new QLabel(groupBox_2);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(20, 110, 121, 21));
        lineEditSetbackRearMin = new QLineEdit(groupBox_2);
        lineEditSetbackRearMin->setObjectName(QStringLiteral("lineEditSetbackRearMin"));
        lineEditSetbackRearMin->setGeometry(QRect(150, 110, 71, 20));
        lineEditSetbackRearMax = new QLineEdit(groupBox_2);
        lineEditSetbackRearMax->setObjectName(QStringLiteral("lineEditSetbackRearMax"));
        lineEditSetbackRearMax->setGeometry(QRect(240, 110, 71, 20));
        label_20 = new QLabel(groupBox_2);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(226, 110, 16, 21));
        label_21 = new QLabel(groupBox_2);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(20, 140, 121, 21));
        lineEditSetbackSideMin = new QLineEdit(groupBox_2);
        lineEditSetbackSideMin->setObjectName(QStringLiteral("lineEditSetbackSideMin"));
        lineEditSetbackSideMin->setGeometry(QRect(150, 140, 71, 20));
        lineEditSetbackSideMax = new QLineEdit(groupBox_2);
        lineEditSetbackSideMax->setObjectName(QStringLiteral("lineEditSetbackSideMax"));
        lineEditSetbackSideMax->setGeometry(QRect(240, 140, 71, 20));
        label_22 = new QLabel(groupBox_2);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(226, 140, 16, 21));
        groupBox_3 = new QGroupBox(ScenarioGenerationDialog);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 430, 521, 51));
        label_33 = new QLabel(groupBox_3);
        label_33->setObjectName(QStringLiteral("label_33"));
        label_33->setGeometry(QRect(20, 20, 121, 21));
        lineEditNumStoriesMin = new QLineEdit(groupBox_3);
        lineEditNumStoriesMin->setObjectName(QStringLiteral("lineEditNumStoriesMin"));
        lineEditNumStoriesMin->setGeometry(QRect(150, 20, 71, 20));
        lineEditNumStoriesMax = new QLineEdit(groupBox_3);
        lineEditNumStoriesMax->setObjectName(QStringLiteral("lineEditNumStoriesMax"));
        lineEditNumStoriesMax->setGeometry(QRect(240, 20, 71, 20));
        label_34 = new QLabel(groupBox_3);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setGeometry(QRect(226, 20, 16, 21));

        retranslateUi(ScenarioGenerationDialog);

        QMetaObject::connectSlotsByName(ScenarioGenerationDialog);
    } // setupUi

    void retranslateUi(QDialog *ScenarioGenerationDialog)
    {
        ScenarioGenerationDialog->setWindowTitle(QApplication::translate("ScenarioGenerationDialog", "Scenario Generation Dialog", 0));
        pushButtonOK->setText(QApplication::translate("ScenarioGenerationDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("ScenarioGenerationDialog", "Cancel", 0));
        label->setText(QApplication::translate("ScenarioGenerationDialog", "# Scenarios:", 0));
        label_2->setText(QApplication::translate("ScenarioGenerationDialog", "Output directory:", 0));
        pushButtonOutputDirectory->setText(QApplication::translate("ScenarioGenerationDialog", "...", 0));
        label_13->setText(QApplication::translate("ScenarioGenerationDialog", "Zone:", 0));
        label_14->setText(QApplication::translate("ScenarioGenerationDialog", "Terrain:", 0));
        pushButtonZone->setText(QApplication::translate("ScenarioGenerationDialog", "...", 0));
        pushButtonTerrain->setText(QApplication::translate("ScenarioGenerationDialog", "...", 0));
        groupBox->setTitle(QApplication::translate("ScenarioGenerationDialog", "Roads", 0));
        label_9->setText(QApplication::translate("ScenarioGenerationDialog", "Road curvature:", 0));
        label_6->setText(QApplication::translate("ScenarioGenerationDialog", "Street segment length:", 0));
        label_3->setText(QApplication::translate("ScenarioGenerationDialog", "Avenue segment length:", 0));
        label_4->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
        label_7->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
        label_10->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
        label_5->setText(QApplication::translate("ScenarioGenerationDialog", "[m]", 0));
        label_8->setText(QApplication::translate("ScenarioGenerationDialog", "[m]", 0));
        groupBox_2->setTitle(QApplication::translate("ScenarioGenerationDialog", "Parcels", 0));
        label_11->setText(QApplication::translate("ScenarioGenerationDialog", "Park ratio:", 0));
        label_12->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
        label_15->setText(QApplication::translate("ScenarioGenerationDialog", "Parcel area:", 0));
        label_16->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
        label_17->setText(QApplication::translate("ScenarioGenerationDialog", "Setback front:", 0));
        label_18->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
        label_19->setText(QApplication::translate("ScenarioGenerationDialog", "Setback rear:", 0));
        label_20->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
        label_21->setText(QApplication::translate("ScenarioGenerationDialog", "Setback side:", 0));
        label_22->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
        groupBox_3->setTitle(QApplication::translate("ScenarioGenerationDialog", "Buildings", 0));
        label_33->setText(QApplication::translate("ScenarioGenerationDialog", "# Stories", 0));
        label_34->setText(QApplication::translate("ScenarioGenerationDialog", "~", 0));
    } // retranslateUi

};

namespace Ui {
    class ScenarioGenerationDialog: public Ui_ScenarioGenerationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENARIOGENERATIONDIALOG_H
