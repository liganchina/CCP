//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : Record.c
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2014.5.7
//* Description        : 该文件专门用于历史记录处理部分功能；每分钟存储一次数据以及有故障发生时即时存储
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "derivative.h" /* include peripheral declarations */
#include "BMS20.h" 

//*************************************************************************************************
unsigned int g_storageSysVariableOut[STORE_NUMBER]= 
{
    0,  //记录索引
    0,  //系统总电压
    0,  //系统总电流
    1.0,  //SOC显示值
    0,   //继电器状态
    0.0,   //故障等级
    0,    //故障信息1
    0,   //故障信息2
    0,   //故障信息3
    0,   //故障信息4
    0,//second        10
    0,//minute////////11
    0,//hour
    0,//week
    0,//day
    0,//month
    0, //year
    0,//最高单体温度
    0,//最低单体温度
    0,//平均温度
    0, //最高单体电压
    0, //最低单体电压
    0, //平均电压
    1000,//绝缘阻值正
    1000,//绝缘阻值负
    198,
    4000,
    198,
    4000,
    198,
    4000,
    0   //校验
};
unsigned char g_storageSysVariableT[36];
/*unsigned int g_storageSysVariableCell[200]= //存储200个单体电压用
{  
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0
};*/
/*unsigned int g_storageSysVariableCellOut[200]= //读出200个单体电压用
{  
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0
};*/

unsigned int g_storageSysVariableCell[36]= //存储200个单体电压用
{  
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0
};
unsigned int g_storageSysVariableCellOut[36]= //读出200个单体电压用
{  
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0
};

unsigned int g_storageSysVariable[120];
//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
//******************************************************************************
//* Function name:   ReadErrorIndex
//* Description:     读出历史数据索引号
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
unsigned int ReadErrorIndex(void) 
{
    unsigned int counter=0;   
    unsigned int cc[1];
    
    Read24c64_Byte(ERROR_COUNT_ADDR,(unsigned char* )cc,2); //获取当前历史记录计数器  
    return ((unsigned int)cc[0]);   
}
//******************************************************************************
//* Function name:   storeErrorIndex
//* Description:     存储历史记录索引号
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void storeErrorIndex(unsigned int index) 
{
    unsigned int counter[1];
    
    counter[0]= index;
    Write24c64_Byte(ERROR_COUNT_ADDR,(unsigned char *)&counter,2);//更新故障计数? 
}
//******************************************************************************
//* Function name:   StoreSysVariable
//* Description:     存储历史记录信息
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void StoreSysVariable(void)  //存储故障信息和SOC值，地址从at45db161 的 0x3ff 开始
{
	unsigned char i=0,j=0; 
	unsigned int buffer1[STORE_NUMBER],buffer2_1[128],buffer2_2[72];//,test_1[128],test_2[72],test1[32]
	unsigned int check=0;   //校验和

	g_errorCounter++;
	if(g_errorCounter>=500){  //保存500条后，循环存储
		g_errorCounter = 0;
	}
	
    storeErrorIndex(g_errorCounter);//更新故障计数器 
	
    g_storageSysVariable[INDEX]= g_errorCounter;
    g_storageSysVariable[TOTAL_VOLTAGE]= (unsigned int)g_highVoltageV1;   
    g_storageSysVariable[TOTAL_CURRENT]= (unsigned int)((g_systemCurrent+400)*50);
    g_storageSysVariable[PARA_SOC_DISPLAY]= (unsigned int)(g_socValue*100*2.5);
    g_storageSysVariable[SYS_CONTACTORS_STATE]= BmsCtlStat0 & 0xff;//BMS控制状态 
    
    //故障信息在故障处理程序里
    //实时时间
    for(j=SYS_REALTIME_SECOND;j<SYS_REALTIME_YEAR+1;j++){
        g_storageSysVariable[j]= CurrentTime[j-10];
    }

	g_storageSysVariable[CELL_MAX_TEM]			= (unsigned int)((g_bms_msg.CellTempMax-40)+48);
	g_storageSysVariable[CELL_MIN_TEM]			= (unsigned int)((g_bms_msg.CellTempMin-40)+48);
	g_storageSysVariable[CELL_AVERAGE_TEM]		= (unsigned int)((g_averageTemperature-40)+48);
	g_storageSysVariable[CELL_MAX_VOLTAGE]		= (unsigned int)(g_bms_msg.CellVoltageMax);
	g_storageSysVariable[CELL_MIN_VOLTAGE]		= (unsigned int)(g_bms_msg.CellVoltageMin);
	g_storageSysVariable[CELL_AVERAGE_VOLTAGE]	= (unsigned int)(g_averageVoltage*10000); 
	g_storageSysVariable[SYS_INSULATION_P]		= Rp_Vpn_Value;
	g_storageSysVariable[SYS_INSULATION_N]		= Rn_Vpn_Value;
	
    //单体电压值在BMU数据处理里
    //存入AD45DB161
    //以下存储数据因为超过256个，所以分成两部分来保存。 Wrt_Flash函数的入口参数是unsigned char型的。改成unsigned int型的有时候读不对
    //这里出现跨页错误问题。。。所以，每条记录占一整页。528字节一页。      
    for(i=0;i<=35;i++) 
    {    
        buffer2_1[i] = g_storageSysVariableCell[i];//记录所有单体电压极值
        check += buffer2_1[i];
    }
    M95_Write_Memory(256*(unsigned long)2*g_errorCounter,(unsigned char* )buffer2_1,36*2);//从第1页开始（地址从0开始,0~255,存18个最高单体电压和18个最低单体电压, 18*2bytes*2, max and min
    
    //for test
    /*
    DisableInterrupts;
    M95_Read_Memory(256*(unsigned long)2*g_errorCounter,(unsigned char* )test_1,256);//从第1页开始（地址从0开始,0~255,存128个单体电压  
    EnableInterrupts; //开中断?
    */
    //end of test
    
    for(i=128;i<200;i++)
    {
//		buffer2_2[i-128] = g_storageSysVariableCell[i];//记录所有单体电压
		buffer2_2[i-128] = 0;   //fill 0 for unused area. just check calculate, project name S278.

		check += buffer2_2[i-128];  
    }
      
    M95_Write_Memory(256*(unsigned long)(2*g_errorCounter+1),buffer2_2,72*2);//从第1页开始（地址从256开始，256~399,存另外72个电压
    
    //for test
    /*
    DisableInterrupts;
    M95_Read_Memory(256*(unsigned long)(2*g_errorCounter+1),(unsigned char* )test_2,72*2);//从第1页开始（地址从0开始,0~255,存128个单体电压  
    EnableInterrupts; //开中断
    */
    //end of test  
    
    //先存后半页，再存前半页，因为校验和在前半页里
    for(i=0;i<=SYS_INSULATION_N;i++) 
    {
        buffer1[i]= g_storageSysVariable[i];//记录64位其他字节 
        check += buffer1[i];
    }
    //***增加保存V1，V2，V3的K,B值。。。因为B值些值是int类型，所以要用补码进行保存
   
	buffer1[VOLT_K1] =Vpn_K1;
	check += buffer1[VOLT_K1];
	
	buffer1[VOLT_B1] =(unsigned int)(32767-Vpn_B1);
	check += buffer1[VOLT_B1];
	
	buffer1[VOLT_K2] =Vpn_K2;
	check += buffer1[VOLT_K2];
	
	buffer1[VOLT_B2] =(unsigned int)(32767-Vpn_B2);
	check += buffer1[VOLT_B2];
	
	buffer1[VOLT_K3] =Vpn_K3;
	check += buffer1[VOLT_K3];
	
	buffer1[VOLT_B3] =(unsigned int)(32767-Vpn_B3);
	check += buffer1[VOLT_B3];
           
    buffer1[VERIFICATION] = check;
    
    M95_Write_Memory(256*(unsigned long)(2*g_errorCounter+1)+72*2,(unsigned char*)buffer1,STORE_NUMBER*2);//从第1页开始（地址从400开始，463存储64个参数数据
    
    //for test
    /*
    DisableInterrupts; //
        M95_Read_Memory(256*(unsigned long)(2*g_errorCounter+1)+72*2,test1,STORE_NUMBER*2);
    EnableInterrupts;
    */
    //for test
}
//******************************************************************************
//* Function name:   ReadOutErrorRecord
//* Description:     读出历史记录信息
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
 
unsigned char ReadOutErrorRecord(unsigned int counter) //   1:校验不通过
{
	unsigned int i=0;
	unsigned int check=0;
	unsigned int verification=0;
	unsigned int buffer1[STORE_NUMBER],buffer2_1[128],buffer2_2[72];

	DisableInterrupts; //
        M95_Read_Memory(256*(unsigned long)2*counter,(unsigned char* )buffer2_1,256);//从第1页开始（地址从0开始,0~255,存128个单体电压  
	EnableInterrupts; //开中断

	for(i=0;i<128;i++) 
	{      
         check += buffer2_1[i];
    }


    DisableInterrupts; //
        M95_Read_Memory(256*((unsigned long)2*counter+1),(unsigned char* )buffer2_2,72*2);//从第1页开始（地址从0开始,0~255,存128个单体电压  
    EnableInterrupts; //开中断  
    for(i=0;i<72;i++) 
    {      
         check += buffer2_2[i];
    }
    
    
    DisableInterrupts; //
        M95_Read_Memory((256*(unsigned long)(2*counter+1)+72*2),(unsigned char* )buffer1,STORE_NUMBER*2);
    EnableInterrupts; //开中断
    
    for(i=0;i<=VOLT_B3;i++) 
    {      
        check += buffer1[i];
    }
    verification =  buffer1[VERIFICATION];

    if(verification == check)
    {
		for(i=0;i<STORE_NUMBER;i++) 
		    g_storageSysVariableOut[i] = buffer1[i]; //参数个数 32
//		for(i=0;i<128;i++) 
//		    g_storageSysVariableCellOut[i]= buffer2_1[i];  //单体电压 
//		for(i=128;i<200;i++)
//		    g_storageSysVariableCellOut[i]= buffer2_2[i-128];  //单体电压     

//		for(i=128;i<200;i++){
//			g_storageSysVariableCellOut[i]= buffer2_2[i-128];  //单体电压	  
//		}

		for(i=0;i<36;i++){
			g_storageSysVariableCellOut[i]= buffer2_1[i];  //单体电压 
		}
		return 0;
    }
    else if(counter > g_errorCounter) 
    {
        for(i=0;i<STORE_NUMBER;i++) 
            g_storageSysVariableOut[i] = 0;
//        for(i=0;i<200;i++)
//            g_storageSysVariableCellOut[i]=0;   
        for(i=0;i<36;i++)
            g_storageSysVariableCellOut[i]=0;   
        return 0; 
    }
    return 1;                   
}
//******************************************************************************
//* Function name:   TaskRecordProcess
//* Description:     历史记录处理任务过程，读取2000条
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void TaskRecordProcess(void)
{
	int i,j;

	if(!g_errorRecordRead_flag){ 
		return;
	}
	  
	DisableInterrupts; //是否读取故障信息，当读取故障信息时，所有中断停止
	for(i=499;i>=0;i--) 
	{	    
		_FEED_COP();   //2s内不喂内狗，则系统复位
		//PORTB_PB4 ^=1; //feed the hardware dog
		if(1==ReadOutErrorRecord(i))//如果校验不通过，则重新读取	 
		{
			ReadOutErrorRecord(i);
		}
		
		delay(1000);
		delay(1000);      
		RecordSystem();
		delay(1000);
		delay(1000);
		RecordFaultTemperture();
		delay(1000);
		delay(1000);
		RecordRealtime();
		delay(1000);
		delay(1000);
		RecordExtremCellVoltage();
		delay(1000);
		delay(1000);
		RecordInsulation();
		delay(1000);
		delay(1000);
		RecordVoltKB();
		delay(1000);
		delay(1000);
		for(j=0;j<50;j++) 
		{	        
			RecordCellVoltage(j);//0~4*49+3=199
			delay(1000);
			delay(1000);   //此处必须延时足够
			delay(1000);
			delay(1000);
		}
    }
	
	delay(1000);
	delay(1000);
	RecordEndFlag();//end	 

	g_errorRecordRead_flag=0;	
	EnableInterrupts;	
}
//******************************************************************************
//* Function name:   parametersClean
//* Description:     参数变量清零
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void parametersClean(void)
{
	unsigned int i,k;//,j

	for(i=0;i<BMU_NUMBER;i++){
		for(k=0;k<CELL_NUMBER;k++){
			g_bmu_msg.cell_V[i][k] = 0;
		}
		for(k=0;k<Tem_NUMBER;k++){
			g_bmu_msg.cell_T[i][k] = 0;
		}
	}
	
//	g_highestCellVoltage = 0;
//	g_lowestCellVoltage = 0;
	g_bms_msg.CellVoltageMax = 0;
	g_bms_msg.CellVoltageMin = 0;
	g_averageVoltage = 0; 

	for(i=0;i<STORE_NUMBER;i++){
        g_storageSysVariableOut[i] = 0;
	}

	VCU_ChgControl_2.Bit.rise_Eleband_No_OK = 1;
	VCU_ChgControl.Bit.rise_Eleband_Switch = 0;
	VCU_ChgControl.Bit.downC_OK = 1;
}
//********************************************************************************************
//***********************************the end*************************************************
//********************************************************************************************
//********************************************************************************************
