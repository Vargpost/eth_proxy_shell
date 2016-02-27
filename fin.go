package main

import (
	"encoding/json"
	"fmt"

	"log"
	"net/http"
	"os"
	"path/filepath"

	"./overdrive"

	"github.com/goji/httpauth"
	"github.com/gorilla/mux"
	"github.com/yvasiyarov/gorelic"
)

var cfg overdrive.Config

func SimpleTest(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "DFG %s!", r.URL.Path[1:])
}

// Что это такое ? ;)
func startNewrelic() {
	if cfg.NewrelicEnabled {
		// Непонятная штука cоздаётся
		nr := gorelic.NewAgent()
		nr.Verbose = cfg.NewrelicVerbose
		nr.NewrelicLicense = cfg.NewrelicKey
		nr.NewrelicName = cfg.NewrelicName
		// и запускается, нигде далее не управляется...
		nr.Run()
	}
}

func readConfig(cfg *overdrive.Config) {
	configFileName := "config.json"
	if len(os.Args) > 1 {
		configFileName = os.Args[1]
	}
	configFileName, _ = filepath.Abs(configFileName)
	log.Printf("Loading config: %v", configFileName)

	configFile, err := os.Open(configFileName)
	if err != nil {
		log.Fatal("File error: ", err.Error())
	}
	defer configFile.Close()
	jsonParser := json.NewDecoder(configFile)
	if err = jsonParser.Decode(&cfg); err != nil {
		log.Fatal("Config error: ", err.Error())
	}
}

func main() {
	readConfig(&cfg)
	startNewrelic()

	// Паралельный процесс последовательно опрашивающий адаптеры
	// Результат работы в экземпляре структуры s тип GpuServer
	s := overdrive.PollingGPU(&cfg)
	if nil == s {
		return // Ничего не  получилось завершаем работу сервера
	}

	r := mux.NewRouter()
	r.HandleFunc("/o", s.StartPage)      // начальная страница, работаем пока так
	r.HandleFunc("/lst", s.StatsList)    // Загрузка основной страницы со скриптами
	r.HandleFunc("/stats", s.StatsIndex) // Json для работы java script-a
	r.PathPrefix("/").Handler(http.FileServer(http.Dir("./tmpl/")))
	// В адресной строке браузера вписывать localhost:8080/lst

	// Вариант с ограничением доступа по паролю
	var err error
	if len(cfg.Frontend.Password) > 0 {
		auth := httpauth.SimpleBasicAuth(cfg.Frontend.Login, cfg.Frontend.Password)
		err = http.ListenAndServe(cfg.Frontend.Listen, auth(r))
	} else {
		// Без пароля
		err = http.ListenAndServe(cfg.Frontend.Listen, r)
	}
	if err != nil {
		log.Fatal(err)
	}

	// По простому http.ListenAndServe(":8084", nil)
}
