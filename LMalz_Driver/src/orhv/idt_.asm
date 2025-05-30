trap_frame struct
  ; general-purpose registers
  $rax qword ?
  $rcx qword ?
  $rdx qword ?
  $rbx qword ?
  $rbp qword ?
  $rsi qword ?
  $rdi qword ?
  $r8  qword ?
  $r9  qword ?
  $r10 qword ?
  $r11 qword ?
  $r12 qword ?
  $r13 qword ?
  $r14 qword ?
  $r15 qword ?

  ; interrupt vector
  $vector qword ?

  ; _MACHINE_FRAME
  $error  qword ?
  $rip    qword ?
  $cs     qword ?
  $rflags qword ?
  $rsp    qword ?
  $ss     qword ?
trap_frame ends


extern ALhvIdtHandler : proc
.code

; pushes error code to stack
IDT_HANDLER macro  proc_name:req,code:req
proc_name proc
  ; allocate space for the trap_frame structure (minus the size of the
  ; _MACHINE_FRAME, error code, and interrupt vector)
  sub rsp, 78h

  ; general-purpose registers
  mov trap_frame.$rax[rsp], rax
  mov trap_frame.$rcx[rsp], rcx
  mov trap_frame.$rdx[rsp], rdx
  mov trap_frame.$rbx[rsp], rbx
  mov trap_frame.$rbp[rsp], rbp
  mov trap_frame.$rsi[rsp], rsi
  mov trap_frame.$rdi[rsp], rdi
  mov trap_frame.$r8[rsp],  r8
  mov trap_frame.$r9[rsp],  r9
  mov trap_frame.$r10[rsp], r10
  mov trap_frame.$r11[rsp], r11
  mov trap_frame.$r12[rsp], r12
  mov trap_frame.$r13[rsp], r13
  mov trap_frame.$r14[rsp], r14
  mov trap_frame.$r15[rsp], r15

  ; first argument is the trap frame
  mov rcx, rsp

  ; call handle_host_interrupt
  sub rsp, 20h
  call ALhvIdtHandler
  add rsp, 20h

  mov trap_frame.$vector[rsp],rax ;把获取到的地址放在向量里
 ; int 3
  ; general-purpose registers
  mov rax, trap_frame.$rax[rsp]
  mov rcx, trap_frame.$rcx[rsp]
  mov rdx, trap_frame.$rdx[rsp]
  mov rbx, trap_frame.$rbx[rsp]
  mov rbp, trap_frame.$rbp[rsp]
  mov rsi, trap_frame.$rsi[rsp]
  mov rdi, trap_frame.$rdi[rsp]
  mov r8,  trap_frame.$r8[rsp]
  mov r9,  trap_frame.$r9[rsp]
  mov r10, trap_frame.$r10[rsp]
  mov r11, trap_frame.$r11[rsp]
  mov r12, trap_frame.$r12[rsp]
  mov r13, trap_frame.$r13[rsp]
  mov r14, trap_frame.$r14[rsp]
  mov r15, trap_frame.$r15[rsp]

  ; free the trap_frame
  add rsp, 78h
  ; pop the interrupt vector
  add rsp, 8
  test qword ptr[rsp-8],-1
  jnz to_system
  ; pop the error code
  add rsp, 8
  ;int 3	
  iretq
to_system:

  ;pop 错误代码
  add rsp, code
  ;mov rax,qword ptr[rsp - 8-code]
  ;int 3
  jmp qword ptr[rsp - 8-code] ;跳到system idt 
proc_name endp
endm
; pushes error code to stack
DEFINE_ISR macro interrupt_vector:req, proc_name:req
proc_name proc
  ; interrupt vector is stored right before the machine frame
  push interrupt_vector

  jmp ALhvIdtHandler_asm_yes
proc_name endp
endm

; doesn't push error code to stack
DEFINE_ISR_NO_ERROR macro interrupt_vector:req, proc_name:req
proc_name proc
  ; push a dummy error code onto the stack
  push 0

  ; interrupt vector is stored right before the machine frame
  push interrupt_vector

  jmp ALhvIdtHandler_asm_no
proc_name endp
endm



IDT_HANDLER		ALhvIdtHandler_asm_no	,8
IDT_HANDLER		ALhvIdtHandler_asm_yes	,0

DEFINE_ISR_NO_ERROR  0, ALhvIDT_interrupt_handler_00_DE_asm      ;;/** Divide-by-Zero-Error Exception (Vector 0) */    
DEFINE_ISR_NO_ERROR  1, ALhvIDT_interrupt_handler_01_DB_asm      ;;/** Debug Exception (Vector 1) */    
DEFINE_ISR_NO_ERROR  2, ALhvIDT_interrupt_handler_02_NMI_asm     ;;/** Non-Maskable-Interrupt Exception (Vector 2) */    
DEFINE_ISR_NO_ERROR  3, ALhvIDT_interrupt_handler_03_BP_asm      ;;/** Breakpoint Exception (Vector 3) */    
DEFINE_ISR_NO_ERROR  4, ALhvIDT_interrupt_handler_04_OF_asm      ;;/** Overflow Exception (Vector 4) */    
DEFINE_ISR_NO_ERROR  5, ALhvIDT_interrupt_handler_05_BR_asm      ;;/** Bound-Range Exception (Vector 5) */    
DEFINE_ISR_NO_ERROR  6, ALhvIDT_interrupt_handler_06_UD_asm      ;;/** Invalid-Opcode Exception (Vector 6) */    
DEFINE_ISR_NO_ERROR  7, ALhvIDT_interrupt_handler_07_NM_asm      ;;/** Device-Not-Available Exception (Vector 7) */    
DEFINE_ISR           8, ALhvIDT_interrupt_handler_08_DF_asm      ;;/** Double-Fault Exception (Vector 8) */    

DEFINE_ISR			10, ALhvIDT_interrupt_handler_10_TS_asm      ;;/** Invalid-TSS Exception (Vector 10) */    
DEFINE_ISR          11, ALhvIDT_interrupt_handler_11_NP_asm      ;;/** Segment-Not-Present Exception (Vector 11) */    
DEFINE_ISR          12, ALhvIDT_interrupt_handler_12_SS_asm      ;;/** Stack Exception (Vector 12) */    
DEFINE_ISR          13, ALhvIDT_interrupt_handler_13_GP_asm      ;;/** General-Protection Exception (Vector 13) */    
DEFINE_ISR          14, ALhvIDT_interrupt_handler_14_PF_asm      ;;/** Page-Fault Exception (Vector 14) */    

DEFINE_ISR_NO_ERROR 16, ALhvIDT_interrupt_handler_16_MF_asm      ;;/** x87 Floating-Point Exception-Pending (Vector 16) */    
DEFINE_ISR          17, ALhvIDT_interrupt_handler_17_AC_asm      ;;/** Alignment-Check Exception (Vector 17) */    
DEFINE_ISR_NO_ERROR 18, ALhvIDT_interrupt_handler_18_MC_asm      ;;/** Machine-Check Exception (Vector 18) */    
DEFINE_ISR_NO_ERROR 19, ALhvIDT_interrupt_handler_19_XF_asm      ;;/** SIMD Floating-Point Exception (Vector 19) */    
DEFINE_ISR_NO_ERROR 20, ALhvIDT_interrupt_handler_20_VE_asm      ;;EPT violations This exception can occur only on processors 
;;														       that support the 1-setting of the “EPT-violation #VE” VM-execution control.
DEFINE_ISR          21, ALhvIDT_interrupt_handler_21_CP_asm      ;;/** Control-Protection Exception (Vector 21) */  



ALhvIDTreadMemory64_asm proc
	mov rax,qword ptr[rcx]
	ret
ALhvIDTreadMemory64_asm endp   

ALhvIDTreadMemory32_asm proc
	mov eax,dword ptr[rcx]
	ret
ALhvIDTreadMemory32_asm endp	 
ALhvIDTreadMemory16_asm proc
	mov ax,word ptr[rcx]
	ret
ALhvIDTreadMemory16_asm endp	
ALhvIDTreadMemory8_asm proc
	mov al,byte ptr[rcx]
	ret
ALhvIDTreadMemory8_asm endp			



ALhvIDTwriteMemory64_asm proc
	mov rax,qword ptr[rdx]
	mov qword ptr[rcx],rax
	ret
ALhvIDTwriteMemory64_asm endp   

ALhvIDTwriteMemory32_asm proc
	mov eax,dword ptr[rdx]
	mov dword ptr[rcx],eax
	ret
ALhvIDTwriteMemory32_asm endp	  

ALhvIDTwriteMemory16_asm proc
	mov ax,word ptr[rdx]
	mov word ptr[rcx],ax
	ret
ALhvIDTwriteMemory16_asm endp	  

ALhvIDTwriteMemory8_asm proc
	mov al,byte ptr[rdx]
	mov byte ptr[rcx],al
	ret
ALhvIDTwriteMemory8_asm endp

ALhvIDTxsetbv_asm proc
	and rcx,0ffffffffh
	mov eax,edx
	shr rdx, 32
	xsetbv
	ret
ALhvIDTxsetbv_asm endp


ALhvIDTrdmsr_asm proc

	rdmsr	
	shl rdx,32
	or rax,rdx
	ret
	
ALhvIDTrdmsr_asm endp

ALhvIDTwrmsr_asm proc

	mov eax,edx
	shr rdx,32
	wrmsr
	ret
	
ALhvIDTwrmsr_asm endp


end