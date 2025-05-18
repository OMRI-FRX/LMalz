//#include "orhv.h"
//#include "..\main.h"
//
//
//
//
//
//static bool CheckCPUtype()
//{
//	CPUID_reg data = { 0 };
//	char vendor[0x20] = { 0 };
//	__cpuid((int*)&data, 0);
//	*(int*)(vendor) = data.ebx;
//	*(int*)(vendor + 4) = data.edx;
//	*(int*)(vendor + 8) = data.ecx;
//
//	if (memcmp(vendor, "GenuineIntel", 12) == 0)
//		return 1;
//	if (memcmp(vendor, "AuthenticAMD", 12) == 0)
//	{
//		ALhvSetErr("AMD CPU ,go out");
//		return 0;
//	}
//	ALhvSetErr("??? CPU ,go out");
//	return 0;
//}
//
//hv_data ghv = { 0 };
//
//static cr3 GetCr3ByObject(PEPROCESS process)
//{
//	KAPC_STATE pr;
//	KeStackAttachProcess(process, &pr);
//	cr3 a = { 0 };
//	a.flags = __readcr3();
//	KeUnstackDetachProcess(&pr);
//	return a;
//}
//int ALhvGetCurrentCoreObjectNumber()
//{
//	return KeGetCurrentProcessorNumberEx(0);
//}
//vcpu* ALhvGetVcpu()
//{
//	if (ghv.vcpus == 0)
//	{
//		ALhvSetErr("����δ��ʼ��");
//		return 0;
//	}
//	auto  rsp = (UINT64)_AddressOfReturnAddress();
//	for (UINT32 i = 0; i < ghv.vcpu_count; i++)
//		if (rsp >= (UINT64)(&ghv.vcpus[i]) && rsp < (UINT64)(&ghv.vcpus[i + 1]))
//			return  &ghv.vcpus[i];
//	//��������Ҳ���˵������rootģʽ,ֱ�ӵ���api
//	return  &ghv.vcpus[ALhvGetCurrentCoreObjectNumber()];
//}
//bool ALhvIsHost()
//{
//	auto obj = ALhvGetVcpu();
//	if (obj != 0)
//	{
//		return obj->isHost;
//	}
//	return 0;
//}
//
//// initialize the host page tables
////��ʼ��host cr3
//bool prepare_host_page_tables()
//{
//	//����system cr3 �� pml4��256����,֮�����¹�������,idt,gdt��ӳ��·��,��Ҫ�޸���ӳ��
//	auto Hostpml4 = (pml4e_64*)ALhvMMallocateMemory(0x1000);
//
//
//	// kernel PML4 address
//	auto const guest_pml4 = ghv.system_page_table;
//	memcpy(&Hostpml4[256], &guest_pml4[256], sizeof(pml4e_64) * 256);
//	if (!ALhvMMsetAllPA(Hostpml4))
//	{
//		ALhvAddErr("ӳ�����������ڴ�ʧ��");
//		return 0;
//	}
//
//
//	auto Hostpml4_pa = ALmemGetPA(Hostpml4);
//	if (!Hostpml4_pa)
//	{
//		ALhvSetErr("��ȡHostpml4_paʧ��");
//		return 0;
//	}
//	//�޸���ӳ��
//	auto fixselfmap = [guest_pml4, Hostpml4, Hostpml4_pa]()->bool {
//		for (int i = 255; i < 512; i++)
//		{
//			if (guest_pml4[i].page_frame_number == ghv.system_cr3.address_of_page_directory)
//			{
//				Hostpml4[i].page_frame_number = Hostpml4_pa >> 12;
//				return 1;
//			}
//		}
//		return 0;
//	};
//
//	if (fixselfmap() == 0)
//	{
//		ALhvSetErr("�޸���ӳ��ʧ��");
//		return 0;
//	}
//
//
//
//	auto guestcr3 = ghv.system_cr3;
//	auto hostcr3 = guestcr3;
//	hostcr3.address_of_page_directory = Hostpml4_pa >> 12;
//	ghv.host_cr3 = hostcr3;
//
//	ghv.host_page_table = Hostpml4;
//
//
//
//
//
//
//	return 1;
//}
////��ʼ��һЩ��Ҫ���� 
//static bool vmInitialization()
//{
//	//��ֹ˯��
//	{
//		ghv.SystemState = PoRegisterSystemState(NULL, ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
//
//		//	void PoUnregisterSystemState(
//		//  [in, out] PVOID StateHandle
//		//);
//	}
//	auto ret = ALhvMMinitPool();
//	if (!ret)
//	{
//		ALhvAddErr("��ʼ���ڴ��ʧ��");
//		return 0;
//	}
//	{
//		ghv.vcpu_count = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
//
//		auto const arr_size = sizeof(vcpu) * ghv.vcpu_count;
//
//		ghv.vcpus = (vcpu*)ALhvMMallocateMemory(arr_size);
//
//		if (ghv.vcpus == 0)
//		{
//			ALhvAddErr("����vcpu�ڴ�ʧ��");
//			return 0;
//		}
//	}
//
//	{
//
//		auto sys_cr3 = GetCr3ByObject((PEPROCESS)PsInitialSystemProcess);
//		if (sys_cr3.flags == 0)
//		{
//			ALhvSetErr("��ȡϵͳcr3ʧ��");
//			return 0;
//		}
//		PHYSICAL_ADDRESS pml4_address;
//		pml4_address.QuadPart = ghv.system_cr3.address_of_page_directory << 12;
//		auto pt = static_cast<pml4e_64*>(ALmemGetVA(pml4_address.QuadPart));
//		if (pt == 0)
//		{
//			ALhvSetErr("��ȡϵͳcr3�����ַʧ��");
//			return 0;
//		}
//
//		ghv.system_cr3 = sys_cr3;
//		ghv.system_page_table = pt;
//	}
//
//	{
//		auto a = (UINT64)(vmInitialization);
//		a = _2pg(a);
//		while (*(PUINT16)a != (UINT16)'ZM')
//			a -= 0x1000;
//		PIMAGE_DOS_HEADER DOS = (PIMAGE_DOS_HEADER)a;									   //����DOSͷ
//		PIMAGE_NT_HEADERS NT = (PIMAGE_NT_HEADERS)(DOS->e_lfanew + (PUCHAR)a);			   //����NTͷ
//		PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)NT; // PEͷ
//		PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((ULONG64)pNTHeader + sizeof(IMAGE_NT_HEADERS));
//		int nAlign = pNTHeader->OptionalHeader.SectionAlignment; //�ζ����ֽ���
//		//// ��������ͷ�ĳߴ硣����dos, coff, peͷ �� �α�Ĵ�С
//		int ImageSize = (pNTHeader->OptionalHeader.SizeOfHeaders + nAlign - 1) / nAlign * nAlign;
//		// �������нڵĴ�С
//		for (int i = 0; i < pNTHeader->FileHeader.NumberOfSections; ++i)
//		{
//			//�õ��ýڵĴ�С
//			int CodeSize = pSectionHeader[i].Misc.VirtualSize;
//			int LoadSize = pSectionHeader[i].SizeOfRawData;
//			int MaxSize = (LoadSize > CodeSize) ? (LoadSize) : (CodeSize);
//
//			int SectionSize = (pSectionHeader[i].VirtualAddress + MaxSize + nAlign - 1) / nAlign * nAlign;
//			if (ImageSize < SectionSize)
//				ImageSize = SectionSize; //Use the Max;
//		}
//		ghv.driver_base = (PVOID)a;
//		ghv.driver_size = ImageSize;
//	}
//	//��ʼ��host ҳ��
//	{
//
//		ret = prepare_host_page_tables();
//		if (ret == 0)
//		{
//			ALhvAddErr("����hostҳ��ʧ��");
//			return 0;
//		}
//
//		ret = ALhvMMrebuildPath(ghv.system_page_table, ghv.host_page_table, ghv.driver_base, ghv.driver_size); //���¹�������ӳ��·��
//		if (!ret)
//		{
//			ALhvAddErr("���¹�������ӳ��·��ʧ��");
//			return 0;
//		}
//
//		ret = ALhvMMrebuildMemPoolPath(ghv.system_page_table, ghv.host_page_table);
//		if (!ret)
//		{
//			ALhvAddErr("���¹����ڴ��·��ʧ��");
//			return 0;
//		}
//	}
//
//
//	return 1;
//
//}
//bool ALvmxStart()
//{
//	if (!CheckCPUtype())
//	{
//		ALhvAddErr("CPU �ͺŲ���֧��");
//		return 0;
//	}
//	if (!vmInitialization())
//	{
//		ALhvAddErr("��ʼ��ʧ��");
//		return 0;
//	}
//
//
//	PROCESSOR_NUMBER processorNumber = { 0 };
//	GROUP_AFFINITY   affinity = { 0 }, oldAffinity = { 0 };
//	UINT64 success = 0;
//	ALhvSetErr("");
//	for (ULONG i = 0; i < ghv.vcpu_count; i++)
//	{
//		//�л�����������
//		auto status = KeGetProcessorNumberFromIndex(i, &processorNumber);
//		if (!NT_SUCCESS(status)) {
//			ALhvSetErr("�л�����ʧ��%d", i);
//			break;
//		}
//		affinity.Group = processorNumber.Group;
//		affinity.Mask = 1ULL << processorNumber.Number;
//		affinity.Reserved[0] = affinity.Reserved[1] = affinity.Reserved[2] = 0;
//		KeSetSystemGroupAffinityThread(&affinity, &oldAffinity);
//
//		//��ʼ��ǰ�������⻯
//		if (ALvmxCoreStart(&ghv.vcpus[i]))
//		{
//			(success++);
//		}
//		else
//		{
//			ALhvAddErr("%d�������⻯ʧ��", i);
//		}
//
//		//����ԭ����
//		KeRevertToUserGroupAffinityThread(&oldAffinity);
//
//	}
//
//
//	//ALdbgPut("VT Engine has been loaded!\n");
//
//	return success == ghv.vcpu_count;
//
//
//
//}