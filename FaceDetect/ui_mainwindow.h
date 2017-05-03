/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QLabel *m_lbShow;
    QSpacerItem *verticalSpacer;
    QPushButton *m_btnNext;
    QPushButton *m_btnExcute;
    QSpacerItem *verticalSpacer_3;
    QPushButton *m_btnExit;
    QSpacerItem *verticalSpacer_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(681, 481);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        m_lbShow = new QLabel(centralWidget);
        m_lbShow->setObjectName(QStringLiteral("m_lbShow"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_lbShow->sizePolicy().hasHeightForWidth());
        m_lbShow->setSizePolicy(sizePolicy);
        m_lbShow->setMaximumSize(QSize(640, 480));
        m_lbShow->setStyleSheet(QStringLiteral("background-color: rgb(201, 255, 160);"));
        m_lbShow->setScaledContents(true);

        gridLayout->addWidget(m_lbShow, 0, 0, 6, 1);

        verticalSpacer = new QSpacerItem(20, 108, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 0, 1, 1, 1);

        m_btnNext = new QPushButton(centralWidget);
        m_btnNext->setObjectName(QStringLiteral("m_btnNext"));

        gridLayout->addWidget(m_btnNext, 1, 1, 1, 1);

        m_btnExcute = new QPushButton(centralWidget);
        m_btnExcute->setObjectName(QStringLiteral("m_btnExcute"));

        gridLayout->addWidget(m_btnExcute, 2, 1, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 95, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 3, 1, 1, 1);

        m_btnExit = new QPushButton(centralWidget);
        m_btnExit->setObjectName(QStringLiteral("m_btnExit"));

        gridLayout->addWidget(m_btnExit, 4, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 95, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 5, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 681, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "FaceDetect", Q_NULLPTR));
        m_lbShow->setText(QString());
        m_btnNext->setText(QApplication::translate("MainWindow", "Next", Q_NULLPTR));
        m_btnExcute->setText(QApplication::translate("MainWindow", "Excute", Q_NULLPTR));
        m_btnExit->setText(QApplication::translate("MainWindow", "Exit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
