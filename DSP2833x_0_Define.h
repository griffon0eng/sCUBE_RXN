//###########################################################################
//
// FILE:	DSP2833x_Motor Run.h
//
//###########################################################################
// USE GPIO0,GPIO1
// USE bit. NO PWM
// 2012.10.31
//###########################################################################

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

/************************기본설정 **************************************************/
#define		MAX_STAGE	400                       // 400이상은 불가 *용량 부족*
#define     NULL 0
#define		SYSTEM_CLOCK		150E6	/* 150MHz */
#define		TBCLK				150E6	/* 150MHz */
#define		PWMCARRIER			30E3	/* 30kHz */
#define 		ECHO_DELAY			300

/************************Process中 사용 변수 *****************************************/

// KNF PM17659-86과 같은 BLDC 내장형 다이어프램 펌프
#define PUMP_STOP_THRESHOLD    	200   				// 이 값보다 작으면 펌프 완전 정지 (5%)
#define PUMP_MIN_RUN_VAL      	600   				// 최소 회전을 보장하는 값 (15%)
#define PUMP_MAX_VAL          	4000  				// 최대 입력 값


char 	Recipe[MAX_STAGE][58];                     // PC로 부터 Data 받아 저장되는 배열

/*Motor*/

unsigned int Report_Count;
#pragma 	 DATA_SECTION(Motor_A,"SCUBE_A")      // 1st_82C55 Data 영역 설정
#pragma 	 DATA_SECTION(Motor_B,"SCUBE_B")      // 2nd_82C55 Data 영역 설정
#pragma 	 DATA_SECTION(SenSor_A,"SCUBE_E")     // 3rd_82C55 Data 영역 설정
#pragma 	 DATA_SECTION(SenSor_B,"SCUBE_F")     // 4th_82C55 Data 영역 설정

unsigned int Motor_A[4];                          // 1st_82C55
unsigned int Motor_B[4];                          // 2nd_82C55
unsigned int SenSor_A[4];                         // 3rd_82C55
unsigned int SenSor_B[4];                         // 4th_82C55

char    	 M_ValveCurPos[16];                   // 현재 모터 위치 '1','2','3','0'
char		 M_ValveRot[16];                      // 모터가 돌아야 할 방향 cw/ccw
char    	 M_ValveSetPos[16];                   // 모터가 가야할 위치 '1','2','3'
char 		 Pos;								  // Motor_Postion
Uint16 		 SS_ON_A[16];						  // Sensor On/Off 결과값 배열
Uint16 		 SS_ON_B[16];						  // Sensor On/Off 결과값 배열

Uint16 		 CW_SS_A_ON_COUNT[16];				  // CW방향일때 센서의 on 카운터 횟수
Uint16 		 CW_SS_A_OFF_COUNT[16];				  // CW방향일때 센서의 off 카운터 횟수
Uint16 		 CCW_SS_A_ON_COUNT[16];				  // CCW방향일때 센서의 on 카운터 횟수
Uint16 		 CCW_SS_A_OFF_COUNT[16];				  // CCW방향일때 센서의 off 카운터 횟수

Uint16 		 CW_SS_B_ON_COUNT[16];				  // CW방향일때 센서의 on 카운터 횟수
Uint16 		 CW_SS_B_OFF_COUNT[16];				  // CW방향일때 센서의 off 카운터 횟수
Uint16 		 CCW_SS_B_ON_COUNT[16];				  // CCW방향일때 센서의 on 카운터 횟수
Uint16 		 CCW_SS_B_OFF_COUNT[16];				  // CCW방향일때 센서의 off 카운터 횟수

/* Motor */
Uint16 		Motor_All_Check;
int         INI_CK;

/*Sol_Valve*/
int     	 sol[16];							  // SOL on/off 배열
char    	 S_Solvalve[20];					  // SOL on/off 배열   Recipe의 data를 S_Solvalve[20]로 옮긴후 sol[16]으로 다시 옮겨 on/off

/*232C통신*/
char 		 Order_Code[5];                      // Process 전에 명령어 받아와 Mode 결정하는 배열 	$$STF/$$RUN/$$MAN/$$UCM/$$OCM/$$EJT
char 		 Process_TEMP[2];					 // Process中 명령어를 받는 배열_빠른 속도처리를 위해 $$는 배열에서 제외	$$STP/$$PAS/$$RUN/$$CUT/$$NEX
char    	 temp;                               // RX 버퍼영역
int     	 Mode;								 // Mode=1 : PC_Data receive / Mode=2 : Process Run / Mode=3 : 비상정지
												 // Mode=4 : 일시정지 Mode=5 : Manual Mode
/*Heater*/
Uint16     Temp_Delay_Start_Heater[2];			 // Heater 의 설정온도에서의 Delay 시간
int        Current_Temp_Heater[2];				 // 계산되어진 ADC값의 Heater 온도
Uint16     SET_Temp_Heater[2];					 // 엑셀에서 가져온 설정온도
Uint16     Delay_SET_Time_Heater[2];			 // 엑셀에서 가져온 설정온도 Delay 시간
Uint16     first_Temp0,first_Temp1;
Uint16 	   Heater0_Delay_Time_End;
Uint16 	   Heater1_Delay_Time_End;

Uint16 temp0avr[6], temp1avr[6];
Uint16 count0, count1, TCK0 ,TCK1;


/*Step 진행 변수*/
int 	   Step_SET_Delay_Time;					 // 엑셀에서 가져온 Step의 Delay 시간
Uint16     EMS;									 // 비상정지 확인변수  0 : 정상  1 : 비상정지 명령수신 or 모든 스텝 종료
Uint16 	   istage;								 // 진행 Stage-1  1Step -> istage = 0 / 2Step -> istage = 1
Uint16 	   i;									 // Process中 사용하는변수
Uint16     Manual;
Uint16     NGO;
Uint16     End_step;
//Uint16     Now_Step;                       // Step 을 진행하기위한 변수


/*CPU Timer*/
float 	   CPU_Timer0_Isr_cnt;					 // Process 에 사용하는 CPUTimer
float 	   CPU_Timer;							 // Process 에 사용하는 CPUTimer


/*RI/UV Data*/
Uint16 	   RI[4];								 // 1~3의 RI 값

/*Flow*/
Uint16    V_FLOW;
Uint16    A_FLOW;

Uint16    Next_istage;

/*Test 용 */
Uint16        asd_test;							 // Test 용 삭제해도 됨

// #define PUMP_MIN_RUN_VAL      600    // 최소 15% 속도 보장 (이하로 설정 시 펌프 보호) [cite_start]
// #define KICK_START_DURATION   2      // 0.1초 타이머 기준 200ms 유지 [cite: 33-35]
// #define KICK_START_DUTY       0.5    // 기동 시 50% 출력 (Inverse Logic: 0.5)

// Uint16 Pump1_Kick_Cnt = 0;
// Uint16 Prev_Pump1_Status = 0;        // 0: 정지상태, 1: 가동상태

// 초기화 상태 관리용
Uint16 Motor_Init_Step;   			// 0: 대기, 1: 시작/기동, 2: 위치 추적 중, 3: 완료
Uint16 Motor_Homed[16];      			// 각 모터별 완료 플래그 (0 또는 1)
Uint16 Home_Complete_Cnt;


#ifdef __cplusplus
}
#endif /* extern "C" */

//===========================================================================
// End of file.
//===========================================================================

