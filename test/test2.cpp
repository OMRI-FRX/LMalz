#include <windows.h>
#include <intrin.h>
#include <stdio.h>
class A
{
	int aa;
public:
	A(int a, int b) {
		aa = a + b;
    };    
    A(int a) {
		aa = a;
    };
    A()
    {
		aa = -1;
    }
    int g()
    {
        return aa;
    };
};
int main() {
    //// ͨ��Windows API��ȡ��ǰ�߼���������
    //DWORD win_core = GetCurrentProcessorNumber();

    //// ͨ��CPUID��ȡAPIC ID��ʾ����Leaf 0x01��EBX��24λ��
    //int regs[4];
    //__cpuid(regs, 1);
    //DWORD apic_id = (regs[1] >> 24) & 0xFF;

    //printf("Windows Logical Core: %u\n", win_core);
    //printf("CPUID APIC ID: %u\n", apic_id);
	A b[] = { {} ,{1},2,{3,3} };
	printf("%d %d %d %d", b[0].g(), b[1].g(), b[2].g(), b[3].g());



    return 0;
}