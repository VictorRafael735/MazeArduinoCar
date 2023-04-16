//Configuração dos motores

  //A   B

  //C   D

//********************************************************
//*                                                      *
//*             Inicialização dos pinos                  *
//*                                                      *
//********************************************************

//motores
const int engineAPin = 0;
const int engineBPin = 0;                 
const int engineCPin = 0;                     
const int engineDPin = 0;

//sensores
const int sensorAPin = 0;
const int sensorBPin = 0;

//********************************************************
//*                                                      *
//*                 Variáveis globais                    *
//*                                                      *
//********************************************************

//valores lidos dos sensores
int sensorAValue = analogRead(sensorAPin);
int sensorBValue = analogRead(sensorBPin);

//estado da aplicação
//bool isRunningApp = false;

//Dado recebido via bluetooth
int receivedData = 0;

//********************************************************
//*                                                      *
//*                     Funções                          *
//*                                                      *
//********************************************************
void initializePins() {
  pinMode(engineAPin, OUTPUT);
  pinMode(engineBPin, OUTPUT);
  pinMode(engineCPin, OUTPUT);
  pinMode(engineDPin, OUTPUT);
  pinMode(sensorAPin, INPUT);
  pinMode(sensorBPin, INPUT);
}

void resetEngines() {
  analogWrite(engineAPin, 0);
  analogWrite(engineBPin, 0);
  analogWrite(engineCPin, 0);
  analogWrite(engineDPin, 0);
}

//********************************************************
//*                                                      *
//*                 Setup da aplicação                   *
//*                                                      *
//********************************************************

//Inicialização do setup da aplicação. Este código é executado apenas uma vez antes do loop.
void setup() {

  //Inicialização dos pinos
  initializePins();

  //Seta os valores de PWM dos motores para 0
  resetEngines();

  //Seta o baud rate da comunicação serial
  Serial.begin(9600);

}

//********************************************************
//*                                                      *
//*                     Aplicação                        *
//*                                                      *
//********************************************************

//Loop de operação da aplicação
void loop() {

  //Verifica se existe dados sendo recebidos pela porta serial
  if(Serial.avalible() > 0) {
    receivedData = Serial.read();
  }

  //Visualização do dado recebido
  Serial.println("Dado recebido pelo bluetooth: \n");
  Serial.println(receivedData);

}
