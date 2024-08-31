#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>
#include <esp_task_wdt.h>
#include "I2SMicSampler.h"
#include "config.h"
#include "Application2.h"
#include "state_machine/DetectWakeWordState.h"


#define I2S_WS 19     // aka LRCL
#define I2S_SD 4     // aka DOUT
#define I2S_SCK 18        // aka BCLK

#define RED_PIN 14
#define GREEN_PIN 12
#define BLUE_PIN 13



// IOT ssid
const char *IOT_ssid = "Home04";
const char *IOT_password = "13243546";

// i2s config for reading from both channels of I2S
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


// i2s microphone pins
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD};


// This task does all the heavy lifting for our application
void applicationTask(void *param)
{
  DetectWakeWordState *detect_marvin = static_cast<DetectWakeWordState *>(param);

  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10000);
  // Serial.printf("In application task\n");
  while (true)
  {
    if (detect_marvin->m_sample_provider->stop_task == true)
    {
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, LOW);
      digitalWrite(RED_PIN, HIGH);
      Serial.printf("Before deleting task\n");
      detect_marvin->exitState();
      // application->run();
      vTaskDelete(NULL);
    }
    else
    {
//  wait for some audio samples to arrive
      uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
      if (ulNotificationValue > 0)
      {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(BLUE_PIN, LOW);
        digitalWrite(GREEN_PIN, HIGH);
        // Serial.printf("In application task2\n");
        detect_marvin->run();
      }
    }
    //
  }
}

void setup()
{

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);



  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting up");
  Serial.println("Connecting to WIFI ...");
  // start up wifi
  // launch WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(IOT_ssid, IOT_password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.printf("Total heap: %d\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());

  // make sure we don't get killed for our long running tasks
  esp_task_wdt_init(100, false);

  // start up the I2S input (from either an I2S microphone or Analogue microphone via the ADC)
  I2SSampler *i2s_sampler = new I2SMicSampler(i2s_mic_pins, i2sMemsConfigBothChannels, false);

  // create our application
  DetectWakeWordState *detect_marvin = new DetectWakeWordState(i2s_sampler);
  detect_marvin->enterState();


  // set up the i2s sample writer task
  TaskHandle_t applicationTaskHandle; 
  xTaskCreate(applicationTask, "Application Task", 8192, detect_marvin, 1, &applicationTaskHandle);

  // start sampling from i2s device - use I2S_NUM_0 as that's the one that supports the internal ADC
  i2s_sampler->start(I2S_NUM_0, applicationTaskHandle);
  while(true)
  {
    vTaskDelay(100);
    // Serial.printf("In while\n");
    if (i2s_sampler->stop_task == true)
    {
      Serial.printf("In if stop task\n");
      break;
    }
  }
  // delete detect_marvin;
  // Serial.printf("After deleteing application");
}

void loop()
{
  bool stop_loop = false;
  Phase2* phase2_obj = new Phase2();
  while(!stop_loop)
  {
  phase2_obj->run();
  }
  Serial.printf("In loop\n");
}
