package overdrive

type Config struct {

	// Секция конфигурации страницы отображения результатов
	Frontend Frontend `json:"frontend"`

	// Общее количество установленных GPU
	// равно количество записей в файле конфигурации, можно и не использовать
	NumberAdapters int `json:"gpunumber"`
	// Список имеющихся GPU и их сервисных настроек
	DeviceGPU []DeviceGPU `json:"gpudevice"`

	// Время опроса состояния адаптеров,
	// покругу переключаемся с одного устройства на другое, в каждый момент времени считываем
	// данные только с одного устройства не пытаемся работать одновременно с разными GPU адаптерами
	GpuCheckInterval string `json:"gpucheck"`

	Threads int `json:"threads"`

	// Что такое Newrelic для меня непонятно
	NewrelicName    string `json:"newrelicName"`
	NewrelicKey     string `json:"newrelicKey"`
	NewrelicVerbose bool   `json:"newrelicVerbose"`
	NewrelicEnabled bool   `json:"newrelicEnabled"`
}

// Ограничение доступа к панели конфигурирования
type Frontend struct {
	Name     string `json:"name"`
	Listen   string `json:"listen"`
	Login    string `json:"login"`
	Password string `json:"password"`
}

// Параметры конкретной платы
type DeviceGPU struct {
	Name                 string `json:"name"` // имя адаптера, почти ненужно
	AdapterId            int    `json:"aid"`  // номер адаптера в списке драйвера
	ThermalController    int    `json:"tid"`  // у GPU может быть несколько контроллеров температуры
	FanSpeed             int    `json:"fan"`
	PowerControlTreshold int    `json:"pwrcontrol"` // управление питанием ?
	EngineClock          int    `json:"engclock"`   // частота ядра
	MemoryClock          int    `json:"memclock"`   // частота памяти адаптера

	// минимальное время опроса различных переменных состояния адаптера
	// почти ненужен оставим на расширение
	Timeout string `json:"timeout"`

	// Эта структура в сериализации параметров из файла конфигурации не участвует
	Value DeviceValueGPU
}

// Поля описывающиен текущие настройки GPU адаптера
// заполняются через вызовы функций	драйвера
type DeviceValueGPU struct {
	Temperature    int
	Fan            int
	FanSpeedReport int // RPM or Percent
	FanMin         int
	FanMax         int

	// Параметр связанный с питанием
	PwrControlCurrent int
	// Границы уставок питания, в непонятных единицах, точно не в вольтах
	PwmStepValue int
	PwmMinValue  int
	PwmMaxValue  int

	// Текущие значения настроек
	iEngineClock     int
	iMemoryClock     int
	iVddc            int
	iActivityPercent int

	// Границы уставок, частот и питания
	sEngineClockMin  int
	sEngineClockMax  int
	sEngineClockStep int

	sMemoryClockMin  int
	sMemoryClockMax  int
	sMemoryClockStep int

	// Похоже в вольтах
	sVddcMin  int
	sVddcMax  int
	sVddcStep int
}

/* -------------------------------------------------------
// Отдельного веб порта  для RPC у нас сейчас не будет
// Proxy                 Proxy      `json:"proxy"`

type Proxy struct {
	Listen               string `json:"listen"`
	ClientTimeout        string `json:"clientTimeout"`
	BlockRefreshInterval string `json:"blockRefreshInterval"`
	HashrateWindow       string `json:"hashrateWindow"`
	SubmitHashrate       bool   `json:"submitHashrate"`
	LuckWindow           string `json:"luckWindow"`
	LargeLuckWindow      string `json:"largeLuckWindow"`
}

*/
