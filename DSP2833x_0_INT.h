//###########################################################################
//
// FILE:	DSP2833x_232C_TX_RX.h
//
//###########################################################################
// TX, RX of 232C
// 2012.10.31
//###########################################################################


#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// SCI Individual Register Bit Definitions

//----------------------------------------------------------
// SCICCR communication control register bit definitions:
//
#include "DSP2833x_0_Define.h"

float32 Cpu_Clk;							 // CPUtimer 에 필요한 변수
float32 Timer_Prd0;				 // CPUtimer 에 필요한 변수
Uint16 RI_Mod[5];
Uint16 buff_Temp[2];

void CPUTimer_INT_Init()
{
	// CPU Timer 초기화
		InitCpuTimers();
		Cpu_Clk = 150;				// 현재 시스템 클럭을 설정 (MHz 단위)
		Timer_Prd0 = 100000;		// 타이머 주기 설정 (usec 단위) 100 000 = 0.1sec
	// CPU Timer0 을 1초 주기로 설정
		ConfigCpuTimer(&CpuTimer0, Cpu_Clk, Timer_Prd0);
	// CPU Timer0 시작
		StartCpuTimer0();
	// CPU Timer0 인터럽트 활성화
		PieCtrlRegs.PIEIER1.bit.INTx7 = 1;			// PIE 인터럽트(TINT0) 활성화
		IER |= M_INT1; 								// CPU 인터럽트(INT1)  활성화
		CPU_Timer0_Isr_cnt = 0;
		EINT;   // Enable Global interrupt INTM
		ERTM;   // Enable Global realtime interrupt DBGM
}

void ADC_INT_Init()
{
	InitAdc();
	// ADC 설정
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 3;	   		// ADCCLK = HSPCLK/(ADCCLKPS*2)/(CPS+1)
	AdcRegs.ADCTRL1.bit.CPS = 1;				// ADCCLK = 75MHz/(3*2)/(1+1) = 6.25MHz
	AdcRegs.ADCTRL1.bit.ACQ_PS = 3;				// 샘플/홀드 사이클 = ACQ_PS + 1 = 4 (ADCCLK기준)
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;			// 시퀀스 모드 설정: 직렬 시퀀스 모드 (0:병렬 모드, 1:직렬 모드)
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 7;		// ADC 채널수 설정: 1개(=MAX_CONV+1)채널을 ADC
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0; 		// ADC 순서 설정: 첫번째로 ADCINA0 채널을 ADC
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1; 		// ADC 순서 설정: 2번째로 ADCINA1 채널을 ADC
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 2; 		// ADC 순서 설정: 3번째로 ADCINA2 채널을 ADC
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 3; 		// ADC 순서 설정: 4번째로 ADCINA3 채널을 ADC
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 4; 		// ADC 순서 설정: 5번째로 ADCINA4 채널을 ADC
	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 5; 		// ADC 순서 설정: 6번째로 ADCINA5 채널을 ADC
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 8; 		// ADC 순서 설정: 7번째로 ADCINA8 채널을 ADC
	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 9; 		// ADC 순서 설정: 8번째로 ADCINA9 채널을 ADC

	AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ = 1; 		// ePWM_SOCB로 ADC 시퀀스 시동
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;		// ADC 시퀀스 완료시 인터럽트 발생 설정


	//ePWM_SOCB 이벤트 트리거 설정					// 샘플은 Pwm3이지만 펌프에서 Pwm3을 사용해서 Pwm4로 변경
	EPwm4Regs.ETSEL.bit.SOCBEN = 1;				// SOCB 이벤트 트리거 Enable
	EPwm4Regs.ETSEL.bit.SOCBSEL = 2;			// SCCB 트리거 조건 : 카운터 주기 일치 시
	EPwm4Regs.ETPS.bit.SOCBPRD = 1;				// SOCB 이벤트 분주 설정 : 트리거 조건 한번 마다
	EPwm4Regs.TBCTL.bit.CTRMODE = 0;			// 카운트 모드 설정: Up-conut 모드
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = 1;			// TBCLK = [SYSCLKOUT / ((HSPCLKDIV*2) * 2^(CLKDIV))]
	EPwm4Regs.TBCTL.bit.CLKDIV = 5;				// TBCLK = [150MHz / (2*2)] = 37.5MHz  2019-12-23 김종민 수정
	EPwm4Regs.TBPRD = 37500;					// TB주기= (TBPRD+1)/TBCLK = 1875/37.5MHz = 50us(20KHz) * 1ms *
	EPwm4Regs.TBCTR = 0x0000;					// TB 카운터 초기화


	// PIE의 ADC 인터럽트 활성화
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;			// PIE 인터럽트(ADCINT) 활성화
	IER |= M_INT1; 								// CPU 인터럽트(INT1)  활성화
}
interrupt void cpu_timer0_isr(void)				// cpu_timer
{
	CPU_Timer0_Isr_cnt++;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Acknowledge interrupt to PIE

}
interrupt void adc_isr(void)					// ADC
{
/////////////////////////////////////////////////////////////////////////////////////////////190830 temp2->temp1 변경
	/*if(AdcRegs.ADCRESULT0<5875){
		temp0avr[0] = (4.9 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 6550){
		temp0avr[0] = (4.8 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 7300){
		temp0avr[0] = (4.68 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 8000){
		temp0avr[0] = (4.7 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 9500){
		temp0avr[0] = (4.68 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 10450){
		temp0avr[0] = (4.63 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 11950){
		temp0avr[0] = (4.6 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 12600){
		temp0avr[0] = (4.57 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 13450){
		temp0avr[0] = (4.55 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 15005){
		temp0avr[0] = (4.54 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 16300){
		temp0avr[0] = (4.5 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 17700){
		temp0avr[0] = (4.54 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 20600){
		temp0avr[0] = (4.53 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 21900){
		temp0avr[0] = (4.57 * AdcRegs.ADCRESULT0 * 100)/65536;
	}
	else if(AdcRegs.ADCRESULT0 < 22500){
		temp0avr[0] = (4.59 * AdcRegs.ADCRESULT0 * 100)/65536;
		}
	else if(AdcRegs.ADCRESULT0 < 24370){
		temp0avr[0] = (4.60 * AdcRegs.ADCRESULT0 * 100)/65536;
		}
	else if(AdcRegs.ADCRESULT0 < 25200){
		temp0avr[0] = (4.64 * AdcRegs.ADCRESULT0 * 100)/65536;
		}
	else if(AdcRegs.ADCRESULT0 < 25500){
		temp0avr[0] = (4.68 * AdcRegs.ADCRESULT0 * 100)/65536;
		}
	else if(AdcRegs.ADCRESULT0 < 26900){
		temp0avr[0] = (4.71 * AdcRegs.ADCRESULT0 * 100)/65536;
		}
	else if(AdcRegs.ADCRESULT0 < 27200){
		temp0avr[0] = (4.75 * AdcRegs.ADCRESULT0 * 100)/65536;
		}
	else{
		temp0avr[0] = (4.78 * AdcRegs.ADCRESULT0 * 100)/65536;
	}*/
	////////////////////////////////////////////////////////////////////////////////////220323온도 보정식 계산방식 변경 직선식
	/*if(AdcRegs.ADCRESULT0<6520){
		temp0avr[0] = ((0.0093 * AdcRegs.ADCRESULT0 ) - 4.8418) ;
	}
	else if(AdcRegs.ADCRESULT0 < 13380){
		temp0avr[0] = ((0.0067 * AdcRegs.ADCRESULT0 ) + 13.078);
	}
	else if(AdcRegs.ADCRESULT0 < 13475){
		temp0avr[0] = ((0.0951 * AdcRegs.ADCRESULT0 ) - 1169.7);
	}
	else if(AdcRegs.ADCRESULT0 < 16855){
		temp0avr[0] = ((0.0066 * AdcRegs.ADCRESULT0 ) + 23.107);
	}
	else if(AdcRegs.ADCRESULT0 < 17100){
		temp0avr[0] = ((0.0439 * AdcRegs.ADCRESULT0 ) - 605.43);
	}
	else{
		temp0avr[0] = ((0.0076 * AdcRegs.ADCRESULT0 ) + 15.4);
	}*/
////////////////////////////////////////////////////////////////////////////////////온도 검교정용 보정식

	temp0avr[0] = ((0.0068 * AdcRegs.ADCRESULT0 ) + 1.6718);

/////////////////////////////////////////////////////////////////////////////////////
	temp0avr[count0]=temp0avr[0];
	count0++;

	if(count0 > 6){
		buff_Temp[0] = (temp0avr[1] + temp0avr[2] + temp0avr[3] + temp0avr[4] + temp0avr[5])/5;
		count0 = 1;

		if(buff_Temp[0] > (Current_Temp_Heater[0]+20)){
			buff_Temp[0] = 0;

		}
		else{
			if(buff_Temp[0]<220){
				Current_Temp_Heater[0] = buff_Temp[0];
			}
			else
			{
				buff_Temp[0] = 0;
			}
		}
	buff_Temp[0] = 0;
	}

	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
}

interrupt void sciaRxIsr(void)
{
	if(Next_istage != istage){							// 명령이 반복적으로 계속 수행되는걸
		for(i=0;i<2;i++){	Process_TEMP[i] = '.';	}   // 막기 위해 명령배열 초기화
	}

	// temp = SciaRegs.SCIRXBUF.bit.RXDT;				// 명령 코드 자기위치로 삽입

	temp = scia_Rx();									// 2019-12-23 수정

	if(temp == 'T'){ 
		Process_TEMP[1]='T';	
		Next_istage = istage;
	}																							// 					T of STP
	//if((temp == 'P')&&(Process_TEMP[0]=='T')){ Process_TEMP[1]='P';	Next_istage = istage;}  // 					P of STP

	if(temp == 'A'){ 
		Process_TEMP[1]='A';	
		Next_istage = istage;
	}																							// 					A of PAS
	//if((temp == 'S')&&(Process_TEMP[0]=='A')){ Process_TEMP[1]='S';	Next_istage = istage;}  // 					S of PAS

	if(temp == 'E'){ 
		Process_TEMP[1]='E';	
		Next_istage = istage;
	}																							// 					E of NEX
	//if((temp == 'X')&&(Process_TEMP[0]=='E')){ Process_TEMP[1]='X';	Next_istage = istage;}  // 					X of NEX

	if(temp == 'G'){ 
		Process_TEMP[1]='G';	
		Next_istage = istage;
	}																							// 					G of NGO
	//if((temp == 'O')&&(Process_TEMP[0]=='G')){ Process_TEMP[1]='O';	Next_istage = istage;}  // 					O of NGO

	switch(Process_TEMP[1]){
	case 'T' : // STP
		string_Tx("@$$STP#");								// 191202, 2022-02-21 시작문자 끝문자 추가
		//DELAY_US(ECHO_DELAY);
		/*scia_Tx('$');										// 2019-12-23
		scia_Tx('$');
		scia_Tx('S');
		scia_Tx('T');
		scia_Tx('P');*/
		DELAY_US(50);

		EMS = 1;
		NGO = 1;											// 각 for/while 문들을 빠져나가기위한
		Step_SET_Delay_Time = 0;							// 조건들 수정
		Delay_SET_Time_Heater[0]=0;
		Delay_SET_Time_Heater[1]=0;
		Heater0_Delay_Time_End = 1;
		Heater1_Delay_Time_End = 1;
		//Motor_All_Check = 1;

		for(i=0;i<2;i++){									// 2019-12-23 버퍼정리
			Process_TEMP[i] = '.';
		}

		break;

	case 'A' : // PAS
		//string_Tx("@$$PAS#");								// 191202, 2022-02-21 시작문자 끝문자 추가 220404 에코제거
		//DELAY_US(ECHO_DELAY);
		/*scia_Tx('$');										// 2019-12-23
		scia_Tx('$');
		scia_Tx('P');
		scia_Tx('A');
		scia_Tx('S');*/
		DELAY_US(50);

		Mode = 4;
		break;

	case 'E' : // NEX
		//string_Tx("@$$NEX#");								// 191202, 2022-02-21 시작문자 끝문자 추가 220404 에코제거
		//DELAY_US(ECHO_DELAY);
		/*scia_Tx('$');										// 2019-12-23
		scia_Tx('$');
		scia_Tx('N');
		scia_Tx('E');
		scia_Tx('X');*/
		DELAY_US(50);

		Step_SET_Delay_Time=0;								// 각 for/while 문들을 빠져나가기위한
		Delay_SET_Time_Heater[0]=0;
		Delay_SET_Time_Heater[1]=0;							// 조건들 수정
		Heater0_Delay_Time_End = 1;
		Heater1_Delay_Time_End = 1;
		//Motor_All_Check = 1;

		for(i=0;i<2;i++){
			Process_TEMP[i] = '.';
		}
		break;

	case 'G' : // NGO
		//string_Tx("$$GGO");								// 191202
		//DELAY_US(ECHO_DELAY);

		/* scia_Tx('$');
		scia_Tx('$');
		scia_Tx('G');
		scia_Tx('G');
		scia_Tx('O');
		DELAY_US(50); */									// 2019-12-23

		NGO = 1;

		for(i=0;i<2;i++){
			Process_TEMP[i] = '.';
		}
		break;
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;			// Acknowledge interrupt to PIE
}


#ifdef __cplusplus
}
#endif /* extern "C" */


//===========================================================================
// End of file.
//===========================================================================

