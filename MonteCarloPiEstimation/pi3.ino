// PI Estimation with Montecarlo
// Arduino Nano R4, TM1637 display, buzzer and push button
// Roni Bandini, 08.25
// MIT License

#include <Arduino.h>
#include <TM1637TinyDisplay.h>

#define CLK 4
#define DIO 5
#define BUTTON 6
#define BUZZER A7 

TM1637TinyDisplay display(CLK, DIO);

// Estimation state
unsigned long inside_circle = 0;
unsigned long total_points = 0;

unsigned long last_update = 0;
double pi_estimate = 0.0;

bool last_button_state = HIGH;   

void red() {
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
}

void green() {
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, HIGH);
}

void blue() {
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);
}

void rgbOff() {
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
}

void beepBuzzer(int duration = 50) {
  tone(BUZZER, 1000, duration);
}

void resetSimulation() {
  inside_circle = 0;
  total_points = 0;
  pi_estimate = 0.0;
  display.showString("RESET");
  beepBuzzer(200);  
  delay(300);
}

void piToStringWithSpaceDot(double pi_estimate, char *buffer, int buffer_size) {
  dtostrf(pi_estimate, 1, 6, buffer);  // 6 dec
  for (int i = 0; i < buffer_size && buffer[i] != '\0'; i++) {
    if (buffer[i] == '.') {
      buffer[i] = '-';
      break;
    }
  }
}

void setup() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(BUZZER, OUTPUT); 
  pinMode(BUTTON, INPUT_PULLUP);  

  digitalWrite(BUZZER, LOW);
  blue();

  Serial.begin(115200);
  delay(2000);
  Serial.println("π Monte Carlo Estimation with Arduino Nano R4 - Roni Bandini 08.25");

  display.setBrightness(4); 
  display.showString("NANO R4");
  delay(500);
  display.showString("PI ESTIMATION");
  delay(500);
  
  randomSeed(analogRead(A0));
  
  display.showString("----");

  while ( digitalRead(BUTTON)==HIGH) {
    Serial.println("Waiting button...");
    delay(50);
  }

  Serial.println("Starting");
}

void loop() {

  bool current_button_state = digitalRead(BUTTON);
  if (current_button_state == LOW) {
    Serial.print("Reset");
    resetSimulation();
  }


  // Simulation
  for (int i = 0; i < 100; i++) {
    double x = random(0, 10001) / 10000.0;
    double y = random(0, 10001) / 10000.0;
    if ((x * x + y * y) <= 1.0) {
      inside_circle++;
    }
    total_points++;
  }

  beepBuzzer();   
  red(); 
  delay(100);

  if (millis() - last_update > 900) {
    last_update = millis();
    pi_estimate = 4.0 * (double)inside_circle / (double)total_points;

    Serial.print("π ≈ ");
    Serial.print(pi_estimate, 8);
    Serial.print(" (");
    Serial.print(inside_circle);
    Serial.print(" / ");
    Serial.print(total_points);
    Serial.println(")");
    
    char pi_str[16]; 
    piToStringWithSpaceDot(pi_estimate, pi_str, sizeof(pi_str));
    green();
    display.showString(pi_str); 
  }
}
