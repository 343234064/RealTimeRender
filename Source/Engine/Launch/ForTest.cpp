#include "Global/GlobalConfigs.h"

#if FOR_TEST_CPP

#include "Global/GlobalType.h"
#include "Global/Utilities/AtomicCounter.h"
#include "HAL/Platform.h"
#include "Global/Utilities/Misc.h"
#include "Global/Utilities/SmartPointer.h"
#include "Global/Utilities/ContainerAllocator.h"
#include "Global/Utilities/DynamicArray.h"
#include "HAL/Memory/AllocatorInterface.h"
#include "HAL/Memory/AllocIntelTBB.h"
#include "HAL/Memory/AllocJemalloc.h"
#include "HAL/Memory/AllocDefault.h"
#include "HAL/Memory/AllocThreadSafeDecorator.h"
#include "HAL/Memory.h"
#include "Global/Utilities/String.h"
#include "HAL/Chars.h"
#include "HAL/Time.h"


#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

using namespace std;





class TestClass:public SharedObject
{
public:

	static int a;
};
int TestClass::a = 2;

class TestClass2 :public TestClass
{
public:
	static void func()
	{
		cout << "a=" << a << endl;
	}
};


void func(int* ptr1, const int* ptr2)
{
	ptr1++;
	ptr2++;
	cout << ptr1 << endl;
	cout << ptr2 << endl;

}

template <typename InType>
static typename FuncTrigger<IsCharFixedEncoding<InType>::Value, int32>::Type
ConvertedLength(int32 SrcSize)
{
	return SrcSize;
}

#define PRINTEXT(text) wprintf(text);

int main()
{

	PlatformMemory::Init();

	PlatformTime::InitTime();

	double sec = PlatformTime::Time_Seconds();

	String timeStr = PlatformTime::SecondToFormatTime(float(sec));
	PRINTEXT(*timeStr);

	cout << "Seconds:" << sec << endl;
	cout << "Cycles:" << PlatformTime::Time_Cycles() << endl;

	TChar TimeBuffer[20];
	PlatformTime::TimeToStr(TimeBuffer, 20);
	PRINTEXT(TimeBuffer);

	cout << endl;

	PlatformTime::DateToStr(TimeBuffer, 20);
	PRINTEXT(TimeBuffer);

	PlatformTime::UpdateCPUTime();
	cout << endl;
	PlatformTime::UpdateCPUTime();

	CPUTime cput = PlatformTime::GetCPUTime();

	cout << "CPU:" << cput.CPUTimeUtilization << endl;
	cout << "CPU cores:" << cput.CPUTimeUtilizationCores << endl;

	PlatformMemory::UnInit();

	

	int temin;
	cin >> temin;
	return 0;
	
}



#endif