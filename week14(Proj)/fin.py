import urequests, ujson
import xtools, utime
from machine import Pin, UART
import config
from umqtt.simple import MQTTClient

xtools.connect_wifi_led()

ADAFRUIT_IO_USERNAME = ""
ADAFRUIT_IO_KEY = ""
FEED = "target_city"
FEED_TEMP = "target-city-temp"

"""
FEEDS_CITIES = [
    "taipei", "kaohsiung", "new taipei", "taichung", "tainan", "taoyuan",
    "keelung", "hsinchu", "chiayi", "changhua", "pingtung", "yilan",
    "hualien", "taitung", "nantou", "miaoli", "yunlin"
]
"""
#ledG = Pin(12, Pin.OUT)
#ledG.value(0)

com = UART(2, 9600, tx=17, rx=16)
com.init(9600)

#led = Pin(2, Pin.OUT, value=1)  # 設定LED接腳
print('MicroPython Ready...')  # 輸出訊息到終端機

COUNTRY_CODE = "TW"
txt = None

# MQTT 客戶端
client = MQTTClient (
    client_id = xtools.get_id(),
    #server = "io.adafruit.com",
    user = ADAFRUIT_IO_USERNAME,
    password = ADAFRUIT_IO_KEY,
    ssl = False,
)

def sub_cb(topic, msg):
    #global ledG
    global txt
    txt = msg.decode()
    print("收到訊息: ", txt)
    
    """
    if msg.decode() == "ON":
        ledG.value(1)
    if msg.decode() == "OFF":
        ledG.value(0)
    """
client.set_callback(sub_cb)   # 指定回撥函數來接收訊息
client.connect()              # 連線

topic = ADAFRUIT_IO_USERNAME + "/feeds/" +FEED
print(topic)
client.subscribe(topic)      # 訂閱主題

def get_temperature_for_city(city_name, country_code):
    url  = "https://api.openweathermap.org/data/2.5/weather?"
    url += "q=" + city_name + "," + country_code 
    url += "&units=metric&lang=zh_tw&" # 單位：攝氏度，語言：繁體中文
    url += "appid=" + "c98e9610fb02b1bd97baeb74c02dd8e1" # API_KEY
    
    try:
        response = urequests.get(url)
        if response.status_code == 200:
            data = ujson.loads(response.text)
            main_data = data.get("main", {})
            temp = main_data.get("temp")
            response.close() # 及時釋放資源
            if temp is not None:
                print(f"城市 {city_name} 的溫度: {temp}°C")
                return temp
            else:
                #print(f"ERROR：找不到到城市 {city_name} 的 'temp' 數據。")
                return None 
        else:
            #print(f"獲取城市 {city_name} 的 OWM 數據失敗。狀態碼: {response.status_code}")
            response.close()
            return None
    except Exception as e:
        return None

while True:
    client.check_msg()
    utime.sleep(2)
    while txt != None:
        txt = txt.lower()
        #print("送出訊息:", txt)
        #client.publish(topic, txt)
        #utime.sleep(2)
        
        current_temp = get_temperature_for_city(txt.replace(" ", "%20"), COUNTRY_CODE) #在 URL encoding 中，空格以"%20"或是"+"表示 
        if current_temp is not None:       
            adafruit_feed_key = FEED_TEMP # 須為 global
            adafruit_url = "https://io.adafruit.com/api/v2/" + ADAFRUIT_IO_USERNAME
            adafruit_url += "/feeds/"+ adafruit_feed_key + "/data?X-AIO-Key=" + ADAFRUIT_IO_KEY     # 在group city下的FEED路徑要改
                     
            data_to_send = {"value": current_temp}
            print(f" {adafruit_feed_key} : {current_temp}")
            xtools.webhook_post(adafruit_url, data_to_send)
            com.write(str(current_temp).encode())# 不確定需不需要 encode()
            
        else:
            print(f"未能獲取城市 {txt} 的數據，本次跳過發送。")

        utime.sleep(15)# 30 改為 15

        #led.value(0)
        utime.sleep(1)
        if com.any() > 0:
            choice = com.readline()
            print(choice)
            
        txt = None
