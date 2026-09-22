# Final Project: IoT Weather Station & UART Bridge -- 2025.06.03

> 基於 **ESP32 (MicroPython)** 與 **8051 單晶片 (C 語言)** 的物聯網氣象站專案，透過 MQTT 協定與 OpenWeatherMap API 實現即時天氣查詢，並透過序列埠（UART）進行雙向通訊與數值顯示。

---

## 💡 專案簡介 (About)

本專案旨在結合物聯網（IoT）與嵌入式系統，實現一個完整的遠端天氣監控與硬體顯示系統：

1. **雲端控制**：透過 MQTT 訂閱 Adafruit IO 的主題來接收目標城市名稱。
2. **天氣數據獲取**：使用 MicroPython 呼叫 OpenWeatherMap API 抓取指定城市的即時溫度。
3. **雲端回報**：將抓取到的溫度資料回傳至 Adafruit IO 儀表板。
4. **硬體通訊與顯示**：透過 UART 將溫度數值傳送給 8051 單晶片，驅動硬體動態顯示溫度。

---

## ✨ 主要功能 (Key Features)

* 🌐 **Wi-Fi 與 MQTT 聯網**：ESP32 自動連線無線網路，並即時監聽雲端指令。
* 🌡️ **OpenWeatherMap 整合**：支援多國語言（繁體中文）與攝氏溫度自動轉換。
* 🔄 **UART 串列通訊**：ESP32 與 8051 單晶片之間進行資料序列傳輸。
* 🔢 **動態數值解析**：8051 端透過中斷程式精準接收字串，並在 7 段顯示器上動態呈現溫度。

---

## 🛠️ 技術堆疊 (Tech Stack)

* **ESP32 端**：
* 程式語言：`MicroPython` (`fin.py`, `xtools.py`)
* 通訊協定：`MQTT` (`umqtt.simple`), `UART`, `HTTP/REST API` (`urequests`)


* **8051 端**：
* 程式語言：`C 語言` (`串聯通訊中斷_esp32_fin.c`)
* 硬體介面：UART 中斷接收、7 段顯示器掃描顯示



---

## 📂 專案結構 (Directory Structure)

```text
FinProj/
│
├── fin.py                     # ESP32 主程式（負責 Wi-Fi、MQTT、API 抓取與 UART 傳輸）
├── config.py                  # 設定檔（存放 Wi-Fi SSID 與密碼）
├── 串聯通訊中斷_esp32_fin.c     # 8051 單晶片端程式（負責 UART 中斷接收與 7段顯示器控制）
├── xtools.py                  # 工具函式庫（LED、聯網、Webhook 支援）
├── xrequests.py               # 網路請求輔助模組
└── urlencode.py               # URL 編碼工具

```

---

## ⚙️ 快速開始 (Getting Started)

### 1. 環境設定

* 請確保你的 ESP32 開發板已刷入支援 **MicroPython** 的韌體。
* 準備 Keil C 或相容的 8051 編譯環境以燒錄 C 語言程式。

### 2. 設定參數

在 `config.py` 中填入你的 Wi-Fi 資訊：

```python
SSID = "你的_WiFi_名稱"
PASSWORD = "你的_WiFi_密碼"

```

同時，在 `fin.py` 中填入你的 `ADAFRUIT_IO_USERNAME` 與 `ADAFRUIT_IO_KEY`。

### 3. 執行專案

* 將 `fin.py`、`config.py` 與相關輔助模組上傳至 ESP32。
* 將 8051 與 ESP32 的 UART 接腳正確連接（TX / RX）。
* 重新啟動裝置即可開始接收雲端指令並顯示天氣溫度！
