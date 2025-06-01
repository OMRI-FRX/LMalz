#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#define __R3
#define _CRT_SECURE_NO_WARNINGS

#include "F:\g_un\ALJT\Code\omri\include\driver\driver.h"
#include "F:\g_un\ALJT\Code\omri\include\orvm\orvm.h"
#include "F:\g_un\ALJT\Code\omri\include\debug\debug.h"
#include "F:\g_un\ALJT\Code\omri\include\memory\memory.h"
#include "F:\g_un\ALJT\Code\omri\include\PEanalysis\PEanalysis.h"
#include "F:\g_un\ALJT\Code\omri\include\process\process.h"
#include "F:\g_un\ALJT\Code\omri\include\resource\omri3\resource.h"
#include "resource.h"
UINT64 ALdrLoadDriver3(PVOID file, PVOID parameter, PUINT64 ret);
UINT64 ALdrLoadDriver4(PVOID file, PVOID parameter, PUINT64 ret);
UINT64 out[7] = { 0 };
int ALminiLoadDriver(UINT64 ���ط�ʽ = 1)
{
	auto driverRc = ALrcFindResource(IDR_DRIVER_DEX1, "driver_dex");
	//ALdbgPutValue(ALrcGetRcSize(driverRc));
	auto driver = ALrcGetResource(driverRc);
	UINT64 ret = 0;
	//out[6] = info;
	if (!driver || driver == (PVOID)-1)
		ALdbgBoxEx("��ȡ��Դ�ļ�ʧ��");
	else
	{
		bool loaded = 0;
		if (ALshaGetOsInfo() != OR_SHA_OS_WIN7)
		{
			if (���ط�ʽ == 1)								//��ͥ
			{
				loaded = ALdrLoadDriver2(driver, out, &ret);
			}
			if (!loaded)
			{
				loaded = ALdrLoadDriver3(driver, out, &ret);
			}
			if (!loaded)
			{
				loaded = ALdrLoadDriver4(driver, out, &ret);
			}
			if (!loaded)
			{
				ALdbgBox("�������7272");
				ALdbgBox("�������7272");
				return 0;
			}
		}
		else
		{
			loaded = ALdrLoadDriver(driver, out, &ret);
		}

		if (!loaded)
		{
			ALdbgBox("�������7272");
			ALdbgBox("�������7272");
			return 0;
		}

	}
	//ALdbgStoput();

	return 1;

}
int main() {
	ALminiLoadDriver();


    return 0;
}