#pragma once
#include "vcpu_vmx.h"
#include "vmx_exitHandler.h"

//vmx.cpp
extern OR_HV_VMX* gALvmxVCPU;
bool ALvmxStart(OR_HV_VMX* vcpu);	  //vmx ��ʼ


bool ALvmxIsRoot();//��ǰ�����Ƿ�Ϊhostģʽ


OR_HV_VMX_CORE* ALvmxGetCurrVcore();  //��ȡ��ǰ����CPU�ṹ��	  ���δ��ʼ����Ϊ0

//vmxasm.asm
extern "C" void ALvmxInvept_asm(invept_type type, invept_descriptor const& desc);
extern "C" void ALvmxHostEnter_asm();
extern "C" void ALvmxGuestEnter_asm(OR_HV_VMX_CORE*);

//vmx_exitHandler.cpp
bool ALvmxVmexitInit();
inline void vmx_increment_rip(OR_HV_VMX_CORE* vcpu);
VMEXIT_HANDLERS_CLS* ALvmxGetExitHandlers();