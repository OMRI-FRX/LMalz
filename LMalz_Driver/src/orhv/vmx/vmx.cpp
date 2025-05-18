#include <intrin.h>
#include <wdm.h>
#include "../orhv.h"
#include "vmx.h"
#include "ept.h"
#include "vmx.inl"
OR_HV_VMX* gALvmxVCPU = 0;

static bool ALvmxSupported() {
	cpuid_eax_01 cpuid_01;
	__cpuid(reinterpret_cast<int*>(&cpuid_01), 0x01);

	// 3.23.6
	if (!cpuid_01.cpuid_feature_information_ecx.virtual_machine_extensions) {
		DbgPrint("[hv] VMX not supported by CPUID.\n");
		return false;
	}
	ia32_feature_control_register feature_control;
	feature_control.flags = __readmsr(IA32_FEATURE_CONTROL);

	// 3.23.7
	if (!feature_control.lock_bit ||
		!feature_control.enable_vmx_outside_smx) {
		DbgPrint("[hv] VMX not enabled outside SMX.\n");
		return false;
	}

	_disable();

	auto cr0 = __readcr0();
	auto cr4 = __readcr4();

	// 3.23.7
	cr4 |= CR4_VMX_ENABLE_FLAG;

	// reserved bits in CR0/CR4
	uint64_t vmx_cr0_fixed0 = __readmsr(IA32_VMX_CR0_FIXED0);;
	uint64_t vmx_cr0_fixed1 = __readmsr(IA32_VMX_CR0_FIXED1);;
	uint64_t vmx_cr4_fixed0 = __readmsr(IA32_VMX_CR4_FIXED0);;
	uint64_t vmx_cr4_fixed1 = __readmsr(IA32_VMX_CR4_FIXED1);;

	// 3.23.8
	cr0 |= vmx_cr0_fixed0;
	cr0 &= vmx_cr0_fixed1;
	cr4 |= vmx_cr4_fixed0;
	cr4 &= vmx_cr4_fixed1;

	__writecr0(cr0);
	__writecr4(cr4);

	_enable();

	return true;
}
bool ALvmxCoreStart(OR_HV_VMX_CORE* core)
{
	if (!ALvmxSupported())
	{
		ALhvAddErr("CPU��֧�����⻯");
		return 0;
	}

	ORVM_CONTEXT_t ctx = { 0 };
	if (ALhvGetContext_asm(&ctx) == 1) {

		/*ALdbgPutValue(currCore->vmxInfo.vm_exit_tsc_overhead);
		ALdbgPutValue(currCore->vmxInfo.vm_exit_mperf_overhead);
		ALdbgPutValue(currCore->vmxInfo.vm_exit_ref_tsc_overhead);*/
		///cr04����λ��ȡ�ĳ�ʼֵ cr3load(�Ȳ�������) 	�޸�hostcr3


		//�������
		return 1;
	}
	auto vmxon_mem = ALhvMMallocateMemory(0x1000);
	if (vmxon_mem == NULL)
	{
		ALhvAddErr("����vmon�ڴ�ʧ��");
		return 0;
	}
	auto vmcs_mem = ALhvMMallocateMemory(0x1000);
	if (vmcs_mem == NULL)
	{
		ALhvAddErr("����vmcs�ڴ�ʧ��");
		return 0;
	}
	core->vmxon.vv = (UINT64)vmxon_mem;
	core->vmxon.pv = ALhvMMgetPA(vmxon_mem);
	core->vmcs.vv = (UINT64)vmcs_mem;
	core->vmcs.pv = ALhvMMgetPA(vmcs_mem);

	auto& vmxon = core->vmxon;
	auto& vmcs = core->vmcs;

	ia32_vmx_basic_register vmx_basic;
	vmx_basic.flags = __readmsr(IA32_VMX_BASIC);

	// 3.24.11.5
	vmxon->revision_id = vmx_basic.vmcs_revision_id;
	vmxon->must_be_zero = 0;

	// �������⻯,����non-rootģʽ
	if (!__vmx_on(&vmxon.pv)) {
		ALhvSetErr("[hv] VMXON failed.\n");
		return 0;
	}

	// 3.28.3.3.4
	ALvmxInvept_asm(invept_all_context, {});

	//���õ�ǰcs
	// 3.24.2
	vmcs->revision_id = vmx_basic.vmcs_revision_id;
	vmcs->shadow_vmcs_indicator = 0;

	if (!__vmx_vmclear(&vmcs.pv)) {
		ALhvSetErr("[hv] VMCLEAR failed.\n");
		return false;
	}

	if (!__vmx_vmptrld(&vmcs.pv)) {
		ALhvSetErr("[hv] VMPTRLD failed.\n");
		return false;
	}
	auto& reg = core->reg;
	//����guest �Ĵ���
	{
		reg.rax = 1;                              //
		reg.rbx = ctx.rbx;                        //
		reg.rcx = ctx.rcx;                        //
		reg.rdx = ctx.rdx;                        //
		reg.rsi = ctx.rsi;                        //
		reg.rdi = ctx.rdi;                        //
		reg.rbp = ctx.rbp;                        //
		reg.rsp = ctx.rsp;                        //
		reg.rip = ctx.rip;                        //
		reg.rflags.flags = ctx.rflags;                     //
		reg.r8 = ctx.r8;                         //
		reg.r9 = ctx.r9;                         //
		reg.r10 = ctx.r10;                        //
		reg.r11 = ctx.r11;                        //
		reg.r12 = ctx.r12;                        //
		reg.r13 = ctx.r13;                        //
		reg.r14 = ctx.r14;                        //
		reg.r15 = ctx.r15;                        //
		reg.cr0 = ctx.cr0;                        //
		reg.cr2 = ctx.cr2;                        //
		reg.cr3 = ctx.cr3;                        //
		reg.cr4.flags = ctx.cr4;                        //
		reg.cr8 = ctx.cr8;                        //
		reg.dr0 = ctx.dr0;                        //
		reg.dr1 = ctx.dr1;                        //
		reg.dr2 = ctx.dr2;                        //
		reg.dr3 = ctx.dr3;                        //
		reg.dr4 = ctx.dr4;                        //
		reg.dr5 = ctx.dr5;                        //
		reg.dr6 = ctx.dr6;                        //
		reg.dr7.flags = ctx.dr7;                        //
		memcpy(reg.xmm0, ctx.xmm0, 16);                  //
		memcpy(reg.xmm1, ctx.xmm1, 16);                  //
		memcpy(reg.xmm2, ctx.xmm2, 16);                  //
		memcpy(reg.xmm3, ctx.xmm3, 16);                  //
		memcpy(reg.xmm4, ctx.xmm4, 16);                  //
		memcpy(reg.xmm5, ctx.xmm5, 16);                  //
		memcpy(reg.xmm6, ctx.xmm6, 16);                  //
		memcpy(reg.xmm7, ctx.xmm7, 16);                  //
	}
	{
		ia32_vmx_pinbased_ctls_register pin_based_ctrl;
		pin_based_ctrl.flags = 0;
		pin_based_ctrl.virtual_nmi = 1;				  //��������nmi
		pin_based_ctrl.nmi_exiting = 1;				  //����nmi�˳�
		//pin_based_ctrl.activate_vmx_preemption_timer = 1;	 //��ʱvmexit,�ݲ�����
		write_ctrl_pin_based_safe(pin_based_ctrl);
	}


	{
		// 3.24.6.2
		ia32_vmx_procbased_ctls_register proc_based_ctrl;
		proc_based_ctrl.flags = 0;
		//#ifndef NDEBUG
		//proc_based_ctrl.cr3_load_exiting = 1;					 //����cr3 vmexit
		//proc_based_ctrl.cr3_store_exiting           = 1;
	  //#endif
		proc_based_ctrl.use_msr_bitmaps = 1;		 //msr�˳�λͼ
		{
			__vmx_vmwrite(VMCS_CTRL_MSR_BITMAP_ADDRESS, gALvmxVCPU->msr_bitmap.pv);
		}
		proc_based_ctrl.use_tsc_offsetting = 1;		 //tscƫ��
		proc_based_ctrl.activate_secondary_controls = 1;  //������չ����λ
		write_ctrl_proc_based_safe(proc_based_ctrl);
	}

	{

		// 3.24.6.2
		ia32_vmx_procbased_ctls2_register proc_based_ctrl2;
		proc_based_ctrl2.flags = 0;
		proc_based_ctrl2.enable_ept = 1;		//����EPT
		{
			__vmx_vmwrite(VMCS_CTRL_EPT_POINTER, EPT_CLS::getConstEptp().flags);
		}
		proc_based_ctrl2.enable_rdtscp = 1;		//rdtscpָ��exit
		proc_based_ctrl2.enable_vpid = 1;		//���û����ʶ
		{
			__vmx_vmwrite(VMCS_CTRL_VIRTUAL_PROCESSOR_IDENTIFIER, ALhvGetCurrVcoreIndex() + 1);
		}
		proc_based_ctrl2.enable_invpcid = 1;	//..
		proc_based_ctrl2.enable_xsaves = 1;		//..
		proc_based_ctrl2.enable_user_wait_pause = 1; //����һЩָ��
		proc_based_ctrl2.conceal_vmx_from_pt = 1;	//��ֹ����non-rootģʽ�²���
		write_ctrl_proc_based2_safe(proc_based_ctrl2);
	}
	 //vmexitʱ��Ϊ����
	{
		// 3.24.7
		ia32_vmx_exit_ctls_register exit_ctrl;
		exit_ctrl.flags = 0;
		exit_ctrl.save_debug_controls = 1;		  //����guest debug���ƼĴ���
		exit_ctrl.host_address_space_size = 1;	  //����hostΪ��ģʽ
		exit_ctrl.save_ia32_pat = 1;			  //����guest��ؼĴ��� (�ڴ�����)
		exit_ctrl.load_ia32_pat = 1;			  //����host��ؼĴ���
		{
			ia32_pat_register host_pat;
			host_pat.flags = 0;
			host_pat.pa0 = MEMORY_TYPE_WRITE_BACK;
			host_pat.pa1 = MEMORY_TYPE_WRITE_THROUGH;
			host_pat.pa2 = MEMORY_TYPE_UNCACHEABLE_MINUS;
			host_pat.pa3 = MEMORY_TYPE_UNCACHEABLE;
			host_pat.pa4 = MEMORY_TYPE_WRITE_BACK;
			host_pat.pa5 = MEMORY_TYPE_WRITE_THROUGH;
			host_pat.pa6 = MEMORY_TYPE_UNCACHEABLE_MINUS;
			host_pat.pa7 = MEMORY_TYPE_UNCACHEABLE;
			__vmx_vmwrite(VMCS_HOST_PAT, host_pat.flags);
		}
		exit_ctrl.save_ia32_efer = 1;			  //����guest��ؼĴ��� (���ܼ�����)
		//exit_ctrl.load_ia32_efer = 1;			  //����host��ؼĴ���	 (����Ҫ,����޹ؽ�Ҫhost��������guest)
		{
			//__vmx_vmwrite(VMCS_HOST_EFER, __readmsr(IA32_EFER));
		}
		exit_ctrl.load_ia32_perf_global_ctrl = 1; //����host��ؼĴ���	(ĳЩ����λ)
		{
			__vmx_vmwrite(VMCS_HOST_PERF_GLOBAL_CTRL, 0);
		}
		exit_ctrl.conceal_vmx_from_pt = 1;		  //��ֹ����vmexit
		write_ctrl_exit_safe(exit_ctrl);
		
	}
	//vm����ʱ��Ϊ����
	{
		// 3.24.8
		ia32_vmx_entry_ctls_register entry_ctrl;
		entry_ctrl.flags = 0;
		entry_ctrl.load_debug_controls = 1;		  //����guest debug���ƼĴ���
		entry_ctrl.ia32e_mode_guest = 1;		  //vm����ʱ���볤ģʽ
		entry_ctrl.load_ia32_pat = 1;			  //����guest��ؼĴ���
		{
			__vmx_vmwrite(VMCS_GUEST_PAT, __readmsr(IA32_PAT));
		}
		//entry_ctrl.load_ia32_perf_global_ctrl = 1;	  //����guest��ؼĴ���  ���ڴ˴�����
		{
			//__vmx_vmwrite(VMCS_GUEST_PERF_GLOBAL_CTRL, __readmsr(IA32_PERF_GLOBAL_CTRL));
		}
		entry_ctrl.load_ia32_efer = 1;			   //����guest��ؼĴ���
		{
			__vmx_vmwrite(VMCS_GUEST_EFER, __readmsr(IA32_EFER));
		}
		entry_ctrl.conceal_vmx_from_pt = 1;
		write_ctrl_entry_safe(entry_ctrl);
	}
	//guest msr����ָ�

	{
		auto& msr_store = core->msr_store;
		msr_store.perf_global_ctrl.msr_idx = IA32_PERF_GLOBAL_CTRL;
		msr_store.aperf.msr_idx = IA32_APERF;
		msr_store.mperf.msr_idx = IA32_MPERF;
		msr_store.perf_global_ctrl.msr_data = __readmsr(IA32_PERF_GLOBAL_CTRL);
		msr_store.aperf.msr_data = __readmsr(IA32_APERF);
		msr_store.mperf.msr_data = __readmsr(IA32_MPERF);
		auto pa = ALhvMMgetPA(&msr_store);
		__vmx_vmwrite(VMCS_CTRL_VMEXIT_MSR_STORE_COUNT, 3);
		__vmx_vmwrite(VMCS_CTRL_VMEXIT_MSR_STORE_ADDRESS, pa);
		__vmx_vmwrite(VMCS_CTRL_VMENTRY_MSR_LOAD_COUNT, 3);
		__vmx_vmwrite(VMCS_CTRL_VMENTRY_MSR_LOAD_ADDRESS, pa);
	}












}

bool ALvmxInit(OR_HV_VMX* vcpu)
{
	auto bitmap = ALhvMMallocateMemory(0x1000);
	if (!bitmap)
	{
		ALhvAddErr("�����ڴ�ʧ��");
		return 0;
	}
	vcpu->msr_bitmap.vv = (UINT64)bitmap;
	vcpu->msr_bitmap.pv = ALhvMMgetPA(bitmap);

	if (!EPT_CLS::init())
	{
		ALhvAddErr("EPT��ʼ��ʧ��");
		return 0;
	}

	return 1;
}



bool ALvmxStart(OR_HV_VMX* vcpu)
{
	//��ʼ������string
	ALhvSetErr("");
	if (!ALvmxInit(vcpu))
	{
		ALhvAddErr("vmx��ʼ��ʧ��");
		return 0;
	}
	gALvmxVCPU = vcpu;
	PROCESSOR_NUMBER processorNumber = { 0 };
	GROUP_AFFINITY   affinity = { 0 }, oldAffinity = { 0 };
	UINT64 success = 0;
	
	for (ULONG i = 0; i < vcpu->core_count; i++)
	{
		//�л�����������
		auto status = KeGetProcessorNumberFromIndex(i, &processorNumber);
		if (!NT_SUCCESS(status)) {
			ALhvSetErr("�л�����ʧ��%d", i);
			break;
		}
		affinity.Group = processorNumber.Group;
		affinity.Mask = 1ULL << processorNumber.Number;
		affinity.Reserved[0] = affinity.Reserved[1] = affinity.Reserved[2] = 0;
		KeSetSystemGroupAffinityThread(&affinity, &oldAffinity);
		//��ʼ��ǰ�������⻯
		if (ALvmxCoreStart(&vcpu->cores[i]))
		{
			(success++);
		}
		else
		{
			ALhvAddErr("%d�������⻯ʧ��", i);
		}

		//����ԭ����
		KeRevertToUserGroupAffinityThread(&oldAffinity);

	}


	//ALdbgPut("VT Engine has been loaded!\n");

	return success == vcpu->core_count;









}


bool ALvmxIsRoot()
{
	auto vcpu = ALvmxGetVCPU();
	if (vcpu)
		return vcpu->isRoot;
	return 0;
}

OR_HV_VMX_CORE* ALvmxGetCurrVcore()
{
	return &gALvmxVCPU->cores[ALhvGetCurrVcoreIndex()];
}

//OR_HV_VMX_CORE* ALvmxGetCurrVcore()
//{
//	if (!gALvmxVCPU)
//		return 0;
//	auto  rsp = (UINT64)_AddressOfReturnAddress();
//	for (int i = 0; i < gALvmxVCPU->core_count; i++)
//		if (rsp >= (UINT64)(&gALvmxVCPU->cores[i]) && rsp < (UINT64)(&gALvmxVCPU->cores[i + 1]))
//			return  &gALvmxVCPU->cores[i];
//	//��������Ҳ���˵������rootģʽ,ֱ�ӵ���api
//	return  &gALvmxVCPU->cores[KeGetCurrentProcessorIndex()];
//}
//int ALvmxGetCurrVcoreIndex()
//{
//	if (!gALvmxVCPU)
//		return KeGetCurrentProcessorIndex();
//	auto  rsp = (UINT64)_AddressOfReturnAddress();
//	for (int i = 0; i < gALvmxVCPU->core_count; i++)
//		if (rsp >= (UINT64)(&gALvmxVCPU->cores[i]) && rsp < (UINT64)(&gALvmxVCPU->cores[i + 1]))
//			return i;
//	//��������Ҳ���˵������rootģʽ,ֱ�ӵ���api
//	return KeGetCurrentProcessorIndex();
//}

