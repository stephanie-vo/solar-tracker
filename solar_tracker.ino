#include <Servo.h>
#include <Wire.h>
#include <math.h>

// Servo setup
Servo servo1;  // Servo for tilt (up/down)
Servo servo2;  // Servo for rotation (left/right)

int tiltAngle = 90;    // Neutral tilt position
int rotateAngle = 90;  // Neutral rotation position

// Photoresistor setup
#define TOP_SENSOR A2
#define BOTTOM_SENSOR A6
#define LEFT_SENSOR A1
#define RIGHT_SENSOR A3

// Gyroscope setup
const int MPU = 0x68;
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;
float tiltAngleGyro;

void setup() {
  // Initialize servos
  servo1.attach(8);           // Attach tilt servo to D8
  servo2.attach(9);           // Attach rotation servo to D9
  servo1.write(tiltAngle);    // Set initial tilt position
  servo2.write(rotateAngle);  // Set initial rotation position

  // Initialize serial communication
  Serial.begin(9600);

  // Initialize gyroscope
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

// Function to control tilting (up/down)
void tiltServoControl() {
  int topValue = analogRead(TOP_SENSOR);
  int bottomValue = analogRead(BOTTOM_SENSOR);

  Serial.print("Top: ");
  Serial.print(topValue);
  Serial.print(" | Bottom: ");
  Serial.println(bottomValue);

  if (topValue > bottomValue + 10) {
    tiltAngle += 2;  // Tilt DOWN
  } else if (bottomValue > topValue + 10) {
    tiltAngle -= 2;  // Tilt UP
  }

  tiltAngle = constrain(tiltAngle, 10, 170);
  servo1.write(tiltAngle);
}

// Function to control rotation (left/right)
void rotateServoControl() {
  int leftValue = analogRead(LEFT_SENSOR);
  int rightValue = analogRead(RIGHT_SENSOR);

  Serial.print("Left: ");
  Serial.print(leftValue);
  Serial.print(" | Right: ");
  Serial.println(rightValue);

  if (leftValue > rightValue + 10) {
    rotateAngle += 2;  // Rotate RIGHT
  } else if (rightValue > leftValue + 10) {
    rotateAngle -= 2;  // Rotate LEFT
  }

  rotateAngle = constrain(rotateAngle, 10, 170);
  servo2.write(rotateAngle);
}

// Function to read gyroscope and compute tilt angle
void readGyroscope() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // Compute tilt angle from accelerometer data
  tiltAngleGyro = atan2(AcY, AcZ) * 180.0 / M_PI;

  Serial.print("Tilt Angle (Pitch): ");
  Serial.println(tiltAngleGyro);
}

void loop() {
  tiltServoControl();    // Adjust tilt based on top/bottom light
  rotateServoControl();  // Adjust rotation based on left/right light
  readGyroscope();       // Read gyroscope and compute tilt angle
  delay(10);             // Smooth movement
}
