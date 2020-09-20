#include "CrashReportCore.h"



int main(int argc, char* argv[])
{
    CrashReportCore Reporter(argc, argv);
    Reporter.Init();

    Reporter.Show();

    return Reporter.Exit();
}