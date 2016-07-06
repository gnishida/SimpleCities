/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLWIDGET_H
#define UI_CONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QGroupBox *groupBox;
    QLabel *label_3;
    QLineEdit *lineEditAvenueSegmentLength;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_9;
    QLineEdit *lineEditStreetSegmentLength;
    QLineEdit *lineEditRoadCurvature;
    QLabel *label_7;
    QGroupBox *groupBox_2;
    QLabel *label_11;
    QLineEdit *lineEditParkRatio;
    QLabel *label_15;
    QLabel *label_21;
    QLabel *label_17;
    QLabel *label_19;
    QLineEdit *lineEditSetbackRear;
    QLineEdit *lineEditParcelArea;
    QLineEdit *lineEditSetbackFront;
    QLineEdit *lineEditSetbackSide;
    QGroupBox *groupBox_3;
    QLineEdit *lineEditNumStoriesMin;
    QLabel *label_33;
    QLineEdit *lineEditNumStoriesMax;
    QLabel *label_34;
    QPushButton *pushButtonGenerateAll;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QStringLiteral("ControlWidget"));
        ControlWidget->resize(261, 474);
        ControlWidget->setMinimumSize(QSize(261, 474));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 241, 111));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 20, 121, 21));
        lineEditAvenueSegmentLength = new QLineEdit(groupBox);
        lineEditAvenueSegmentLength->setObjectName(QStringLiteral("lineEditAvenueSegmentLength"));
        lineEditAvenueSegmentLength->setGeometry(QRect(140, 20, 71, 20));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(220, 20, 16, 21));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 50, 121, 21));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 80, 121, 21));
        lineEditStreetSegmentLength = new QLineEdit(groupBox);
        lineEditStreetSegmentLength->setObjectName(QStringLiteral("lineEditStreetSegmentLength"));
        lineEditStreetSegmentLength->setGeometry(QRect(140, 50, 71, 20));
        lineEditRoadCurvature = new QLineEdit(groupBox);
        lineEditRoadCurvature->setObjectName(QStringLiteral("lineEditRoadCurvature"));
        lineEditRoadCurvature->setGeometry(QRect(140, 80, 71, 20));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(220, 50, 16, 21));
        groupBox_2 = new QGroupBox(dockWidgetContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 130, 241, 171));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 20, 121, 21));
        lineEditParkRatio = new QLineEdit(groupBox_2);
        lineEditParkRatio->setObjectName(QStringLiteral("lineEditParkRatio"));
        lineEditParkRatio->setGeometry(QRect(140, 20, 71, 20));
        label_15 = new QLabel(groupBox_2);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(10, 50, 121, 21));
        label_21 = new QLabel(groupBox_2);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(10, 140, 121, 21));
        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(10, 80, 121, 21));
        label_19 = new QLabel(groupBox_2);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(10, 110, 121, 21));
        lineEditSetbackRear = new QLineEdit(groupBox_2);
        lineEditSetbackRear->setObjectName(QStringLiteral("lineEditSetbackRear"));
        lineEditSetbackRear->setGeometry(QRect(140, 110, 71, 20));
        lineEditParcelArea = new QLineEdit(groupBox_2);
        lineEditParcelArea->setObjectName(QStringLiteral("lineEditParcelArea"));
        lineEditParcelArea->setGeometry(QRect(140, 50, 71, 20));
        lineEditSetbackFront = new QLineEdit(groupBox_2);
        lineEditSetbackFront->setObjectName(QStringLiteral("lineEditSetbackFront"));
        lineEditSetbackFront->setGeometry(QRect(140, 80, 71, 20));
        lineEditSetbackSide = new QLineEdit(groupBox_2);
        lineEditSetbackSide->setObjectName(QStringLiteral("lineEditSetbackSide"));
        lineEditSetbackSide->setGeometry(QRect(140, 140, 71, 20));
        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 310, 241, 81));
        lineEditNumStoriesMin = new QLineEdit(groupBox_3);
        lineEditNumStoriesMin->setObjectName(QStringLiteral("lineEditNumStoriesMin"));
        lineEditNumStoriesMin->setGeometry(QRect(140, 20, 71, 20));
        label_33 = new QLabel(groupBox_3);
        label_33->setObjectName(QStringLiteral("label_33"));
        label_33->setGeometry(QRect(10, 20, 121, 21));
        lineEditNumStoriesMax = new QLineEdit(groupBox_3);
        lineEditNumStoriesMax->setObjectName(QStringLiteral("lineEditNumStoriesMax"));
        lineEditNumStoriesMax->setGeometry(QRect(140, 50, 71, 20));
        label_34 = new QLabel(groupBox_3);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setGeometry(QRect(120, 50, 16, 21));
        pushButtonGenerateAll = new QPushButton(dockWidgetContents);
        pushButtonGenerateAll->setObjectName(QStringLiteral("pushButtonGenerateAll"));
        pushButtonGenerateAll->setGeometry(QRect(30, 410, 201, 31));
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        groupBox->setTitle(QApplication::translate("ControlWidget", "Roads", 0));
        label_3->setText(QApplication::translate("ControlWidget", "Avenue segment length:", 0));
        label_5->setText(QApplication::translate("ControlWidget", "[m]", 0));
        label_6->setText(QApplication::translate("ControlWidget", "Street segment length:", 0));
        label_9->setText(QApplication::translate("ControlWidget", "Road curvature:", 0));
        label_7->setText(QApplication::translate("ControlWidget", "[m]", 0));
        groupBox_2->setTitle(QApplication::translate("ControlWidget", "Parcels", 0));
        label_11->setText(QApplication::translate("ControlWidget", "Park ratio:", 0));
        label_15->setText(QApplication::translate("ControlWidget", "Parcel area:", 0));
        label_21->setText(QApplication::translate("ControlWidget", "Setback side:", 0));
        label_17->setText(QApplication::translate("ControlWidget", "Setback front:", 0));
        label_19->setText(QApplication::translate("ControlWidget", "Setback rear:", 0));
        groupBox_3->setTitle(QApplication::translate("ControlWidget", "Buildings", 0));
        label_33->setText(QApplication::translate("ControlWidget", "# Stories", 0));
        label_34->setText(QApplication::translate("ControlWidget", "~", 0));
        pushButtonGenerateAll->setText(QApplication::translate("ControlWidget", "Generate City", 0));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
