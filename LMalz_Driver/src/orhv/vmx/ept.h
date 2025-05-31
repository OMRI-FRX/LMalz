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

	int EPT_CLS::need_remap(UINT64 GPA, __out UINT64* pt_v);   //�ж��Ƿ���Ҫ����ӳ��(Ϊ�˲�Ӱ��const_ept)

public:

	static ept_pointer getConstEptp();
	//EPT���ʼ��
	static bool init();
	//��¡һ�ű�(ֻ����pml4e,ȫ����)
	EPT_CLS();
	//��¡һ�ű�(����pml4e��pdpte,������pdpte����)
	EPT_CLS(__in bool _1gb_R, __in bool _1gb_W, __in bool _1gb_X);


	bool set_pte(UINT64 GPA, ept_pte pte);
	bool set_pte(UINT64 GPA, bool _r, bool _w, bool _x);
	bool get_pte(UINT64 GPA, ept_pte* pte);

	ept_pointer	getEptp();

	OR_PTR<ept_pml4e> getPml4e();


};
