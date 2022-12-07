// Include I2S driver
#include <driver/i2s.h>

// Connections to INMP441 I2S microphone
#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32

// Use I2S Processor 0
#define I2S_PORT I2S_NUM_0

// Define input buffer length
#define bufferLen 16
int32_t sBuffer[bufferLen];

void i2s_install() {

  const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
        .sample_rate = 16000,                              // 16KHz 44100
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,      // could only get it to work with 32bits
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,      // although the SEL config should be left, it seems to transmit on right
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1
        .dma_buf_count = 2,                       // number of buffers
        .dma_buf_len = bufferLen                 // samples per buffer
    };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

void setup() {

  // Set up Serial Monitor
  Serial.begin(115200);
  Serial.println(" ");

  delay(1000);

  // Set up I2S
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  delay(500);
}

void loop() {

  // False print statements to "lock range" on serial plotter display
  // Change rangelimit value to adjust "sensitivity"
//  int rangelimit = 3000;
//  Serial.print(rangelimit * -1);
//  Serial.print(" ");
//  Serial.print(rangelimit);
//  Serial.print(" ");
     

  // Get I2S data and place in data buffer
  
  size_t bytesIn = 0;
                
  i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);

  for (int i = 0; i < bytesIn / 4; i++) 
  {
      int32_t sample = sBuffer[i];
      sample >>= 14;
      int16_t out = sample;
   
      //CONVERTED TO SIGNED 16 BIT SAMPLE, save in same array
      ((int16_t *)sBuffer)[i] = out;
      Serial.println(out);    
  }
  bytesIn = bytesIn / 2; //reduce array size, since int16_t now
}
