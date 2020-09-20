#pragma once

#include <QtWidgets>
#include "ui_CrashReportClient.h"
#include <QtWidgets/QApplication>



class CrashReportWidget : public QWidget
{
    Q_OBJECT

public:
	CrashReportWidget()
		: QWidget(Q_NULLPTR)
	{
		ui.setupUi(this);
	}

	void Show()
	{
		this->show();
	}

private:
	
	Ui::CrashReportClientClass ui;
	
};


class CrashReportClient
{
public:
	CrashReportClient(int argc, char* argv[]) :
		Application(argc, argv)
	{}

	void Init()
	{}

	void Show()
	{
		Client.Show();
	}

	int Exit()
	{
		return Application.exec();
	}

private:
	QApplication Application;
	CrashReportWidget Client;
};
