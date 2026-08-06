.syntax unified
.thumb
.cpu cortex-m3
.fpu softvfp
.thumb_func

.global PendSVHandler
.global startFirstTask
.global SystickHandler

.extern pCurrentTcb
.extern selectNextTask
.extern vTaskIncrementTick

.section .text

.type PendSVHandler, %function

PendSVHandler:
	CPSID	I
	MRS 	R0,	PSP
	STMDB	R0!, {R4, R11}

	LDR 	R3, =pCurrentTcb
	LDR		R2, [R3]
	LDR		R0, [R2]

	PUSH 	{LR}
	BL		selectNextTask
	POP		{LR}

	LDR		R3, =pCurrentTcb
	LDR		R2, [R3]
	LDR		R0, [R2]

	LDMIA	R0!, {R4-R11}
	MSR		PSP, R0

	CPSIE	I
	BX 		LR

.type startFirstTask, %function
startFirstTask:
	LDR		R3, =pCurrentTcb
	LDR		R2, [R3]
	LDR		R0, [R2]

	LDMIA	R0!, {R4-R11}
	MSR		PSP, R0

	CPSIE	I

	MOV		R0, #0x02
	MSR		CONTROL, R0
	ISB

	MOV		R14, #0xFFFFFFFD
	BX		R14

.type SystickHandler, %function
SystickHandler:
	CPSID	I
	PUSH	{LR}
	BL		incrementTick
	POP		{LR}
	CPSIE	I
	BX 		LR

.end
