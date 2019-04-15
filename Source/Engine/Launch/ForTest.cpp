#include "Global/GlobalConfigs.h"

#if FOR_TEST_CPP

#include "Global/GlobalType.h"
#include "Global/Utilities/AtomicCounter.h"
#include "HAL/Platform.h"
#include "Global/Utilities/Misc.h"

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
	setlocale(LC_ALL, "chs");

	Array<int32> Test = { 5,4,3,2,0,1,5,5,5};

	for (int32 i = 0; i < Test.CurrentNum();i++)
		cout << Test[i]<<" ";
	cout << endl;

	Test.RemoveAllMatch(5);

	for (int32 i = 0; i < Test.CurrentNum();i++)
		cout << Test[i] << " ";
	cout << endl;
	
	TChar Test2[13] = TEXTS("abbcdcdbccdc");
	String Str(Test2);
	PRINTEXT(*Str);

	Str.RemoveAllMatch(TEXTS('c'));
	cout << endl;
	PRINTEXT(*Str);

	Obj remove(1);
	Array<Obj> Test3 = {Obj(5), Obj(5), Obj(5), Obj(5), Obj(5), Obj(5), Obj(5), Obj(5), Obj(1)};
	cout << "+++++++++++++++" << endl;
	Test3.RemoveAllMatch(remove);
	cout << "+++++++++++++++" << endl;
	for (int32 i = 0; i < Test3.CurrentNum();i++)
		cout << Test3[i].a << " ";
	cout << endl;


	PlatformMemory::UnInit();

	

	int temin;
	cin >> temin;
	return 0;
	
}



#endif