// linking dll in one operation
// gcc -rdynamic -o lnk lnk.cpp -ldl
// Перенаправление вывода в файл
// gcc ovr.cpp >f  2>&1

// Получение объектного файла
// gcc -export-dynamic  lnk.cpp -ldl
// Или разделяемой библиотеки Dll
// gcc -shared -o lnk.so lnk.cpp -fpic -ldl

// Сборка в варианте для С
// gcc -c lnk.c -fpic -ldl -std=c99
// ar q lnk.a lnk.o


/// Базируется on sample test application to test Overdrive get functions
/// Author: Ilia Blank
/// Copyright (c) 2013 Advanced Micro Devices, Inc.
/// \file Overdrive_Sample.cpp
/// \brief C/C++ Overdrive Color sample application, based on ADL_Sample project
/// Автор wrapper Сергей Токаревских


#define LINUX

#if defined (LINUX)
#include "adl_sdk.h"
//#include "include/adl_defines.h"
//#include "include/customer/oem_structures.h"
#include <dlfcn.h>	//dyopen, dlsym, dlclose
#include <stdlib.h>
#include <string.h>	//memeset
#include <unistd.h>	//sleep
#include "lnk.h"

#else
#include <windows.h>
#include <tchar.h>
#include "include/adl_sdk.h" 	//Replace with local location of ADL SDK
#endif

#include <stdio.h>

#define AMDVENDORID             (1002)
#define ADL_WARNING_NO_DATA      -100



/* ---------------------------------------------------------------------------------
При инициализации ADL эти переменные заполняются один раз 
есть смысл сохранить их один раз в статических переменных и вынимать 
в других функциях по мере необходимости, не выпуская их наружу в вызовы из Go.

а'ля Синглетон
*/
static void *hDLL;					// Handle to .so library
static int iNumberAdapters = 0;
static int iOverdriveSupported = 0;
static int iOverdriveEnabled = 0;
static int iOverdriveVersion = 0;

int NumberAdapters()     { return iNumberAdapters;}			// Количество установленных плат 
int OverdriveSupported() { return iOverdriveSupported;}		// Остальные цифры больше для любопытства
int OverdriveEnabled()   { return iOverdriveEnabled;}
int OverdriveVersion()   { return iOverdriveVersion;}


	



// Добавлено для подавления ошибок  09.02.2016
void gets_s(char input_str[]){
//    gets(input_str);
};

int _stricmp(char * string1, const char * string2 ){
    return strcmp(string1,string2 );
};

static volatile int nory;
int add3(int p) {
    return ++nory + p;		// Тест вызова функции 
}

#ifdef _UNICODE
typedef wchar_t _TCHAR;
#else
typedef char _TCHAR;
#endif



// Definitions of the used function pointers. Add more if you use other ADL APIs
typedef int(*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
typedef int(*ADL_MAIN_CONTROL_DESTROY)();
typedef int(*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (int*);
typedef int(*ADL_ADAPTER_ADAPTERINFO_GET) (LPAdapterInfo, int);
typedef int(*ADL_ADAPTER_ACTIVE_GET) (int, int*);
typedef int(*ADL_OVERDRIVE_CAPS) (int iAdapterIndex, int *iSupported, int *iEnabled, int *iVersion);
typedef int(*ADL_OVERDRIVE5_THERMALDEVICES_ENUM) (int iAdapterIndex, int iThermalControllerIndex, ADLThermalControllerInfo *lpThermalControllerInfo);
typedef int(*ADL_OVERDRIVE5_ODPARAMETERS_GET) (int  iAdapterIndex, ADLODParameters *  lpOdParameters);
typedef int(*ADL_OVERDRIVE5_TEMPERATURE_GET) (int iAdapterIndex, int iThermalControllerIndex, ADLTemperature *lpTemperature);
typedef int(*ADL_OVERDRIVE5_FANSPEED_GET) (int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue);
typedef int(*ADL_OVERDRIVE5_FANSPEEDINFO_GET) (int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedInfo *lpFanSpeedInfo);
typedef int(*ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET) (int iAdapterIndex, int iDefault, ADLODPerformanceLevels *lpOdPerformanceLevels);
typedef int(*ADL_OVERDRIVE5_ODPARAMETERS_GET) (int iAdapterIndex, ADLODParameters *lpOdParameters);
typedef int(*ADL_OVERDRIVE5_CURRENTACTIVITY_GET) (int iAdapterIndex, ADLPMActivity *lpActivity);
typedef int(*ADL_OVERDRIVE5_FANSPEED_SET)(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue);
typedef int(*ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET) (int iAdapterIndex, ADLODPerformanceLevels *lpOdPerformanceLevels);
typedef int(*ADL_OVERDRIVE5_POWERCONTROL_CAPS)(int iAdapterIndex, int *lpSupported);
typedef int(*ADL_OVERDRIVE5_POWERCONTROLINFO_GET)(int iAdapterIndex, ADLPowerControlInfo *lpPowerControlInfo);
typedef int(*ADL_OVERDRIVE5_POWERCONTROL_GET)(int iAdapterIndex, int *lpCurrentValue, int *lpDefaultValue);
typedef int(*ADL_OVERDRIVE5_POWERCONTROL_SET)(int iAdapterIndex, int iValue);
typedef int(*ADL_OVERDRIVE6_FANSPEED_GET)(int iAdapterIndex, ADLOD6FanSpeedInfo *lpFanSpeedInfo);
typedef int(*ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS)(int iAdapterIndex, ADLOD6ThermalControllerCaps *lpThermalControllerCaps);
typedef int(*ADL_OVERDRIVE6_TEMPERATURE_GET)(int iAdapterIndex, int *lpTemperature);
typedef int(*ADL_OVERDRIVE6_CAPABILITIES_GET) (int iAdapterIndex, ADLOD6Capabilities *lpODCapabilities);
typedef int(*ADL_OVERDRIVE6_STATEINFO_GET)(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo);
typedef int(*ADL_OVERDRIVE6_CURRENTSTATUS_GET)(int iAdapterIndex, ADLOD6CurrentStatus *lpCurrentStatus);
typedef int(*ADL_OVERDRIVE6_POWERCONTROL_CAPS) (int iAdapterIndex, int *lpSupported);
typedef int(*ADL_OVERDRIVE6_POWERCONTROLINFO_GET)(int iAdapterIndex, ADLOD6PowerControlInfo *lpPowerControlInfo);
typedef int(*ADL_OVERDRIVE6_POWERCONTROL_GET)(int iAdapterIndex, int *lpCurrentValue, int *lpDefaultValue);
typedef int(*ADL_OVERDRIVE6_FANSPEED_SET)(int iAdapterIndex, ADLOD6FanSpeedValue *lpFanSpeedValue);
typedef int(*ADL_OVERDRIVE6_STATE_SET)(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo);
typedef int(*ADL_OVERDRIVE6_POWERCONTROL_SET)(int iAdapterIndex, int iValue);

// Memory allocation function
void* __stdcall ADL_Main_Memory_Alloc(int iSize)
{
    void* lpBuffer = malloc(iSize);
    return lpBuffer;
}

// Optional Memory de-allocation function
void __stdcall ADL_Main_Memory_Free(void** lpBuffer)
{
    if (NULL != *lpBuffer)
    {
        free(*lpBuffer);
        *lpBuffer = NULL;
    }
}

#if defined (LINUX)
// equivalent functions in linux
void * GetProcAddress(void * pLibrary, const char * name)
{
    return dlsym(pLibrary, name);
}
#endif


#if defined (LINUX)
int Overdrive5Sample(int adapterId, void* hDLL)
#else
int Overdrive5Sample(int adapterId, HINSTANCE hDLL)
#endif
{
    ADL_OVERDRIVE5_THERMALDEVICES_ENUM		ADL_Overdrive5_ThermalDevices_Enum;
    ADL_OVERDRIVE5_ODPARAMETERS_GET			ADL_Overdrive5_ODParameters_Get;
    ADL_OVERDRIVE5_TEMPERATURE_GET			ADL_Overdrive5_Temperature_Get;
    ADL_OVERDRIVE5_FANSPEED_GET				ADL_Overdrive5_FanSpeed_Get;
    ADL_OVERDRIVE5_FANSPEEDINFO_GET			ADL_Overdrive5_FanSpeedInfo_Get;
    ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	ADL_Overdrive5_ODPerformanceLevels_Get;
    ADL_OVERDRIVE5_CURRENTACTIVITY_GET		ADL_Overdrive5_CurrentActivity_Get;
    ADL_OVERDRIVE5_FANSPEED_SET				ADL_Overdrive5_FanSpeed_Set;
    ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET  ADL_Overdrive5_ODPerformanceLevels_Set;
    ADL_OVERDRIVE5_POWERCONTROL_CAPS		ADL_Overdrive5_PowerControl_Caps;
    ADL_OVERDRIVE5_POWERCONTROLINFO_GET		ADL_Overdrive5_PowerControlInfo_Get;
    ADL_OVERDRIVE5_POWERCONTROL_GET			ADL_Overdrive5_PowerControl_Get;
    ADL_OVERDRIVE5_POWERCONTROL_SET			ADL_Overdrive5_PowerControl_Set;
    ADL_OVERDRIVE6_STATE_SET				ADL_Overdrive6_State_Set;

    LPAdapterInfo lpAdapterInfo = NULL;
    int  ADL_Err = ADL_ERR;
    int  iNumberAdapters = 0;
    int  iOverdriveSupported = 0;
    int  iOverdriveEnabled = 0;
    ADLFanSpeedInfo fanSpeedInfo = { 0 };
    int	 iOverdriveVersion = 0;
    int fanSpeedReportingMethod = 0;
    int maxThermalControllerIndex = 0;
    ADLODPerformanceLevels* performanceLevels = 0;
    int powerControlSupported = 0;
    ADLPowerControlInfo powerControlInfo = { 0 };
    int powerControlCurrent = 0;
    int powerControlDefault = 0;


    ADL_Overdrive5_ThermalDevices_Enum = (ADL_OVERDRIVE5_THERMALDEVICES_ENUM)GetProcAddress(hDLL, "ADL_Overdrive5_ThermalDevices_Enum");
    ADL_Overdrive5_ODParameters_Get = (ADL_OVERDRIVE5_ODPARAMETERS_GET)GetProcAddress(hDLL, "ADL_Overdrive5_ODParameters_Get");
    ADL_Overdrive5_Temperature_Get = (ADL_OVERDRIVE5_TEMPERATURE_GET)GetProcAddress(hDLL, "ADL_Overdrive5_Temperature_Get");
    ADL_Overdrive5_FanSpeed_Get = (ADL_OVERDRIVE5_FANSPEED_GET)GetProcAddress(hDLL, "ADL_Overdrive5_FanSpeed_Get");
    ADL_Overdrive5_FanSpeedInfo_Get = (ADL_OVERDRIVE5_FANSPEEDINFO_GET)GetProcAddress(hDLL, "ADL_Overdrive5_FanSpeedInfo_Get");
    ADL_Overdrive5_ODPerformanceLevels_Get = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET)GetProcAddress(hDLL, "ADL_Overdrive5_ODPerformanceLevels_Get");
    ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(hDLL, "ADL_Overdrive5_CurrentActivity_Get");
    ADL_Overdrive5_FanSpeed_Set = (ADL_OVERDRIVE5_FANSPEED_SET)GetProcAddress(hDLL, "ADL_Overdrive5_FanSpeed_Set");
    ADL_Overdrive5_ODPerformanceLevels_Set = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET)GetProcAddress(hDLL, "ADL_Overdrive5_ODPerformanceLevels_Set");
    ADL_Overdrive5_PowerControl_Caps = (ADL_OVERDRIVE5_POWERCONTROL_CAPS)GetProcAddress(hDLL, "ADL_Overdrive5_PowerControl_Caps");
    ADL_Overdrive5_PowerControlInfo_Get = (ADL_OVERDRIVE5_POWERCONTROLINFO_GET)GetProcAddress(hDLL, "ADL_Overdrive5_PowerControlInfo_Get");
    ADL_Overdrive5_PowerControl_Get = (ADL_OVERDRIVE5_POWERCONTROL_GET)GetProcAddress(hDLL, "ADL_Overdrive5_PowerControl_Get");
    ADL_Overdrive5_PowerControl_Set = (ADL_OVERDRIVE5_POWERCONTROL_SET)GetProcAddress(hDLL, "ADL_Overdrive5_PowerControl_Set");
    ADL_Overdrive6_State_Set = (ADL_OVERDRIVE6_STATE_SET)GetProcAddress(hDLL, "ADL_Overdrive6_State_Set");

    if (
        NULL == ADL_Overdrive5_ThermalDevices_Enum ||
        NULL == ADL_Overdrive5_Temperature_Get ||
        NULL == ADL_Overdrive5_FanSpeedInfo_Get ||
        NULL == ADL_Overdrive5_ODPerformanceLevels_Get ||
        NULL == ADL_Overdrive5_ODParameters_Get ||
        NULL == ADL_Overdrive5_CurrentActivity_Get ||
        NULL == ADL_Overdrive5_FanSpeed_Set ||
        NULL == ADL_Overdrive5_ODPerformanceLevels_Set ||
        NULL == ADL_Overdrive5_PowerControl_Caps ||
        NULL == ADL_Overdrive5_PowerControlInfo_Get ||
        NULL == ADL_Overdrive5_PowerControl_Get ||
        NULL == ADL_Overdrive5_PowerControl_Set)
    {
        printf("ADL's API is missing!\n");
        return 0;
    }

    ADLThermalControllerInfo termalControllerInfo = { 0 };
    termalControllerInfo.iSize = sizeof(ADLThermalControllerInfo);

    for (int iThermalControllerIndex = 0; iThermalControllerIndex < 10; iThermalControllerIndex++)
    {
        ADL_Err = ADL_Overdrive5_ThermalDevices_Enum(adapterId, iThermalControllerIndex, &termalControllerInfo);

        if (ADL_Err == ADL_WARNING_NO_DATA)
        {
            maxThermalControllerIndex = iThermalControllerIndex - 1;
            break;
        }

        if (ADL_Err == ADL_WARNING_NO_DATA)
        {
            printf("Failed to enumerate thermal devices\n");
            return 0;
        }

        if (termalControllerInfo.iThermalDomain == ADL_DL_THERMAL_DOMAIN_GPU)
        {
            ADLTemperature adlTemperature = { 0 };
            adlTemperature.iSize = sizeof(ADLTemperature);
            if (ADL_OK != ADL_Overdrive5_Temperature_Get(adapterId, iThermalControllerIndex, &adlTemperature))
            {
                printf("Failed to get thermal devices temperature\n");
                return 0;
            }
            int temperatureInDegreesCelsius = adlTemperature.iTemperature / 1000; // The temperature is returned in millidegrees Celsius.


            fanSpeedInfo.iSize = sizeof(ADLFanSpeedInfo);
            if (ADL_OK != ADL_Overdrive5_FanSpeedInfo_Get(adapterId, iThermalControllerIndex, &fanSpeedInfo))
            {
                printf("Failed to get fan caps\n");
                return 0;
            }

            ADLFanSpeedValue fanSpeedValue = { 0 };
            fanSpeedReportingMethod = ((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_READ) == ADL_DL_FANCTRL_SUPPORTS_RPM_READ) ? ADL_DL_FANCTRL_SPEED_TYPE_RPM : ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
            //Set to ADL_DL_FANCTRL_SPEED_TYPE_RPM or to ADL_DL_FANCTRL_SPEED_TYPE_PERCENT to request fan speed to be returned in rounds per minute or in percentage points.
            //Note that the call might fail if requested fan speed reporting method is not supported by the GPU.
            fanSpeedValue.iSpeedType = fanSpeedReportingMethod;
            if (ADL_OK != ADL_Overdrive5_FanSpeed_Get(adapterId, iThermalControllerIndex, &fanSpeedValue))
            {
                printf("Failed to get fan speed\n");
                return 0;
            }

            printf("Thermal controller id:%d \n", iThermalControllerIndex);
            printf("Current temperature: %d\n", temperatureInDegreesCelsius);
            if (fanSpeedReportingMethod == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
            {
                printf("Current fan speed: %d rpm\n", fanSpeedValue.iFanSpeed);
                printf("Minimum fan speed: %d rpm\n", fanSpeedInfo.iMinRPM);
                printf("Maximum fan speed: %d rpm\n", fanSpeedInfo.iMaxRPM);
            }
            else
            {
                printf("Current fan speed: %d percent\n", fanSpeedValue.iFanSpeed);
                printf("Minimum fan speed: %d percent\n", fanSpeedInfo.iMinPercent);
                printf("Maximum fan speed: %d percent\n", fanSpeedInfo.iMaxPercent);
            }
        }
    }

    if (ADL_OK != ADL_Overdrive5_PowerControl_Caps(adapterId, &powerControlSupported))
    {
        printf("Failed to get Power Controls support\n");
        return 0;
    }

    if (powerControlSupported)
    {
        if (ADL_OK != ADL_Overdrive5_PowerControlInfo_Get(adapterId, &powerControlInfo))
        {
            printf("Failed to get Power Controls Info\n");
            return 0;
        }

        if (ADL_OK != ADL_Overdrive5_PowerControl_Get(adapterId, &powerControlCurrent, &powerControlDefault))
        {
            printf("Failed to get Power Control current value\n");
            return 0;
        }

        printf("The Power Control threshold range is %d to %d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        printf("Current value of Power Control threshold is %d \n", powerControlCurrent);
    }



    ADLODParameters overdriveParameters = { 0 };
    overdriveParameters.iSize = sizeof(ADLODParameters);

    if (ADL_OK != ADL_Overdrive5_ODParameters_Get(adapterId, &overdriveParameters))
    {
        printf("Failed to get overdrive parameters\n");
        return 0;
    }

    printf("The GPU  Engine clock range is %d..%d MHz with step of %d Mhz \n",
        overdriveParameters.sEngineClock.iMin / 100,
        overdriveParameters.sEngineClock.iMax / 100,
        overdriveParameters.sEngineClock.iStep / 100);

    printf("The GPU  Memory clock range is %d..%d MHz with step of %d MHz\n",
        overdriveParameters.sMemoryClock.iMin / 100,
        overdriveParameters.sMemoryClock.iMax / 100,
        overdriveParameters.sMemoryClock.iStep);


    printf("The GPU  Core Voltage range is %d..%d with step of %d \n",
        overdriveParameters.sVddc.iMin,
        overdriveParameters.sVddc.iMax,
        overdriveParameters.sVddc.iStep);


    if (overdriveParameters.iNumberOfPerformanceLevels > 0)
    {
        //Overdrive5 GPUs support few performance levels. Application can set distinct engine clocks, memory clocks, core voltage
        //for each of the performance levels. Performance level with index 0 corresponds to lowest performance system state.
        //Performance level with highest index corresponds to highest performance system state Ц 3D game playing for example.
        //Users are usually interested in overclocking highest index performance level.

        printf("The GPU supports %d performance levels: \n", overdriveParameters.iNumberOfPerformanceLevels);

        int size = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (overdriveParameters.iNumberOfPerformanceLevels - 1);
        void* performanceLevelsBuffer = malloc(size);
        memset(performanceLevelsBuffer, 0, size);
        performanceLevels = (ADLODPerformanceLevels*)performanceLevelsBuffer;
        performanceLevels->iSize = size;


        if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Get(adapterId, 1/*Getting default values first*/, performanceLevels))
        {
            printf("Failed to get information about supported performance levels.  \n");
            return 0;
        }

        for (int i = 0; i < overdriveParameters.iNumberOfPerformanceLevels; i++)
        {
            printf("Performance level %d - Default Engine Clock:%d MHz, Default Memory Clock:%d MHz, Default Core Voltage:%d \n",
                i,
                performanceLevels->aLevels[i].iEngineClock / 100,
                performanceLevels->aLevels[i].iMemoryClock / 100,
                performanceLevels->aLevels[i].iVddc);
        }

        memset(performanceLevelsBuffer, 0, size);
        performanceLevels->iSize = size;

        if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Get(adapterId, 0/*Getting current values first*/, performanceLevels))
        {
            printf("Failed to get information about supported performance levels.  \n");
            return 0;
        }

        for (int i = 0; i < overdriveParameters.iNumberOfPerformanceLevels; i++)
        {
            printf("Performance level %d - Current Engine Clock:%d MHz, Current Memory Clock:%d MHz, Current Core Voltage:%d \n",
                i,
                performanceLevels->aLevels[i].iEngineClock / 100,
                performanceLevels->aLevels[i].iMemoryClock / 100,
                performanceLevels->aLevels[i].iVddc);
        }
    }

    //Getting real current values for clocks, performance levels, voltage effective in the system.
    ADLPMActivity activity = { 0 };
    activity.iSize = sizeof(ADLPMActivity);
    if (ADL_OK != ADL_Overdrive5_CurrentActivity_Get(adapterId, &activity))
    {
        printf("Failed to get current GPU activity.  \n");
        return 0;
    }
    printf("Current Engine Clock: %d MHz\n", activity.iEngineClock / 100);
    printf("Current Memory Clock: %d MHz\n", activity.iMemoryClock / 100);
    printf("Current Core Voltage: %d \n", activity.iVddc);
    printf("Current Performance Level: %d \n", activity.iCurrentPerformanceLevel);

    if (overdriveParameters.iActivityReportingSupported)
    {
        printf("Current Engine Clock: %d persent\n", activity.iActivityPercent);
    }

    char input_str[10] = "";

    if (((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE) == ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE) ||
        ((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_PERCENT_WRITE) == ADL_DL_FANCTRL_SUPPORTS_PERCENT_WRITE))
    {
        printf("Do you want to change the fan speed(y/n)?:\n");
        gets_s(input_str);
        if (_stricmp(input_str, "y") == 0)
        {
            printf("Enter thermal controller id between 0 and %d \n", maxThermalControllerIndex);
            gets_s(input_str);
            int thermalController = atoi(input_str);

            if ((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE) == ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE)
            {
                printf("Enter requested fan speed between %d and %d RPM:\n", fanSpeedInfo.iMinRPM, fanSpeedInfo.iMaxRPM);
            }
            else
                printf("Enter requested fan speed between %d and %d Percent :\n", fanSpeedInfo.iMinPercent, fanSpeedInfo.iMaxPercent);

            gets_s(input_str);
            int fanSpeed = atoi(input_str);

            ADLFanSpeedValue newFanSpeed = { 0 };
            newFanSpeed.iSpeedType = fanSpeedReportingMethod;
            newFanSpeed.iFanSpeed = fanSpeed;

            if (ADL_OK != ADL_Overdrive5_FanSpeed_Set(adapterId, thermalController, &newFanSpeed))
            {
                printf("Failed to set new fan speed. \n");
                return 0;
            }
        }
    }

    if (overdriveParameters.iNumberOfPerformanceLevels > 0)
    {
        //High performance level is the one that affects 3d graphics during game playing.
        //In most of the cases the applications will be interested in changing only high performance level attributes.
        printf("Do you want to change the Engine Clock of high performance level(y/n)?:\n");

        gets_s(input_str);
        if (_stricmp(input_str, "y") == 0)
        {
            printf("Enter new requested Engine Clock between %d and %d MHz: \n",
                (overdriveParameters.iNumberOfPerformanceLevels == 1) ? overdriveParameters.sEngineClock.iMin / 100 :
                performanceLevels->aLevels[overdriveParameters.iNumberOfPerformanceLevels - 2].iEngineClock / 100,
                overdriveParameters.sEngineClock.iMax / 100);

            gets_s(input_str);
            int clocks = atoi(input_str) * 100;

            performanceLevels->aLevels[overdriveParameters.iNumberOfPerformanceLevels - 1].iEngineClock = clocks;

            if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Set(adapterId, performanceLevels))
            {
                printf("Failed to set new Engine Clock. \n");
                return 0;
            }
        }
        else
        {
            printf("Do you want to change the Memory Clock of high performance level(y/n)?:\n");
            gets_s(input_str);
            if (_stricmp(input_str, "y") == 0)
            {
                printf("Enter new requested Memory Clock between %d and %d MHz: \n",
                    (overdriveParameters.iNumberOfPerformanceLevels == 1) ? overdriveParameters.sMemoryClock.iMin / 100 :
                    performanceLevels->aLevels[overdriveParameters.iNumberOfPerformanceLevels - 2].iMemoryClock / 100,
                    overdriveParameters.sMemoryClock.iMax / 100);

                gets_s(input_str);
                int clocks = atoi(input_str) * 100;

                performanceLevels->aLevels[overdriveParameters.iNumberOfPerformanceLevels - 1].iMemoryClock = clocks;

                if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Set(adapterId, performanceLevels))
                {
                    printf("Failed to set new Memory Clock. \n");
                    return 0;
                }
            }
            else if (powerControlSupported)
            {
                printf("Do you want to change the value of Power Control threshold (y/n)?:\n");
                gets_s(input_str);
                if (_stricmp(input_str, "y") == 0)
                {
                    printf("Enter new value of Power Control threshold between %d and %d: \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue);
                    gets_s(input_str);
                    int newPowerControlTreshold = atoi(input_str);
                    if (ADL_OK != ADL_Overdrive5_PowerControl_Set(adapterId, newPowerControlTreshold))
                    {
                        printf("Failed to set new Power Control treshold. \n");
                        return 0;
                    }
                }
            }
        }
    }
    return 0;
}


#if defined (LINUX)
int Overdrive6Sample(int adapterId, void* hDLL)
#else
int Overdrive6Sample(int adapterId, HINSTANCE hDLL)
#endif
{
    ADLOD6FanSpeedInfo fanSpeedInfo = { 0 };
    ADLOD6ThermalControllerCaps thermalControllerCaps = { 0 };
    ADLOD6Capabilities od6Capabilities = { 0 };
    int temperature = 0;
    ADLOD6CurrentStatus currentStatus = { 0 };
    int powerControlSupported = 0;
    ADLOD6PowerControlInfo powerControlInfo = { 0 };
    int powerControlCurrent = 0;
    int powerControlDefault = 0;
    ADLOD6FanSpeedValue fanSpeedValue = { 0 };

    ADL_OVERDRIVE6_FANSPEED_GET ADL_Overdrive6_FanSpeed_Get;
    ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS ADL_Overdrive6_ThermalController_Caps;
    ADL_OVERDRIVE6_TEMPERATURE_GET ADL_Overdrive6_Temperature_Get;
    ADL_OVERDRIVE6_CAPABILITIES_GET ADL_Overdrive6_Capabilities_Get;
    ADL_OVERDRIVE6_STATEINFO_GET ADL_Overdrive6_StateInfo_Get;
    ADL_OVERDRIVE6_CURRENTSTATUS_GET ADL_Overdrive6_CurrentStatus_Get;
    ADL_OVERDRIVE6_POWERCONTROL_CAPS ADL_Overdrive6_PowerControl_Caps;
    ADL_OVERDRIVE6_POWERCONTROLINFO_GET ADL_Overdrive6_PowerControlInfo_Get;
    ADL_OVERDRIVE6_POWERCONTROL_GET ADL_Overdrive6_PowerControl_Get;
    ADL_OVERDRIVE6_FANSPEED_SET ADL_Overdrive6_FanSpeed_Set;
    ADL_OVERDRIVE6_STATE_SET ADL_Overdrive6_State_Set;
    ADL_OVERDRIVE6_POWERCONTROL_SET ADL_Overdrive6_PowerControl_Set;



    ADL_Overdrive6_FanSpeed_Get = (ADL_OVERDRIVE6_FANSPEED_GET)GetProcAddress(hDLL, "ADL_Overdrive6_FanSpeed_Get");
    ADL_Overdrive6_ThermalController_Caps = (ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS)GetProcAddress(hDLL, "ADL_Overdrive6_ThermalController_Caps");
    ADL_Overdrive6_Temperature_Get = (ADL_OVERDRIVE6_TEMPERATURE_GET)GetProcAddress(hDLL, "ADL_Overdrive6_Temperature_Get");
    ADL_Overdrive6_Capabilities_Get = (ADL_OVERDRIVE6_CAPABILITIES_GET)GetProcAddress(hDLL, "ADL_Overdrive6_Capabilities_Get");
    ADL_Overdrive6_StateInfo_Get = (ADL_OVERDRIVE6_STATEINFO_GET)GetProcAddress(hDLL, "ADL_Overdrive6_StateInfo_Get");
    ADL_Overdrive6_CurrentStatus_Get = (ADL_OVERDRIVE6_CURRENTSTATUS_GET)GetProcAddress(hDLL, "ADL_Overdrive6_CurrentStatus_Get");
    ADL_Overdrive6_PowerControl_Caps = (ADL_OVERDRIVE6_POWERCONTROL_CAPS)GetProcAddress(hDLL, "ADL_Overdrive6_PowerControl_Caps");
    ADL_Overdrive6_PowerControlInfo_Get = (ADL_OVERDRIVE6_POWERCONTROLINFO_GET)GetProcAddress(hDLL, "ADL_Overdrive6_PowerControlInfo_Get");
    ADL_Overdrive6_PowerControl_Get = (ADL_OVERDRIVE6_POWERCONTROL_GET)GetProcAddress(hDLL, "ADL_Overdrive6_PowerControl_Get");
    ADL_Overdrive6_FanSpeed_Set = (ADL_OVERDRIVE6_FANSPEED_SET)GetProcAddress(hDLL, "ADL_Overdrive6_FanSpeed_Set");
    ADL_Overdrive6_State_Set = (ADL_OVERDRIVE6_STATE_SET)GetProcAddress(hDLL, "ADL_Overdrive6_State_Set");
    ADL_Overdrive6_PowerControl_Set = (ADL_OVERDRIVE6_POWERCONTROL_SET)GetProcAddress(hDLL, "ADL_Overdrive6_PowerControl_Set");


    if (NULL == ADL_Overdrive6_FanSpeed_Get ||
        NULL == ADL_Overdrive6_ThermalController_Caps ||
        NULL == ADL_Overdrive6_Temperature_Get ||
        NULL == ADL_Overdrive6_Capabilities_Get ||
        NULL == ADL_Overdrive6_StateInfo_Get ||
        NULL == ADL_Overdrive6_CurrentStatus_Get ||
        NULL == ADL_Overdrive6_PowerControl_Caps ||
        NULL == ADL_Overdrive6_PowerControlInfo_Get ||
        NULL == ADL_Overdrive6_PowerControl_Get ||
        NULL == ADL_Overdrive6_FanSpeed_Set ||
        NULL == ADL_Overdrive6_State_Set ||
        NULL == ADL_Overdrive6_PowerControl_Set)
    {
        printf("ADL's API is missing!\n");
        return 0;
    }

    if (ADL_OK != ADL_Overdrive6_ThermalController_Caps(adapterId, &thermalControllerCaps))
    {
        printf("Failed to get thermal controller capabilities \n");
        return 0;
    }

    if (ADL_OD6_TCCAPS_FANSPEED_CONTROL == (thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_CONTROL)) //Verifies that fan speed controller exists on the GPU.
    {
        if (ADL_OD6_TCCAPS_FANSPEED_PERCENT_READ == (thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_PERCENT_READ) ||
            ADL_OD6_TCCAPS_FANSPEED_RPM_READ == (thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_RPM_READ))
        {
            /*fanSpeedInfo.iSpeedType = ((thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_RPM_READ) == ADL_OD6_TCCAPS_FANSPEED_RPM_READ)?
            ADL_OD6_FANSPEED_TYPE_RPM :
            ADL_OD6_FANSPEED_TYPE_PERCENT;*/

            if (ADL_OK != ADL_Overdrive6_FanSpeed_Get(adapterId, &fanSpeedInfo))
            {
                printf("Failed to get fan speed info\n");
                return 0;
            }

            if (ADL_OD6_TCCAPS_FANSPEED_RPM_READ == (thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_RPM_READ))
            {
                printf("Fan speed range: %d..%d rpm\n", thermalControllerCaps.iFanMinRPM, thermalControllerCaps.iFanMaxRPM);
            }
            else
            {
                printf("Fan speed range: %d..%d percent\n", thermalControllerCaps.iFanMinPercent, thermalControllerCaps.iFanMaxPercent);
            }


            if (ADL_OD6_FANSPEED_TYPE_RPM == (fanSpeedInfo.iSpeedType & ADL_OD6_FANSPEED_TYPE_RPM))
                printf("Current fan speed: %d rpm \n", fanSpeedInfo.iFanSpeedRPM);
            else
                printf("Current fan speed: %d percent \n", fanSpeedInfo.iFanSpeedPercent);
        }
    }

    if (ADL_OD6_TCCAPS_THERMAL_CONTROLLER == (thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_THERMAL_CONTROLLER)) //Verifies that thermal controller exists on the GPU.
    {
        if (ADL_OK != ADL_Overdrive6_Temperature_Get(adapterId, &temperature))
        {
            printf("Failed to get GPU temperature\n");
            return 0;
        }

        printf("GPU temperature is %d degrees celsius \n", temperature / 1000); //The temperature is returned in mili-degree of Celsius
    }

    if (ADL_OK != ADL_Overdrive6_Capabilities_Get(adapterId, &od6Capabilities))
    {
        printf("Failed to get Overdrive capabilities\n");
        return 0;
    }

    //Overdrive 6 uses performance levels to communicate effective minimum and maximum values for memory and GPU clocks
    //either requested by user or preconfigured by the driver defaults. Thus only 2 performance levels  should be returned.
    if (od6Capabilities.iNumberOfPerformanceLevels != 2)
    {
        printf("Unexpected number of performance levels\n");
        return 0;
    }

    //Calculating buffer needed to contain performance levels information returned by ADL_Overdrive6_StateInfo_Get.
    //The buffer contains instance of ADLOD6StateInfo at the beginning and trailing space needed to include extra ADLOD6PerformanceLevel structures.
    //Note that single ADLOD6PerformanceLevel structure is included in ADLOD6StateInfo itself. So the trailing space is needed to be large enough
    //to hold only one extra ADLOD6PerformanceLevel
    int size = sizeof(ADLOD6StateInfo) + sizeof(ADLOD6PerformanceLevel);
    ADLOD6StateInfo * defaultStateInfo = (ADLOD6StateInfo*)malloc(size);
    memset((void*)defaultStateInfo, 0, size);
    defaultStateInfo->iNumberOfPerformanceLevels = 2;

    //Getting default effective minimum and maximum values for memory and GPU clocks.
    //The only state supported by Overdrive6 is "Performance".
    if (ADL_OK != ADL_Overdrive6_StateInfo_Get(adapterId, ADL_OD6_GETSTATEINFO_DEFAULT_PERFORMANCE, defaultStateInfo))
    {
        printf("Failed to get default performance levels info\n");
        return 0;
    }

    ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*)malloc(size);
    memset((void*)customStateInfo, 0, size);
    customStateInfo->iNumberOfPerformanceLevels = 2;

    //Getting default effective minimum and maximum values for memory and GPU clocks.
    //The only state supported by Overdrive6 is "Performance".
    if (ADL_OK != ADL_Overdrive6_StateInfo_Get(adapterId, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo))
    {
        printf("Failed to get custom performance levels info\n");
        return 0;
    }

    if (ADL_OD6_CAPABILITY_SCLK_CUSTOMIZATION == (od6Capabilities.iCapabilities & ADL_OD6_CAPABILITY_SCLK_CUSTOMIZATION))
    {
        printf("Range of clock supported by GPU core: %d...%d MHz with step of %d MHz \n",
            od6Capabilities.sEngineClockRange.iMin / 100,
            od6Capabilities.sEngineClockRange.iMax / 100,
            od6Capabilities.sEngineClockRange.iStep / 100);

        printf("Default effective range of GPU core clock: %d .. %d \n", defaultStateInfo->aLevels[0].iEngineClock / 100, defaultStateInfo->aLevels[1].iEngineClock / 100);
        printf("Custom effective range of GPU core clock: %d .. %d \n", customStateInfo->aLevels[0].iEngineClock / 100, customStateInfo->aLevels[1].iEngineClock / 100);
    }

    if (ADL_OD6_CAPABILITY_MCLK_CUSTOMIZATION == (od6Capabilities.iCapabilities & ADL_OD6_CAPABILITY_MCLK_CUSTOMIZATION))
    {
        printf("Range of supported memory clock: %d...%d MHz with step of %d MHz \n",
            od6Capabilities.sMemoryClockRange.iMin / 100,
            od6Capabilities.sMemoryClockRange.iMax / 100,
            od6Capabilities.sMemoryClockRange.iStep / 100);

        printf("Default effective range of GPU memory clock: %d .. %d \n", defaultStateInfo->aLevels[0].iMemoryClock / 100, defaultStateInfo->aLevels[1].iMemoryClock / 100);
        printf("Custom effective range of GPU memory clock: %d .. %d \n", customStateInfo->aLevels[0].iMemoryClock / 100, customStateInfo->aLevels[1].iMemoryClock / 100);
    }

    if (ADL_OK != ADL_Overdrive6_CurrentStatus_Get(adapterId, &currentStatus))
    {
        printf("Failed to get custom performance levels info\n");
        return 0;
    }

    printf("Current GPU core clock: %d MHz \n", currentStatus.iEngineClock / 100);
    printf("Current GPU memory clock: %d MHz \n", currentStatus.iMemoryClock / 100);
    printf("Current number of PCI bus lanes: %d \n", currentStatus.iCurrentBusLanes);
    printf("Current PCI bus speed: %d \n", currentStatus.iCurrentBusSpeed);


    //First we need to verify that ASIC supports monitoring of its current activities before we attempt to retrieve its current clock
    if ((od6Capabilities.iCapabilities & ADL_OD6_CAPABILITY_GPU_ACTIVITY_MONITOR) == ADL_OD6_CAPABILITY_GPU_ACTIVITY_MONITOR)
    {
        printf("Current GPU activity level: %d percent \n", currentStatus.iActivityPercent);
    }

    if (ADL_OK != ADL_Overdrive6_PowerControl_Caps(adapterId, &powerControlSupported))
    {
        printf("Failed to get power control capabilities\n");
        return 0;
    }

    if (powerControlSupported)
    {
        if (ADL_OK != ADL_Overdrive6_PowerControlInfo_Get(adapterId, &powerControlInfo))
        {
            printf("Failed to get power control information\n");
            return 0;
        }

        if (ADL_OK != ADL_Overdrive6_PowerControl_Get(adapterId, &powerControlCurrent, &powerControlDefault))
        {
            printf("Failed to get power control current and default settings\n");
            return 0;
        }

        printf("Power Control range: %d...%d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        printf("Power Control current level: %d \n", powerControlCurrent);
        printf("Power Control default level: %d \n", powerControlDefault);
    }


    char input_str[10] = "";

    if (ADL_OD6_TCCAPS_FANSPEED_CONTROL == (thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_CONTROL) &&
        (
            ((thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE) == ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE)
            ||
            ((thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_RPM_WRITE) == ADL_OD6_TCCAPS_FANSPEED_RPM_WRITE)
            )
        )
    {
        printf("Do you want to change the fan speed(y/n)?:\n");
        gets_s(input_str);
        if (_stricmp(input_str, "y") == 0)
        {
            if ((thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE) == ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE)
            {
                printf("Enter requested fan speed between %d and %d Percent :\n", thermalControllerCaps.iFanMinPercent, thermalControllerCaps.iFanMaxPercent);
            }
            else
                printf("Enter requested fan speed between %d and %d RPM:\n", thermalControllerCaps.iFanMinRPM, thermalControllerCaps.iFanMaxRPM);


            gets_s(input_str);
            fanSpeedValue.iFanSpeed = atoi(input_str);
            fanSpeedValue.iSpeedType = ((thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE) == ADL_OD6_TCCAPS_FANSPEED_PERCENT_WRITE) ?
                ADL_OD6_FANSPEED_TYPE_PERCENT :
                ADL_OD6_FANSPEED_TYPE_RPM;

            if (ADL_OK != ADL_Overdrive6_FanSpeed_Set(adapterId, &fanSpeedValue))
            {
                printf("Failed to set new fan speed \n");
            }
            return 0;
        }
    }

    if (ADL_OD6_CAPABILITY_SCLK_CUSTOMIZATION == (od6Capabilities.iCapabilities & ADL_OD6_CAPABILITY_SCLK_CUSTOMIZATION))
    {
        printf("Do you want to change the effective core clock (y/n)?:\n");
        gets_s(input_str);
        if (_stricmp(input_str, "y") == 0)
        {
            //ADL_Overdrive6_State_Set allows changing the effective range of memory and core clocks by setting performance level 0 clocks to desired
            //minimum value and setting performance level 1 clocks to desired maximum. In reality user rarely needs to change minimum value.
            //Therefore this example demonstrates only changing the upper range boundary. The upper range value has to be greater then current
            //minimum value (stored in performance level 0), but less than highest clock supported by the ASIC
            printf("Enter value between %d and %d MHz \n", customStateInfo->aLevels[0].iEngineClock / 100, od6Capabilities.sEngineClockRange.iMax / 100);
            gets_s(input_str);
            customStateInfo->aLevels[1].iEngineClock = atoi(input_str) * 100;

            if (ADL_OK != ADL_Overdrive6_State_Set(adapterId, ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo)) // This is the only value that is currently accepted
            {
                printf("Failed to set new effective core clock\n");
            }
            return 0;
        }
    }

    if (ADL_OD6_CAPABILITY_MCLK_CUSTOMIZATION == (od6Capabilities.iCapabilities & ADL_OD6_CAPABILITY_MCLK_CUSTOMIZATION))
    {
        printf("Do you want to change the effective memory clock (y/n)?:\n");
        gets_s(input_str);
        if (_stricmp(input_str, "y") == 0)
        {
            //ADL_Overdrive6_State_Set allows changing the effective range of memory and core clocks by setting performance level 0 clocks to desired
            //minimum value and setting performance level 1 clocks to desired maximum. In reality user rarely needs to change minimum value.
            //Therefore this example demonstrates only changing the upper range boundary. The upper range value has to be greater then current
            //minimum value (stored in performance level 0), but less than highest clock supported by the ASIC
            printf("Enter value between %d and %d MHz \n", customStateInfo->aLevels[0].iMemoryClock / 100, od6Capabilities.sMemoryClockRange.iMax / 100);
            gets_s(input_str);
            customStateInfo->aLevels[1].iMemoryClock = atoi(input_str) * 100;

            if (ADL_OK != ADL_Overdrive6_State_Set(adapterId, ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo)) // This is the only value that is currently accepted
            {
                printf("Failed to set new effective core clock\n");
            }
            return 0;
        }
    }

    if (powerControlSupported)
    {
        printf("Do you want to change the value of Power Control threshold (y/n)?:\n");
        gets_s(input_str);
        if (_stricmp(input_str, "y") == 0)
        {
            printf("Enter new value of Power Control threshold between %d and %d: \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue);
            gets_s(input_str);
            int newPowerControlTreshold = atoi(input_str);
            if (ADL_OK != ADL_Overdrive6_PowerControl_Set(adapterId, newPowerControlTreshold))
            {
                printf("Failed to set new Power Control treshold. \n");
                return 0;
            }
        }
    }

    return 0;
}


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

ADLTemperatureFan Overdrive5_TemperaFan(	int adapterId, int iThermalControllerIndex, int SetFanSpeed )
{
    ADL_OVERDRIVE5_THERMALDEVICES_ENUM		ADL_Overdrive5_ThermalDevices_Enum;
    ADL_OVERDRIVE5_TEMPERATURE_GET			ADL_Overdrive5_Temperature_Get;
   
    ADL_OVERDRIVE5_FANSPEEDINFO_GET			ADL_Overdrive5_FanSpeedInfo_Get;
	ADL_OVERDRIVE5_FANSPEED_GET				ADL_Overdrive5_FanSpeed_Get;
    ADL_OVERDRIVE5_FANSPEED_SET				ADL_Overdrive5_FanSpeed_Set;
	
    LPAdapterInfo lpAdapterInfo = NULL;
    int  ADL_Err = ADL_ERR;
    int  iNumberAdapters = 0;
    int  iOverdriveSupported = 0;
    int  iOverdriveEnabled = 0;
	
    ADLFanSpeedInfo fanSpeedInfo = { 0 };
    int	 iOverdriveVersion = 0;
    int fanSpeedReportingMethod = 0;
    int maxThermalControllerIndex = 0;
	
	
	ADLTemperatureFan TemperatureFan = { 0 };
	
/*	
    ADLODPerformanceLevels* performanceLevels = 0;
    int powerControlSupported = 0;
    ADLPowerControlInfo powerControlInfo = { 0 };
    int powerControlCurrent = 0;
    int powerControlDefault = 0;
*/

    ADL_Overdrive5_ThermalDevices_Enum = (ADL_OVERDRIVE5_THERMALDEVICES_ENUM)GetProcAddress(hDLL, "ADL_Overdrive5_ThermalDevices_Enum");
    ADL_Overdrive5_Temperature_Get = (ADL_OVERDRIVE5_TEMPERATURE_GET)GetProcAddress(hDLL, "ADL_Overdrive5_Temperature_Get");
    ADL_Overdrive5_FanSpeedInfo_Get = (ADL_OVERDRIVE5_FANSPEEDINFO_GET)GetProcAddress(hDLL, "ADL_Overdrive5_FanSpeedInfo_Get");
	ADL_Overdrive5_FanSpeed_Get = (ADL_OVERDRIVE5_FANSPEED_GET)GetProcAddress(hDLL, "ADL_Overdrive5_FanSpeed_Get");
    ADL_Overdrive5_FanSpeed_Set = (ADL_OVERDRIVE5_FANSPEED_SET)GetProcAddress(hDLL, "ADL_Overdrive5_FanSpeed_Set");	

    if (
        NULL == ADL_Overdrive5_ThermalDevices_Enum ||
        NULL == ADL_Overdrive5_Temperature_Get ||
        NULL == ADL_Overdrive5_FanSpeedInfo_Get ||
		NULL == ADL_Overdrive5_FanSpeed_Get||
        NULL == ADL_Overdrive5_FanSpeed_Set)
		
    {
        printf("ADL's API is missing!\n");
		TemperatureFan.iTemperature = 0;
        return TemperatureFan;
    }

    ADLThermalControllerInfo termalControllerInfo = { 0 };
    termalControllerInfo.iSize = sizeof(ADLThermalControllerInfo);

//  for (int iThermalControllerIndex = 0; iThermalControllerIndex < 10; iThermalControllerIndex++)
    
        ADL_Err = ADL_Overdrive5_ThermalDevices_Enum(adapterId, iThermalControllerIndex, &termalControllerInfo);

        if (ADL_Err == ADL_WARNING_NO_DATA)
        {
            maxThermalControllerIndex = iThermalControllerIndex - 1;
        		//  use in for case break;
        }

        if (ADL_Err == ADL_WARNING_NO_DATA)
        {
            printf("Failed to enumerate thermal devices\n");
			TemperatureFan.iTemperature = 0;
        		return TemperatureFan;
        }

        if (termalControllerInfo.iThermalDomain == ADL_DL_THERMAL_DOMAIN_GPU)
        {
            ADLTemperature adlTemperature = { 0 };
            adlTemperature.iSize = sizeof(ADLTemperature);
            if (ADL_OK != ADL_Overdrive5_Temperature_Get(adapterId, iThermalControllerIndex, &adlTemperature))
            {
        	    		printf("Failed to get thermal devices temperature\n");
				TemperatureFan.iTemperature = 0;
        			return TemperatureFan;
            }
			
            int temperatureInDegreesCelsius = adlTemperature.iTemperature / 1000; // The temperature is returned in millidegrees Celsius.


            fanSpeedInfo.iSize = sizeof(ADLFanSpeedInfo);
            if (ADL_OK != ADL_Overdrive5_FanSpeedInfo_Get(adapterId, iThermalControllerIndex, &fanSpeedInfo))
            {
                	printf("Failed to get fan caps\n");
				TemperatureFan.iTemperature = 0;
        			return TemperatureFan;
            }

            ADLFanSpeedValue fanSpeedValue = { 0 };
            fanSpeedReportingMethod = ((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_READ) == ADL_DL_FANCTRL_SUPPORTS_RPM_READ) ? ADL_DL_FANCTRL_SPEED_TYPE_RPM : ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
            //Set to ADL_DL_FANCTRL_SPEED_TYPE_RPM or to ADL_DL_FANCTRL_SPEED_TYPE_PERCENT to request fan speed to be returned in rounds per minute or in percentage points.
            //Note that the call might fail if requested fan speed reporting method is not supported by the GPU.
            fanSpeedValue.iSpeedType = fanSpeedReportingMethod;
            if (ADL_OK != ADL_Overdrive5_FanSpeed_Get(adapterId, iThermalControllerIndex, &fanSpeedValue))
            {
                	printf("Failed to get fan speed\n");
				TemperatureFan.iTemperature = 0;
        			return TemperatureFan;
            }

            printf("Thermal controller id:%d \n", iThermalControllerIndex);
            printf("Current temperature: %d\n", temperatureInDegreesCelsius);
            if (fanSpeedReportingMethod == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
            {
                printf("Current fan speed: %d rpm\n", fanSpeedValue.iFanSpeed);
                printf("Minimum fan speed: %d rpm\n", fanSpeedInfo.iMinRPM);
                printf("Maximum fan speed: %d rpm\n", fanSpeedInfo.iMaxRPM);
            }
            else
            {
                printf("Current fan speed: %d percent\n", fanSpeedValue.iFanSpeed);
                printf("Minimum fan speed: %d percent\n", fanSpeedInfo.iMinPercent);
                printf("Maximum fan speed: %d percent\n", fanSpeedInfo.iMaxPercent);
            }
        
			TemperatureFan.iTemperature = temperatureInDegreesCelsius;
			TemperatureFan.iFanSpeed = fanSpeedValue.iFanSpeed;
			TemperatureFan.iMinRPM = fanSpeedInfo.iMinRPM;
	        TemperatureFan.iMaxRPM = fanSpeedInfo.iMaxRPM;
			TemperatureFan.iMinPercent = fanSpeedInfo.iMinPercent;
	        TemperatureFan.iMaxPercent = fanSpeedInfo.iMaxPercent;
			TemperatureFan.fanSpeedReportingMethod = fanSpeedReportingMethod;
		}
		
	// Если требуется поменять обороты ...
	if (0 > SetFanSpeed) 
	{
		if (((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE) == ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE) ||
			((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_PERCENT_WRITE) == ADL_DL_FANCTRL_SUPPORTS_PERCENT_WRITE))
		{
			// Этот код с условием ненужен 
            if ((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE) == ADL_DL_FANCTRL_SUPPORTS_RPM_WRITE)
            {	printf("Enter requested fan speed between %d and %d RPM:\n", fanSpeedInfo.iMinRPM, fanSpeedInfo.iMaxRPM);
            }   else
                printf("Enter requested fan speed between %d and %d Percent :\n", fanSpeedInfo.iMinPercent, fanSpeedInfo.iMaxPercent);

            ADLFanSpeedValue newFanSpeed = { 0 };
            newFanSpeed.iSpeedType = fanSpeedReportingMethod;
            newFanSpeed.iFanSpeed = SetFanSpeed;

            if (ADL_OK != ADL_Overdrive5_FanSpeed_Set(adapterId, iThermalControllerIndex, &newFanSpeed))
            {
                	printf("Failed to set new fan speed. \n");
				TemperatureFan.iTemperature = 0;
        			return TemperatureFan;
            }
		}
	} //SetFanSpeed
	
    return TemperatureFan;	// всё нормально 
}


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

ADLPowerControlInfo	Overdrive5_PowerCtl(int adapterId, int iPowerControlTreshold )
{
    ADL_OVERDRIVE5_POWERCONTROL_CAPS		ADL_Overdrive5_PowerControl_Caps;
    ADL_OVERDRIVE5_POWERCONTROLINFO_GET		ADL_Overdrive5_PowerControlInfo_Get;
    ADL_OVERDRIVE5_POWERCONTROL_GET			ADL_Overdrive5_PowerControl_Get;
    ADL_OVERDRIVE5_POWERCONTROL_SET			ADL_Overdrive5_PowerControl_Set;
	
/*
    LPAdapterInfo lpAdapterInfo = NULL;
    int  ADL_Err = ADL_ERR;
	
    int  iNumberAdapters = 0;
    int  iOverdriveSupported = 0;
    int  iOverdriveEnabled = 0;
    ADLFanSpeedInfo fanSpeedInfo = { 0 };
    int	 iOverdriveVersion = 0;
    int fanSpeedReportingMethod = 0;
    int maxThermalControllerIndex = 0;
	
    ADLODPerformanceLevels* performanceLevels = 0;
*/	
	
    int powerControlSupported = 0;
    ADLPowerControlInfo powerControlInfo = { 0 };
	ADLPowerControlInfo ZeroInfo = { 0 };
    int powerControlDefault = 0;
	int powerControlCurrent = 0;

    
	ADL_Overdrive5_PowerControl_Caps = (ADL_OVERDRIVE5_POWERCONTROL_CAPS)GetProcAddress(hDLL, "ADL_Overdrive5_PowerControl_Caps");
    ADL_Overdrive5_PowerControlInfo_Get = (ADL_OVERDRIVE5_POWERCONTROLINFO_GET)GetProcAddress(hDLL, "ADL_Overdrive5_PowerControlInfo_Get");
    ADL_Overdrive5_PowerControl_Get = (ADL_OVERDRIVE5_POWERCONTROL_GET)GetProcAddress(hDLL, "ADL_Overdrive5_PowerControl_Get");
    ADL_Overdrive5_PowerControl_Set = (ADL_OVERDRIVE5_POWERCONTROL_SET)GetProcAddress(hDLL, "ADL_Overdrive5_PowerControl_Set");
    

    if (NULL == ADL_Overdrive5_PowerControl_Caps ||
        NULL == ADL_Overdrive5_PowerControlInfo_Get ||
        NULL == ADL_Overdrive5_PowerControl_Get ||
        NULL == ADL_Overdrive5_PowerControl_Set)
    {
        	printf("ADL's API is missing!\n");
		return ZeroInfo;
    }

	
	// Управление  питанием 
    if (ADL_OK != ADL_Overdrive5_PowerControl_Caps(adapterId, &powerControlSupported))
    {
        printf("Failed to get Power Controls support\n");
		return ZeroInfo;
    }

    if (powerControlSupported)
    {
        if (ADL_OK != ADL_Overdrive5_PowerControlInfo_Get(adapterId, &powerControlInfo))
        {
            printf("Failed to get Power Controls Info\n");
			return ZeroInfo;
        }

		powerControlCurrent = 0;
        if (ADL_OK != ADL_Overdrive5_PowerControl_Get(adapterId, &powerControlCurrent, &powerControlDefault))
        {
            printf("Failed to get Power Control current value\n");
			return ZeroInfo;
        }

        printf("The Power Control threshold range is %d to %d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        printf("Current value of Power Control threshold is %d \n", powerControlCurrent);
    }

	/*	Вывод на рабочем устройстве, хрень какая-то :)
		The Power Control threshold range is -20 to 20 with step of 1 
		Current value of Power Control threshold is 20 
	*/


	if ((powerControlSupported) && (iPowerControlTreshold > 0))
	{
        // printf("Do you want to change the value of Power Control threshold (y/n)?:\n");
        // printf("Enter new value of Power Control threshold between %d and %d: \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue);
        if (ADL_OK != ADL_Overdrive5_PowerControl_Set(adapterId, iPowerControlTreshold))
        {
            printf("Failed to set new Power Control treshold. \n");
			return ZeroInfo;
		}
	}
	
	return powerControlInfo;	// всё нормально 
}


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

ADLPMActivity  Overdrive5_Clock( int adapterId, int SetEngineClock, int SetMemoryClock )
{
    ADL_OVERDRIVE5_CURRENTACTIVITY_GET		ADL_Overdrive5_CurrentActivity_Get;
    ADL_OVERDRIVE5_ODPARAMETERS_GET			ADL_Overdrive5_ODParameters_Get;	
    ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	ADL_Overdrive5_ODPerformanceLevels_Get;
    ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET  ADL_Overdrive5_ODPerformanceLevels_Set;

    LPAdapterInfo lpAdapterInfo = NULL;
	
    int  ADL_Err = ADL_ERR;
/*	
    int  iNumberAdapters = 0;
    int  iOverdriveSupported = 0;
    int  iOverdriveEnabled = 0;
    ADLFanSpeedInfo fanSpeedInfo = { 0 };
	
    int	 iOverdriveVersion = 0;
    int fanSpeedReportingMethod = 0;
    int maxThermalControllerIndex = 0;
*/
	
    ADLODPerformanceLevels* performanceLevels = 0;
    int powerControlSupported = 0;
	
/*    ADLPowerControlInfo powerControlInfo = { 0 };
    int powerControlCurrent = 0;
    int powerControlDefault = 0;
*/	
	
	ADLPMActivity ZeroInfo = { 0 };


    ADL_Overdrive5_ODParameters_Get = (ADL_OVERDRIVE5_ODPARAMETERS_GET)GetProcAddress(hDLL, "ADL_Overdrive5_ODParameters_Get");
    ADL_Overdrive5_ODPerformanceLevels_Get = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET)GetProcAddress(hDLL, "ADL_Overdrive5_ODPerformanceLevels_Get");
	ADL_Overdrive5_ODPerformanceLevels_Set = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET)GetProcAddress(hDLL, "ADL_Overdrive5_ODPerformanceLevels_Set");
    ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(hDLL, "ADL_Overdrive5_CurrentActivity_Get");
    


    if (NULL == ADL_Overdrive5_ODParameters_Get ||
        NULL == ADL_Overdrive5_CurrentActivity_Get ||		
        NULL == ADL_Overdrive5_ODPerformanceLevels_Get ||
        NULL == ADL_Overdrive5_ODPerformanceLevels_Set)
    {
        printf("ADL's API is missing!\n");
        return ZeroInfo;
    }
	
	// Управление частотами
    ADLODParameters overdriveParameters = { 0 };
    overdriveParameters.iSize = sizeof(ADLODParameters);

    if (ADL_OK != ADL_Overdrive5_ODParameters_Get(adapterId, &overdriveParameters))
    {
        printf("Failed to get overdrive parameters\n");
        return ZeroInfo;
    }

    printf("The GPU  Engine clock range is %d..%d MHz with step of %d Mhz \n",
        overdriveParameters.sEngineClock.iMin / 100,
        overdriveParameters.sEngineClock.iMax / 100,
        overdriveParameters.sEngineClock.iStep / 100);

    printf("The GPU  Memory clock range is %d..%d MHz with step of %d MHz\n",
        overdriveParameters.sMemoryClock.iMin / 100,
        overdriveParameters.sMemoryClock.iMax / 100,
        overdriveParameters.sMemoryClock.iStep);


    printf("The GPU  Core Voltage range is %d..%d with step of %d \n",
        overdriveParameters.sVddc.iMin,
        overdriveParameters.sVddc.iMax,
        overdriveParameters.sVddc.iStep);
	// Кусок выше нужно выделить в отдельную функцию,  здесь эти параметры возвращать не будем 



    if (overdriveParameters.iNumberOfPerformanceLevels > 0)
    {
        //Overdrive5 GPUs support few performance levels. Application can set distinct engine clocks, memory clocks, core voltage
        //for each of the performance levels. Performance level with index 0 corresponds to lowest performance system state.
        //Performance level with highest index corresponds to highest performance system state Ц 3D game playing for example.
        //Users are usually interested in overclocking highest index performance level.

        printf("The GPU supports %d performance levels: \n", overdriveParameters.iNumberOfPerformanceLevels);

        int size = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (overdriveParameters.iNumberOfPerformanceLevels - 1);
        void* performanceLevelsBuffer = malloc(size);
        memset(performanceLevelsBuffer, 0, size);
        performanceLevels = (ADLODPerformanceLevels*)performanceLevelsBuffer;
        performanceLevels->iSize = size;


        if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Get(adapterId, 1/*Getting default values first*/, performanceLevels))
        {
            printf("Failed to get information about supported performance levels.  \n");
            return ZeroInfo;
        }

        for (int i = 0; i < overdriveParameters.iNumberOfPerformanceLevels; i++)
        {
            printf("Performance level %d - Default Engine Clock:%d MHz, Default Memory Clock:%d MHz, Default Core Voltage:%d \n",
                i,
                performanceLevels->aLevels[i].iEngineClock / 100,
                performanceLevels->aLevels[i].iMemoryClock / 100,
                performanceLevels->aLevels[i].iVddc);
        }

        memset(performanceLevelsBuffer, 0, size);
        performanceLevels->iSize = size;

        if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Get(adapterId, 0/*Getting current values first*/, performanceLevels))
        {
            printf("Failed to get information about supported performance levels.  \n");
            return ZeroInfo;
        }

        for (int i = 0; i < overdriveParameters.iNumberOfPerformanceLevels; i++)
        {
            printf("Performance level %d - Current Engine Clock:%d MHz, Current Memory Clock:%d MHz, Current Core Voltage:%d \n",
                i,
                performanceLevels->aLevels[i].iEngineClock / 100,
                performanceLevels->aLevels[i].iMemoryClock / 100,
                performanceLevels->aLevels[i].iVddc);
        }
    }
	// Предустановки конфигурации, может быть интересно, сейчас оставим как заготовку  


	
    // Getting real current values for clocks, performance levels, voltage effective in the system.
	// Текущие настройки выбранного адаптера, их будем возвращать 
    ADLPMActivity activity = { 0 };
    activity.iSize = sizeof(ADLPMActivity);
    if (ADL_OK != ADL_Overdrive5_CurrentActivity_Get(adapterId, &activity))
    {
        printf("Failed to get current GPU activity.  \n");
        return ZeroInfo;
    }
    printf("Current Engine Clock: %d MHz\n", activity.iEngineClock / 100);
    printf("Current Memory Clock: %d MHz\n", activity.iMemoryClock / 100);
    printf("Current Core Voltage: %d \n", activity.iVddc);
    printf("Current Performance Level: %d \n", activity.iCurrentPerformanceLevel);
	

    if (overdriveParameters.iActivityReportingSupported)
    {
        printf("Current Engine Clock: %d persent\n", activity.iActivityPercent);
    }
	
	
	
	// Изменение параметров, если значение > 0
    if (overdriveParameters.iNumberOfPerformanceLevels > 0)
    {
        //High performance level is the one that affects 3d graphics during game playing.
        //In most of the cases the applications will be interested in changing only high performance level attributes.
        //printf("Do you want to change the Engine Clock of high performance level(y/n)?:\n");

        if ( SetEngineClock > 0 )
        {
            printf("Enter new requested Engine Clock between %d and %d MHz: \n",
                (overdriveParameters.iNumberOfPerformanceLevels == 1) ? overdriveParameters.sEngineClock.iMin / 100 :
                performanceLevels->aLevels[overdriveParameters.iNumberOfPerformanceLevels - 2].iEngineClock / 100,
                overdriveParameters.sEngineClock.iMax / 100);

				
            int clocks = SetEngineClock  * 100;
            performanceLevels->aLevels[overdriveParameters.iNumberOfPerformanceLevels - 1].iEngineClock = clocks;

            if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Set(adapterId, performanceLevels))
            {
                printf("Failed to set new Engine Clock. \n");
                return ZeroInfo;
            }
        }
        else
        {
            // printf("Do you want to change the Memory Clock of high performance level(y/n)?:\n");
            if ( SetMemoryClock > 0 )
            {
                printf("Enter new requested Memory Clock between %d and %d MHz: \n",
                    (overdriveParameters.iNumberOfPerformanceLevels == 1) ? overdriveParameters.sMemoryClock.iMin / 100 :
                    performanceLevels->aLevels[overdriveParameters.iNumberOfPerformanceLevels - 2].iMemoryClock / 100,
                    overdriveParameters.sMemoryClock.iMax / 100);


                int clocks = SetMemoryClock * 100;
                performanceLevels->aLevels[overdriveParameters.iNumberOfPerformanceLevels - 1].iMemoryClock = clocks;

                if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Set(adapterId, performanceLevels))
                {
                    printf("Failed to set new Memory Clock. \n");
                    return ZeroInfo;
                }
            }
          
        }
    }
	
    return activity;	// всё нормально
}


/* --------------------------------------------------------------------------
ADLODParameters Overdrive5_ClockRange(
		int adapterId, 
		выбрать желаемую плату GPU по номеру 

	Возвращает структуру ADLODParameters с возможными значениями переменных 
	The GPU  Engine clock range is %d..%d MHz with step of %d Mhz
    The GPU  Memory clock range is %d..%d MHz with step of %d MHz
    The GPU  Core Voltage range is %d..%d with step of %d 
*/

ADLODParameters Overdrive5_ClockRange( int adapterId )
{
    ADL_OVERDRIVE5_CURRENTACTIVITY_GET		ADL_Overdrive5_CurrentActivity_Get;
    ADL_OVERDRIVE5_ODPARAMETERS_GET			ADL_Overdrive5_ODParameters_Get;	
    ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	ADL_Overdrive5_ODPerformanceLevels_Get;
    ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET  	ADL_Overdrive5_ODPerformanceLevels_Set;

    LPAdapterInfo lpAdapterInfo = NULL;
	
    int  ADL_Err = ADL_ERR;
/*	
    int  iNumberAdapters = 0;
    int  iOverdriveSupported = 0;
    int  iOverdriveEnabled = 0;
    ADLFanSpeedInfo fanSpeedInfo = { 0 };
	
    int	 iOverdriveVersion = 0;
    int fanSpeedReportingMethod = 0;
    int maxThermalControllerIndex = 0;
*
	
    ADLODPerformanceLevels* performanceLevels = 0;
    int powerControlSupported = 0;

*	
    ADLPowerControlInfo powerControlInfo = { 0 };
    int powerControlCurrent = 0;
    int powerControlDefault = 0;
*/

 	ADLODParameters ZeroInfo = { 0 };

    ADL_Overdrive5_ODParameters_Get = (ADL_OVERDRIVE5_ODPARAMETERS_GET)GetProcAddress(hDLL, "ADL_Overdrive5_ODParameters_Get");
    ADL_Overdrive5_ODPerformanceLevels_Get = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET)GetProcAddress(hDLL, "ADL_Overdrive5_ODPerformanceLevels_Get");
	ADL_Overdrive5_ODPerformanceLevels_Set = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET)GetProcAddress(hDLL, "ADL_Overdrive5_ODPerformanceLevels_Set");
    ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(hDLL, "ADL_Overdrive5_CurrentActivity_Get");
    


    if (NULL == ADL_Overdrive5_ODParameters_Get ||
        NULL == ADL_Overdrive5_CurrentActivity_Get ||		
        NULL == ADL_Overdrive5_ODPerformanceLevels_Get ||
        NULL == ADL_Overdrive5_ODPerformanceLevels_Set)
    {
        printf("ADL's API is missing!\n");
        return ZeroInfo;
    }
	
	// Управление частотами
    ADLODParameters overdriveParameters = { 0 };
    overdriveParameters.iSize = sizeof(ADLODParameters);

    if (ADL_OK != ADL_Overdrive5_ODParameters_Get(adapterId, &overdriveParameters))
    {
        printf("Failed to get overdrive parameters\n");
        return ZeroInfo;
    }

    printf("The GPU  Engine clock range is %d..%d MHz with step of %d Mhz \n",
        overdriveParameters.sEngineClock.iMin / 100,
        overdriveParameters.sEngineClock.iMax / 100,
        overdriveParameters.sEngineClock.iStep / 100);

    printf("The GPU  Memory clock range is %d..%d MHz with step of %d MHz\n",
        overdriveParameters.sMemoryClock.iMin / 100,
        overdriveParameters.sMemoryClock.iMax / 100,
        overdriveParameters.sMemoryClock.iStep);


    printf("The GPU  Core Voltage range is %d..%d with step of %d \n",
        overdriveParameters.sVddc.iMin,
        overdriveParameters.sVddc.iMax,
        overdriveParameters.sVddc.iStep);

	return overdriveParameters;
}



// Текстовая расшифровка ошибок, ненужна но пусть будет  
// Небудем делать define, незачем мастерить из мухи слона 
char * ErrorToStr(int err_code)
{
	char * err_string;
	switch(err_code) {
		case 0:
			err_string = "ADL library not found!";
		break;
		
		case 1:
			err_string = "ADL's API is missing!";
		break;
		
		case 2:
			err_string = "ADL Initialization Memory Alloc Error!";
		break;
		
		case 3:
			err_string = "Cannot get the number of adapters!";
		break;
		
		case 4:
			err_string = "CanТt get Overdrive capabilities";
		break;
		
		
		case 5:
			err_string = "Overdrive version 5";
		break;
		case 6:
			err_string = "Overdrive version 6";
		break;
		case 7:
			err_string = "Unsupported Overdrive version";
		break;
		
		
		case 8:
			err_string = "Overdrive is not supported";
		break;
		
		default:
			err_string = "No option selected";
	}
	
	return err_string;
}



// Базовая функция инициализации, ранее называлась main  
// Возвращает ошибку или номер версии Overdrive 
int InitADL(void) 
{
#if defined (LINUX)
    void *hDLL;			// Handle to .so library
#else
    HINSTANCE hDLL;		// Handle to DLL
#endif

    ADL_MAIN_CONTROL_CREATE					ADL_Main_Control_Create;
    ADL_MAIN_CONTROL_DESTROY					ADL_Main_Control_Destroy;
    ADL_ADAPTER_NUMBEROFADAPTERS_GET			ADL_Adapter_NumberOfAdapters_Get;
    ADL_ADAPTER_ADAPTERINFO_GET				ADL_Adapter_AdapterInfo_Get;
    ADL_ADAPTER_ACTIVE_GET					ADL_Adapter_Active_Get;
    ADL_OVERDRIVE_CAPS						ADL_Overdrive_Caps;
    LPAdapterInfo						    lpAdapterInfo = NULL;
	
    int  i;
    int  ADL_Err = ADL_ERR;
	
/*  Сделаем эти переменные глобальными 
	int  iNumberAdapters = 0;
    int  iOverdriveSupported = 0;
    int  iOverdriveEnabled = 0;
    int	 iOverdriveVersion = 0;
*/	

#if defined (LINUX)
    hDLL = dlopen("libatiadlxx.so", RTLD_LAZY | RTLD_GLOBAL);
#else
    hDLL = LoadLibraryA("atiadlxx.dll");
    if (hDLL == NULL)
        // A 32 bit calling application on 64 bit OS will fail to LoadLIbrary.
        // Try to load the 32 bit library (atiadlxy.dll) instead
        hDLL = LoadLibraryA("atiadlxy.dll");
#endif

    if (NULL == hDLL)	{        		
		printf("ADL library not found!\n");
        return 0;
    }

    ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)GetProcAddress(hDLL, "ADL_Main_Control_Create");
    ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)GetProcAddress(hDLL, "ADL_Main_Control_Destroy");
    ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(hDLL, "ADL_Adapter_NumberOfAdapters_Get");
    ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET)GetProcAddress(hDLL, "ADL_Adapter_AdapterInfo_Get");
    ADL_Adapter_Active_Get = (ADL_ADAPTER_ACTIVE_GET)GetProcAddress(hDLL, "ADL_Adapter_Active_Get");
    ADL_Overdrive_Caps = (ADL_OVERDRIVE_CAPS)GetProcAddress(hDLL, "ADL_Overdrive_Caps");

    if (NULL == ADL_Main_Control_Create ||
        NULL == ADL_Main_Control_Destroy ||
        NULL == ADL_Adapter_NumberOfAdapters_Get ||
        NULL == ADL_Adapter_AdapterInfo_Get ||
        NULL == ADL_Adapter_Active_Get ||
        NULL == ADL_Overdrive_Caps
        ) 
	{
		printf("ADL's API is missing!\n");
		return 1;
	}
    

    // Initialize ADL. The second parameter is 1, which means:
    // retrieve adapter information only for adapters that are physically present and enabled in the system
    if (ADL_OK != ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1)) 
	{
        printf("ADL Initialization Error!\n");
		return 2;		        							
	}
    
    // Obtain the number of adapters for the system
    if (ADL_OK != ADL_Adapter_NumberOfAdapters_Get(&iNumberAdapters)) 
	{
		printf("Cannot get the number of adapters!\n");
	    return 3;											
	}
	

    if (0 < iNumberAdapters)
    {
        lpAdapterInfo = (LPAdapterInfo)malloc(sizeof(AdapterInfo) * iNumberAdapters);
        memset(lpAdapterInfo, '\0', sizeof(AdapterInfo) * iNumberAdapters);

        // Get the AdapterInfo structure for all adapters in the system
        ADL_Adapter_AdapterInfo_Get(lpAdapterInfo, sizeof(AdapterInfo) * iNumberAdapters);
    }

    // Looking for first present and active adapter in the system
	// Этот кусочек ненужен, пока оставим
    int adapterId = -1;
    for (i = 0; i < iNumberAdapters; i++)
    {
        int adapterActive = 0;
        AdapterInfo adapterInfo = lpAdapterInfo[i];
        ADL_Adapter_Active_Get(adapterInfo.iAdapterIndex, &adapterActive);

        printf("adapterActive = %x, iVendorID = %x \n", adapterActive,adapterInfo.iVendorID );

        if (adapterActive && adapterInfo.iVendorID == AMDVENDORID)
        {
            adapterId = adapterInfo.iAdapterIndex;
            break;
        }
    }

	
/*  if (-1 == adapterId)	{
        printf("Cannot find active AMD adapter\n");
        adapterId = 1;	// костылик ;)		*/	


    //Overdrive 5 APIs should be used if returned version indicates 5. Overdrive 6 APIs are used if 6 is returned.
    //Overdrive 5 is supported on legacy ASICs. Newer ASICs (CIK+) should report Overdrive 6
    if (ADL_OK != ADL_Overdrive_Caps(adapterId, &iOverdriveSupported, &iOverdriveEnabled, &iOverdriveVersion))
	{
		printf("CanТt get Overdrive capabilities \n");
        return 4;							
	}		

    if (!iOverdriveSupported)				
	{
		printf("Overdrive is not supported \n");
        return 8;
	}

	if (iOverdriveVersion == 5) {
		printf("Overdrive version 5 \n");
		return 5;				
	} else if (iOverdriveVersion == 6) {
		printf("Overdrive version 6 \n");
		return 6;					
	}    else {
		printf("Unsupported Overdrive version \n");
		return 7;							
	}		
}



