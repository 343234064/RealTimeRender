#include "Global/GlobalConfigs.h"

#if FOR_TEST_CPP

#include "Global/GlobalType.h"
#include "Global/Utilities/AtomicCounter.h"
#include "HAL/Platform.h"
#include "Global/Utilities/Misc.h"
#include "Global/Utilities/Assertion.h"
#include "Global/Utilities/ContainerAllocator.h"
#include "Global/Utilities/DynamicArray.h"
#include "HAL/Memory/AllocatorInterface.h"
#include "HAL/Memory/AllocIntelTBB.h"
#include "HAL/Memory/AllocJemalloc.h"
#include "HAL/Memory/AllocDefault.h"
#include "HAL/Memory/AllocThreadSafeDecorator.h"
#include "HAL/Memory/Memory.h"
#include "Global/Utilities/String.h"
#include "HAL/Chars.h"
#include "HAL/Time.h"
#include "Log/LogMacros.h"
#include "HAL/File/FileMisc.h"
#include "HAL/File/Path.h"


#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

using namespace std;



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


class Obj
{
public:
	Obj(int32 i=0):
		a(i)
	{}
	~Obj()
	{
		cout << "delete:" << a << endl;
	}

	int32 a;
};
bool operator==(const Obj& left, const Obj& right)
{
	return left.a == right.a;
}




#define PRINTEXT(text) wprintf(text);

int main()
{

	PlatformMemory::Init();
	




	PlatformMemory::UnInit();



	int temin;
	cin >> temin;
	return 0;
	
}



#endif