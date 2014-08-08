/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Aug 8 05:30:38 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionAbout;
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *sample_frequency_edit;
    QLineEdit *sample_prob_edit;
    QLineEdit *sample_vol_edit;
    QPushButton *save_sample_button;
    QLineEdit *sample_path_edit;
    QListView *listView;
    QPushButton *pushButton;
    QPushButton *add_sample_button;
    QPushButton *remove_sample_button;
    QMenuBar *menubar;
    QMenu *menuPGTA_Editor;
    QMenu *menuFile;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(754, 501);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 40, 351, 241));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(5);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 61, 31));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 50, 81, 31));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 80, 81, 31));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 110, 131, 31));
        sample_frequency_edit = new QLineEdit(groupBox);
        sample_frequency_edit->setObjectName(QString::fromUtf8("sample_frequency_edit"));
        sample_frequency_edit->setGeometry(QRect(180, 50, 141, 31));
        sample_prob_edit = new QLineEdit(groupBox);
        sample_prob_edit->setObjectName(QString::fromUtf8("sample_prob_edit"));
        sample_prob_edit->setGeometry(QRect(180, 80, 141, 31));
        sample_vol_edit = new QLineEdit(groupBox);
        sample_vol_edit->setObjectName(QString::fromUtf8("sample_vol_edit"));
        sample_vol_edit->setGeometry(QRect(180, 110, 141, 31));
        save_sample_button = new QPushButton(groupBox);
        save_sample_button->setObjectName(QString::fromUtf8("save_sample_button"));
        save_sample_button->setGeometry(QRect(120, 170, 98, 27));
        sample_path_edit = new QLineEdit(groupBox);
        sample_path_edit->setObjectName(QString::fromUtf8("sample_path_edit"));
        sample_path_edit->setGeometry(QRect(180, 20, 141, 31));
        listView = new QListView(centralwidget);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setEnabled(true);
        listView->setGeometry(QRect(460, 40, 256, 192));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(listView->sizePolicy().hasHeightForWidth());
        listView->setSizePolicy(sizePolicy1);
        listView->setMaximumSize(QSize(16777215, 281));
        listView->setSelectionMode(QAbstractItemView::SingleSelection);
        listView->setSelectionBehavior(QAbstractItemView::SelectItems);
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(190, 410, 360, 27));
        add_sample_button = new QPushButton(centralwidget);
        add_sample_button->setObjectName(QString::fromUtf8("add_sample_button"));
        add_sample_button->setGeometry(QRect(460, 270, 98, 27));
        remove_sample_button = new QPushButton(centralwidget);
        remove_sample_button->setObjectName(QString::fromUtf8("remove_sample_button"));
        remove_sample_button->setGeometry(QRect(610, 270, 111, 27));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 754, 25));
        menuPGTA_Editor = new QMenu(menubar);
        menuPGTA_Editor->setObjectName(QString::fromUtf8("menuPGTA_Editor"));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuPGTA_Editor->menuAction());
        menubar->addAction(menuFile->menuAction());
        menuPGTA_Editor->addAction(actionAbout);
        menuFile->addAction(actionOpen);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Sample Properties", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "File Path:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Frequency:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Probability:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Volume Multiplier:", 0, QApplication::UnicodeUTF8));
        save_sample_button->setText(QApplication::translate("MainWindow", "Save Sample", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "Save Track File", 0, QApplication::UnicodeUTF8));
        add_sample_button->setText(QApplication::translate("MainWindow", "Add Sample", 0, QApplication::UnicodeUTF8));
        remove_sample_button->setText(QApplication::translate("MainWindow", "Remove Sample", 0, QApplication::UnicodeUTF8));
        menuPGTA_Editor->setTitle(QApplication::translate("MainWindow", "PGTA Editor", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
