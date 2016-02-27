// lnk.h
#include "adl_structures.h"	

	char * ErrorToStr(int err_code);
	int NumberAdapters(void);
	int OverdriveSupported(void);
	int OverdriveEnabled(void);
	int OverdriveVersion(void);

// Определим для своих целей
typedef struct ADLTemperatureFan
{
	int iSize;	
	int iTemperature;  				// Temperature in millidegrees Celsius.
  
	int iFanSpeed;
	int fanSpeedReportingMethod;	// RPM or Percent
	int iMinRPM;
	int iMaxRPM;
	int iMinPercent;
	int iMaxPercent;
} ADLTemperatureFan;	


// Тест передачи параметров 
/* Возвращение параметров в структуре как результат работы функции
ADLDisplayProperty TeDL(void) { 

	ADLDisplayProperty cir;
	cir.iPropertyType = 15;
	cir.iExpansionMode = 11;
	return cir;
};	


/* ----------------------------------------------------------------------------------
	Базовая функция инициализации, ранее называлась main  
	Возвращает ошибку или номер версии Overdrive 
*/	
int InitADL(void);	

/* ----------------------------------------------------------------------------------
ADLTemperatureFan 	Overdrive5_TemperaFan(
		int adapterId, 						выбрать желаемую плату GPU 
		int iThermalControllerIndex, 		контроллер температуры и вентилятора, 
		типично на плате один, однако в описании предлагается  сделать поиск дополнительных контроллеров температуры
		for (int iThermalControllerIndex = 0; iThermalControllerIndex < 10; iThermalControllerIndex++)

		Теперь внутри структуры
		int temperatureInDegreesCelsius, 	текущая температура 
		int fanSpeedReportingMethod,  		тип данных: в процентах или в RPM 
		int iFanSpeed, 						текущие обороты вентилятора 
		int iMinFan, 						пределы оборотов 
		int iMaxFan 
		
		int SetFanSpeed 					если >0, изменить обороты вентилятора 
*/
ADLTemperatureFan Overdrive5_TemperaFan(	int adapterId, int iThermalControllerIndex, int SetFanSpeed );


/*--------------------------------------------------------------------------
ADLPowerControlInfo	 Overdrive5_PowerCtl(
		int adapterId, 						выбрать желаемую плату GPU 

		Эти величины возвращаются в структуре 
		int powerControlCurrent, 			что это за величина пока непонятно, 
											в структуре её нет, если понадобится переопределим новый вариант   
		int iStepValue, 
		int iMinValue, 
		int iMaxValue, 
		
		int iPowerControlTreshold 			если >0, изменить параметр 
*/
ADLPowerControlInfo	Overdrive5_PowerCtl(int adapterId, int iPowerControlTreshold );

	

/* ---------------------------------------------------------------------
ADLPMActivity Overdrive5_Clock(

		int adapterId, 						выбрать желаемую плату GPU 
		
		в возвращаемой структуре 
		int	iEngineClock, 					текущие настройки GPU 
		int iMemoryClock,
		int iVddc, 
		int iActivityPercent,

		int SetEngineClock, 				если >0, изменить параметр 
		int SetMemoryClock
*/
ADLPMActivity  Overdrive5_Clock( int adapterId, int SetEngineClock, int SetMemoryClock );				
 
 
 
/* --------------------------------------------------------------------------
ADLODParameters Overdrive5_ClockRange(
		int adapterId, 
		выбрать желаемую плату GPU по номеру 

	Возвращает структуру ADLODParameters с возможными значениями переменных 
	The GPU  Engine clock range is %d..%d MHz with step of %d Mhz
    The GPU  Memory clock range is %d..%d MHz with step of %d MHz
    The GPU  Core Voltage range is %d..%d with step of %d 
*/
ADLODParameters Overdrive5_ClockRange( int adapterId );
	
	
