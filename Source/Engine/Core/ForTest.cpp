#include "GlobalConfigs.h"

#if FOR_TEST_CPP

#include "GlobalDefinations.h"
#include "Utilities/AtomicCounter.h"
#include "HAL/Platform.h"
#include "Utilities/Misc.h"
#include "Utilities/SmartPointer.h"
#include "Utilities/ContainerAllocator.h"
#include "Utilities/DynamicArray.h"
#include "HAL/Memory/AllocatorInterface.h"
#include "HAL/Memory/AllocIntelTBB.h"
#include "HAL/Memory/AllocJemalloc.h"
#include "HAL/Memory/AllocDefault.h"
#include "HAL/Memory/AllocThreadSafeDecorator.h"
#include "HAL/Memory.h"
#include "Core/Utilities/String.h"
#include "HAL/Chars.h"
#include <iostream>
#include <string>
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
#include <locale>
#include <codecvt>
int main()
{

	PlatformMemory::Init();
	Array<int> IntArray;
	IntArray = { 8,3,5,6,32,56,23,2,3,5 };
	cout << "CurrentNum:" << IntArray.CurrentNum() << endl;
	cout << "MaxNum:" << IntArray.MaxNum() << endl;
	IntArray.Sort();

	cout << "Array:";
	for (int i = 0;i < IntArray.CurrentNum();i++)
		cout << IntArray[i] << " ";
	cout << endl;

	IntArray.Add(100);
	IntArray.Add(50);
	cout << "FindIndex:" << IntArray.Find(5) << endl;//cout << "In ele:" << In << endl;return (In == 32);
	cout << "FindIndex:" << IntArray.FindBySelector([](const int& In)->bool {if (In == 32) { return true; }return false;}) << endl;

	cout << "CurrentNum:" << IntArray.CurrentNum() << endl;
	cout << "MaxNum:" << IntArray.MaxNum() << endl;

	IntArray.Add(20);
	
	cout << "CurrentNum:" << IntArray.CurrentNum() << endl;
	cout << "MaxNum:" << IntArray.MaxNum() << endl;

	IntArray.Shrink();

	cout << "CurrentNum:" << IntArray.CurrentNum() << endl;
	cout << "MaxNum:" << IntArray.MaxNum() << endl;

	IntArray.RemoveAt(2, 3);

	cout << "Array:";
	for (int i = 0;i < IntArray.CurrentNum();i++)
		cout << IntArray[i] << " ";
	cout << endl;

	cout << "CurrentNum:" << IntArray.CurrentNum() << endl;
	cout << "MaxNum:" << IntArray.MaxNum() << endl;

	IntArray.ForceShrink();

	cout << "CurrentNum:" << IntArray.CurrentNum() << endl;
	cout << "MaxNum:" << IntArray.MaxNum() << endl;

	
	PlatformMemory::UnInit();

	CHAR16 a = u'a';
	CHAR16 b = u'ñá';

	cout << sizeof(a) << endl;
	cout << sizeof(b) << endl;
	cout << b << endl;
	

	int temin;
	cin >> temin;
	return 0;
	
}



#endif