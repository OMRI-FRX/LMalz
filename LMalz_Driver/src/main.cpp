#include "orhv\orhv.h"
#include "dbgput.h"
bool ALhvStart();
int main(UINT64*)
{
	ALdbgPut("�������سɹ�,��ʼ����VT");

	if (!ALhvStart())
	{
		auto err = ALhvGetErr();
		ALdbgPut("���س���:%s", err);
	}
	else
	{
		ALdbgPut("���سɹ�");
	}
	return 0;
}