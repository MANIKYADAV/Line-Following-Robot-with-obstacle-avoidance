#define IR_SENSOR_RIGHT      11
#define IR_SENSOR_LEFT       12
#define IR_SENSOR_FAR_RIGHT  13
#define IR_SENSOR_FAR_LEFT   4

#define MOTOR_SPEED       220
#define SOFT_TURN_SPEED   60
#define SHARP_TURN_SPEED  150
#define SEARCH_SPEED      200

// Ultrasonic pins
#define TRIG_PIN  2
#define ECHO_PIN  3

// Right motor
int enableRightMotor = 6;
int rightMotorPin1   = 7;
int rightMotorPin2   = 8;

// Left motor
int enableLeftMotor = 5;
int leftMotorPin1   = 9;
int leftMotorPin2   = 10;

// Search state tracking
bool searching = false;
int searchPhase = 0;
unsigned long searchTimer = 0;

#define BACKWARD_DURATION 1200
#define ROTATE_DURATION    700

void setup()
{
  // Increase PWM frequency
  TCCR0B = TCCR0B & B11111000 | B00000010;

  // Motor pins
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // IR sensors
  pinMode(IR_SENSOR_RIGHT, INPUT);
  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(IR_SENSOR_FAR_RIGHT, INPUT);
  pinMode(IR_SENSOR_FAR_LEFT, INPUT);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  stopMotors();
}

void loop()
{
  long distance = getDistance();

  // ================= OBSTACLE DETECTION =================
  if (distance > 0 && distance < 15)
  {
    stopMotors();
    return;
  }

  // ================= SENSOR READINGS =================
  int rightIRSensorValue    = digitalRead(IR_SENSOR_RIGHT);
  int leftIRSensorValue     = digitalRead(IR_SENSOR_LEFT);
  int farRightIRSensorValue = digitalRead(IR_SENSOR_FAR_RIGHT);
  int farLeftIRSensorValue  = digitalRead(IR_SENSOR_FAR_LEFT);

  bool anyBlackDetected =
      (rightIRSensorValue == HIGH ||
       leftIRSensorValue == HIGH ||
       farRightIRSensorValue == HIGH ||
       farLeftIRSensorValue == HIGH);

  bool allWhiteDetected =
      (rightIRSensorValue == LOW &&
       leftIRSensorValue == LOW &&
       farRightIRSensorValue == LOW &&
       farLeftIRSensorValue == LOW);

  // ================= LINE LOST =================
  if (allWhiteDetected)
  {
    // Move backward until line found again
    rotateMotor(-SEARCH_SPEED, -SEARCH_SPEED);
    return;
  }

  // ================= NORMAL LINE FOLLOW =================
  if (anyBlackDetected)
  {
    searching = false;
    searchPhase = 0;

    followLine(rightIRSensorValue,
               leftIRSensorValue,
               farRightIRSensorValue,
               farLeftIRSensorValue);
  }
  else
  {
    searchForLine();
  }
}

// =======================================================
// LINE FOLLOWING
// =======================================================
void followLine(int rightVal,
                int leftVal,
                int farRightVal,
                int farLeftVal)
{
  // ================= STRAIGHT =================
  if (rightVal == HIGH && leftVal == HIGH)
  {
    rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
  }

  // ================= TURN RIGHT =================
  else if (leftVal == HIGH && rightVal == LOW)
  {
    rotateMotor(MOTOR_SPEED, SOFT_TURN_SPEED);
  }

  // ================= TURN LEFT =================
  else if (rightVal == HIGH && leftVal == LOW)
  {
    rotateMotor(SOFT_TURN_SPEED, MOTOR_SPEED);
  }

  // ================= SHARP RIGHT =================
  else if (farLeftVal == HIGH)
  {
    rotateMotor(MOTOR_SPEED, -SHARP_TURN_SPEED);
  }

  // ================= SHARP LEFT =================
  else if (farRightVal == HIGH)
  {
    rotateMotor(-SHARP_TURN_SPEED, MOTOR_SPEED);
  }

  // ================= DEFAULT =================
  else
  {
    rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
  }
}

// =======================================================
// SEARCH MODE
// =======================================================
void searchForLine()
{
  // Start search only once
  if (searching)
  {
    searching = true;
    searchPhase = 1;
    searchTimer = millis();
  }

  switch (searchPhase)
  {
    // Move backward
    case 1:
      rotateMotor(-SEARCH_SPEED, -SEARCH_SPEED);

      if (millis() - searchTimer >= BACKWARD_DURATION)
      {
        searchPhase = 2;
        searchTimer = millis();
      }
      break;

    // Rotate right
    case 2:
      rotateMotor(-SEARCH_SPEED, SEARCH_SPEED);

      if (millis() - searchTimer >= ROTATE_DURATION)
      {
        searchPhase = 3;
        searchTimer = millis();
      }
      break;

    // Rotate left
    case 3:
      rotateMotor(SEARCH_SPEED, -SEARCH_SPEED);

      if (millis() - searchTimer >= ROTATE_DURATION * 2)
      {
        searchPhase = 4;
        searchTimer = millis();
      }
      break;

    // Move backward again
    case 4:
      rotateMotor(-SEARCH_SPEED, -SEARCH_SPEED);

      if (millis() - searchTimer >= BACKWARD_DURATION)
      {
        searchPhase = 2;
        searchTimer = millis();
      }
      break;

    default:
      stopMotors();
      break;
  }
}

// =======================================================
// ULTRASONIC SENSOR
// =======================================================
long getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  long distance = duration * 0.034 / 2;

  return distance;
}

// =======================================================
// STOP MOTORS
// =======================================================
void stopMotors()
{
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);

  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);

  analogWrite(enableRightMotor, 0);
  analogWrite(enableLeftMotor, 0);
}

// =======================================================
// MOTOR CONTROL
// =======================================================
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  // ================= RIGHT MOTOR =================
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  }
  else
  {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  }

  // ================= LEFT MOTOR =================
  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  }
  else
  {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  }

  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));
}
```
