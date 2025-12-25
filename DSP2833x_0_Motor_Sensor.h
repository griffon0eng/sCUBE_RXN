//###########################################################################
//
// FILE:	DSP2833x_0_Motor_Sensor.h
//
//###########################################################################
// USE GPIO SETUP
// TEST 기판용 추후 완제품은 재작업 필요
// 2012.12.14
//###########################################################################


#ifdef __cplusplus
extern "C" {
#endif

#include "DSP2833x_0_Define.h"
//---------------------------------------------------------------------------

void Motor_Init_Tick();
void Stop_Individual_Motor(Uint16 idx);

void Motor_Auto();
void Sensor_input();
void Motor_Count_Init();
void Motor_valve(int istage);
void Motor_Output();
void Motor_Sensor_Postion_Check();
void Motor_Stop();
void Motor_State_Check();


/* 각 모터 상태변수 < 0: 정위치 아님_구동이필요 / 1: 정위치_구동완료 > Motor_All_Check : 모든 모터 정위치완료*/
Uint16 Motor_ST[16];

void Motor_Init_Tick()
{
    Uint16 i;

    switch(Motor_Init_Step) {
        case 0: // 대기 상태 (명령 수신 시 시작)
            break;

        case 1: // 초기 설정 및 기동
            Motor_A[3] = 0x0080; Motor_B[3] = 0x0080; // 8255 출력 설정
            SenSor_A[3] = 0x009B; SenSor_B[3] = 0x009B; // 8255 입력 설정

            Motor_A[0] = 0x00AA; Motor_A[1] = 0x00AA; // 모든 모터 회전 시작
            Motor_B[0] = 0x00AA; Motor_B[1] = 0x00AA;

            for(i=0; i<16; i++) Motor_Homed[i] = 0;
            Home_Complete_Cnt = 0;
            Motor_Init_Step = 2; // 추적 단계로 이동
            break;

        case 2: // 위치 추적 (매 호출 시마다 센서 1회 확인)
            Sensor_input();
            Home_Complete_Cnt = 0;

            for(i=0; i<16; i++) {
                if(Motor_Homed[i] == 0) {
                    // 센서 A/B가 모두 감지되면 위치 '2'로 판단
                    if((SS_ON_A[i] == 1) && (SS_ON_B[i] == 1)) {
                        Stop_Individual_Motor(i); // 특정 모터 정지 (비트 연산 함수)
                        Motor_Homed[i] = 1;
                        M_ValveCurPos[i] = '2';
                    }
                }
                Home_Complete_Cnt += Motor_Homed[i];
            }

            if(Home_Complete_Cnt == 16) Motor_Init_Step = 3; // 모두 완료
            break;

        case 3: // 초기화 종료
            Motor_Init_Step = 0; // 초기화 완료 상태로 리셋
            // string_Tx("@Motor_Init_Done#"); // 통신 보고 (비차단형)
            break;
    }
}

void Stop_Individual_Motor(Uint16 idx)
{
    switch(idx) {
        // Motor_A[1] 그룹
        case 0: Motor_A[1] &= 0x003F; break;
        case 1: Motor_A[1] &= 0x00CF; break;
        case 2: Motor_A[1] &= 0x00F3; break;
        case 3: Motor_A[1] &= 0x00FC; break;

        // Motor_A[0] 그룹
        case 4: Motor_A[0] &= 0x003F; break;
        case 5: Motor_A[0] &= 0x00CF; break;
        case 6: Motor_A[0] &= 0x00F3; break;
        case 7: Motor_A[0] &= 0x00FC; break;

        // Motor_B[0] 그룹 (패턴 주의)
        case 8:  Motor_B[0] &= 0x00FC; break;
        case 9:  Motor_B[0] &= 0x00F3; break;
        case 10: Motor_B[0] &= 0x00CF; break;
        case 11: Motor_B[0] &= 0x003F; break;

        // Motor_B[1] 그룹 (패턴 주의)
        case 12: Motor_B[1] &= 0x00FC; break;
        case 13: Motor_B[1] &= 0x00F3; break;
        case 14: Motor_B[1] &= 0x00CF; break;
        case 15: Motor_B[1] &= 0x003F; break;
    }
}


void Motor_Auto()										// Process 中 Motor 구동 함수
{
	Motor_valve(istage);								// Motor 방향 설정
	Motor_Output();										// Motor 구동
	Motor_Count_Init();								    // Motor 위치 파악 변수들 초기화
	while(Motor_All_Check != 16){                        // 모든 Motor 위치 완료시 종료
		Motor_Sensor_Postion_Check();					// Motor 위치 파악
		Motor_Stop();									// 정위치에 온 Motor 정지
		Motor_State_Check();							// Motor 상태 확인
	}
	for(i=0;i<16;i++){	M_ValveCurPos[i] = M_ValveSetPos[i];	}
}

void Sensor_input()										// 센서 감지 유무
{    // 1 = sensor on , 0 == sensor off
	SS_ON_A[0]=(SenSor_A[1]/0x40)%0x2;		SS_ON_B[0]=(SenSor_A[1]/0x80)%0x2;
	SS_ON_A[1]=(SenSor_A[1]/0x10)%0x2; 		SS_ON_B[1]=(SenSor_A[1]/0x20)%0x2;
	SS_ON_A[2]=(SenSor_A[1]/0x4)%0x2;		SS_ON_B[2]=(SenSor_A[1]/0x8)%0x2;
	SS_ON_A[3]=(SenSor_A[1]/0x1)%0x2;		SS_ON_B[3]=(SenSor_A[1]/0x2)%0x2;

	SS_ON_A[4]=(SenSor_A[0]/0x40)%0x2;		SS_ON_B[4]=(SenSor_A[0]/0x80)%0x2;
	SS_ON_A[5]=(SenSor_A[0]/0x10)%0x2;		SS_ON_B[5]=(SenSor_A[0]/0x20)%0x2;
	SS_ON_A[6]=(SenSor_A[0]/0x4)%0x2;		SS_ON_B[6]=(SenSor_A[0]/0x8)%0x2;
	SS_ON_A[7]=(SenSor_A[0]/0x1)%0x2;		SS_ON_B[7]=(SenSor_A[0]/0x2)%0x2;

	SS_ON_A[8]=(SenSor_B[0]/0x1)%0x2;		SS_ON_B[8]=(SenSor_B[0]/0x2)%0x2;
	SS_ON_A[9]=(SenSor_B[0]/0x4)%0x2;		SS_ON_B[9]=(SenSor_B[0]/0x8)%0x2;
	SS_ON_A[10]=(SenSor_B[0]/0x10)%0x2;		SS_ON_B[10]=(SenSor_B[0]/0x20)%0x2;
	SS_ON_A[11]=(SenSor_B[0]/0x40)%0x2;		SS_ON_B[11]=(SenSor_B[0]/0x80)%0x2;

	SS_ON_A[12]=(SenSor_B[1]/0x1)%0x2;		SS_ON_B[12]=(SenSor_B[1]/0x2)%0x2;
	SS_ON_A[13]=(SenSor_B[1]/0x4)%0x2;		SS_ON_B[13]=(SenSor_B[1]/0x8)%0x2;
	SS_ON_A[14]=(SenSor_B[1]/0x10)%0x2;		SS_ON_B[14]=(SenSor_B[1]/0x20)%0x2;
	SS_ON_A[15]=(SenSor_B[1]/0x40)%0x2;		SS_ON_B[15]=(SenSor_B[1]/0x80)%0x2;


	DELAY_US(10);  // Must Need Delay time!!!  모터 보드1개 일때는 없으면 오동작 가능성 있음. 모터보드2개일때는 테스트 후 수정 및 삭제 하겠음.
}


void Motor_Sensor_Postion_Check()                   // 센서 감지로 인해 위치 파악.
{												    // 1~8모터 ㅏ = 3위치/ ㅜ = 2위치/ ㅓ = 1위치  //9~16	 ㅏ = 3위치/ ㅗ = 2위치/ ㅓ = 1위치
	Sensor_input();    								// Sensor check
	for(i=0;i<16;i++)								// 센서가 한번감지되고 그다음 감지되는것을 카운터로 해서 위치파악
		{	Pos = '9';
			if(M_ValveRot[i]=='1'){ //  상단 1->2 / 2->3 하단 2->1 / 3->2
				if(M_ValveSetPos[i] == '2'){    if((SS_ON_A[i] == 1) && (SS_ON_B[i]==1)) {Pos = '2';} }                      // Postion2  1->2 / 3->2
				else{
					if(( SS_ON_A[i] == 0)&&(SS_ON_B[i] == 0)){ CCW_SS_A_OFF_COUNT[i] = 1; }
					if((SS_ON_A[i] == 1)&&( CCW_SS_A_OFF_COUNT[i] == 1)&&(SS_ON_B[i] == 0))
					{   if(M_ValveSetPos[i] == '1'){ Pos = '1'; }   // Postion1    2->1
				    	else{ Pos = '3'; }}  // Postion1    2->3
				}
			}

			else if(M_ValveRot[i]=='2'){ //  상단 1->3  하단 3->1
				 if(( SS_ON_A[i] == 0)&&(SS_ON_B[i] == 0)){ CCW_SS_A_OFF_COUNT[i] = 1; }
				 if((SS_ON_A[i] == 1)&&( CCW_SS_A_OFF_COUNT[i] == 1)&&(CCW_SS_A_ON_COUNT[i] != 1))
				   { CCW_SS_A_ON_COUNT[i] = 1; CCW_SS_A_OFF_COUNT[i] = 0; }
				 else if((SS_ON_A[i] == 1)&&(CCW_SS_A_ON_COUNT[i] == 1)&&( CCW_SS_A_OFF_COUNT[i] == 1)){
					  if(M_ValveSetPos[i] == '3'){ Pos = '3';	}    // Postion1    1->3
					  else{ Pos = '1';	}}   // Postion1    3->1
			}

			 else if(M_ValveRot[i]=='3') {//  상단 2->1 / 3->2 하단1->2 / 2->3
				 if(M_ValveSetPos[i] == '2'){	if((SS_ON_A[i] == 1) && (SS_ON_B[i] == 1))  {Pos = '2'; }}                    // Postion2  3->2 / 1->2
				 else{
				 	if(( SS_ON_A[i] == 0)&&(SS_ON_B[i] == 0)){CW_SS_A_OFF_COUNT[i] = 1; }
					if((SS_ON_A[i] == 1)&&(CW_SS_A_OFF_COUNT[i] == 1)&&(SS_ON_B[i] == 0))
					{  if(M_ValveSetPos[i] == '1'){ Pos = '1'; }	// Postion1    2->1
					   else{ Pos = '3'; }}	 // Postion1    2->3
				 }
			}

			 else if(M_ValveRot[i]=='4'){ //  상단 3->1 하단1->3
				 if(( SS_ON_A[i] == 0)&&(SS_ON_B[i] == 0)){ CW_SS_A_OFF_COUNT[i] = 1; }
				 if((SS_ON_A[i] == 1)&&( CW_SS_A_OFF_COUNT[i] == 1)&&(CW_SS_A_ON_COUNT[i] != 1))
				 { CW_SS_A_ON_COUNT[i] = 1; CW_SS_A_OFF_COUNT[i] = 0; }
				 else if((SS_ON_A[i] == 1)&&(CW_SS_A_ON_COUNT[i] == 1)&&( CW_SS_A_OFF_COUNT[i] == 1)){
					 if(M_ValveSetPos[i] == '3'){ Pos = '3';	}    // Postion1    1->3
					 else{ Pos = '1';	}}   // Postion1    3->1
			}

			 else if(M_ValveRot[i]=='0') {	Pos = M_ValveSetPos[i];		}  // No Need Run

			M_ValveCurPos[i] = Pos;
		} // for end
} //void Motor_Sensor_Postion_CK() END


void Motor_Count_Init()
{	for(i=0;i<16;i++)
	{	CW_SS_A_ON_COUNT[i]=0; CW_SS_A_OFF_COUNT[i]=0; CCW_SS_A_ON_COUNT[i]=0; CCW_SS_A_OFF_COUNT[i]=0;
		CW_SS_B_ON_COUNT[i]=0; CW_SS_B_OFF_COUNT[i]=0; CCW_SS_B_ON_COUNT[i]=0; CCW_SS_B_OFF_COUNT[i]=0;
        Motor_ST[i]=0;}
	Motor_All_Check=0;
DELAY_US(10000); // 1000=0.01s 10000=0.1s 100000=1s
}


void Motor_Output()																// 모터 구동
{	unsigned int MOT_DIR0,MOT_DIR1;
	unsigned int MOT_DIR2,MOT_DIR3;
	MOT_DIR0=0;MOT_DIR1=0; MOT_DIR2=0;MOT_DIR3=0;
	// Motor1 //
	if((M_ValveRot[0]=='1')||(M_ValveRot[0]=='2')) {MOT_DIR0 = MOT_DIR0 + 0x40;} //ccw
	if((M_ValveRot[0]=='3')||(M_ValveRot[0]=='4')) {MOT_DIR0 = MOT_DIR0 + 0x80;} //cw
	/// Motor2 //
	if((M_ValveRot[1]=='1')||(M_ValveRot[1]=='2')) {MOT_DIR0 = MOT_DIR0 + 0x10;} //ccw
	if((M_ValveRot[1]=='3')||(M_ValveRot[1]=='4')) {MOT_DIR0 = MOT_DIR0 + 0x20;} //cw
	// Motor3 //
	if((M_ValveRot[2]=='1')||(M_ValveRot[2]=='2')) {MOT_DIR0 = MOT_DIR0 + 0x04;} //ccw
	if((M_ValveRot[2]=='3')||(M_ValveRot[2]=='4')) {MOT_DIR0 = MOT_DIR0 + 0x08;} //cw
	// Motor4 //
	if((M_ValveRot[3]=='1')||(M_ValveRot[3]=='2')) {MOT_DIR0 = MOT_DIR0 + 0x01;} //ccw
	if((M_ValveRot[3]=='3')||(M_ValveRot[3]=='4')) {MOT_DIR0 = MOT_DIR0 + 0x02;} //cw
	Motor_A[1] = MOT_DIR0; DELAY_US(2);
	// Motor5 //
	if((M_ValveRot[4]=='1')||(M_ValveRot[4]=='2')) {MOT_DIR1 = MOT_DIR1 + 0x40;} //ccw
	if((M_ValveRot[4]=='3')||(M_ValveRot[4]=='4')) {MOT_DIR1 = MOT_DIR1 + 0x80;} //cw
	// Motor6 //
	if((M_ValveRot[5]=='1')||(M_ValveRot[5]=='2')) {MOT_DIR1 = MOT_DIR1 + 0x10;} //ccw
	if((M_ValveRot[5]=='3')||(M_ValveRot[5]=='4')) {MOT_DIR1 = MOT_DIR1 + 0x20;} //cw
	// Motor7 //
	if((M_ValveRot[6]=='1')||(M_ValveRot[6]=='2')) {MOT_DIR1 = MOT_DIR1 + 0x04;} //ccw
	if((M_ValveRot[6]=='3')||(M_ValveRot[6]=='4')) {MOT_DIR1 = MOT_DIR1 + 0x08;} //cw
	// Motor8 //
	if((M_ValveRot[7]=='1')||(M_ValveRot[7]=='2')) {MOT_DIR1 = MOT_DIR1 + 0x01;} //ccw
	if((M_ValveRot[7]=='3')||(M_ValveRot[7]=='4')) {MOT_DIR1 = MOT_DIR1 + 0x02;} //cw
	Motor_A[0] = MOT_DIR1; DELAY_US(2);
	// Motor9 //
	if((M_ValveRot[8]=='1')||(M_ValveRot[8]=='2')) {MOT_DIR2 = MOT_DIR2 + 0x01;} //ccw
	if((M_ValveRot[8]=='3')||(M_ValveRot[8]=='4')) {MOT_DIR2 = MOT_DIR2 + 0x02;} //cw
	/// Motor10 //
	if((M_ValveRot[9]=='1')||(M_ValveRot[9]=='2')) {MOT_DIR2 = MOT_DIR2 + 0x04;} //ccw
	if((M_ValveRot[9]=='3')||(M_ValveRot[9]=='4')) {MOT_DIR2 = MOT_DIR2 + 0x08;} //cw
	// Motor11 //
	if((M_ValveRot[10]=='1')||(M_ValveRot[10]=='2')) {MOT_DIR2 = MOT_DIR2 + 0x10;} //ccw
	if((M_ValveRot[10]=='3')||(M_ValveRot[10]=='4')) {MOT_DIR2 = MOT_DIR2 + 0x20;} //cw
	// Motor12 //
	if((M_ValveRot[11]=='1')||(M_ValveRot[11]=='2')) {MOT_DIR2 = MOT_DIR2 + 0x40;} //ccw
	if((M_ValveRot[11]=='3')||(M_ValveRot[11]=='4')) {MOT_DIR2 = MOT_DIR2 + 0x80;} //cw
	Motor_B[0] = MOT_DIR2; DELAY_US(2);
	// Motor13 //
	if((M_ValveRot[12]=='1')||(M_ValveRot[12]=='2')) {MOT_DIR3 = MOT_DIR3 + 0x01;} //ccw
	if((M_ValveRot[12]=='3')||(M_ValveRot[12]=='4')) {MOT_DIR3 = MOT_DIR3 + 0x02;} //cw
	// Motor14 //
	if((M_ValveRot[13]=='1')||(M_ValveRot[13]=='2')) {MOT_DIR3 = MOT_DIR3 + 0x04;} //ccw
	if((M_ValveRot[13]=='3')||(M_ValveRot[13]=='4')) {MOT_DIR3 = MOT_DIR3 + 0x08;} //cw
	// Motor15 //
	if((M_ValveRot[14]=='1')||(M_ValveRot[14]=='2')) {MOT_DIR3 = MOT_DIR3 + 0x10;} //ccw
	if((M_ValveRot[14]=='3')||(M_ValveRot[14]=='4')) {MOT_DIR3 = MOT_DIR3 + 0x20;} //cw
	// Motor16 //
	if((M_ValveRot[15]=='1')||(M_ValveRot[15]=='2')) {MOT_DIR3 = MOT_DIR3 + 0x40;} //ccw
	if((M_ValveRot[15]=='3')||(M_ValveRot[15]=='4')) {MOT_DIR3 = MOT_DIR3 + 0x80;} //cw
	Motor_B[1] = MOT_DIR3; DELAY_US(100);


}

void Motor_Stop()
{  																							  // 해당 위치로 이동 확인후 motor Stop
	if(M_ValveSetPos[0]==M_ValveCurPos[0]){Motor_A[1] = Motor_A[1] & 0x003f; DELAY_US(1); Motor_ST[0]=1;}    // Motor1 Stop
	if(M_ValveSetPos[1]==M_ValveCurPos[1]){Motor_A[1] = Motor_A[1] & 0x00cf; DELAY_US(1); Motor_ST[1]=1;}    // Motor2 Stop
	if(M_ValveSetPos[2]==M_ValveCurPos[2]){Motor_A[1] = Motor_A[1] & 0x00f3; DELAY_US(1); Motor_ST[2]=1;}    // Motor3 Stop
	if(M_ValveSetPos[3]==M_ValveCurPos[3]){Motor_A[1] = Motor_A[1] & 0x00fc; DELAY_US(1); Motor_ST[3]=1;}    // Motor4 Stop

	if(M_ValveSetPos[4]==M_ValveCurPos[4]){Motor_A[0] = Motor_A[0] & 0x003f; DELAY_US(1); Motor_ST[4]=1;}    // Motor5 Stop
	if(M_ValveSetPos[5]==M_ValveCurPos[5]){Motor_A[0] = Motor_A[0] & 0x00cf; DELAY_US(1); Motor_ST[5]=1;}    // Motor6 Stop
	if(M_ValveSetPos[6]==M_ValveCurPos[6]){Motor_A[0] = Motor_A[0] & 0x00f3; DELAY_US(1); Motor_ST[6]=1;}    // Motor7 Stop
	if(M_ValveSetPos[7]==M_ValveCurPos[7]){Motor_A[0] = Motor_A[0] & 0x00fc; DELAY_US(1); Motor_ST[7]=1;}    // Motor8 Stop

	if(M_ValveSetPos[8]==M_ValveCurPos[8]){Motor_B[0] = Motor_B[0] & 0x00fc; DELAY_US(1); Motor_ST[8]=1;}    // Motor9 Stop
	if(M_ValveSetPos[9]==M_ValveCurPos[9]){Motor_B[0] = Motor_B[0] & 0x00f3; DELAY_US(1); Motor_ST[9]=1;}   // Motor10 Stop
	if(M_ValveSetPos[10]==M_ValveCurPos[10]){Motor_B[0] = Motor_B[0] & 0x00cf; DELAY_US(1); Motor_ST[10]=1;} // Motor11 Stop
	if(M_ValveSetPos[11]==M_ValveCurPos[11]){Motor_B[0] = Motor_B[0] & 0x003f; DELAY_US(1); Motor_ST[11]=1;} // Motor12 Stop

	if(M_ValveSetPos[12]==M_ValveCurPos[12]){Motor_B[1] = Motor_B[1] & 0x00fc; DELAY_US(1); Motor_ST[12]=1;} // Motor13 Stop
	if(M_ValveSetPos[13]==M_ValveCurPos[13]){Motor_B[1] = Motor_B[1] & 0x00f3; DELAY_US(1); Motor_ST[13]=1;} // Motor14 Stop
	if(M_ValveSetPos[14]==M_ValveCurPos[14]){Motor_B[1] = Motor_B[1] & 0x00cf; DELAY_US(1); Motor_ST[14]=1;} // Motor15 Stop
	if(M_ValveSetPos[15]==M_ValveCurPos[15]){Motor_B[1] = Motor_B[1] & 0x003f; DELAY_US(1); Motor_ST[15]=1;} // Motor16 Stop
	DELAY_US(1);
}
void Motor_State_Check()
{
	Motor_All_Check = 0;																	  // 모터 상태확인으로 모든 모터의 동작확인
	for(i=0;i<16;i++){ if (M_ValveRot[i] == '0'){Motor_ST[i]=1;} }
	for(i=0;i<16;i++){	Motor_All_Check = Motor_All_Check + Motor_ST[i];	}
}

void Motor_valve(int istage)																// 모터의 정,역방향 및 센서감지카운터 숫자 결정
{
	int rot;
	for(i=42;i<58;i++)              // Recipe 배열 -> Motor 상단 배열로 이동
	{	M_ValveSetPos[i-42] = Recipe[istage][i];	}
	for(i=0;i<8;i++)                                               // 상단 motor   ㅏ = 3 / ㅜ = 2 / ㅓ = 1
	{	rot = M_ValveCurPos[i] - M_ValveSetPos[i];
		if (rot == 0){ M_ValveRot[i] = '0'; } // No Need RUN
		if (rot == -1){ M_ValveRot[i] = '1';	} // ccw
		if (rot == -2){ M_ValveRot[i] = '2';	} // ccw_x2
		if (rot == 1){ M_ValveRot[i] = '3';	} // cw
		if (rot == 2){ M_ValveRot[i] = '4';	} // cw_x2
	}
	for(i=8;i<16;i++)												// 하단 motor   ㅏ = 3 / ㅗ = 2 / ㅓ = 1
	{	rot = M_ValveCurPos[i] - M_ValveSetPos[i];
		if (rot == 0){ M_ValveRot[i] = '0'; } // No Need RUN
		if (rot == 1){ M_ValveRot[i] = '1';	} // ccw
		if (rot == 2){ M_ValveRot[i] = '2';	} // ccw_x2
		if (rot == -1){ M_ValveRot[i] = '3';	} // cw
		if (rot == -2){ M_ValveRot[i] = '4';	} // cw_x2
	}
}

#ifdef __cplusplus
}
#endif /* extern "C" */

//===========================================================================
// End of file.
//===========================================================================

