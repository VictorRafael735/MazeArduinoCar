//Configuração dos motores

//A   B

//********************************************************
//*                                                      *
//*             Inicialização dos pinos                  *
//*                                                      *
//********************************************************

//motores
const int engine1APin = 0;
const int engine1BPin = 0;
const int engine2APin = 0;
const int engine2BPin = 0;

//sensores
const int sensor1Pin = 0;
const int sensor2Pin = 0;

//********************************************************
//*                                                      *
//*            Construção do algoritmo Fuzzy             *
//*                                                      *
//********************************************************

//valores lidos dos sensores
int sensor1Value = 0;
int sensor2Value = 0;

//Estrutura contendo os valores de pertinência das entradas (0.0 -> 1.0)
struct sensorRelevance {
  double high; //Indica que está perto de um objeto
  double mid; 
  double low; //Indica que está longe de um objeto
};

//Estrutura de pertinência de cada sensor
struct sensorRelevance sensorARelevance;
struct sensorRelevance sensorBRelevance;

//Variáveis contendo os pontos do sinal de entrada variando de 0 a 1024
const int P1 = 100;
const int P2 = 250;
const int P3 = 350;
const int P4 = 400;
const int P5 = 700;
const int P6 = 800;
const int P7 = 900;
const int P8 = 1000;

//Calcula a pertinência de cada conjunto de um sensor. A estrutura precisa ser passada como ponteiro, para não gerar cópias de dados.
void setSensorRelevance(double value, double p1,double p2,double p3,double p4,double p5, double p6, double p7, double p8, sensorRelevance *sensor) {
  // Calcula a pertinência do conjunto Low
  if(value<p1)
    sensor->low=1.0;
  
  if(value>=p1&&value<p3) 
    sensor->low=(p3-value) / (p3-p1);
  
  if(value>=p3) 
    sensor->low=0.0;
  
  // calcula a pertinência ao conjunto medio
  if(value<p2)
    sensor->mid=0.0;
  
  if(value>=p2&&value<p4)
    sensor->mid=(value - p2) / (p4 - p2);
  
  if(value>=p4&&value<p5)
    sensor->mid=1.0;
  
  if(value>=p5&&value<p7)
    sensor->mid=(p7-value) / (p7-p5);
  
  if(value>=p7)
    sensor->mid=0.0;
  

  // calcula a pertinência ao conjunto alto
  if(value<p6)
    sensor->high=0.0;
    
  if(value>=p6&&value<p8)
    sensor->high=(value - p6) / (p8 - p6);

  if(value>=p8)
    sensor->high=1.0;

}

// Estrutura de avaliação das regras
struct Rule
{
 double s1;
 double s2;
 double active;
};

struct Rule R[9];
// Estes são os conjuntos fuzzy de saída, bem como a sessão de Interferência (regras de ativação)
//
// rotateLeft
// turnLeft
// move
// turnRight
// rotateRight
//
//Base de regras
//
//R1 -> SE s1==alto && s2 ==alto ENTÃO saída = rotateRight
//R2 -> SE s1==alto && s2 ==medio ENTÃO saída = turnRight
//R3 -> SE s1==alto && s2 ==baixo ENTÃO saída = turnRight
//R4 -> SE s1==medio && s2 ==alto ENTÃO saída = turnLeft
//R5 -> SE s1==medio && s2 ==medio ENTÃO saída = move
//R6 -> SE s1==medio && s2 ==baixo ENTÃO saída = move
//R7 -> SE s1==baixo && s2 ==alto ENTÃO saída = rotateLeft
//R8 -> SE s1==baixo && s2 ==medio ENTÃO saída = move
//R9 -> SE s1==baixo && s2 ==baixo ENTÃO saída = move

// Busca os valores de pertinência e armazena nas regras
void setFuzzyRules()
{
  //R1 -> SE s1==alto && s2 ==alto ENTÃO saída = rotateRight
  R[0].s1=sensorARelevance.high;
  R[0].s2=sensorBRelevance.high;
  //R2 -> SE s1==alto && s2 ==medio ENTÃO saída = turnRight
  R[1].s1=sensorARelevance.high;
  R[1].s2=sensorBRelevance.mid;
  //R3 -> SE s1==alto && s2 ==baixo ENTÃO saída = turnRight
  R[2].s1=sensorARelevance.high;
  R[2].s2=sensorARelevance.low;
  //R4 -> SE s1==medio && s2 ==alto ENTÃO saída = turnLeft
  R[3].s1=sensorARelevance.mid;
  R[3].s2=sensorARelevance.high;
  //R5 -> SE s1==medio && s2 ==medio ENTÃO saída = move
  R[4].s1=sensorARelevance.mid;
  R[4].s2=sensorBRelevance.mid;
  //R6 -> SE s1==medio && s2 ==baixo ENTÃO saída = move
  R[5].s1=sensorARelevance.mid;
  R[5].s2=sensorBRelevance.low;
  //R7 -> SE s1==baixo && s2 ==alto ENTÃO saída = rotateLeft
  R[6].s1=sensorARelevance.low;
  R[6].s2=sensorBRelevance.high;
  //R8 -> SE s1==baixo && s2 ==medio ENTÃO saída = move
  R[7].s1=sensorARelevance.low;
  R[7].s2=sensorBRelevance.mid;
  //R9 -> SE s1==baixo && s2 ==baixo ENTÃO saída = move
  R[8].s1=sensorARelevance.low;
  R[8].s2=sensorBRelevance.low;
}

//Retorna o mínimo de 2 valores
double minValue(double v1, double v2)
{
  double min = v1;
  if(v2<min) min=v2;
  return min;
}

//Calcula o valor de ativação de cada regra ("chance" de ocorrer) pelo "MÍNIMO". 
void calculateRuleActive()
{
  int cont;
  for(cont=0; cont<9; cont++)
  {
    R[cont].active=minValue(R[cont].s1,R[cont].s2);
  }
}

//Estrutura contendo os valores de pertinência das saídas
struct outRelevance
{
  double rotateLeft;
  double turnLeft;
  double move;
  double turnRight;
  double rotateRight;
};

//Pertinência da saída
struct outRelevance outRel;

//Calcula a pertinência de cada saída pelo "MÁXIMO". Caso a saída esteja atrelada somente a uma regra, ela recebe a própria ativação da regra.
//Lembre-se que o vetor sempre recebe um número a menos que o valor original da regra, devido ao índice começar por 0.
//Ex: Regra 7 -> R[6]
void calculateOutRelevance()
{
  outRel.rotateLeft = R[6].active; // rotateLeft -> R7 
  
  outRel.turnLeft = R[3].active; // turnLeft -> R4
  
  outRel.move = R[4].active; // move -> R5,R6,R8, R9
  if(outRel.move < R[5].active)
    outRel.move = R[5].active;
  if(outRel.move < R[7].active)
    outRel.move = R[7].active;
  if(outRel.move < R[8].active)
    outRel.move = R[8].active;

  outRel.turnRight = R[1].active; // turnRight -> R2,R3
  if(outRel.turnRight < R[2].active)
    outRel.turnRight = R[2].active;
  
  outRel.rotateRight = R[0].active; // rotateRight -> R0
}

//Valor da saída
double out = 0;

// Calcula a saída pelo Método da Média Ponderada dos Máximos
// Calcula o centro de cada conjunto de saída (rotateLeft,turnLeft,move,turnRight,rotateRight) vide gráfico
void calculateOut()
{
  double rotateLeftCenter;
  double turnLeftCenter;
  double moveCenter;
  double turnRightCenter;
  double rotateRightCenter;
  double num = 0;
  double den = 0;
  
  //Pontos do gráfico
  rotateLeftCenter = (0+40)/2;
  turnLeftCenter = (30+60)/2;
  moveCenter = (50+130)/2;
  turnRightCenter = (100+165)/2;
  rotateRightCenter = (140+180)/2;
  
  //calcula a média
  if(outRel.rotateLeft > 0)
  {
    num = num + rotateLeftCenter * outRel.rotateLeft;
    den = den + outRel.rotateLeft;
  }

  if(outRel.turnLeft > 0)
  {
    num = num + turnLeftCenter * outRel.turnLeft;
    den = den + outRel.turnLeft;
  }

  if(outRel.move > 0)
  {
    num = num + moveCenter * outRel.move;
    den = den + outRel.move;
  }

  if(outRel.turnRight > 0)
  {
    num = num + turnRightCenter * outRel.turnRight;
    den = den + outRel.turnRight;
  }

  if(outRel.rotateRight > 0)
  {
    num = num + rotateRightCenter * outRel.rotateRight;
    den = den + outRel.rotateRight;
  }

  out=num/den;
}

//Aciona os motores de acordo com o valor calculado na saída
//
void initEngines()
{
  // Primeiro quadrante, girando para a esquerda
  if(out<=45)
  {
    digitalWrite(engine1APin, LOW);
    analogWrite(engine1BPin, 255);
    digitalWrite(engine2APin, HIGH);
    analogWrite(engine2BPin, (out*5)+30); //45 - 0 => (255 - 0) + 30
  }

  // Segundo quadrante, virando para a esquerda
  if(out<88 && out > 45)
  {
    digitalWrite(engine1APin, LOW);
    analogWrite(engine1BPin, 255);
    digitalWrite(engine2APin, LOW);
    analogWrite(engine2BPin, (out-46)*5+50); //46 - 87 => (0 - 205) + 50
  }

  // Andando (valor ~ 90)
  if(out>=88 && out <=92)
  {
    digitalWrite(engine1APin, LOW);
    analogWrite(engine1BPin, 255);
    digitalWrite(engine2APin, LOW);
    analogWrite(engine2BPin, 255);
  }

  // Terceiro quadrante, virando para a direita
  if(out>92 && out <135)
  {
    digitalWrite(engine1APin, LOW);
    analogWrite(engine1BPin, (134-out)*5+50); //93 - 134 => (205 - 0) + 50);
    digitalWrite(engine2APin, LOW);
    analogWrite(engine2BPin, 255);
  }

  // Quarto quadrante, girando para a direita
  if(out>=135)
  {
    digitalWrite(engine1APin, HIGH);
    analogWrite(engine1BPin,((45-(out-135))*5)+30); //135 - 180 => (255 - 0) + 30
    digitalWrite(engine2APin, LOW);
    analogWrite(engine2BPin,255);
  }
}

//********************************************************
//*                                                      *
//*                     Funções                          *
//*                                                      *
//********************************************************

//Inicializa os pinos dos motores. É necessário inicializar os pinos de saída digital como HIGH para o carrinho andar somente para frente.
void initializePins() {
  pinMode(engine1APin, OUTPUT);
  pinMode(engine1BPin, OUTPUT);
  pinMode(engine2APin, OUTPUT);
  pinMode(engine2BPin, OUTPUT);
  
  pinMode(sensor1Pin, INPUT);
  pinMode(sensor2Pin, INPUT);
}

void setEngines() {
  // Seta os motores para andarem somente para frente
  digitalWrite(engine1APin, LOW);
  digitalWrite(engine2APin, LOW);
}

//********************************************************
//*                                                      *
//*                 Setup da aplicação                   *
//*                                                      *
//********************************************************

//Inicialização do setup da aplicação. Este código é executado apenas uma vez antes do loop.
void setup() {

  initializePins();

  setEngines();

}

//********************************************************
//*                                                      *
//*                     Aplicação                        *
//*                                                      *
//********************************************************

//Loop de operação da aplicação
void loop() {

  //Realiza leitura dos sensores
  sensor1Value = analogRead(sensor1Pin);
  sensor2Value = analogRead(sensor2Pin);
  
  //Calcula a pertinência do conjunto Fuzzy de entrada
  setSensorRelevance((double) sensor1Value, P1, P2, P3, P4, P5, P6, P7, P8, &sensorARelevance);
  setSensorRelevance((double) sensor2Value, P1, P2, P3, P4, P5, P6, P7, P8, &sensorBRelevance);

  //Processo de Interferência (avaliação das regras)
  setFuzzyRules();

  //Calcula o valor de ativação de cada regra
  calculateRuleActive();

  //Calcula pertinência do conjuntoi Fuzzy de saídas
  calculateOutRelevance();

  //Calcula a saída pelo método da média ponderada dos máximos
  calculateOut();

  //Manda o valor de saída para os motores
  initEngines();
}
