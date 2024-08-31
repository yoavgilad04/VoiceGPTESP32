# 1 "C:\\Users\\yoavgila\\AppData\\Local\\Temp\\tmpht8b5w4j"
#include <Arduino.h>
# 1 "C:/Users/yoavgila/OneDrive - Intel Corporation/Desktop/VoiceAssitantProject/iot/src/main.ino"
#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>
#include <esp_task_wdt.h>
#include "I2SMicSampler.h"
#include "ADCSampler.h"
#include "I2SOutput.h"
#include "config.h"
#include "Application.h"
#include "SPIFFS.h"




#define I2S_WS 25
#define I2S_SD 32
#define I2S_SCK 33



i2s_config_t i2sMemsConfigBothChannels = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_MIC_CHANNEL,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
    };


i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD};
# 50 "C:/Users/yoavgila/OneDrive - Intel Corporation/Desktop/VoiceAssitantProject/iot/src/main.ino"
void applicationTask(void *param);
void setup();
void loop();
#line 50 "C:/Users/yoavgila/OneDrive - Intel Corporation/Desktop/VoiceAssitantProject/iot/src/main.ino"
void applicationTask(void *param)
{
  Application *application = static_cast<Application *>(param);

  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000);
  while (true)
  {

    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    if (ulNotificationValue > 0)
    {
      application->run();
    }
  }
}

void setup()
{
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting up");
  Serial.println("Connecting to WIFI ...");




  WiFi.mode(WIFI_STA);
  WiFi.begin("Home04", "13243546");
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.printf("Total heap: %d\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());







  I2SSampler *i2s_sampler = new I2SMicSampler(i2s_mic_pins, false);
# 113 "C:/Users/yoavgila/OneDrive - Intel Corporation/Desktop/VoiceAssitantProject/iot/src/main.ino"
  Application *application = new Application(i2s_sampler);


  TaskHandle_t applicationTaskHandle;
  xTaskCreate(applicationTask, "Application Task", 8192, application, 1, &applicationTaskHandle);


  i2s_sampler->start(I2S_NUM_0, i2sMemsConfigBothChannels, applicationTaskHandle);
}

void loop()
{
  vTaskDelay(1000);
}