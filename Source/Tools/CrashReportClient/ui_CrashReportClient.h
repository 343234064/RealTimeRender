/********************************************************************************
** Form generated from reading UI file 'CrashReportClient.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRASHREPORTCLIENT_H
#define UI_CRASHREPORTCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CrashReportClientClass
{
public:
    QLabel *BasicLabel;
    QTextBrowser *TextBrowser;

    void setupUi(QWidget *CrashReportClientClass)
    {
        if (CrashReportClientClass->objectName().isEmpty())
            CrashReportClientClass->setObjectName(QString::fromUtf8("CrashReportClientClass"));
        CrashReportClientClass->resize(600, 630);
        BasicLabel = new QLabel(CrashReportClientClass);
        BasicLabel->setObjectName(QString::fromUtf8("BasicLabel"));
        BasicLabel->setGeometry(QRect(20, 20, 311, 121));
        TextBrowser = new QTextBrowser(CrashReportClientClass);
        TextBrowser->setObjectName(QString::fromUtf8("TextBrowser"));
        TextBrowser->setGeometry(QRect(20, 160, 561, 451));

        retranslateUi(CrashReportClientClass);

        QMetaObject::connectSlotsByName(CrashReportClientClass);
    } // setupUi

    void retranslateUi(QWidget *CrashReportClientClass)
    {
        CrashReportClientClass->setWindowTitle(QCoreApplication::translate("CrashReportClientClass", "CrashReportClient", nullptr));
        BasicLabel->setText(QCoreApplication::translate("CrashReportClientClass", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CrashReportClientClass: public Ui_CrashReportClientClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRASHREPORTCLIENT_H
