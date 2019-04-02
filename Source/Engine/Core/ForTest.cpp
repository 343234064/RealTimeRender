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

	TChar Path[12] = TEXTS("Reatl/dasd");
	TChar RawData[8] = TEXTS("ÕâÊÇÒ»¸ö×Ö·û´®");
	setlocale(LC_ALL, "chs");

	String MyString(RawData);
	MyString.Append(TEXTS('¥»'));

	cout << "CurrentNum:" << MyString.Len() << endl;
	
	MyString.AppendPath(Path,10);
	PRINTEXT(*MyString);
	
	MyString += TEXTS('\n');
	PRINTEXT(*MyString);
	MyString.AppendFloat(-7.1545000);
	MyString += TEXTS('\n');
	PRINTEXT(*MyString);


	MyString.AppendInt(-5646554);
	MyString += TEXTS('\n');
	PRINTEXT(*MyString);

	MyString.AppendInt(46554);
	MyString += TEXTS('\n');
	PRINTEXT(*MyString);
	
	MyString.Insert(5, TEXTS("Ù¤Ä¦"));
	PRINTEXT(*MyString);

	String tmp1 = MyString.Front(5);
	PRINTEXT(*tmp1);

	String tmp2 = MyString.End(5);
	PRINTEXT(*tmp2);

	String tmp3 = MyString.Range(5, 9);
	PRINTEXT(*tmp3);

	MyString.Clear();
	MyString.Empty();

	if (tmp3 == TEXTS("Ù¤Ä¦·û´®"))
		cout << "true" << endl;

	MyString += TEXTS("adsÙ¤Ä¦·û´®\n");
	PRINTEXT(*MyString);

	cout << "Find:" << MyString.Find(TEXTS("Ù¤Ä¦·û´®"), 1) << endl;;
	
	MyString.ToUpper();
	PRINTEXT(*MyString);

	MyString.ToLower();
	PRINTEXT(*MyString);

	String Prefix = TEXTS("·û´®\n");
	MyString.RemoveSuffix(Prefix, false);
	PRINTEXT(*MyString);


	if (MyString.EndWith(Prefix, true))
		cout << "true" << endl;
	MyString.Clear();
	MyString = String::FromFloat(56.655060);
	cout << MyString.ToBool() << endl;
	cout << MyString.ToFloat() << endl;
	cout << MyString.ToInt32() << endl;
	MyString += TEXTS("·û  ´®sad ad adwqc°®»ª¶ÙÀ­ÈøµÂ¿¨£»\n");

	MyString.RemoveSpaces();
	PRINTEXT(*MyString);
	PlatformMemory::UnInit();

	

	int temin;
	cin >> temin;
	return 0;
	
}



#endif