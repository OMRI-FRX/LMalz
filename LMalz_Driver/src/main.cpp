#include "orhv\orhv.h"
#include "dbgput.h"
bool ALhvStart();

#pragma code_seg(".onon")


void test()
{
	ALdbgPut("���Գɹ�");
}



#pragma  code_seg()

bool ALvmxMNTinit();
bool ALvmxMNTsetTarget(PVOID va, UINT64 size);
int main(UINT64*)
{
	ALdbgPut("�������سɹ�,��ʼ����VT");

	if (!ALhvStart())
	{
		auto err = ALhvGetErr();
		ALdbgPut("���س���:%s", err);
		return 0;
	}
	else
	{
		ALdbgPut("���سɹ�");
		if (!ALvmxMNTinit())
		{
			ALdbgPut("��س�ʼ��ʧ��");
			return 0;
		}
		auto b = ALvmxMNTsetTarget((PVOID)((UINT64)test & ~0xfffLL), 0x1000);
		if (!b)
		{
			ALdbgPut("�������ʧ��");
			return 0;
		}
		ALdbgPut("��ʼ���ò��Ժ���");
		test();
		ALdbgPut("���ò��Ժ������");

	}
	return 0;
}