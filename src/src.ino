#include <NewPing.h>

// Seleccionar estrategia
constexpr int strategy_index{ 3 };

// Distancias
const unsigned long CLOSE{ 15 };
const unsigned long FAR{ 35 };
const unsigned long MAX_DISTANCE{ 40 };

// Forward declarations
class Motor;

// Definición de pines
constexpr int pulsador = 5;
constexpr int ledControl = 13;
constexpr int motor2_A = 10;
constexpr int motor2_B = 11;
constexpr int motor2_enable = 6;
constexpr int motor1_A = 8;
constexpr int motor1_B = 7;
constexpr int motor1_enable = 9;
constexpr int sensorLinea = A5;

// Pines de los sensores de ultrasonido
constexpr int CANTIDAD_SENSORES{ 3 };

const int ultra_trig[CANTIDAD_SENSORES] = { A1, 3, A3 };
const int ultra_echo[CANTIDAD_SENSORES] = { A2, 4, A4 };

NewPing sonar[CANTIDAD_SENSORES] = {
  NewPing(ultra_trig[0], ultra_echo[0], MAX_DISTANCE),
  NewPing(ultra_trig[1], ultra_echo[1], MAX_DISTANCE),
  NewPing(ultra_trig[2], ultra_echo[2], MAX_DISTANCE)
};

// Variable para el estado del pulsador
bool run = false;
bool estadoLed = false;

// Temporizadores
unsigned long tempLed{ 0 };
unsigned long tempRun{ 0 };
unsigned long tempAction{ 0 };

// Delays
const unsigned long runDelay{ 5000 };
const unsigned long blinkDelay{ 500 };
const unsigned long actionDelay{};

enum class Direccion {
  ADELANTE,
  ATRAS,
  DERECHA,
  IZQUIERDA
};

class Motor {
private:
  const int m_pin_a{};
  const int m_pin_b{};
  const int m_enable{};

  void setSpeed(int velocidad) {
    velocidad = constrain(velocidad, 0, 255);
    analogWrite(m_enable, velocidad);
  }

  void setForward() {
    digitalWrite(m_pin_a, HIGH);
    digitalWrite(m_pin_b, LOW);
  }

  void setBackward() {
    digitalWrite(m_pin_a, LOW);
    digitalWrite(m_pin_b, HIGH);
  }

  void setDirection(Direccion dir) {
    switch (dir) {
      case Direccion::ADELANTE:
        setForward();
        break;
      case Direccion::ATRAS:
        setBackward();
        break;
      default:
        break;
    }
  }

public:
  void parar() {
    setSpeed(0);
  }

  Motor(const int pin_a, const int pin_b, const int enable)
    : m_pin_a{ pin_a }, m_pin_b{ pin_b }, m_enable{ enable } {
    pinMode(m_pin_a, OUTPUT);
    pinMode(m_pin_b, OUTPUT);
    pinMode(m_enable, OUTPUT);
  }

  void mover(Direccion dir, int velocidad) {
    setDirection(dir);
    setSpeed(velocidad);
  }
};

struct MedidasSensores 
{
public:
  unsigned long izquierda{};
  unsigned long adelante{};
  unsigned long derecha{};

  MedidasSensores() : izquierda{}, derecha{}, adelante{}
  {}

  MedidasSensores(unsigned long izquierda, unsigned long adelante, unsigned long derecha)
  {
    this->izquierda = izquierda;
    this->derecha = derecha;
    this->adelante = adelante;
  }

  unsigned long& operator[](int idx) 
  {
    switch (idx) {
      case 0:
        return izquierda;
      case 1:
        return adelante;
      case 2:
        return derecha;
      default:
        return adelante;
    }
  }

  size_t length() 
  {
    return 3;
  }
};

Motor motor2{ motor1_A, motor1_B, motor1_enable };
Motor motor1{ motor2_A, motor2_B, motor2_enable };

class Estrategia 
{
protected:
  Motor m_motor_d;
  Motor m_motor_i;
public:
  Estrategia(Motor& motor_d, Motor& motor_i)
    : m_motor_d{ motor_d }, m_motor_i{ motor_i } {}
  
  virtual void onIdle(){}
  virtual void onLine(){}
  virtual void closeFront(){}
  virtual void farFront(){}
  virtual void closeRight(){}
  virtual void farRight(){}
  virtual void closeLeft(){}
  virtual void farLeft(){}

  void stop()
  {
    m_motor_d.parar();
    m_motor_i.parar();
  }
};

class Estrategia1 : public Estrategia 
{
public:
  Estrategia1(Motor& motor_d, Motor& motor_i) : Estrategia{motor_d, motor_i} {}

  void onIdle() override {
    m_motor_d.mover(Direccion::ADELANTE, 255);
    m_motor_i.mover(Direccion::ADELANTE, 255);
  }

  void onLine() override {
    //m_motor_d.mover(Direccion::ATRAS, 125);
    //m_motor_i.mover(Direccion::ATRAS, 125);
    closeFront();
  }

  void closeFront() final {
    onIdle();
  }
  void farFront() override {
    //m_motor_d.mover(Direccion::ADELANTE, 80);
    //m_motor_i.mover(Direccion::ADELANTE, 80);
    onIdle();
  }

  void closeRight() override {
    //m_motor_d.mover(Direccion::ATRAS, 80);
    //m_motor_i.mover(Direccion::ADELANTE, 80);
    onIdle();
  }

  void farRight() override {
    onIdle();
  }

  void closeLeft() override {
    onIdle();
  }

  void farLeft() override {
    onIdle();
  }
};

class Estrategia2 : public Estrategia 
{
public:
  Estrategia2(Motor& motor_d, Motor& motor_i) : Estrategia{motor_d, motor_i} {}

  void onIdle() final {
    m_motor_d.mover(Direccion::ADELANTE, 150);
    //m_motor_i.mover(Direccion::ADELANTE, 150);
  }

  void onLine() final {
    m_motor_i.parar();
    m_motor_d.mover(Direccion::ATRAS, 220);
    delay(150);
  }

  void closeFront() final {
    m_motor_d.mover(Direccion::ADELANTE, 255);
    m_motor_i.mover(Direccion::ADELANTE, 255);
  }
  void farFront() final {
    //m_motor_d.mover(Direccion::ADELANTE, 80);
    //m_motor_i.mover(Direccion::ADELANTE, 80);
    m_motor_d.mover(Direccion::ADELANTE, 180);
    m_motor_i.mover(Direccion::ADELANTE, 90);
  }

  void closeRight() final {
    m_motor_d.mover(Direccion::ATRAS, 150);
    m_motor_i.mover(Direccion::ATRAS, 90);
  }

  void farRight() final {
    m_motor_d.mover(Direccion::ADELANTE, 90);
    m_motor_i.mover(Direccion::ADELANTE, 120);
  }

  void closeLeft() final {
    m_motor_d.mover(Direccion::ATRAS, 90);
    m_motor_i.mover(Direccion::ATRAS, 150);
  }

  void farLeft() final {
    m_motor_d.mover(Direccion::ADELANTE, 120);
    m_motor_i.mover(Direccion::ADELANTE, 90);
  }
};

class Estrategia3 : public Estrategia 
{
public:
  Estrategia3(Motor& motor_d, Motor& motor_i) : Estrategia{motor_d, motor_i} {}

  void onIdle() final {
    m_motor_d.mover(Direccion::ADELANTE, 100);
    m_motor_i.mover(Direccion::ADELANTE, 100);
  }

  void onLine() final {
    m_motor_i.mover(Direccion::ATRAS, 220);
    m_motor_d.mover(Direccion::ATRAS, 220);
    delay(1500);
    m_motor_i.mover(Direccion::ADELANTE, 120);
    m_motor_d.mover(Direccion::ATRAS, 120);
    delay(500);
    
  }

  void closeFront() final {
    m_motor_d.mover(Direccion::ADELANTE, 255);
    m_motor_i.mover(Direccion::ADELANTE, 255);
  }
  void farFront() final {
    //m_motor_d.mover(Direccion::ADELANTE, 80);
    //m_motor_i.mover(Direccion::ADELANTE, 80);
    onIdle();
  }

  void closeRight() final {
    m_motor_d.mover(Direccion::ADELANTE, 100);
    m_motor_i.mover(Direccion::ADELANTE, 110);
  }

  void farRight() final {
    onIdle();
  }

  void closeLeft() final {
    m_motor_d.mover(Direccion::ADELANTE, 100);
    m_motor_i.mover(Direccion::ADELANTE, 110);
  }

  void farLeft() final {
    onIdle();
  }
};

class Estrategia4 : public Estrategia 
{
public:
  Estrategia4(Motor& motor_d, Motor& motor_i) : Estrategia{motor_d, motor_i} {}

  void onIdle() final {
    m_motor_i.mover(Direccion::ADELANTE, 150);
    m_motor_d.mover(Direccion::ATRAS, 150);
  }

  void onLine() final {
    m_motor_i.mover(Direccion::ATRAS, 220);
    m_motor_d.mover(Direccion::ATRAS, 220);
    delay(1500);
    m_motor_i.mover(Direccion::ADELANTE, 120);
    m_motor_d.mover(Direccion::ATRAS, 120);
    delay(500);
    
  }

  void closeFront() final {
    m_motor_d.mover(Direccion::ADELANTE, 255);
    m_motor_i.mover(Direccion::ADELANTE, 255);
    delay(500);
  }
  void farFront() final {
    //m_motor_d.mover(Direccion::ADELANTE, 80);
    //m_motor_i.mover(Direccion::ADELANTE, 80);
    onIdle();
  }

  void closeRight() final {
    onIdle();
  }

  void farRight() final {
    onIdle();
  }

  void closeLeft() final {
    onIdle();
  }

  void farLeft() final {
    onIdle();
  }
};

MedidasSensores medirDistancias() 
{
  MedidasSensores medidas;
  for (int i{ 0 }; i < CANTIDAD_SENSORES; ++i)
  {
  //   digitalWrite(ultra_trig[i], LOW);
  //   delayMicroseconds(2);
  // //for (int i{ 0 }; i < CANTIDAD_SENSORES; ++i)
  //   digitalWrite(ultra_trig[i], HIGH);
  //   delayMicroseconds(10);
  // //for (int i{ 0 }; i < CANTIDAD_SENSORES; ++i)
  //   digitalWrite(ultra_trig[i], LOW);
  //   unsigned long duracion = pulseIn(ultra_echo[i], HIGH);
  //   medidas[i] = (duracion * 0.034) / 2;
    medidas[i] = sonar[i].ping_cm();
    delay(29);
  }

  // for (int i{ 0 }; i < CANTIDAD_SENSORES; i++) 
  // {
  //   unsigned long duracion = pulseIn(ultra_echo[i], HIGH);
  //   // Convertir a cm.
  //   medidas[i] = (duracion * 0.034) / 2;
  // }
  // medidas.izquierda = (pulseIn(ultra_echo[0], HIGH) * 0.034) / 2;
  // medidas.adelante = (pulseIn(ultra_echo[1], HIGH) * 0.034) / 2;
  // medidas.derecha = (pulseIn(ultra_echo[2], HIGH) * 0.034) / 2;

  return medidas;
}

inline bool leerSensorLinea() 
{
  return digitalRead(sensorLinea);
}

inline bool leerPulsador() 
{
  return digitalRead(pulsador);
}

void printSerial(MedidasSensores& ultra, bool linea)
{
  // Mostrar los valores en el monitor serie
  Serial.print("A: ");
  Serial.print(ultra.adelante);
  Serial.print("\t"); // Tabulación para columnas
  Serial.print("D: ");
  Serial.print(ultra.derecha);
  Serial.print("\t");
  Serial.print("I: ");
  Serial.print(ultra.izquierda);
  Serial.print("\t");
  Serial.print(linea ? "Blanco" : "Negro");
  Serial.print("\t");
  Serial.println(run ? "ON" : "OFF");
}

inline void setLed(bool state)
{
  digitalWrite(ledControl, state);
}

void handleLed(unsigned long currentTime, MedidasSensores& ultra) 
{
  unsigned int deltaLed{ currentTime - tempLed };
  unsigned int deltaRun{ currentTime - tempRun };

  if (!run)
    estadoLed = false;
  else if(deltaRun > runDelay)
  {
    estadoLed = (ultra.adelante < CLOSE);
  }
  else if(deltaLed > blinkDelay)
  {
    estadoLed = !estadoLed;
    tempLed = millis();
  }

  setLed(estadoLed);  
}

void handleStrategy(Estrategia& strat, unsigned long currentTime, MedidasSensores& ultra, bool linea)
{
  unsigned long deltaRun{ currentTime - tempRun };
  if((!run) || (deltaRun < runDelay))
  {
    Serial.print("X");
    return;
  }
  Serial.print("Z");

  if(linea)
  {
    strat.onLine();
  }
  else if(ultra.adelante < CLOSE)
  {
    strat.closeFront();
  }
  else if(ultra.adelante < FAR)
  {
    strat.farFront();
  }
  else if(ultra.derecha < 4)
  {
    strat.closeRight();
  }
  // else if(ultra.derecha < FAR)
  // {
  //   strat.farRight();
  // }
  else if(ultra.izquierda < 4)
  {
    strat.closeLeft();
  }
  else
  {
    strat.onIdle();
  }
}

void chooseStrategy(unsigned long currentTime, MedidasSensores& ultra, bool linea)
{
  if(strategy_index == 0)
  {
    Estrategia1 s{motor1, motor2};
    handleStrategy(s, currentTime, ultra, linea);
    Serial.print("0");
  }
  else if(strategy_index == 1)
  {
    Estrategia2 s2{motor1, motor2};
    handleStrategy(s2, currentTime, ultra, linea);
    Serial.print("1");
  }
  else if(strategy_index == 2)
  {
    Estrategia3 s3{motor1, motor2};
    handleStrategy(s3, currentTime, ultra, linea);
    Serial.print("2");
  }
  else if(strategy_index == 3)
  {
    Estrategia4 s4{motor1, motor2};
    handleStrategy(s4, currentTime, ultra, linea);
    Serial.print("3");
  }
}

void setup() {
  // Configuración de pines
  pinMode(pulsador, INPUT);
  pinMode(ledControl, OUTPUT);
  pinMode(sensorLinea, INPUT);

  // Configuración de los pines de los sensores ultrasonicos
  for (int i{ 0 }; i < CANTIDAD_SENSORES; ++i)
  {
    pinMode(ultra_trig[i], OUTPUT);
    pinMode(ultra_echo[i], INPUT);
  }

  // Iniciar la comunicación serie
  Serial.begin(9600);

  // Imprimir los encabezados de las columnas
  Serial.println("S_del\tS_der\tS_izq\tLinea");
}

void loop() {
  // Leer sensores
  bool lecturaPulsador{ leerPulsador() };
  bool lecturaLinea{ leerSensorLinea() };
  MedidasSensores lecturaUltra{ medirDistancias() };

  printSerial(lecturaUltra, lecturaLinea);

  // Verificar si el pulsador está presionado para iniciar el programa
  if (lecturaPulsador && !run) {
    // Indicar que el pulsador fue presionado
    run = true;
    tempRun = millis();
    tempLed = millis();
    estadoLed = true;
  }

  // Actualizar tiempo
  unsigned long currentTime{ millis() };

  handleLed(currentTime, lecturaUltra);
  chooseStrategy(currentTime, lecturaUltra, lecturaLinea);
}