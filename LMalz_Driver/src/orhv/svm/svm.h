#pragma once
#include "vcpu_svm.h"

//svm.cpp
extern OR_HV_SVM* gALsvmVCPU;
bool ALsvmStart(OR_HV_SVM* vcpu);//svm��ʼ

bool ALsvmIsRoot();//�жϵ�ǰ�Ƿ�Ϊhostģʽ

OR_HV_SVM_CORE* ALsvmGetCurrVcore();//��ȡ��ǰVCPU


