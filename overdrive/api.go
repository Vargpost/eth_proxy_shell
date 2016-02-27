package overdrive

import (
	"encoding/json"
	"fmt"
	"html/template"
	"net/http"
	// "sync/atomic"
	//	"time"

	//"../rpc"
	//"../util"
)

/*	Понадобится
- входная страница со списком видеокарт
  нужна таблица динамически подгружаемая, требуется сделать скрипт !

- страница подробного описания  состояния каждой карты + кнопка перейти к редактированию настроек
- страница редактирования настроек
*/

type Result struct {
	FirstName string `json:"first"`
	LastName  string `json:"last"`
}

func (s *GpuServer) StatsIndex(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json; charset=UTF-8")
	w.WriteHeader(http.StatusOK)

	stats := map[string]interface{}{
		"name":   s.config.Frontend.Name,
		"numdev": s.NumberAdapters,
		"ovrver": s.OverdriveVersion,
	}

	var groupGpu []interface{}
	for i, u := range s.groupGpu {
		cGpu := convertGpu(u)
		groupGpu = append(groupGpu, cGpu)
		i++
	}
	stats["groupGpu"] = groupGpu

	// Отправляет строку клиенту
	// Больше в ответе сервера нет ничего, никаких скриптов как обрабатывать эту строку
	// Скрипт для отображения должен быть загружен ранее
	json.NewEncoder(w).Encode(stats)
}

func convertGpu(u *DeviceGPU) map[string]interface{} {
	dGpu := map[string]interface{}{
		"name": u.Name,
		"aid":  u.AdapterId,
		"tmpr": u.Value.Temperature,
		"fan":  u.Value.Fan,

		"engclk": u.Value.iEngineClock,
		"memclk": u.Value.iMemoryClock,

		"vdd":     u.Value.iVddc,
		"actperc": u.Value.iActivityPercent,
	}
	return dGpu
}

// Загружаем основной шаблон(страницу) для динамичного вывода параметров адаптера через JavaScript
// На странице хотелось бы отобразить список доступных адаптеров
// с "градусниками" параметров температуры, частоты, питания и тд см.Handlebars
func (s *GpuServer) StatsList(w http.ResponseWriter, r *http.Request) {
	t, _ := template.ParseFiles("tmpl/index.html")
	t.Execute(w, "deadbeaf")
	fmt.Fprintf(w, "")
}

// Вариант вывода параметров просто страницей,
// для перерисовки потребуется перезагружать страницу
func (s *GpuServer) StartPage(w http.ResponseWriter, r *http.Request) {
	//	var pi Result2
	//	pi = Result2{"sd", "card"}

	t, _ := template.ParseFiles("tmpl/decode.html")
	t.Execute(w, &s)
	fmt.Fprintf(w, "")
}

/* Пример из Ether-proxy
func convertUpstream(u *rpc.RPCClient) map[string]interface{} {
	upstream := map[string]interface{}{
		"name":             u.Name,
		"url":              u.Url.String(),
		"pool":             u.Pool,
		"sick":             u.Sick(),
		"accepts":          atomic.LoadUint64(&u.Accepts),
		"rejects":          atomic.LoadUint64(&u.Rejects),
		"lastSubmissionAt": atomic.LoadInt64(&u.LastSubmissionAt),
		"failsCount":       atomic.LoadUint64(&u.FailsCount),
	}
	return upstream
}

func (s *ProxyServer) collectMinersStats() (int64, int64, int, []interface{}) {
	now := util.MakeTimestamp()
	var result []interface{}
	totalHashrate := int64(0)
	totalHashrate24h := int64(0)
	totalOnline := 0
	//	window24h := 24 * time.Hour

	miner1 := NewMiner("op", "128.0.1.2")
	miner2 := NewMiner("op2", "128.0.1.3")

	s.miners.Set("op2", miner1)
	s.miners.Set("op3", miner2)

	for m := range s.miners.Iter() {
		stats := make(map[string]interface{})
		lastBeat := now //m.Val.getLastBeat()
		/*				hashrate := m.Val.hashrate(s.hashrateWindow)
						hashrate24h := m.Val.hashrate(window24h)
						totalHashrate += hashrate
						totalHashrate24h += hashrate24h
						stats["name"] = m.Key
						stats["hashrate"] = hashrate
						stats["hashrate24h"] = hashrate24h
						stats["lastBeat"] = lastBeat
						stats["validShares"] = atomic.LoadUint64(&m.Val.validShares)
						stats["invalidShares"] = atomic.LoadUint64(&m.Val.invalidShares)
						stats["accepts"] = atomic.LoadUint64(&m.Val.accepts)
						stats["rejects"] = atomic.LoadUint64(&m.Val.rejects)
		*
		stats["ip"] = m.Val.IP

		if now-lastBeat > (int64(s.timeout/2) / 1000000) {
			stats["warning"] = true
		}
		if now-lastBeat > (int64(s.timeout) / 1000000) {
			stats["timeout"] = true
		} else {
			totalOnline++
		}
		result = append(result, stats)
	}
	return totalHashrate, totalHashrate24h, totalOnline, result
}

func (s *ProxyServer) getLuckStats() map[string]interface{} {
	now := util.MakeTimestamp()
	var variance float64
	var totalVariance float64
	var blocksCount int
	var totalBlocksCount int

	s.blocksMu.Lock()
	defer s.blocksMu.Unlock()

	for k, v := range s.blockStats {
		if k >= now-int64(s.luckWindow) {
			blocksCount++
			variance += v
		}
		if k >= now-int64(s.luckLargeWindow) {
			totalBlocksCount++
			totalVariance += v
		} else {
			delete(s.blockStats, k)
		}
	}
	if blocksCount != 0 {
		variance = variance / float64(blocksCount)
	}
	if totalBlocksCount != 0 {
		totalVariance = totalVariance / float64(totalBlocksCount)
	}
	result := make(map[string]interface{})
	result["variance"] = variance
	result["blocksCount"] = blocksCount
	result["window"] = s.config.Proxy.LuckWindow
	result["totalVariance"] = totalVariance
	result["totalBlocksCount"] = totalBlocksCount
	result["largeWindow"] = s.config.Proxy.LargeLuckWindow
	return result
}
*/
