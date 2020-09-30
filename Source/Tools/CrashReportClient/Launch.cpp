#include "CrashReportCore.h"



int main(int argc, char* argv[])
{
    CrashReportCore Reporter(argc, argv);
   
    Reporter.Run();
   
    return Reporter.Exit();
}