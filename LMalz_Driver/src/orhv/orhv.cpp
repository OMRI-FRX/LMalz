#include "orhv.h"
#include "vmx\vmx.h"
#include "svm\svm.h"
#include "..\main.h"

static OR_HV_CPU_TYPE GetCPUtype()
{
	typedef struct _CPUID_reg
	{
		int eax;
		int ebx;
		int ecx;
		int edx;
	} CPUID_reg, * PCPUID_reg;
	CPUID_reg data = { 0 };
	char vendor[0x20] = { 0 };
	__cpuid((int*)&data, 0);
	*(int*)(vendor) = data.ebx;
	*(int*)(vendor + 4) = data.edx;
	*(int*)(vendor + 8) = data.ecx;

	if (memcmp(vendor, "GenuineIntel", 12) == 0)
		return OR_HV_CPU_TYPE::OR_VM_CPU_TYPE_intel;
	if (memcmp(vendor, "AuthenticAMD", 12) == 0)
	{
		return OR_HV_CPU_TYPE::OR_VM_CPU_TYPE_AMD;
	}
	return OR_HV_CPU_TYPE::OR_VM_CPU_TYPE_Other;
}
OR_HV gALhvData = {};
inline bool ALhvIsIntelCPU()
{
	return gALhvData.cpu_type == OR_HV_CPU_TYPE::OR_VM_CPU_TYPE_intel;
}

inline bool ALhvIsAMDCPU()
{
	return gALhvData.cpu_type == OR_HV_CPU_TYPE::OR_VM_CPU_TYPE_AMD;
}

int ALhvGetCoreCount()
{
	return KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
}
static bool MapAPICidToCoreIndex();
template<typename VCPU>
bool ALhvInitialization(VCPU* vcpu)
{
	if (!MapAPICidToCoreIndex())
	{
		ALhvAddErr("���ĺ�ӳ��ʧ��");
		return 0;
	}
	auto ret = ALhvMMinitPool();
	if (!ret)
	{
		ALhvAddErr("��ʼ���ڴ��ʧ��");
		return 0;
	}
	//������Ķ�������
	{
		auto core_count = ALhvGetCoreCount();

		auto const arr_size = sizeof(*vcpu->cores) * core_count;

		auto cores = (GT(vcpu->cores))ALhvMMallocateMemory(arr_size);

		if (cores == 0)
		{
			ALhvAddErr("����vcpu�ڴ�ʧ��");
			return 0;
		}

		vcpu->core_count = core_count;
		vcpu->cores = cores;

	}
	//��ȡϵͳcr3
	{

		auto sys_cr3 = ALhvMMgetCr3ByObject((PEPROCESS)PsInitialSystemProcess);
		if (sys_cr3.flags == 0)
		{
			ALhvSetErr("��ȡϵͳcr3ʧ��");
			return 0;
		}
		PHYSICAL_ADDRESS pml4_address;
		pml4_address.QuadPart = sys_cr3.address_of_page_directory << 12;

		vcpu->system_cr3 = sys_cr3;
	}
	//��ȡhost cr3
	{
		cr3 host_cr3 = {};
		auto ret = ALhvMMgetHostCr3(vcpu->system_cr3, &host_cr3);
		if (!ret)
		{
			ALhvAddErr("����hostcr3ʧ��");
			return 0;
		}
		vcpu->host_cr3 = host_cr3;
	}
	return 1;
}


bool ALhvStart()
{
	{
		gALhvData.cpu_type = GetCPUtype();
		//��ֹ˯��
		gALhvData.SystemState = PoRegisterSystemState(NULL, ES_SYSTEM_REQUIRED | ES_CONTINUOUS);

		//	void PoUnregisterSystemState(
		//  [in, out] PVOID StateHandle
		//);
	}
	if (gALhvData.cpu_type == OR_HV_CPU_TYPE::OR_VM_CPU_TYPE_intel)
	{
		if (!ALhvInitialization(&gALhvData.vmx))
		{
			ALhvAddErr("hv��ʼ��ʧ��");
			return 0;
		}
		return ALvmxStart(&gALhvData.vmx);
	}
	else if (gALhvData.cpu_type == OR_HV_CPU_TYPE::OR_VM_CPU_TYPE_AMD)
	{
		if (!ALhvInitialization(&gALhvData.svm))
		{
			ALhvAddErr("hv��ʼ��ʧ��");
			return 0;
		}
		return ALsvmStart(&gALhvData.svm);
	}
	else
	{
		ALhvSetErr("δ֪CPU����");
	}
}


bool ALhvIsHost()
{
	if (ALhvIsIntelCPU())
		return ALvmxIsRoot();
	else
		return ALsvmIsRoot();
}

#define APIC_MAP_SIZE 512
static UINT8 apid_to_coreIndex[APIC_MAP_SIZE] = {};//����apic���Ϊ511

UINT32 ALhvGetAPIC_id()
{
	// ͨ��CPUID��ȡAPIC ID��ʾ����Leaf 0x01��EBX��24λ��
	int regs[4];
	__cpuid(regs, 1);
	UINT32 apic_id = (regs[1] >> 24) & 0xFF;

	return 	apic_id;
}
static bool MapAPICidToCoreIndex()
{
	//apid_to_coreIndex
	auto core_count = ALhvGetCoreCount();
	PROCESSOR_NUMBER processorNumber = { 0 };
	GROUP_AFFINITY   affinity = { 0 }, oldAffinity = { 0 };
	UINT64 success = 0;

	for (ULONG i = 0; i < core_count; i++)
	{
		//�л�����������
		auto status = KeGetProcessorNumberFromIndex(i, &processorNumber);
		if (!NT_SUCCESS(status)) {
			ALhvSetErr("�л�����ʧ��%d", i);
			return 0;
		}
		affinity.Group = processorNumber.Group;
		affinity.Mask = 1ULL << processorNumber.Number;
		affinity.Reserved[0] = affinity.Reserved[1] = affinity.Reserved[2] = 0;
		KeSetSystemGroupAffinityThread(&affinity, &oldAffinity);
		//��ʼ��ǰ�������⻯
		auto apic_id = ALhvGetAPIC_id();
		if (apic_id >= APIC_MAP_SIZE)
		{
			ALhvSetErr("APIC_id����%d %d", i, apic_id);
			return 0;
		}
		apid_to_coreIndex[apic_id] = i;
		//����ԭ����
		KeRevertToUserGroupAffinityThread(&oldAffinity);

	}
	return 0;
}


int ALhvGetCurrVcoreIndex()	 
{
	return apid_to_coreIndex[ALhvGetAPIC_id()];
}

