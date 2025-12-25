//###########################################################################
//
// FILE:	DSP2833x_0_GPIO.h
//
//###########################################################################

#ifdef __cplusplus
extern "C" {
#endif

#include "DSP2833x_0_Define.h"

void Init_SOL_GPIO_SETUP(void);								// SOL GPIO Setting

void Sol_Auto(int istage);									// Check of Process 中  SOL Use
void Sol_Manual(Uint16 Sol_No, Uint16 Sol_Enable);			// Manual Run
void SOL_Output();											// SOL Output

void Init_SSR_GPIO_SETUP(void);								// SSR GPIO Setting
void Init_V_Pump_GPIO_SETUP(void);							// Pump GPIO Setting

void InitEPwm2Module();										// PWN of Pump1
void InitEPwm3Module();										// PWN of Pump2

void Init_SOL_GPIO_SETUP(void)
{
    // 1. 보호된 레지스터 수정을 위한 권한 획득
    EALLOW;

    // --- MUX 설정: 모든 핀을 일반 I/O(0) 모드로 설정 ---
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;   // Sol_1
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;   // Sol_2
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;   // Sol_3
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;   // Sol_4
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;  // Sol_5
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;  // Sol_6

    GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;  // Sol_7
    GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;  // Sol_8
    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;  // Sol_9
    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;  // Sol_10
    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;  // Sol_11
    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;  // Sol_12
    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;  // Sol_13
    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;  // Sol_14
    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;  // Sol_15
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;  // Sol_16

    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;  // 8255 RESET
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;  // STATE LED
    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;  // POWER LED

    // --- 안전을 위한 초기 출력값 설정 (모두 OFF) ---
    // 방향(DIR)을 설정하기 전에 데이터 레지스터를 미리 0으로 만들어
    // 부팅 시 솔레노이드가 튀는 현상을 방지합니다.
    GpioDataRegs.GPACLEAR.all = 0x0FC0;       // GPIO 6~11 초기화
    GpioDataRegs.GPBCLEAR.all = 0x30FC0000;  // GPIO 48, 49, 50~59, 61 초기화

    // --- 방향 설정: 모두 출력(1)으로 설정 ---
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;

    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO59 = 1;

    GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;  // 8255 RESET
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;  // STATE LED
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;  // POWER LED

    // 2. 레지스터 보호 다시 설정
    EDIS;
}

void Sol_Auto(int istage)
{
	int i;
	// 1. 레시피 데이터(ASCII '1'/'0')를 정수(1/0)로 변환하여 sol 배열에 저장
	// 솔레노이드 데이터는 레시피의 18번~33번 인덱스에 위치함
	for(i = 0; i < 16; i++)
	{
		if(Recipe[istage][i + 18] == '1')
		{
			sol[i] = 1;
		}
		else
		{
			sol[i] = 0;
		}
	}

	// 2. 모든 상태 결정 후 딱 한 번만 하드웨어 출력 수행
	// 이를 통해 불필요한 반복 지연(DELAY_US)을 1/16로 줄임
	SOL_Output();
}

void Sol_Manual(Uint16 Sol_No, Uint16 Sol_Enable)
{
	// 1. 유효성 검사 (배열 범위를 벗어나지 않도록 1~16번 확인)
	if(Sol_No >= 1 && Sol_No <= 16)
	{
		// 2. 루프 없이 즉시 배열 값 변경 (Sol_No가 1이면 sol[0] 수정)
		sol[Sol_No - 1] = Sol_Enable;

		// 3. 하드웨어 출력 업데이트 (모든 상태를 한 번에 갱신)
		SOL_Output();
	}
	// 지연 시간(DELAY_US)은 SOL_Output 내부의 타이밍으로 충분하므로 제거합니다.
}

void SOL_Output()
{
	Uint32 tempA = 0;
	Uint32 tempB = 0;

	// 1. Port A (GPIO 6~11) 데이터 준비
	if(sol[0])  tempA |= 0x0040;  // GPIO6
	if(sol[1])  tempA |= 0x0080;  // GPIO7
	if(sol[2])  tempA |= 0x0100;  // GPIO8
	if(sol[3])  tempA |= 0x0200;  // GPIO9
	if(sol[4])  tempA |= 0x0400;  // GPIO10
	if(sol[5])  tempA |= 0x0800;  // GPIO11

	// 2. Port B (GPIO 50~59) 데이터 준비
	// GPIO50은 Port B의 18번째 비트 (50-32=18)
	if(sol[6])  tempB |= (1L << 18); // GPIO50
	if(sol[7])  tempB |= (1L << 19); // GPIO51
	if(sol[8])  tempB |= (1L << 20); // GPIO52
	if(sol[9])  tempB |= (1L << 21); // GPIO53
	if(sol[10]) tempB |= (1L << 22); // GPIO54
	if(sol[11]) tempB |= (1L << 23); // GPIO55
	if(sol[12]) tempB |= (1L << 24); // GPIO56
	if(sol[13]) tempB |= (1L << 25); // GPIO57
	if(sol[14]) tempB |= (1L << 26); // GPIO58 (주의: 히터 냉각과 중복 확인 필요)
	if(sol[15]) tempB |= (1L << 27); // GPIO59

	// 3. 레지스터에 한 번에 쓰기 (기존 비트 유지하며 해당 핀만 업데이트)
	// SET/CLEAR 레지스터를 사용하여 다른 핀에 영향을 주지 않음
	GpioDataRegs.GPACLEAR.all = 0x0FC0;       // GPIO 6~11 Clear
	GpioDataRegs.GPASET.all = tempA;         // GPIO 6~11 Set

	GpioDataRegs.GPBCLEAR.all = 0x0FFC0000;  // GPIO 50~59 Clear
	GpioDataRegs.GPBSET.all = tempB;         // GPIO 50~59 Set

	// 4. 전체 로직 종료 후 필요하다면 단 한 번의 지연만 수행
	DELAY_US(1);
}

void Init_SSR_GPIO_SETUP(void)
{
	EALLOW;
	// 1. 기능을 GPIO로 설정
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;

	// 2. 출력 방향으로 설정
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;

	// 3. (추가) 초기화 시 히터를 즉시 끔 (안전장치)
	// 부팅 시 핀 상태가 불안정할 수 있으므로 명시적으로 0을 출력합니다.
	GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
	EDIS;
}

void Init_V_Pump_GPIO_SETUP(void)
{
	EALLOW;
	// 1. 펌프 속도 제어 핀 설정 (ePWM 모드)
	GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;   // GPIO2 풀업 활성화
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;   // GPIO5 풀업 활성화

	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;  // GPIO2를 EPWM2B로 설정 (주석 오류 수정)
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;  // GPIO5를 EPWM3A로 설정 (주석 오류 수정)

	// 2. 펌프 제어 핀 초기 출력값 설정 (HIGH)
	GpioDataRegs.GPASET.bit.GPIO2 = 1;
	GpioDataRegs.GPASET.bit.GPIO5 = 1;

	// 3. 펌프 활성화/방향 제어 핀 설정 (GPIO 출력 모드)
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;  // GPIO3 설정 (Pump1 Dir/Enable)
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;  // GPIO4 설정 (Pump2 Dir/Enable)
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;   // 출력으로 설정
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;   // 출력으로 설정

	// 4. 안전을 위해 펌프 초기 상태를 OFF(또는 특정 방향)로 명시적 설정
	GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;

	EDIS;

	// 5. ePWM 모듈 초기화 호출
	InitEPwm2Module();
	InitEPwm3Module();
}

void InitEPwm2Module()
{
	EPwm2Regs.TBPRD = (TBCLK/PWMCARRIER)/2;
	EPwm2Regs.TBCTR = 0;

	EPwm2Regs.CMPA.half.CMPA = 0;
	EPwm2Regs.CMPB = 0;

	/* Setup counter mode */
	EPwm2Regs.TBCTL.bit.CTRMODE = 2;		/* Count Up/Down (Symmetric) */
	EPwm2Regs.TBPHS.half.TBPHS = 0;			/* Phase is 0 */
	EPwm2Regs.TBCTL.bit.PHSEN = 0;			/* Disable phase loading */
	EPwm2Regs.TBCTL.bit.PRDLD = 0;			/* Period Register is loaded from its shadow when CNTR=Zero */
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;		/* Clock ratio to SYSCLKOUT */
	EPwm2Regs.TBCTL.bit.CLKDIV = 0;			/* TBCLK = SYSCLK / (HSPCLKDIV * CLKDIV) */

	/* Setup shadowing */
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = 0;		/* Enable Shadowing */
	EPwm2Regs.CMPCTL.bit.LOADAMODE = 0;		/* Load on CNTR=Zero */
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = 0;		/* Enable Shadowing */
	EPwm2Regs.CMPCTL.bit.LOADBMODE = 0;		/* Load on CNTR=Zero */

	/* Set actions */
	EPwm2Regs.AQCTLA.bit.CAU = 2;			/* Set EPWMxA on event A, up count */
	EPwm2Regs.AQCTLA.bit.CAD = 1;			/* Clear EPWMxA on event A, down count */
	EPwm2Regs.AQCTLB.bit.CBU = 2;			/* Set EPWMxB on event B, up count */
	EPwm2Regs.AQCTLB.bit.CBD = 1;			/* Clear EPWMxB on event B, down count */

	/* Set Interrupts */
	EPwm2Regs.ETSEL.bit.INTSEL = 1;			/* Select INT on CNTR=Zero */
	EPwm2Regs.ETPS.bit.INTPRD = 1;			/* Generate INT on 1st event */
	EPwm2Regs.ETSEL.bit.INTEN = 1;			/* Enable INT */
}

void InitEPwm3Module()
{
	EPwm3Regs.TBPRD = (TBCLK/PWMCARRIER)/2;
	EPwm3Regs.TBCTR = 0;

	EPwm3Regs.CMPA.half.CMPA = 0;
	EPwm3Regs.CMPB = 0;

	/* Setup counter mode */
	EPwm3Regs.TBCTL.bit.CTRMODE = 2;		/* Count Up/Down (Symmetric) */
	EPwm3Regs.TBPHS.half.TBPHS = 0;			/* Phase is 0 */
	EPwm3Regs.TBCTL.bit.PHSEN = 0;			/* Disable phase loading */
	EPwm3Regs.TBCTL.bit.PRDLD = 0;			/* Period Register is loaded from its shadow when CNTR=Zero */
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;		/* Clock ratio to SYSCLKOUT */
	EPwm3Regs.TBCTL.bit.CLKDIV = 0;			/* TBCLK = SYSCLK / (HSPCLKDIV * CLKDIV) */

	/* Setup shadowing */
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = 0;		/* Enable Shadowing */
	EPwm3Regs.CMPCTL.bit.LOADAMODE = 0;		/* Load on CNTR=Zero */
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = 0;		/* Enable Shadowing */
	EPwm3Regs.CMPCTL.bit.LOADBMODE = 0;		/* Load on CNTR=Zero */

	/* Set actions */
	EPwm3Regs.AQCTLA.bit.CAU = 2;			/* Set EPWMxA on event A, up count */
	EPwm3Regs.AQCTLA.bit.CAD = 1;			/* Clear EPWMxA on event A, down count */
	EPwm3Regs.AQCTLB.bit.CBU = 2;			/* Set EPWMxB on event B, up count */
	EPwm3Regs.AQCTLB.bit.CBD = 1;			/* Clear EPWMxB on event B, down count */

	/* Set Interrupts */
	EPwm3Regs.ETSEL.bit.INTSEL = 1;			/* Select INT on CNTR=Zero */
	EPwm3Regs.ETPS.bit.INTPRD = 2;			/* Generate INT on 1st event */
	EPwm3Regs.ETSEL.bit.INTEN = 1;			/* Enable INT */
}


/***************************************************************************************************************Pump end***********/


#ifdef __cplusplus
}
#endif /* extern "C" */

//===========================================================================
// End of file.
//===========================================================================

