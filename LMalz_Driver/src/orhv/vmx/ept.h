#pragma once
#include "../orhv.h"
#include "vmx.h"

//ʹ��ʱ����ȫ��ָ��ָ��ֲ���̬����
class EPT_CLS
{
	OR_PTR<ept_pml4e> ept_pml4;
	ept_pointer eptp_v;

	static OR_PTR<ept_pml4e> const_ept;
	static const UINT64 const_ept_size = 512;  //��С(GB)
	static ept_pointer const_eptp_value;//д��vmcs��ֵ

	////����һ�ſ�¡��(���ʼ�����)
	//bool getCloneTable();
public:

	static ept_pointer getConstEptp();
	//EPT���ʼ��
	static bool init();
	//��¡һ�ű�
	EPT_CLS();
	bool set_pte(UINT64 GPA, ept_pte pte);
	bool get_pte(UINT64 GPA, ept_pte* pte);

	ept_pointer	getEptp();

	OR_PTR<ept_pml4e> getPml4e();


};
