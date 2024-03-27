.syntax unified
.cpu cortex-m33
.fpu softvfp
.thumb


.text
.type SVC_Handler, %function
.global SVC_Handler
SVC_Handler:
	push {r4-r11, lr}	@ IMPORTANT: save link register (others are optional if you don't change them)
	mov r0, sp			@ load stack pointer
	adds r0, r0, #60	@ address of pc in stack: 9*4 (register just saved) + 6*4 (saved exception stack)
	ldr r1, [r0]		@ load program counter
	subs r1, r1, #2		@ get address of previous instruction (SVC on 16 bits in T32)
	ldr r2, [r1]		@ load instruction and extract SVC code
	ands r2, r2, #0xff

	@ Select handler depending on SVC code
	@ (better to use a literal table if they are a lot)
	cmp r2, #0xf4
	beq f4
	cmp r2, #0xf5
	beq f5
	b svc_exit
f4:
	bl save_stack_fram
	b svc_exit
f5:
	bl restore_ckp_fram
svc_exit:
	pop {r4-r11, lr} @ restore registers (IMPORTANT for link register)
	bx lr


.text
.type save_stack_fram, %function
.global save_stack_fram
save_stack_fram:
	push {r4-r11, lr} @save registers of caller
	@ see if it important also to save other CPU registers (control, faultmask, primask, etc..)
	mov r0, sp			@ pass stack pointer to checkpoint function
	bl write_ckp_fram
	pop {r4-r11, lr}
	bx lr


@ --------- STACK POINTER RESTORATION FUNCTION--------------
@ r0: lenght of stack

.text
.type restore_sp_fram, %function
.global restore_sp_fram
restore_sp_fram:
	push {r4-r11, lr}	@save registers of caller (probably useless)

	ldr r1, =_estack	@ load base stack address
	subs r1, r1, r0		@ compute old stack pointer
	adds r1, r1, #1
	mov sp, r1		  	@ restore stack pointer

	bl restore_ckp_fram_stack_drivers

	pop {r4-r11, lr}	@ pop old stack
	bx lr				@ continue from checkpointed state



