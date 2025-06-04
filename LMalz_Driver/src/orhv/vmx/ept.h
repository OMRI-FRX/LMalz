#pragma once
#include "../orhv.h"
#include "vmx.h"

//�������ڿ�¡,�������ڴ���Υ��,�����״�eptΥ�����붼�ø�����,guest���ڸ�����

//ʹ��ʱ����ȫ��ָ��ָ��ֲ���̬����
class EPT_CLS
{					
	static OR_PTR<ept_pml4e> const_ept;
	static const UINT64 const_ept_size = 512;  //��С(GB)
	static ept_pointer const_eptp_value;//д��vmcs��ֵ


	OR_PTR<ept_pml4e> ept_pml4;
	ept_pointer eptp_v;
	EPT_CLS* clone;			  //��¡�Ķ���,��Ϊ0���ǿ�¡��const,��֧���½�,��������
	int need_remap(EPT_CLS* target, UINT64 GPA, __out UINT64* pt_v);
	int need_remap(UINT64 GPA, __out UINT64* pt_v);   //�ж��Ƿ���Ҫ����ӳ��(Ϊ�˲�Ӱ��const_ept)

public:

	static ept_pointer getConstEptp();
	static OR_PTR<ept_pml4e>* getConstEpt() { return &const_ept; };

	static EPT_CLS* getViceEpt();	  //��ȡ����ept,�ñ�̳���const


	//EPT���ʼ��
	static bool init();
	//��¡const_ept(ֻ����pml4e,ȫ����)
	EPT_CLS();
	//��¡conset_ept(����pml4e��pdpte,������pdpte����)
	EPT_CLS(__in bool _1gb_R, __in bool _1gb_W, __in bool _1gb_X);
	//��¡����ָ��ı�
	EPT_CLS(EPT_CLS* clone);

	bool set_pte(UINT64 GPA, ept_pte pte);
	bool set_pte(UINT64 GPA, bool _r, bool _w, bool _x);


	bool get_pte(UINT64 GPA, ept_pte* pte);
	bool get_pde(UINT64 GPA, ept_pde* pde);
	bool get_pdpte(UINT64 GPA, ept_pdpte* pdpte);
	bool get_pml4e(UINT64 GPA, ept_pml4e* pml4e);
	ept_pml4e  get_pml4e(UINT64 GPA);
	ept_pdpte  get_pdpte(UINT64 GPA);
	ept_pde  get_pde(UINT64 GPA);
	ept_pte  get_pte(UINT64 GPA);

		

	ept_pte* pte(UINT64 GPA);
	ept_pde* pde(UINT64 GPA);
	ept_pdpte* pdpte(UINT64 GPA);
	ept_pml4e* pml4e(UINT64 GPA);



	ept_pointer	getEptp();

	OR_PTR<ept_pml4e> getPml4e();


};
