//============================================================================================
// ㅏ,ㅜ,ㅓ Motor Ro_ Test End Program
// 232C USE Display.
// New MainBoard 190829
// 191023 Temp 보정
// 191202 인터럽트 명령어 에코 제거
// 220221 문자열에 시작(@) 끝(#) 문자 추가 8.02(EUMC)에서 파생
// 220323 온도보정식 계산방법 변경 직선식 온도보정
// 220404 인터럽트 에코제거,  NOW_State_ReQST 프로세스 넘버 출력방식변경
// 220715 온도유지중 가열 속도 저하, 온도보정
// 220728 8.5.4에서 파생 온도유지중 가열속도 저하 부분 예전으로 변경
//--------------------------------------------------------------------------------------------
#include "DSP28x_Project.h"					   										// Device Headerfile and Examples Include File
#include "DSP2833x_Device.h"				   										// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"													// DSP2833x Examples Include File
#include "DSP2833x_0_Define.h"													// 변수정의
#include "DSP2833x_0_Motor_Sensor.h"											// Motor & Sensor 프로그램 설정
#include "DSP2833x_0_GPIO.h"				   										// GPIO 설정
#include "DSP2833x_0_Process.h"													// 히터, 펌프, 통신 설정
#include "DSP2833x_0_INT.h"														// 인터럽트 설정
#include "DSP2833x_Xintf.h"															// 외부인터페이스 설정
//============================================================================================
void InitXintf(void);
void First_num();
Uint16     Now_Step;                       												// Step 을 진행하기위한 변수
Uint16     Double_EQ;                      												// EQ

void main(void)
{
	DINT;
	InitSysCtrl();
	InitSciaGpio();
	InitScibGpio();
	InitScicGpio();																		// 190830 NewMB
	EALLOW;

	Init_SOL_GPIO_SETUP();
	Init_SSR_GPIO_SETUP();
	Init_V_Pump_GPIO_SETUP();

/******************************************************************** 시스템 컨트롤 초기화******/
	SysCtrlRegs.HISPCP.bit.HSPCLK = 1;  											// HSPCLK = SYSCLKOUT/(HISPCP*2)
	EDIS;																						// HSPCLK = 150MHz/(1*2) = 75MHz
/********************************************************************  인터럽트 초기화*********/
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	// Vector Remapping
	EALLOW;
	PieVectTable.ADCINT = &adc_isr;
/********************************************************************************************/
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.SCIRXINTA = &sciaRxIsr;
	EDIS;
	ADC_INT_Init();																	// ADC 초기화
	CPUTimer_INT_Init();																// INT CPUTimer0 SETTING
//	Rx_INT_Init();																		// Process中 에만 Rx 통신을 인터럽트 사용
/********************************************************************************************/
	InitXintf();																			// external interface initial

	// GpioDataRegs.GPADAT.all = 0x0000;										// GPIO output all '0'
	// GpioDataRegs.GPBDAT.all = 0x0000;										// GPIO output all '0'

	Hold_GPIO_Output();
/********************************************************************************************/
/*************************************************************************Process Start******/
	First_num();

	// DELAY_US(70000);
	DELAY_US(100000);

	// Motor_Init_Tick();      // 모터 초기화 진행 (절대 블로킹되지 않음)
	// Motor_Init_2();			// 2015.09.23 - 모터 한바퀴 돌림
	// Motor_Stop();

	while(1)
	{
		Order_COM();																// PC에서 오는 명령코드 확인 후 Mode 결정
		DELAY_US(1);

		switch(Mode){
			case 1 :																	// Mode=1 : PC_Data receive
				PC_Data_Recive();                                    				// PC data 를 Recipe 배열에 삽입
				break;
			case 2 :																	// Mode=2 : Process Start
				Manual_run:
				Rx_INT_Init();														// Rx 통신 인터럽트 사용
				Now_Step = 1, istage = 0;
				EMS = 0;

				while(Now_Step != istage){
					if(Recipe[istage][0] == ';'){									// Step end Check!!
						Now_Step = istage;
						EMS = 1;
					}
					else
					{																// Step end Check!!
						Delay_Time_Check(istage);									// Step Delay Time 확인
						Sol_Auto(istage);											// SOL 동작

						Pump_Check(istage);											// Vacuum Pump 동작확인< Speed 값 의 유무 >
						Heater_Check(istage);										// Heater 동작확인             < 온도 값의 유무>

						PAUSE();													// Pause 명령 수행 확인 및 수행

						if(EMS != 1){												// EMS = 1 : PC로 부터 비상정지명령 받음

							Pump_Auto();
							DELAY_US(20000);										// Pump 동작 & 압력을 잡기위한 딜레이
							Motor_Auto();											// Motor 구동 및 정지

							if(Manual != 0){
								NOW_State_M_ReQST();
								DELAY_US(25000);
							} // sending data to PC(Motor/Sol)

							PAUSE();													// Pause 명령 수행 확인 및 수행

							if(SET_Temp_Heater[0] != 0){
								Heater0_Auto();										// Heater1 구동 및 정지
							}
							else if(SET_Temp_Heater[1] != 0){
								Heater1_Auto();										// Heater2 구동 및 정지
							}

							CPU_Timer0_Isr_cnt = 0;

							while(!(Step_SET_Delay_Time*10 <= CPU_Timer0_Isr_cnt)){  	// Step_Delay_Time동안 펌프구동
								NOW_State_ReQST();							// sending data to PC(UI/RI/TEMP/stage)
								DELAY_US(70000);
								PAUSE();
							}
							DELAY_US(1);
							NGO = 0;

							if(Step_SET_Delay_Time == 0){
								NOW_State_ReQST();
								// DELAY_US(300);
								DELAY_US(70000);							// 2019-12-23
							}// sending data to PC(UI/RI/TEMP/stage)

							if(End_step-1 == istage){
								NGO = 1;
							}

							if((Double_EQ == 1)&&(EMS != 1)){
								while(NGO == 0){							// 2EQ
									string_Tx("@SDR#");						// 2022-02-21 시작문자 끝문자 추가
									/*scia_Tx('S');							// 2019-12-23
									scia_Tx('D');
									scia_Tx('R');*/
									// DELAY_US(10000);
									DELAY_US(35000);						// 2019-12-23
									NOW_State_ReQST();
									DELAY_US(70000);
									PAUSE();
								}  // sending data to PC(UI/RI/TEMP/stage)

								NGO = 0;
								string_Tx("@NSR#");							// 2022-02-21 시작문자 끝문자 추가
								/*scia_Tx('N');
								scia_Tx('S');
								scia_Tx('R');*/
								// DELAY_US(10000);
								DELAY_US(35000);							// 2019-12-23
							}
							else
							{
								NOW_State_ReQST();
								// DELAY_US(500);
								DELAY_US(70000);							// 2019-12-23
							}												// sending data to PC(UI/RI/TEMP/stage)

							All_Pump_Off();									// Pump 동작 정지

							if(Manual == 11){
								Mode = 4;
								PAUSE();
							}

							if(EMS != 1){
								if(Manual == 10){
									Manual = 11;
								}
								else
								{
									Now_Step++;
									istage++;
								}
							}
							else
							{
								Now_Step = istage;
							}
						}// if(EMS != 1) END
						else
						{
							Now_Step = istage;
						}
					}// END of Step end Check!!
				}// while(Now_Step != istage) END < Process END>
				All_Parts_Stop();
				SOL_Output();												// 모든 솔벨브 off

				string_Tx("@$$END#");										// 2022-02-21 시작문자 끝문자 추가
 				/*scia_Tx('$');												// 2019012023
				scia_Tx('$');
				scia_Tx('E');
				scia_Tx('N');
				scia_Tx('D');*/
				DELAY_US(ECHO_DELAY);

				Recipe[0][0] = '0';
				Recipe[0][1] = '0';
				Recipe[0][2] = '1';

				NGO = 0;
				Now_Step = 1, istage = 0;	EMS = 0;
			    Mode = 0;
				CPU_Timer0_Isr_cnt = 0;
/* Rx 통신 인터럽트 해제 */
				Rx_INT_STOP();
				break;

			case 3 :														// 2015.09.07 추가 - 전체루틴에서 STOP가 되도록 수정
				SOL_Output();												// 모든 솔벨브 off
				//string_Tx("$$END");										// PC로 Process END를 보고
				DELAY_US(1000);
				Recipe[0][0] = '0';
				Recipe[0][1] = '0';
				Recipe[0][2] = '1';
				NGO = 0;
				Now_Step = 1, istage = 0;	EMS = 0;
			    Mode = 0;
				CPU_Timer0_Isr_cnt = 0;
/* Rx 통신 인터럽트 해제 */
				Rx_INT_STOP();
				break;

			case 5 :		// manual data recipe
				PC_Data_Recive();                                       	// PC data 를 Recipe 배열에 삽입
				DELAY_US(1);
				Mode = 2;
				Manual = 1;
				goto Manual_run;
				//break;
			case 6 :		// sensor data request
				NOW_State_ReQST();
				DELAY_US(1000);												// 2019-12-23
				Mode = 0;
				break;
			case 8 :   // 초기화
				All_Parts_Stop();
				SOL_Output();												// 모든 솔벨브 off

				Motor_init();    											// motor initial
				Motor_Stop();

				Mode = 0;
				Double_EQ = 0;
				// string_Tx("$$RDY");                                  	// Excel_Data transfer request sends a command code
				break;

			case 9 :  // 초기화
				All_Parts_Stop();
				SOL_Output();												// 모든 솔벨브 off

				Motor_init();    											// motor initial
				Motor_Stop();

				Mode = 0;
				Double_EQ = 1;
				// string_Tx("$$RDY");                                  	// Excel_Data transfer request sends a command code
				break;

		}
	} // while(1) END

} // main END


void First_num()
{
	V_FLOW = 135;
	A_FLOW = 246;

	for(i=0;i<17;i++){
		sol[i] = 0;
	}
	//Recipe[0][0] = '0';Recipe[0][1] = '0';Recipe[0][2] = '0';
	Recipe[0][0] = '0';
	Recipe[0][1] = '0';
	Recipe[0][2] = '1';

	// Motor_init();    													// motor initial

	for(i=0;i<6;i++){
		temp0avr[i]=0;
		temp1avr[i]=0;
	}
	count0=1;
	count1=1;
	TCK0=0;
	TCK1 = 0;
	first_Temp0 = 1;
	istage = 0;
	EMS = 0;
	Double_EQ = 0;
	NGO = 0;
	Manual = 0;																		// Manaul mode request
	Mode = 0;																		// Mode=1 : PC_Data receive / Mode=2 : Process Run / Mode=3 : 비상정지
																					// Mode=4 : 일시정지 Mode=5 : Manual Mode
}
