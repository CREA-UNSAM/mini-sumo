// Definición de pines
const int pulsador = 5;
const int ledControl = 13;
const int motor1_A = 10;
const int motor1_B = 11;
const int enableMotor1 = 6;
const int motor2_A = 7;
const int motor2_B = 8;
const int enableMotor2 = 9;
const int sensorLinea = A5;

// Variable para el estado del pulsador
bool pulsadorPresionado = false;

void setup() {
  // Configuración de pines
  pinMode(pulsador, INPUT);
  pinMode(ledControl, OUTPUT);
  pinMode(motor1_A, OUTPUT);
  pinMode(motor1_B, OUTPUT);
  pinMode(enableMotor1, OUTPUT);
  pinMode(motor2_A, OUTPUT);
  pinMode(motor2_B, OUTPUT);
  pinMode(enableMotor2, OUTPUT);
  pinMode(sensorLinea, INPUT);

  // Apagar el LED de control inicialmente
  digitalWrite(ledControl, LOW);

  // Deshabilitar los motores inicialmente
  analogWrite(enableMotor1, 0);
  analogWrite(enableMotor2, 0);
}

void loop() {
  // Verificar si el pulsador está presionado para iniciar el programa
  if (digitalRead(pulsador) == HIGH && !pulsadorPresionado) {
    // Indicar que el pulsador fue presionado
    pulsadorPresionado = true;

    // Parpadeo del LED de control durante 5 segundos
    for (int i = 0; i < 5; i++) {
      digitalWrite(ledControl, HIGH); // Encender LED
      delay(500);                     // Esperar 0.5 segundos
      digitalWrite(ledControl, LOW);  // Apagar LED
      delay(500);                     // Esperar 0.5 segundos
    }

    // Encender el LED de control permanentemente después de 5 segundos
    digitalWrite(ledControl, HIGH);

    // Activar los motores para que avancen
    analogWrite(enableMotor1, 125); // Velocidad máxima del motor 1
    analogWrite(enableMotor2, 125); // Velocidad máxima del motor 2
  }

  // Si el pulsador no se ha presionado, el LED de control permanece apagado
  if (!pulsadorPresionado) {
    digitalWrite(ledControl, LOW);
    return; // Salir de la función loop para no activar los motores
  }

  // Verificar el estado del sensor de línea
  if (digitalRead(sensorLinea) == LOW) { // Se asume que HIGH significa que se detecta la línea blanca
    // Retroceder los motores
    digitalWrite(motor1_A, HIGH);
    digitalWrite(motor1_B, LOW);
    digitalWrite(motor2_A, LOW);
    digitalWrite(motor2_B, HIGH);
  } else {
    // Avanzar los motores si no se detecta la línea blanca
    digitalWrite(motor1_A, LOW);
    digitalWrite(motor1_B, HIGH);
    digitalWrite(motor2_A, HIGH);
    digitalWrite(motor2_B, LOW);
  }
}