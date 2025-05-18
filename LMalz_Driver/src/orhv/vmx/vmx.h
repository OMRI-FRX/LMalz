#pragma once
#include "vcpu_vmx.h"

//vmx.cpp
extern OR_HV_VMX* gALvmxVCPU;
bool ALvmxStart(OR_HV_VMX* vcpu);	  //vmx ��ʼ

OR_HV_VMX_CORE* ALvmxGetVCPU();//��ȡ��ǰ����CPU�ṹ��	  ���δ��ʼ����Ϊ0

bool ALvmxIsRoot();//��ǰ�����Ƿ�Ϊhostģʽ


OR_HV_VMX_CORE* ALvmxGetCurrVcore();

//vmxasm.asm
extern "C" void ALvmxInvept_asm(invept_type type, invept_descriptor const& desc);