#include <Arduino.h>
#include <driver/i2s.h>
#include "motor_sound_model_inferencing.h" // our  Edge Impulse model header file

// ----- I2S Microphone Configuration -----
// ekhaner sob settings must match the configuration of your Edge Impulse project.
#define I2S_SAMPLE_RATE         16000        
#define I2S_READ_LEN            1024         
#define I2S_MIC_CHANNEL         I2S_CHANNEL_FMT_ONLY_LEFT 
#define I2S_MIC_SERIAL_CLOCK    14           
#define I2S_MIC_LEFT_RIGHT_CLOCK 15          
#define I2S_MIC_SERIAL_DATA     32             // The I2S DATA pin.

//  normal buffer jate raw adio signal take hold kore
int16_t i2sData[I2S_READ_LEN];

unsigned long lastInferenceTime = 0;
const int INFERENCE_INTERVAL_MS = 1000; // Run inference every 1 second.
// Add threshold to filter out low-confidence results.
const float CONFIDENCE_THRESHOLD = 0.75; // 75% confidence required for a definitive result.

void i2s_install() {
    // I2S configuration structure.
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // receiving data.
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_MIC_CHANNEL,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = I2S_READ_LEN,
        .use_apll = false
    };
    
    // I2S pin configuration structure.
    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_MIC_SERIAL_CLOCK,
        .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
        .data_out_num = -1, // Not used for a microphone (input).
        .data_in_num = I2S_MIC_SERIAL_DATA
    };

    // Install the I2S driver and configure the pins and clock.
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, I2S_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}


void setup() {
    Serial.begin(115200);
    delay(1000); 
    Serial.println("Motor Condition Monitoring - ESP32");

    i2s_install();
}

void loop() {
    // Check if enough time has passed since the last inference.
    if (millis() - lastInferenceTime >= INFERENCE_INTERVAL_MS) {
        lastInferenceTime = millis();

        size_t bytesRead = 0;
        // Read audio data from the microphone into the buffer.
        i2s_read(I2S_NUM_0, (void*)i2sData, I2S_READ_LEN * sizeof(int16_t), &bytesRead, portMAX_DELAY);

        // Prepare the raw audio data for the ML model.
        signal_t signal;
        numpy::signal_from_buffer(i2sData, bytesRead / sizeof(int16_t), &signal);

        // Run the classification inference.
        ei_impulse_result_t result;
        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
        if (res != EI_IMPULSE_OK) {
            Serial.println("ERROR: Failed to run inference");
            return;
        }

        // Find the most confident classification result.
        String detected_label = "";
        float max_value = 0.0;
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            if (result.classification[ix].value > max_value) {
                max_value = result.classification[ix].value;
                detected_label = String(result.classification[ix].label);
            }
        }

        // Display the results with a clear status message.
        Serial.print("Detected: ");
        Serial.print(detected_label);
        Serial.print(" (");
        Serial.print(max_value * 100, 1);
        Serial.println("%)");
//check confidence
        if (max_value > CONFIDENCE_THRESHOLD) {
            if (detected_label == "Normal") {
                Serial.println("Status: Motor running normally.");
            }
            else if (detected_label == "Lube" || detected_label == "Lubrication") {
                Serial.println("Status: Maintenance required (Lubrication needed).");
            }
            else if (detected_label == "Faulty") {
                Serial.println("Status: Motor Fault Detected! Immediate attention required.");
            }
        } else {
            Serial.println("Status: Inconclusive result, please wait.");
        }
        Serial.println("----------------------------------");
    }
  
}
