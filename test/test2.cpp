#include <windows.h>
#include <intrin.h>
#include <stdio.h>

int main() {
    // ͨ��Windows API��ȡ��ǰ�߼���������
    DWORD win_core = GetCurrentProcessorNumber();

    // ͨ��CPUID��ȡAPIC ID��ʾ����Leaf 0x01��EBX��24λ��
    int regs[4];
    __cpuid(regs, 1);
    DWORD apic_id = (regs[1] >> 24) & 0xFF;

    printf("Windows Logical Core: %u\n", win_core);
    printf("CPUID APIC ID: %u\n", apic_id);

    return 0;
}