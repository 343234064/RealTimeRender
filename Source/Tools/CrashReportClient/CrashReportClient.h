#pragma once

#include <QtWidgets>
#include "ui_CrashReportClient.h"

class CrashReportClient : public QWidget
{
    Q_OBJECT

public:
    CrashReportClient(QWidget *parent = Q_NULLPTR);

private:
    Ui::CrashReportClientClass ui;
};
