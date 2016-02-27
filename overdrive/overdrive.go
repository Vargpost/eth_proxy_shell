package overdrive

/*
#cgo CFLAGS: -I.
#cgo LDFLAGS: /home/egorka/rb2/include/lnk.a -ldl -lstdc++
#include "../include/lnk.h"
*/
import "C"
import (
	"errors"
	"fmt"
	"log"
	"time"
)

type GpuServer struct {
	config *Config

	currentGPU int32        // Текущий обслуживаемый адаптер
	groupGpu   []*DeviceGPU // список адаптеров и их параметров
	timeout    time.Duration

	NumberAdapters     int
	OverdriveSupported int
	OverdriveEnabled   int
	OverdriveVersion   int
}

func PollingGPU(cfg *Config) *GpuServer {

	overdrive := &GpuServer{config: cfg}
	overdrive.groupGpu = make([]*DeviceGPU, len(cfg.DeviceGPU))

	// Инициализация драйвера ADL
	// по результату определяем версию поддерживаемую драйвером,
	// сколько адаптеров GPU установленно
	// и вообще получилось ли к нему корректно подключится
	res := C.InitADL()
	// test res := 5

	// Overdrive version 5 or 6
	if (5 == res) || (6 == res) {
		overdrive.OverdriveVersion = С.OverdriveVersion()
		overdrive.NumberAdapters = С.NumberAdapters()
		log.Printf("Overdrive version => %s", res)
	} else {
		// Нет драйвера AD - завершаем работу сервера
		log.Printf("Error Overdrive \n")
		return nil
	}

	// Количество установленных адаптеров для справки
	// Всё равно работаем только с адаптерами описанными в конфигурации
	// Но проверяем чтобы адаптеров было не больше чем есть в железе на самом деле
	// overdrive.NumberAdapters = C.NumberAdapters()

	for i, v := range cfg.DeviceGPU {
		dGpu, err := NewGpu(v.Name,
			v.AdapterId,
			v.ThermalController,
			v.FanSpeed,
			v.PowerControlTreshold,
			v.EngineClock,
			v.MemoryClock)

		if err != nil {
			log.Fatal(err)
		} else {

			// Список используемых адаптеров
			overdrive.groupGpu[i] = dGpu
			log.Printf("Device GPU: %s => %s", v.Name, v.AdapterId)
		}
	}

	checkIntv, _ := time.ParseDuration(cfg.GpuCheckInterval)
	checkTimer := time.NewTimer(checkIntv)

	go func() {
		for {
			select {
			case <-checkTimer.C:
				overdrive.checkGpu()
				checkTimer.Reset(checkIntv)
			}
		}
	}()

	return overdrive
}

// Проверить доступность адаптера и корректность параметров GPU считанных из конфигурации
// Функцию будем использовать в дальнейшем для смены уставок
func NewGpu(Name string, AdapterId, ThermalController, FanSpeed,
	PowerControlTreshold, EngineClock, MemoryClock int) (*DeviceGPU, error) {

	if AdapterId < C.NumberAdapters() {
		// Есть адаптер GPU с номером в диапазоне имеющихся
		// Вытащим основные параметры уставок из свойств драйвера
		// Сразу установим настройки из конфигурации
		dGpu := &DeviceGPU{Name: Name,
			AdapterId:            AdapterId,
			ThermalController:    ThermalController,
			FanSpeed:             FanSpeed,
			PowerControlTreshold: PowerControlTreshold,
			EngineClock:          EngineClock,
			MemoryClock:          MemoryClock}

		/* вЫключим работу с драйвером, для сборки под Win */

		var tf C.ADLTemperatureFan
		tf = C.Overdrive5_TemperaFan(AdapterId, ThermalController, FanSpeed)
		// Обороты вентилятора СРАЗУ меняем, используем контроллер температуры из конфигурации
		// TODO в дальнейшем сделать поиск всех доступных контроллеров температуры в составе адаптера
		// их может быть больше одного, например несколько вентиляторов

		// Текущая температура и обороты
		dGpu.Value.Temperature = tf.iTemperature
		dGpu.Value.Fan = tf.iFanSpeed
		dGpu.Value.FanSpeedReport = tf.fanSpeedReportingMethod

		// RPM or Percent
		// define ADL_DL_FANCTRL_SPEED_TYPE_PERCENT    	1
		// define ADL_DL_FANCTRL_SPEED_TYPE_RPM  		2

		if 1 != dGpu.Value.FanSpeedReport {
			dGpu.Value.FanMin = tf.iMinRPM
			dGpu.Value.FanMax = tf.iMinRPM
		} else {
			// Значения возможно пересчитать в RPM, для удобства восприятия
			dGpu.Value.FanMin = tf.iMinPercent
			dGpu.Value.FanMax = tf.iMaxPercent
		}

		// непонятный параметр, сделаем заглушку защиты от изменения
		PowerControlTreshold = 0
		var pci С.ADLPowerControlInfo
		pci = C.Overdrive5_PowerCtl(AdapterId, PowerControlTreshold)
		dGpu.Value.PwmMinValue = pci.iMinValue
		dGpu.Value.PwmMaxValue = pci.iMaxValue
		dGpu.Value.PwmStepValue = pci.iStepValue
		// Заглушка на возвращаемый результат
		dGpu.Value.PwrControlCurrent = 0

		// Заглушки для того чтобы ничего не сломать :)
		EngineClock = 0
		MemoryClock = 0
		var clk C.ADLPMActivity
		clk = Overdrive5_Clock(AdapterId, EngineClock, MemoryClock)
		// Текущие значения настроек
		dGpu.Value.iActivityPercent = clk.iActivityPercent
		dGpu.Value.iEngineClock = clk.iEngineClock
		dGpu.Value.iMemoryClock = clk.iMemoryClock
		dGpu.Value.iVddc = clk.iVddc

		// Границы уставок частот пока грузить не будем
		// ADLODParameters Overdrive5_ClockRange( int adapterId );

		/*Для сборки под Win */

		return dGpu, nil
	} else {
		return nil, errors.New("No available Gpu")
	}
}

/* typedef struct ADLPMActivity
{
/// Must be set to the size of the structure
	int iSize;
/// Current engine clock.
	int iEngineClock;
/// Current memory clock.
	int iMemoryClock;
/// Current core voltage.
	int iVddc;
/// GPU utilization.
	int iActivityPercent;
/// Performance level index.
	int iCurrentPerformanceLevel;
/// Current PCIE bus speed.
	int iCurrentBusSpeed;
/// Number of PCIE bus lanes.
	int iCurrentBusLanes;
/// Maximum number of PCIE bus lanes.
	int iMaximumBusLanes;
/// Reserved for future purposes.
	int iReserved;
} ADLPMActivity;



ADLODParameters
Границы уставок

  ADLODParameterRange sEngineClock;
/// Memory clock range.
  ADLODParameterRange sMemoryClock;
/// Core voltage range.
  ADLODParameterRange sVddc;
*/

// Функция вызывается по таймеру и обновляет массив состояния адаптеров
// Сейчас _только_ считывание значений параметров
// Здесь же возможно включить удалённое изменение конфигурации
func (s *GpuServer) checkGpu() {

	for i, v := range s.groupGpu {

		// TODO разобратся с выделением памяти
		dGpu, err := NewGpu(v.Name,
			v.AdapterId,
			v.ThermalController,
			0, // v.FanSpeed,
			0, // v.PowerControlTreshold,
			0, // v.EngineClock,
			0) // v.MemoryClock)
		if err != nil {
			log.Fatal(err)
		} else {
			// Полученную телеметрию помещаем обратно в список адаптеров GPU
			s.groupGpu[i] = dGpu
			log.Printf("Read GPU: %i ", v.AdapterId)
		}
	}
}

func m4() {

	// Указатель на переменную типа ADLDisplayProperty
	// Но нам желательна просто переменная, а не указатель или ссылка
	// rt := new(C.ADLDisplayProperty)

	/* Вариант с простой переменной, позволит очистить код от "птичего языка"
	var rt C.ADLDisplayProperty
	var tf C.ADLTemperatureFan

	rt.iPropertyType = 10
	rt.iExpansionMode = 14

	rt = C.TeDL()
	fmt.Printf("ADLDisplayProperty %d  %d \n", rt.iPropertyType, rt.iExpansionMode)

	C.InitADL()
	tf = C.Overdrive5_TemperaFan(1, 0, 0)
	fmt.Printf("ADLTemperatureFan %d  %d \n", tf.iTemperature, tf.iFanSpeed)
	*/
	fmt.Printf("m4 finished \n")
}

//go build -ldflags "-linkmode external -extldflags -static" m4.go
//go build --ldflags '-extldflags "-static"' file.go
//cgo DFLAGS:-L. lnk.a -lgb -ldl

// http://stackoverflow.com/questions/16747021/how-do-you-statically-link-a-c-library-in-go-using-cgo
/*
Вариант С
http://stackoverflow.com/questions/31476014/linking-c-with-golang?lq=1
Вариант С++
http://stackoverflow.com/questions/1713214/how-to-use-c-in-go

Подключение Длл для Го
http://golangtc.com/member/xuhany

Подключение к серверу
egorka@postgo:~/mainera$ sshfs miner@176.197.54.198:  /home/egorka/mainera -o port=30024
*/
