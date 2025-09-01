
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT22 // DHT 22 (AM2302)
#define RELAY_PIN 16
DHT dht(DHTPIN, DHTTYPE);

// Create an MPU6050 object
Adafruit_MPU6050 mpu;

void setup(void) {
 
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Predictive Maintenance for Motors - Sensor Test");

  Serial.println("Initializing MPU6050 sensor...");
  

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip.");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 found!");
  

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  

  Serial.println("Initializing DHT22 sensor...");
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  
  Serial.println("Sensors initialized. Starting data collection...");
}

void loop() {
 
  delay(2000); 
  float temperature_c = dht.readTemperature();
  // Read humidity
  float humidity = dht.readHumidity();

  if (isnan(humidity) || isnan(temperature_c)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Print the temperature and humidity values to the Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature_c);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %")75

    
    if (temperature_c > 45.0) {
      digitalWrite(RELAY_PIN, HIGH); // Turns the relay ON 
      Serial.println("ALERT: Temperature exceeded 45°C. Exhaust fan is ON.");
    } else {
      digitalWrite(RELAY_PIN, LOW); // Turns the relay OFF
    }
  }

  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Convert acceleration from m/s^2 to g (gravitational force)
  float accelX_g = a.acceleration.x / 9.81;
  float accelY_g = a.acceleration.y / 9.81;
  float accelZ_g = a.acceleration.z / 9.81;

  // Print acceleration data in g
  Serial.print("AccelX: ");
  Serial.print(accelX_g, 2);
  Serial.print(" g | AccelY: ");
  Serial.print(accelY_g, 2);
  Serial.print(" g | AccelZ: ");
  Serial.print(accelZ_g, 2);
  Serial.println(" g");

  // Print gyroscope data
  Serial.print("GyroX: ");
  Serial.print(g.gyro.x, 2);
  Serial.print(" rad/s | GyroY: ");
  Serial.print(g.gyro.y, 2);
  Serial.print(" rad/s | GyroZ: ");
  Serial.print(g.gyro.z, 2);
  Serial.println(" rad/s");

  // Print MPU6050 temperature (this is internal to the chip, not the motor)
  Serial.print("MPU Temp: ");
  Serial.print(temp.temperature);
  Serial.println(" °C");

  Serial.println("--------------------");

  // --- Vibration Alert Logic ---
  // Check if the magnitude of the acceleration on any axis is within the normal range.
  if (abs(accelX_g) >= 0.1 && abs(accelX_g) <= 1.25 && 
      abs(accelY_g) >= 0.1 && abs(accelY_g) <= 1.25 && 
      abs(accelZ_g) >= 0.1 && abs(accelZ_g) <= 1.25) {
    Serial.println("Motor vibration is normal.");
  } 
  
    else if (abs(accelX_g) >= 1.25 && abs(accelX_g) <= 1.75 ||
      abs(accelY_g) >= 1.25 && abs(accelY_g) <= 1.75 || 
      abs(accelZ_g) >= 1.25 && abs(accelZ_g) <= 1.75) {
    Serial.println("Motor vibration is high need a quick maintenance.");
  } 
  else if (abs(accelX_g) > 1.75 || abs(accelY_g) > 1.75 || abs(accelZ_g) > 1.75 {
    // If any axis exceeds 1.5g, it is too high.
    Serial.println("WARNING: Motor vibration is too high!");
  }
  
  // Delay for the next reading. You can adjust this as needed for your model.
  delay(2000);
}
