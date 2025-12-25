//###########################################################################
//
// FILE:	DSP2833x_Process.h
//
//###########################################################################
// main Process 에서 사용하는 함수들 정의
// 2013.02.28
//###########################################################################


#ifdef __cplusplus
extern "C" {
#endif

#include "DSP2833x_0_Define.h"
//#include "DSP2833x_0_232C_TX_RX.h"
/*****************************************************************************************************************************************/
void Heater_Check(int istage);					   									// Check of the delay time of EXCEL.

void Pump_Check();								   									// Check of Pump(0~4000) of EXCEL
void Pump_Output();						  		   									// Pump Output

void NOW_State_ReQST();   						   								// sending data to PC(UI/RI/TEMP/stage)

char scia_Rx(void);
void scia_Tx(char data);
void string_Tx(const char * msg);
void PAUSE();									   											// Pause Mode check
void All_Pump_Off();
void AIR_Pump_off();

void Rx_INT_Init();
void Rx_INT_STOP();
/*********************************************************************************************************************** Heater start  ****/
float 	Pump1,Pump2;								// INT of Pump
float	V_Pump1,V_Pump2;			 				// PWM of Pump <1 : Stop -> 0 : Max Run>

Uint16 	il1, il2, Recipe1, Recipe2;					// INT of Recipe

/* 실제로 메모리를 할당하는 정의(Definition) */
Uint16 Home_Complete_Cnt = 0;
Uint16 Motor_Init_Step = 0;
Uint16 Motor_Homed[16] = {0,};

float Temp1_CK, Temp2_CK;
int Temp1_Buf, Temp2_Buf;

void Heater_Check(int istage)														// Check of the delay time of EXCEL. 191020NewMB
{
	SET_Temp_Heater[0] = 0;															// Set TEMP of Now stage Heater0
	Delay_SET_Time_Heater[0] = 0;													// Set TEMP Delay Time of Now stage Heater0

	/*SET_Temp_Heater[0] = SET_Temp_Heater[0] + ((Recipe[istage][8]-48)*1);							// 1	point
	SET_Temp_Heater[0] = SET_Temp_Heater[0] + ((Recipe[istage][7]-48)*10);							// 10	point
	SET_Temp_Heater[0] = SET_Temp_Heater[0] + ((Recipe[istage][6]-48)*100);							// 100	point

	Delay_SET_Time_Heater[0] = Delay_SET_Time_Heater[0] + ((Recipe[istage][11]-48)*1);
	Delay_SET_Time_Heater[0] = Delay_SET_Time_Heater[0] + ((Recipe[istage][10]-48)*10);
	Delay_SET_Time_Heater[0] = Delay_SET_Time_Heater[0] + ((Recipe[istage][9]-48)*100);

	SET_Temp_Heater[1] = 0;    																		// Set TEMP of Now stage Heater1
	Delay_SET_Time_Heater[1] = 0;*/																	// Set TEMP Delay Time of Now stage Heater1

	SET_Temp_Heater[0] = SET_Temp_Heater[0] + ((Recipe[istage][14]-48)*1);
	SET_Temp_Heater[0] = SET_Temp_Heater[0] + ((Recipe[istage][13]-48)*10);
	SET_Temp_Heater[0] = SET_Temp_Heater[0] + ((Recipe[istage][12]-48)*100);

	Delay_SET_Time_Heater[0] = Delay_SET_Time_Heater[0] + ((Recipe[istage][17]-48)*1);
	Delay_SET_Time_Heater[0] = Delay_SET_Time_Heater[0] + ((Recipe[istage][16]-48)*10);
	Delay_SET_Time_Heater[0] = Delay_SET_Time_Heater[0] + ((Recipe[istage][15]-48)*100);
}



void Heater0_Auto()																			    // Process of Heater 20160527
{																								// 191020 NewMB SSR_1 -> SSR_2, cool sol_1 -> cool sol_2
	Heater0_Delay_Time_End = 0;																	// Heater1_Auto 설정 그대로 가져옴
	Temp1_CK = CPU_Timer0_Isr_cnt;
	Temp1_Buf = Current_Temp_Heater[0];
/////현재온도가 설정온도보다 낮을때//Heating System//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*if(SET_Temp_Heater[0] < 70){                                                                    // Temp < 70
		while(((SET_Temp_Heater[0] - 7) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){	// Set TEMP-7 Heater0 Run
			GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
		    DELAY_US(10000);																	    // 0.1sec Delay
		    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
		    NOW_State_ReQST();																	    // sending data to PC(UI/RI/TEMP/stage)
		    DELAY_US(40000);																	    // 0.4sec Delay
		    Current_Temp_Heater[0] = Current_Temp_Heater[0];
		    PAUSE();                                                                                // Pause Mode check

		    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
		    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
		    		Delay_SET_Time_Heater[0]=0;
		    		Heater0_Delay_Time_End=1;
		    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
		    		}
		    	}
		    }
		GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
		}
	else if(SET_Temp_Heater[0] < 130){                                                              // Temp < 130
		while(((SET_Temp_Heater[0] - 5) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){	// Set TEMP-5 Heater0 Run
			GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
		    DELAY_US(10000);																	    // 0.1sec Delay
		    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
		    NOW_State_ReQST();																	    // sending data to PC(UI/RI/TEMP/stage)
		    DELAY_US(40000);																	    // 0.4sec Delay
		    Current_Temp_Heater[0] = Current_Temp_Heater[0];
		    PAUSE();                                                                                // Pause Mode check

		    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
		    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
		    		Delay_SET_Time_Heater[0]=0;
		    		Heater0_Delay_Time_End=1;
		    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
		    		}
		    	}
		    }
		GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
		}
	else{                                                                                           // Temp > 130
		while(((SET_Temp_Heater[0] - 3) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){   // Set TEMP-3` Heater0 Run
			GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
		    DELAY_US(30000);																	    // 0.2sec Delay
		    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
		    NOW_State_ReQST();																    	// sending data to PC(UI/RI/TEMP/stage)
		    DELAY_US(40000);																    	// 0.4sec Delay
		    Current_Temp_Heater[0] = Current_Temp_Heater[0];
		    PAUSE();                                                                                // Pause Mode check

		    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
		    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
		    		Delay_SET_Time_Heater[0]=0;
		    		Heater0_Delay_Time_End=1;
		    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
		    		}
		    	}
		    }
		GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
		}*/
/////220728 가열속도 수정 다이어프램펌프 쿨링//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(SET_Temp_Heater[0] < 70){                                                                    // Temp < 70
			while(((SET_Temp_Heater[0] - 7) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){	// Set TEMP-7 Heater0 Run
				GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
			    DELAY_US(10000);																	    // 0.1sec Delay
			    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
			    NOW_State_ReQST();																	    // sending data to PC(UI/RI/TEMP/stage)
			    DELAY_US(40000);																	    // 0.4sec Delay
			    Current_Temp_Heater[0] = Current_Temp_Heater[0];
			    PAUSE();                                                                                // Pause Mode check

			    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
			    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
			    		Delay_SET_Time_Heater[0]=0;
			    		Heater0_Delay_Time_End=1;
			    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
			    		}
			    	}
			    }
			GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
			}
		else if(SET_Temp_Heater[0] < 100){                                                              // Temp < 100
			while(((SET_Temp_Heater[0] - 7) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){	// Set TEMP- Heater0 Run
				GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
			    DELAY_US(15000);																	    // 0.15sec Delay
			    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
			    NOW_State_ReQST();																	    // sending data to PC(UI/RI/TEMP/stage)
			    DELAY_US(40000);																	    // 0.4sec Delay
			    Current_Temp_Heater[0] = Current_Temp_Heater[0];
			    PAUSE();                                                                                // Pause Mode check

			    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
			    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
			    		Delay_SET_Time_Heater[0]=0;
			    		Heater0_Delay_Time_End=1;
			    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
			    		}
			    	}
			    }
			GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
			}
		else if(SET_Temp_Heater[0] < 130){                                                              // Temp < 130
					while(((SET_Temp_Heater[0] - 5) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){	// Set TEMP-5 Heater0 Run
						GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
					    DELAY_US(20000);																	    // 0.2sec Delay
					    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
					    NOW_State_ReQST();																	    // sending data to PC(UI/RI/TEMP/stage)
					    DELAY_US(40000);																	    // 0.4sec Delay
					    Current_Temp_Heater[0] = Current_Temp_Heater[0];
					    PAUSE();                                                                                // Pause Mode check

					    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
					    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
					    		Delay_SET_Time_Heater[0]=0;
					    		Heater0_Delay_Time_End=1;
					    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
					    		}
					    	}
					    }
					GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
					}
		else if(SET_Temp_Heater[0] < 160){                                                              // Temp < 160
					while(((SET_Temp_Heater[0] - 5) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){	// Set TEMP-5 Heater0 Run
						GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
					    DELAY_US(30000);																	    // 0.3sec Delay
					    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
					    NOW_State_ReQST();																	    // sending data to PC(UI/RI/TEMP/stage)
					    DELAY_US(40000);																	    // 0.4sec Delay
					    Current_Temp_Heater[0] = Current_Temp_Heater[0];
					    PAUSE();                                                                                // Pause Mode check

					    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
					    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
					    		Delay_SET_Time_Heater[0]=0;
					    		Heater0_Delay_Time_End=1;
					    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
					    		}
					    	}
					    }
					GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
					}
		else{                                                                                           // Temp > 160
			while(((SET_Temp_Heater[0] - 3) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){   // Set TEMP-3` Heater0 Run
				GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
			    DELAY_US(30000);																	    // 0.3sec Delay
			    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
			    NOW_State_ReQST();																    	// sending data to PC(UI/RI/TEMP/stage)
			    DELAY_US(40000);																    	// 0.4sec Delay
			    Current_Temp_Heater[0] = Current_Temp_Heater[0];
			    PAUSE();                                                                                // Pause Mode check

			    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
			    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
			    		Delay_SET_Time_Heater[0]=0;
			    		Heater0_Delay_Time_End=1;
			    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
			    		}
			    	}
			    }
			GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
			}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////220318 170도 미만 가열속도 동일화
		/*if(SET_Temp_Heater[0] < 170){                                                                   // Temp < 170
			while(((SET_Temp_Heater[0] - 7) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){	// Set TEMP-7 Heater0 Run
				GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
			    DELAY_US(30000);																	    // 0.3sec Delay
			    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
			    NOW_State_ReQST();																	    // sending data to PC(UI/RI/TEMP/stage)
			    DELAY_US(40000);																	    // 0.4sec Delay
			    Current_Temp_Heater[0] = Current_Temp_Heater[0];
			    PAUSE();                                                                                // Pause Mode check

			    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
		    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
		    		Delay_SET_Time_Heater[0]=0;
		    		Heater0_Delay_Time_End=1;
		    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
		    		}
		    	}
		    }
			GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
		}
		else{                                                                                           // Temp > 180
			while(((SET_Temp_Heater[0] - 3) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){   // Set TEMP-3` Heater0 Run
				GpioDataRegs.GPADAT.bit.GPIO1 = 1; 													    // Heater0 Run
			    DELAY_US(40000);																	    // 0.4sec Delay
			    GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										    // Heater0 Stop
			    NOW_State_ReQST();																    	// sending data to PC(UI/RI/TEMP/stage)
			    DELAY_US(40000);																    	// 0.4sec Delay
			    Current_Temp_Heater[0] = Current_Temp_Heater[0];
			    PAUSE();                                                                                // Pause Mode check

			    if((Temp1_CK+350)==CPU_Timer0_Isr_cnt){
			    	if(Current_Temp_Heater[0]< (Temp1_Buf+5)){
			    		Delay_SET_Time_Heater[0]=0;
			    		Heater0_Delay_Time_End=1;
			    		Step_SET_Delay_Time=0; EMS=1; NGO = 1;
			    		}
			    	}
			    }
			GpioDataRegs.GPADAT.bit.GPIO1 = 0;	DELAY_US(1);
			}*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////현재온도가 설정온도보다 높을때////Cooling System/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	V_Pump2 = 0;	Pump_Output();	DELAY_US(1);
	while((SET_Temp_Heater[0] +2 < Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){ 		// Set TEMP+2` Cooling Run
		GpioDataRegs.GPBDAT.bit.GPIO58 = 1;	DELAY_US(1);									// Cooling Run		//190830 NewMB
		NOW_State_ReQST();																	// sending data to PC(UI/RI/TEMP/stage)
		DELAY_US(60000);
		PAUSE();																			// Pause Mode check
	}
	AIR_Pump_off();	DELAY_US(1);
	GpioDataRegs.GPBDAT.bit.GPIO58 = 0;	DELAY_US(1);										//190830 NewMB
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////Heater Delay Time 시작//////Cooling System->Heating System-> Cycle/////////////////////////////////////////////////////////////////////////////////////
 	CPU_Timer0_Isr_cnt = 0;
 	while(Delay_SET_Time_Heater[0]*10 > CPU_Timer0_Isr_cnt){									// Heater0 Delay start
//Cooling System
 		V_Pump2 = 0;	Pump_Output();	DELAY_US(1);
 		while((SET_Temp_Heater[0] <= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){
 		 			GpioDataRegs.GPBDAT.bit.GPIO58 = 1;	DELAY_US(1);						// Cooling SOL on			//190830 NewMB
 		 			if(Delay_SET_Time_Heater[0]*10 <= CPU_Timer0_Isr_cnt){						// Heater1 Delay Time Check
 		 				SET_Temp_Heater[0] = 300;											// 'While' shall establish the conditions for the go out
 		 				Heater0_Delay_Time_End = 1;
 		 			}
 		 			NOW_State_ReQST();														// sending data to PC(UI/RI/TEMP/stage)
 		 			DELAY_US(70000);
 		 			PAUSE();																// Pause Mode check
 		 		}
 		AIR_Pump_off();	DELAY_US(1);
 		GpioDataRegs.GPBDAT.bit.GPIO58 = 0;	DELAY_US(1);									// Heater 냉각 SOL off		//190830 NewMB
//Heating System
 		while(((SET_Temp_Heater[0] - 1) >= Current_Temp_Heater[0])&&(Heater0_Delay_Time_End!=1)){
 			if(SET_Temp_Heater[0] < 110){                                                 // Temp < 110
 		 		GpioDataRegs.GPADAT.bit.GPIO1 = 1; 											// Heater0 Run
		 		DELAY_US(10000);															// 0.1sec Delay
 		 		GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);								// Heater0 Stop
 		 		NOW_State_ReQST();															// sending data to PC(UI/RI/TEMP/stage)
 		 		//DELAY_US(60000);															// 0.6sec Delay
 		 		DELAY_US(40000);															// 0.4sec Delay 220221 수정
 		 		PAUSE();																	// Pause Mode check
	 		}
 			else if(SET_Temp_Heater[0] < 140){                                              // Temp < 140
 				GpioDataRegs.GPADAT.bit.GPIO1 = 1; 	DELAY_US(1);							// Heater1 Run
 				DELAY_US(18000);															// 0.18sec Delay
 				//DELAY_US(25000);															// 0.25sec Delay 220221 수정
 				GpioDataRegs.GPADAT.bit.GPIO1 = 0; 		DELAY_US(1);						// Heater1 Stop
 				NOW_State_ReQST();
 				//DELAY_US(60000);															// 0.6sec Delay
 				DELAY_US(40000);															// 0.4sec Delay 220221 수정
 				PAUSE();																	// Pause Mode check
			}
			else if(SET_Temp_Heater[0] < 180){                                              // Temp < 180 180829 추가
 				GpioDataRegs.GPADAT.bit.GPIO1 = 1; 	DELAY_US(1);							// Heater1 Run
 				DELAY_US(20000);															// 0.2sec Delay
 				//DELAY_US(30000);															// 0.3sec Delay 220221 수정
 				GpioDataRegs.GPADAT.bit.GPIO1 = 0; 		DELAY_US(1);						// Heater1 Stop
 				NOW_State_ReQST();
 				//DELAY_US(40000);															// 0.4sec Delay
 				DELAY_US(10000);															// 0.1sec Delay 220318 수정
 				PAUSE();																	// Pause Mode check
			}
 			else{                                                                           // Temp > 180 180910 추가
 				GpioDataRegs.GPADAT.bit.GPIO1 = 1; 	DELAY_US(1);							// Heater1 Run
 				DELAY_US(30000);															// 0.3sec Delay
 				GpioDataRegs.GPADAT.bit.GPIO1 = 0; 		DELAY_US(1);						// Heater1 Stop
 				NOW_State_ReQST();
 				DELAY_US(40000);															// 0.4sec Delay
 				PAUSE();
			}
	 		if(Delay_SET_Time_Heater[0]*10 <= CPU_Timer0_Isr_cnt){						// Heater  Delay Time Check
	 			SET_Temp_Heater[0] = 5;													// 'While' shall establish the conditions for the go out
	 			Heater0_Delay_Time_End = 1;
			}
////220715 온도 유지중 가열시간 조정///////////////////////////////////////////////////////////////////////////////////////
 			/*if(SET_Temp_Heater[0] < 100){                                                	 // Temp < 100
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 1; 											// Heater0 Run
 				DELAY_US(5000);																// 0.05sec Delay
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);								// Heater0 Stop
 			 	NOW_State_ReQST();															// sending data to PC(UI/RI/TEMP/stage)
 			 	DELAY_US(40000);															// 0.4sec Delay
 			 	PAUSE();																	// Pause Mode check
		 	}
 			else if(SET_Temp_Heater[0] < 120){                                              // Temp < 120
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 1; 	DELAY_US(1);							// Heater1 Run
 			 	DELAY_US(6000);																// 0.06sec Delay
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 0; 		DELAY_US(1);						// Heater1 Stop
 			 	NOW_State_ReQST();
 			 	DELAY_US(30000);															// 0.3sec Delay
 			 	PAUSE();																	// Pause Mode check
 			 }
 			else if(SET_Temp_Heater[0] < 130){                                              // Temp < 130
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 1; 	DELAY_US(1);							// Heater1 Run
 			 	DELAY_US(7000);																// 0.07sec Delay
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 0; 		DELAY_US(1);						// Heater1 Stop
 				NOW_State_ReQST();
 			 	DELAY_US(30000);															// 0.3sec Delay
 			 	PAUSE();																	// Pause Mode check
 			}
 			else if(SET_Temp_Heater[0] < 150){                                              // Temp < 150
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 1; 	DELAY_US(1);							// Heater1 Run
 			 	DELAY_US(8000);																// 0.08sec Delay
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 0; 		DELAY_US(1);						// Heater1 Stop
 			 	NOW_State_ReQST();
 			 	DELAY_US(30000);															// 0.3sec Delay
 			 	PAUSE();																	// Pause Mode check
 			 }
 			else if(SET_Temp_Heater[0] < 160){                                              // Temp < 160
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 1; 	DELAY_US(1);							// Heater1 Run
 			 	DELAY_US(10000);															// 0.1sec Delay
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 0; 		DELAY_US(1);						// Heater1 Stop
 			 	NOW_State_ReQST();
 			 	DELAY_US(30000);															// 0.3sec Delay
 			 	PAUSE();																	// Pause Mode check
 			 }
 			else{                                                                           // Temp > 160
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 1; 	DELAY_US(1);							// Heater1 Run
 			 	DELAY_US(20000);															// 0.2sec Delay
 			 	GpioDataRegs.GPADAT.bit.GPIO1 = 0; 		DELAY_US(1);						// Heater1 Stop
 			 	NOW_State_ReQST();
 			 	DELAY_US(30000);															// 0.3sec Delay
 			 	PAUSE();
 			}*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 		}
 		GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);										// Heater0 Stop
}		//Heater Delay Time END
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}		// Heater0_Auto() END
void Heater1_Auto()
{
		// 191020 히터제어 제거 Heater0_Auto와 동일한 내용이었음
}		// Heater1_Auto() END

/*********************************************************************************************************************** Heater END  ****/
/*********************************************************************************************************************** Pump start  ****/
void Pump_Auto()													//	Pump Auto Mode
{
	Pump_Output();
}

void Pump_Check(int istage)
{
    float raw_pump_val = 0;

    // 1. 레시피로부터 0~4000 사이의 값 읽기
    raw_pump_val = raw_pump_val + ((Recipe[istage][37]-48)*1);
    raw_pump_val = raw_pump_val + ((Recipe[istage][36]-48)*10);
    raw_pump_val = raw_pump_val + ((Recipe[istage][35]-48)*100);
    raw_pump_val = raw_pump_val + ((Recipe[istage][34]-48)*1000);

    // 2. Dead-zone 및 모터 보호 로직 적용
    if (raw_pump_val < PUMP_STOP_THRESHOLD) {
        // 아주 낮은 입력은 펌프 보호를 위해 강제 정지
        Pump1 = 0;
    }
    else if (raw_pump_val < PUMP_MIN_RUN_VAL) {
        // 기동 토크 확보를 위해 최소 구동 속도로 상향 조정
        Pump1 = PUMP_MIN_RUN_VAL;
    }
    else {
        // 정상 범위 (Max 값 초과 방지)
        Pump1 = (raw_pump_val > PUMP_MAX_VAL) ? PUMP_MAX_VAL : raw_pump_val;
    }

    // 3. PWM 듀티비 계산 (Inverse Logic 반영)
    // V_Pump1 = 1: Stop (0% Duty) / V_Pump1 = 0: Max (100% Duty)
    V_Pump1 = 1.0 - (Pump1 / PUMP_MAX_VAL);

    V_Pump2 = 1.0; // 펌프 2는 기본적으로 정지 상태 유지
}

void Pump_Manual(int Pump_No, float Pump_Speed, int Pump_RunTime)
{
	if(Pump_No == 1){
		CPU_Timer0_Isr_cnt = 0;
		while(Pump_RunTime > CPU_Timer0_Isr_cnt){
		EPwm2Regs.TBPRD = (TBCLK/PWMCARRIER)/2;
		EPwm2Regs.CMPA.half.CMPA = (EPwm2Regs.TBPRD) * Pump_Speed;  // 날선후
		//EPwm2Regs.CMPB = (EPwm2Regs.TBPRD) * Pump_Speed;  // 날선 전
		}
	}
	if(Pump_No == 2){
		CPU_Timer0_Isr_cnt = 0;
		while(Pump_RunTime > CPU_Timer0_Isr_cnt){
		EPwm3Regs.TBPRD = (TBCLK/PWMCARRIER)/2;
		EPwm3Regs.CMPB = (EPwm3Regs.TBPRD) * Pump_Speed;   //날선후
		//EPwm3Regs.CMPA.half.CMPA = (EPwm3Regs.TBPRD) * Pump_Speed;  // 날선전
		}
	}
}

void Pump_Output()													// Pump OutPut
{
	EPwm2Regs.TBPRD = (TBCLK/PWMCARRIER)/2; 	DELAY_US(1);
	EPwm2Regs.CMPA.half.CMPA = (EPwm2Regs.TBPRD) * V_Pump1;  DELAY_US(1);// 날선후
	//EPwm2Regs.CMPB = (EPwm2Regs.TBPRD) * V_Pump1;  // 날선 전

	EPwm3Regs.TBPRD = (TBCLK/PWMCARRIER)/2; 	DELAY_US(1);
	//EPwm3Regs.CMPA.half.CMPA = (EPwm3Regs.TBPRD) * V_Pump2; 	DELAY_US(1);
	EPwm3Regs.CMPB = (EPwm3Regs.TBPRD) * V_Pump2;  DELAY_US(1); // 날선 전
}
void All_Pump_Off()													// Pump 1,2 Stop
{
	EPwm2Regs.TBPRD = (TBCLK/PWMCARRIER)/2; 	DELAY_US(1);
	EPwm2Regs.CMPA.half.CMPA = (EPwm2Regs.TBPRD) * 1;  DELAY_US(1);// 날선후

	EPwm3Regs.TBPRD = (TBCLK/PWMCARRIER)/2; 	DELAY_US(1);
	EPwm3Regs.CMPB = (EPwm3Regs.TBPRD) * 1;  DELAY_US(1); // 날선 전
}
void AIR_Pump_off()
{
	EPwm3Regs.TBPRD = (TBCLK/PWMCARRIER)/2; 	DELAY_US(1);
	EPwm3Regs.CMPB = (EPwm3Regs.TBPRD) * 1;  DELAY_US(1); // 날선 전

}
/*********************************************************************************************************************** Pump END  ****/
/*********************************************************************************************************************** Process   ****/
void Delay_Time_Check(int istage)									// Step Delay Time Check
{
  int a1,a2,a3;
  a1 = ((Recipe[istage][5]-48)*1);
  a2 = ((Recipe[istage][4]-48)*10);
  a3 = ((Recipe[istage][3]-48)*100);
  Step_SET_Delay_Time=a1+a2+a3;
}
/********************************************************************************************************************* Process END  ****/
/********************************************************************************************************************* Process 中 com Start**/

char Proscia_Rx(void)												// Ready signal is not affected by
{																	// For Test
	char data;
	if(SciaRegs.SCIRXST.bit.RXRDY & 1)
	data = SciaRegs.SCIRXBUF.bit.RXDT;	 // Read data
	return data;
}
char scia_Rx(void)													// Receive data _ PC -> DSP
{																	// Continue to wait until it receives data
	char data;														// Apply the data ready signal when receive a ready signal
	while(!(SciaRegs.SCIRXST.bit.RXRDY));
	data = SciaRegs.SCIRXBUF.bit.RXDT;	 // Read data
	return data;
}
void scia_Tx(char data)												// Send data _ DSP -> PC
{																	// One character or One of the variables
	while(!(SciaRegs.SCICTL2.bit.TXRDY));
    SciaRegs.SCITXBUF = data & 0x00FF;
}
char scib_Rx(void)													// Other 232C channels_ Now Unused
{
	char data;
	while(!(ScibRegs.SCIRXST.bit.RXRDY));
	data = ScibRegs.SCIRXBUF.bit.RXDT;	 // Read data
	return data;
}
void scib_Tx(char data)												// Other 232C channels_ Now Unused
{
	while(!(ScibRegs.SCICTL2.bit.TXRDY));
    ScibRegs.SCITXBUF = data & 0x00FF;
}
void string_Tx(const char * msg)									// Send a text
{
	char ch;
	if(msg)
		while(0 != (ch = *(msg++))){
			scia_Tx(ch);
		}
}

void Order_COM()													// Check the Order code received from PC
{
	CCK:
	Order_Code[0] = scia_Rx();

	if(Order_Code[0] =='$'){
		for(i=1;i<5;i++){
			Order_Code[i]=scia_Rx();
			if(Order_Code[3]=='F'){									// 191205P&F와 내용 일치
				Order_Code[3]='.';
				goto CCK;
			}
		}        // Do not modify, and delete
		if(Order_Code[1]!='$'){										// 191205P&F와 내용 일치
			for(i=1;i<5;i++){
				Order_Code[i]='.';
				goto CCK;
			}
		}       // Do not modify, and delete

		if(Order_Code[2]=='S' && Order_Code[3]=='T' && Order_Code[4]=='F'){
			string_Tx("@$$STF#");									// 2022-02-21 시작문자 끝문자 추가
			// DELAY_US(ECHO_DELAY);
			/*scia_Tx('$');											// 2019-12-23
			scia_Tx('$');
			scia_Tx('S');
			scia_Tx('T');
			scia_Tx('F');*/
			DELAY_US(ECHO_DELAY);

			Mode = 1;												// Data TR Mode
		}
		if(Order_Code[2]=='R' && Order_Code[3]=='U' && Order_Code[4]=='N'){
			string_Tx("@$$RUN#");									// 2022-02-21 시작문자 끝문자 추가
			// DELAY_US(ECHO_DELAY);
			/*scia_Tx('$');
			scia_Tx('$');
			scia_Tx('R');
			scia_Tx('U');
			scia_Tx('N');*/
			DELAY_US(ECHO_DELAY);

			Mode = 2;												// Run Mode
		}

		if(Order_Code[2]=='S' && Order_Code[3]=='T' && Order_Code[4]=='P'){

			string_Tx("@$$STP#");									// 2022-02-21 시작문자 끝문자 추가
			// DELAY_US(ECHO_DELAY);
			/*scia_Tx('$');
			scia_Tx('$');
			scia_Tx('S');
			scia_Tx('T');
			scia_Tx('P');*/
			DELAY_US(ECHO_DELAY);

			EMS = 1;
			NGO = 1;																// 각 for/while 문들을 빠져나가기위한
			Step_SET_Delay_Time = 0;
			Delay_SET_Time_Heater[1] = 0;											// 조건들 수정
			Heater1_Delay_Time_End = 1;

			Mode = 3;
		}           																// STOP Mode

		if(Order_Code[2]=='M' && Order_Code[3]=='A' && Order_Code[4]=='N'){
			string_Tx("@$$MAN#");									// 2022-02-21 시작문자 끝문자 추가
			// DELAY_US(ECHO_DELAY);
			/*scia_Tx('$');
			scia_Tx('$');
			scia_Tx('M');
			scia_Tx('A');
			scia_Tx('N');*/
			DELAY_US(ECHO_DELAY);

			Mode = 5;																// Manual Mode
		}

		if(Order_Code[2]=='C' && Order_Code[3]=='S' && Order_Code[4]=='4'){
			Mode = 6; 																// 현재 정보 보고
		}
		if(Order_Code[2]=='I' && Order_Code[3]=='N' && Order_Code[4]=='C'){
			string_Tx("@$$INC#");									// 2022-02-21 시작문자 끝문자 추가
			// DELAY_US(ECHO_DELAY);
			/*scia_Tx('$');
			scia_Tx('$');
			scia_Tx('I');
			scia_Tx('N');
			scia_Tx('C');*/
			DELAY_US(ECHO_DELAY);

			Mode = 8;																// 초기화 1EQ
		}
		if(Order_Code[2]=='I' && Order_Code[3]=='N' && Order_Code[4]=='D'){
			string_Tx("@$$IND#");									// 2022-02-21 시작문자 끝문자 추가
			// DELAY_US(ECHO_DELAY);
			/*scia_Tx('$');
			scia_Tx('$');
			scia_Tx('I');
			scia_Tx('N');
			scia_Tx('D');*/
			DELAY_US(ECHO_DELAY);

			Mode = 9;																// 초기화 2EQ
		}
		if(Order_Code[2]=='U' && Order_Code[3]=='C' && Order_Code[4]=='M'){
			Mode = 10;															// UnClamp Mode
		}
		if(Order_Code[2]=='O' && Order_Code[3]=='C' && Order_Code[4]=='M'){
			Mode = 11;															// Clamp Mode
		}
		if(Order_Code[2]=='E' && Order_Code[3]=='J' && Order_Code[4]=='T'){
			Mode = 12;															// Eject Mode
		}
		if(Order_Code[2]=='C' && Order_Code[3]=='I' && Order_Code[4]=='D'){		// 191202 시작종료 제거 S,__,E
			string_Tx("@CID_01-19-004_RXN#");									// 2022-02-21 시작문자 끝문자 추가, 표시형식 변경
			DELAY_US(ECHO_DELAY);
		}
		if(Order_Code[2]=='F' && Order_Code[3]=='W' && Order_Code[4]=='V'){		//190424 FirmWareVer
			string_Tx("@FWV_V8_02_02_DEV_RXN#");									// 2022-02-21 시작문자 끝문자 추가, 표시형식 변경
			DELAY_US(ECHO_DELAY);
		}
		if(Order_Code[2]=='R' && Order_Code[3]=='M' && Order_Code[4]=='I'){				//191210 모터축 이니셜
			string_Tx("@Motor_Init#");											// 2022-02-21 시작문자 끝문자 추가
			DELAY_US(ECHO_DELAY);
			Motor_Init_2();
		}
		/* Use only in RUN mode*/
		/* STP : Emergency Stop		EMS = 1
		   PAS : Pause	 			Pause is released when 'RUN' the Order code
		   CUT : SOL 하나가 열리고 잠김	SOL 안정해짐_명령코드만 받음_수행안함
		   NEX : NEXT				Forced to proceed to the next step
		*/
		}// if(Order_Code[0] =='$') end
} // void Order_COM() end

void PC_Data_Recive()
{
	Recipe1 = 0;
	Recipe2 = 0;
	for(il1=0;il1<MAX_STAGE;il1++){
		for(il2=0;il2<59;il2++) {
			Recipe[il1][il2]=';';
		}        //  Recipe reset.
	}
/* Excel_Data -> Recipe 배열에 저장*/
	if(Mode == 5){	// Manual Mode Data Recive
		// string_Tx("$$RDY");                                         // Excel_Data transfer request sends a command code
		temp = scia_Rx();
		if(temp == 'S'){}
		for(Recipe2=0;Recipe2<59;Recipe2++){
			temp = scia_Rx();
			if(temp == 'E'){
				Recipe2 = 60;
			}
			else{
				Recipe[istage][Recipe2] = temp;						//  Recipe Save
			}
		}	// for(test2=0;test2<68;test2++) end
	}   // Manual Mode Data Recive End
	else{
		// string_Tx("$$RDY");                                         // Excel_Data transfer request sends a command code
		while(Recipe2 != 61){
			for(Recipe2=0;Recipe2<59;Recipe2++){
				temp = scia_Rx();
				if(temp == '$'){										// Excel_Data transfer is complete & OrderCode check_$$EDF
					Recipe2=60;
				}
				else{
					if(temp == 'E'){									// Step End Code Check (Step change)
						Recipe1++;
					}
					else if(temp == 'S'){
						Recipe2--;
					}
					else{												// or Recipe Save
						Recipe[Recipe1][Recipe2] = temp;
					}
				}
			}	// for(test2=0;test2<68;test2++) end
		} 	// while END											   All data has been saved

		while(temp!='F'){												// Check_$$EDF
			temp = scia_Rx();
		}

		End_step = Recipe1;
		if((Recipe[0][0]=='0')&&(Recipe[0][1]=='0')&&(Recipe[0][2]=='1')&&(Recipe[Recipe1-1][55]!=';')){	// Data Check (Stage1 = 001 / Last Stage Data)
			scia_Tx('E');																					// Report that saving data is completed
			Mode=0;																							// In order to change the mode to the next mode
		}
	} // else end
}	//	PC_Data_Recive END

void NOW_State_ReQST()																// 현재상태보고
{
	int Num_100, Num_10, Num_1;
	Num_100 = 0;
	Num_10 = 0;
	Num_1 = 0;

	scia_Tx('@');																						// 220221 추가
	scia_Tx('S');																					//  보고 시작

	// Step 번호
	/*for(i=0;i<3;i++){
		scia_Tx( Recipe[istage][i] );
	}*/

	scia_Tx( Recipe[istage][0] );								// 220406 프로세스 넘버 출력방식 변경
	scia_Tx( Recipe[istage][1] );								// 220406 프로세스 넘버 출력방식 변경
	scia_Tx( Recipe[istage][2] );								// 220406 프로세스 넘버 출력방식 변경

	//온도센서 1
/*	Num_100 = 0;
	Num_10 = 0;
	Num_1 = 0;

	Num_100   = (Current_Temp_Heater[0]/100)+48;									// 100point	ASCII Code = 'o' + 48
	Num_10    = ((Current_Temp_Heater[0]%100)/10)+48;							// 10point	ASCII Code = 'o' + 48
	Num_1     = ((Current_Temp_Heater[0]%100)%10)+48;							// 1point	ASCII Code = 'o' + 48
*/
	scia_Tx('0');
	scia_Tx('0');
	scia_Tx('0');

	//온도센서 2
	Num_100 = 0; Num_10 = 0; Num_1 = 0;
	Num_100   = (Current_Temp_Heater[0]/100)+48;									// 100point	ASCII Code = 'o' + 48
	Num_10    = ((Current_Temp_Heater[0]%100)/10)+48;							// 10point	ASCII Code = 'o' + 48
	Num_1     = ((Current_Temp_Heater[0]%100)%10)+48;							// 1point	ASCII Code = 'o' + 48

	scia_Tx(Num_100);
	scia_Tx(Num_10);
	scia_Tx(Num_1);

	scia_Tx('E');																						// 보고 끝
	scia_Tx('#');																						// 220221 추가
	DELAY_US(10000);																// P&F와 동일한 Delay 추가
}

void NOW_State_M_ReQST()
{
	int Num_1;
		Num_1 = 0;
		scia_Tx('@');												// 220221 추가
		scia_Tx('M');
	//SOL
		for(i=0;i<16;i++){		Num_1 = sol[i] + 48;	scia_Tx(Num_1);  }	// 1point	ASCII Code = 'o' + 48
	//Motor
		for(i=0;i<16;i++){	scia_Tx(M_ValveCurPos[i]);  } // 1point	ASCII Code = 'o' + 48
		scia_Tx('E');												// 보고 끝
		scia_Tx('#');												// 220221 추가
}

void PAUSE()															// Process中  Pause
{
	Uint16 Pause_Time;
	Pause_Time = CPU_Timer0_Isr_cnt;									// Run Time Save
	while(Mode == 4){
		All_Pump_Off();
		Rx_INT_STOP();														// Rx 통신 인터럽트 사용 해제
	CCK:
		Order_Code[0] = scia_Rx();
			if(Order_Code[0] =='$')
			{
				for(i=1;i<5;i++){
					Order_Code[i]=scia_Rx();
					if(Order_Code[3]=='F'){
						Order_Code[3]='.';
						goto CCK;
					}
				}         // Do not modify, and delete

				if(Order_Code[2]=='C' && Order_Code[3]=='S' && Order_Code[4]=='4'){
					NOW_State_ReQST();
					DELAY_US(1000);
				}  // CS4 REQUEST
				if(Order_Code[2]=='R' && Order_Code[3]=='U' && Order_Code[4]=='N'){
					string_Tx("@$$RUN#");								// 2022-02-21 시작문자 끝문자 추가, 표시형식 변경
					// DELAY_US(ECHO_DELAY);
					/*scia_Tx('$');										// 2019-12-23
					scia_Tx('$');
					scia_Tx('R');
					scia_Tx('U');
					scia_Tx('N');*/
					DELAY_US(ECHO_DELAY);

					Manual = 0;
					Mode = 2;
				}           // Run Mode
				if(Order_Code[2]=='S' && Order_Code[3]=='T' && Order_Code[4]=='P'){
					string_Tx("@$$STP#");								// 2022-02-21 시작문자 끝문자 추가, 표시형식 변경
					// DELAY_US(ECHO_DELAY);
					/*scia_Tx('$');
					scia_Tx('$');
					scia_Tx('S');
					scia_Tx('T');
					scia_Tx('P');*/
					DELAY_US(ECHO_DELAY);

					Manual = 0;
					EMS = 1;
					NGO = 1;														// 각 for/while 문들을 빠져나가기위한
					Step_SET_Delay_Time=0;
					Delay_SET_Time_Heater[1]=0;										// 조건들 수정
					Heater1_Delay_Time_End = 1;
					Mode = 2;
				}           // Run Mode

				if(Order_Code[2]=='M' && Order_Code[3]=='A' && Order_Code[4]=='N'){
					string_Tx("@$$MAN#");                                         // Excel_Data transfer request sends a command code, 2022-02-21 시작문자 끝문자 추가, 표시형식 변경
					// DELAY_US(ECHO_DELAY);
					/*scia_Tx('$');
					scia_Tx('$');
					scia_Tx('M');
					scia_Tx('A');
					scia_Tx('N');*/
					DELAY_US(ECHO_DELAY);

					Manual = 10;         // Manual Mode
					temp = scia_Rx();
					if(temp == 'S'){}
					for(Recipe2=0;Recipe2<59;Recipe2++){
						temp = scia_Rx();
						if(temp == 'E'){
							Recipe2 = 60;
						}
						else
						{
							Recipe[istage][Recipe2] = temp;
						}	//  Recipe Save
					}	// for(test2=0;test2<68;test2++) end
					Step_SET_Delay_Time=0;																// 각 for/while 문들을 빠져나가기위한
					Delay_SET_Time_Heater[0]=0; Delay_SET_Time_Heater[1]=0;								// 조건들 수정
					Heater0_Delay_Time_End = 1;
					Heater1_Delay_Time_End = 1;
					Motor_All_Check = 1;

					Mode = 2;
				}
			}
			Pump_Output();
	} // Mode = 4 End
	Rx_INT_Init();											            // Rx 통신 인터럽트 사용
	CPU_Timer0_Isr_cnt = Pause_Time;
}
void Hold_GPIO_Output()
{
	/* 82C55 reset signal*/
	GpioDataRegs.GPBDAT.bit.GPIO61 = 0;	DELAY_US(100);		//190830 NewMB
	GpioDataRegs.GPBDAT.bit.GPIO61 = 1;	DELAY_US(100);		//190830 NewMB
	GpioDataRegs.GPBDAT.bit.GPIO61 = 0;	DELAY_US(500);		//190830 NewMB
	/* Pump 방향결정  */
	//GpioDataRegs.GPADAT.bit.GPIO2 = 1;	DELAY_US(100);				// Pump ENABLE 날선전

	GpioDataRegs.GPADAT.bit.GPIO3 = 1;	DELAY_US(100);				// Pump1 Dir 날선후
	GpioDataRegs.GPADAT.bit.GPIO4 = 1;	DELAY_US(100);				// Pump2 Dir
	All_Pump_Off();
}
void All_Parts_Stop()
{
	All_Pump_Off();												// Pump1,2 Stop
	for(i=0;i<16;i++){	sol[i] = 0;	}                           // All solenoid valve off
	GpioDataRegs.GPADAT.bit.GPIO0 = 0; DELAY_US(1);				// Heater1 Stop
	GpioDataRegs.GPADAT.bit.GPIO1 = 0; DELAY_US(1);				// Heater2 Stop
}

void Rx_INT_Init()
{
	InitSciaGpio();
	SciaRegs.SCICTL1.bit.SWRESET = 0;												// SCI 소프트웨어 리셋

	SciaRegs.SCICCR.bit.SCICHAR = 7;												// SCI 송수신 Charcter-length 설정 : 8bit
	SciaRegs.SCICCR.bit.LOOPBKENA = 0;												// SCI 루프백 테스트 모드 Enable
	SciaRegs.SCICTL1.bit.RXENA = 1;													// SCI 송신기능 Enable
	SciaRegs.SCICTL1.bit.TXENA = 1;													// SCI 수신기능 Enable
	// SciaRegs.SCICTL2.bit.TXINTENA = 1;											// SCI 송신 인터럽트 Enable
	SciaRegs.SCICTL2.bit.RXBKINTENA = 1;											// SCI 수신 인터럽트 Enable
	SciaRegs.SCIHBAUD = 0x1;       													// 9600 bps
	SciaRegs.SCILBAUD = 0xE7;        												// 243.1525 = 19200bps  // 0xE7 = 9606bps 약 9600bps 0.06%오차
	//SciaRegs.SCIHBAUD = 0x00;      												// SCI Baudrate 설정
	//SciaRegs.SCILBAUD = 0x50;														//  - [SCIHBAUD,SCILBAUD] => 0x0050 => 38580bps(약38400bps)
	SciaRegs.SCIPRI.bit.FREE = 1;													// SCI 에뮬레이션 프리 모드

	SciaRegs.SCICTL1.bit.SWRESET = 1;												// SCI 소프트웨어 리셋 해제

	// PieCtrlRegs.PIEIER9.bit.INTx2 = 1;											// PIE 인터럽트(SCITXINTA) : Enable
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;												// PIE 인터럽트(SCIRXINTA) : Enable
	IER |= M_INT9;
	Next_istage=0;
}

void Rx_INT_STOP()
{
	SciaRegs.SCICTL2.bit.RXBKINTENA = 0;	// SCI 수신 인터럽트 Enable
	PieCtrlRegs.PIEIER9.bit.INTx1 = 0;		// PIE 인터럽트(SCIRXINTA) : Enable
	InitSciaGpio();
	for(i=0;i<3;i++){	Process_TEMP[i] = '.';	}
	//break;
}
#ifdef __cplusplus
}
#endif /* extern "C" */

//===========================================================================
// End of file.
//===========================================================================

