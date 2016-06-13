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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QTabWidget *TabWidget;
    QWidget *ContentEditionTab;
    QGroupBox *groupBox_6;
    QLabel *terrainPaint_sizeLabel;
    QLabel *terrainPaint_changeLabel;
    QSlider *terrainPaint_sizeSlider;
    QSlider *terrainPaint_changeSlider;
    QCheckBox *terrain_2DShader;
    QSlider *render_2DroadsExtraWidthSlider;
    QLabel *render_2DroadsExtraWidthLabel;
    QLabel *render_2DparksLabel;
    QSlider *render_2DparksSlider;
    QCheckBox *render_3DtreesCheckBox;
    QLabel *render_2DparcelLineLabel;
    QSlider *render_2DparcelLineSlider;
    QLineEdit *lineEditMaxBlockSizeForPark;
    QLabel *label_15;
    QLineEdit *terrainPaint_changeEdit;
    QLabel *terrainPaint_changeLabel_2;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QStringLiteral("ControlWidget"));
        ControlWidget->resize(198, 621);
        ControlWidget->setMinimumSize(QSize(190, 240));
        ControlWidget->setStyleSheet(QStringLiteral("background-color: rgb(181, 181, 181);"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        TabWidget = new QTabWidget(dockWidgetContents);
        TabWidget->setObjectName(QStringLiteral("TabWidget"));
        TabWidget->setGeometry(QRect(1, 0, 196, 581));
        ContentEditionTab = new QWidget();
        ContentEditionTab->setObjectName(QStringLiteral("ContentEditionTab"));
        groupBox_6 = new QGroupBox(ContentEditionTab);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 20, 171, 311));
        terrainPaint_sizeLabel = new QLabel(groupBox_6);
        terrainPaint_sizeLabel->setObjectName(QStringLiteral("terrainPaint_sizeLabel"));
        terrainPaint_sizeLabel->setGeometry(QRect(10, 20, 61, 16));
        terrainPaint_changeLabel = new QLabel(groupBox_6);
        terrainPaint_changeLabel->setObjectName(QStringLiteral("terrainPaint_changeLabel"));
        terrainPaint_changeLabel->setGeometry(QRect(10, 40, 41, 16));
        terrainPaint_sizeSlider = new QSlider(groupBox_6);
        terrainPaint_sizeSlider->setObjectName(QStringLiteral("terrainPaint_sizeSlider"));
        terrainPaint_sizeSlider->setGeometry(QRect(80, 20, 81, 15));
        terrainPaint_sizeSlider->setMaximumSize(QSize(16777215, 17));
        terrainPaint_sizeSlider->setMinimum(1);
        terrainPaint_sizeSlider->setMaximum(1000);
        terrainPaint_sizeSlider->setValue(5);
        terrainPaint_sizeSlider->setOrientation(Qt::Horizontal);
        terrainPaint_changeSlider = new QSlider(groupBox_6);
        terrainPaint_changeSlider->setObjectName(QStringLiteral("terrainPaint_changeSlider"));
        terrainPaint_changeSlider->setGeometry(QRect(100, 40, 61, 17));
        terrainPaint_changeSlider->setMaximumSize(QSize(16777215, 17));
        terrainPaint_changeSlider->setMinimum(1);
        terrainPaint_changeSlider->setValue(10);
        terrainPaint_changeSlider->setOrientation(Qt::Horizontal);
        terrain_2DShader = new QCheckBox(groupBox_6);
        terrain_2DShader->setObjectName(QStringLiteral("terrain_2DShader"));
        terrain_2DShader->setGeometry(QRect(10, 60, 111, 20));
        terrain_2DShader->setChecked(true);
        render_2DroadsExtraWidthSlider = new QSlider(groupBox_6);
        render_2DroadsExtraWidthSlider->setObjectName(QStringLiteral("render_2DroadsExtraWidthSlider"));
        render_2DroadsExtraWidthSlider->setGeometry(QRect(90, 80, 71, 17));
        render_2DroadsExtraWidthSlider->setMaximumSize(QSize(16777215, 17));
        render_2DroadsExtraWidthSlider->setMaximum(100);
        render_2DroadsExtraWidthSlider->setOrientation(Qt::Horizontal);
        render_2DroadsExtraWidthLabel = new QLabel(groupBox_6);
        render_2DroadsExtraWidthLabel->setObjectName(QStringLiteral("render_2DroadsExtraWidthLabel"));
        render_2DroadsExtraWidthLabel->setGeometry(QRect(10, 80, 71, 16));
        render_2DparksLabel = new QLabel(groupBox_6);
        render_2DparksLabel->setObjectName(QStringLiteral("render_2DparksLabel"));
        render_2DparksLabel->setGeometry(QRect(10, 130, 61, 16));
        render_2DparksSlider = new QSlider(groupBox_6);
        render_2DparksSlider->setObjectName(QStringLiteral("render_2DparksSlider"));
        render_2DparksSlider->setGeometry(QRect(80, 130, 81, 17));
        render_2DparksSlider->setMaximumSize(QSize(16777215, 17));
        render_2DparksSlider->setValue(10);
        render_2DparksSlider->setOrientation(Qt::Horizontal);
        render_3DtreesCheckBox = new QCheckBox(groupBox_6);
        render_3DtreesCheckBox->setObjectName(QStringLiteral("render_3DtreesCheckBox"));
        render_3DtreesCheckBox->setGeometry(QRect(90, 60, 71, 20));
        render_3DtreesCheckBox->setChecked(true);
        render_2DparcelLineLabel = new QLabel(groupBox_6);
        render_2DparcelLineLabel->setObjectName(QStringLiteral("render_2DparcelLineLabel"));
        render_2DparcelLineLabel->setGeometry(QRect(10, 170, 81, 16));
        render_2DparcelLineSlider = new QSlider(groupBox_6);
        render_2DparcelLineSlider->setObjectName(QStringLiteral("render_2DparcelLineSlider"));
        render_2DparcelLineSlider->setGeometry(QRect(100, 170, 61, 17));
        render_2DparcelLineSlider->setMaximumSize(QSize(16777215, 17));
        render_2DparcelLineSlider->setMaximum(100);
        render_2DparcelLineSlider->setValue(20);
        render_2DparcelLineSlider->setOrientation(Qt::Horizontal);
        lineEditMaxBlockSizeForPark = new QLineEdit(groupBox_6);
        lineEditMaxBlockSizeForPark->setObjectName(QStringLiteral("lineEditMaxBlockSizeForPark"));
        lineEditMaxBlockSizeForPark->setGeometry(QRect(82, 150, 81, 20));
        label_15 = new QLabel(groupBox_6);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(50, 150, 31, 21));
        terrainPaint_changeEdit = new QLineEdit(groupBox_6);
        terrainPaint_changeEdit->setObjectName(QStringLiteral("terrainPaint_changeEdit"));
        terrainPaint_changeEdit->setGeometry(QRect(55, 38, 31, 20));
        terrainPaint_changeLabel_2 = new QLabel(groupBox_6);
        terrainPaint_changeLabel_2->setObjectName(QStringLiteral("terrainPaint_changeLabel_2"));
        terrainPaint_changeLabel_2->setGeometry(QRect(88, 40, 16, 16));
        TabWidget->addTab(ContentEditionTab, QString());
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        TabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        groupBox_6->setTitle(QApplication::translate("ControlWidget", "Terrain Controls/Render", 0));
        terrainPaint_sizeLabel->setText(QApplication::translate("ControlWidget", "Size: 5%", 0));
        terrainPaint_changeLabel->setText(QApplication::translate("ControlWidget", "Change", 0));
        terrain_2DShader->setText(QApplication::translate("ControlWidget", "2D Shader", 0));
        render_2DroadsExtraWidthLabel->setText(QApplication::translate("ControlWidget", "R Width: 0.0", 0));
        render_2DparksLabel->setText(QApplication::translate("ControlWidget", "Park 10 %", 0));
        render_3DtreesCheckBox->setText(QApplication::translate("ControlWidget", "3D Trees", 0));
        render_2DparcelLineLabel->setText(QApplication::translate("ControlWidget", "Par. Line: 2.0", 0));
        label_15->setText(QApplication::translate("ControlWidget", "Size:", 0));
        terrainPaint_changeLabel_2->setText(QApplication::translate("ControlWidget", "m", 0));
        TabWidget->setTabText(TabWidget->indexOf(ContentEditionTab), QApplication::translate("ControlWidget", "Content", 0));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
