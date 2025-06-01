#include "..\ept.h"
#include "..\vmx.inl"
#include "..\vmx_exitHandler.h"

static EPT_CLS* g_target_X_ept = 0;
static EPT_CLS* g_system_X_ept = 0;

//��Ҫ����EPT,һ������Ŀ��ҳ��
bool ALvmxMNTinit()
{
	static EPT_CLS target_X_ept(1, 1, 0);  //�½�һ����Ŀ��ִ�е�ҳ��  �����д
	static EPT_CLS system_X_ept;  //��¡һ����ϵͳִ�е�ҳ��   
	g_target_X_ept = &target_X_ept;
	g_system_X_ept = &system_X_ept;
	if (g_target_X_ept->getPml4e().vv == 0)
	{
		ALhvAddErr("�½�ʧ��");
		return 0;
	}
	if (g_system_X_ept->getPml4e().vv == 0)
	{
		ALhvAddErr("�½�ʧ��");
		return 0;
	}
	return 1;
}
static bool monitor_ept_violation_handler(OR_HV_VMX_CORE* vcpu, bool is2)
{

	vcpu;
	is2;
	return 0;
}
bool ALvmxMNTsetTarget(PVOID va, UINT64 size)
{
	auto start_a = (UINT64)va & ~0xfffLL;
	auto end = (((UINT64)va + size) + 0xfff) & ~0xfffLL;
	
	for (PUINT8 i = (PUINT8)start_a; i < (PUINT8)end; i += 0x1000)
	{
		auto pa = ALhvMMgetPA(i);
		g_target_X_ept->set_pte(pa, 1, 1, 1);	   //����Ŀ��ģ��ִ��

		g_system_X_ept->set_pte(pa, 1, 1, 0);	   //��Ŀ�겻��ִ��
	}
	auto b = ALvmxGetExitHandlers()[VMX_EXIT_REASON_EPT_VIOLATION].add_fun(monitor_ept_violation_handler);
	if (!b)
	{
		ALhvAddErr("��Ӵ�����ʧ��");
		return 0;
	}

	set_ept(g_system_X_ept->getEptp());

	return 1;

}