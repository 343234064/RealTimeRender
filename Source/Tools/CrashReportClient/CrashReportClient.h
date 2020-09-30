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

	void SetBasicLabelText(const QString Text)
	{
		ui.BasicLabel->setText(Text);
	}

	void SetTextBrowser(const QString Text)
	{
		ui.TextBrowser->setText(Text);
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

	void SetBasicLabelText(const QString Text)
	{
		Client.SetBasicLabelText(Text);
	}

	void SetTextBrowser(const QString Text)
	{
		Client.SetTextBrowser(Text);
	}

private:
	QApplication Application;
	CrashReportWidget Client;
};
