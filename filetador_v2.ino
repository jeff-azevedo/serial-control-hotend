#include <Stepper.h>

// Configuração do motor de passo
const int stepsPerRevolution = 50;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

// Variáveis para controle da temperatura e velocidade do motor
float targetTemperature = 230.0;
int motorSpeed = 16;
bool alreadyHot = false;

// Defina os pinos do termistor e do MOSFET
int thermistorPin = A0;
int mosfetPin = 3;

// Constantes para o termistor e o resistor
const float BETA = 3950;
const float R_REF = 3280;

void setup() {
  Serial.begin(9600);
  pinMode(mosfetPin, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("T")) {
      targetTemperature = input.substring(1).toFloat();
      Serial.println("Temperatura alterada:" +  input.substring(1));
    } else if (input.startsWith("S")) {
      motorSpeed = input.substring(1).toInt();      
      Serial.println("Velocidade alterada:" +  input.substring(1));
    }
  }

  if(alreadyHot)
  {
    myStepper.step(stepsPerRevolution);
    myStepper.setSpeed(motorSpeed);
  }
  
  Serial.println(alreadyHot);

  int analogValue = analogRead(thermistorPin);
  float resistance = R_REF / (1023.0 / analogValue - 1);
  float temperatureK = 1 / (1 / (25 + 273.15) + log(resistance / 100000) / BETA);
  float temperatureC = temperatureK - 273.15;

  Serial.print("Temperatura: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  if (temperatureC >= targetTemperature) {
    digitalWrite(mosfetPin, LOW);
    alreadyHot = true;
  } else {
    digitalWrite(mosfetPin, HIGH);

    if((targetTemperature - 60) > temperatureC )
    {
      alreadyHot = false;
    }

    if(!alreadyHot){
      delay(300);
    }
  }
}
