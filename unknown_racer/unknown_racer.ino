// settings 
// #define TESTMOTORS
// #define TESTSENS

#define lDirPin 4
#define lSpeedPin 5
#define rSpeedPin 3
#define rDirPin 2
#define N 8
#define maxJ 100

byte sensPin[N] = {7, 6, 5, 4, 3, 2, 1, 0};
int sensVal[N];
float lightVal[N];

float weight[N] = { -16.0f, -9.0f, -4.0f, -1.0f, 1.0f, 4.0f, 9.0f, 16.0f};
int maxVal[N] = { 0, 0, 0, 0, 0, 0, 0, 0};
int minVal[N] = { 1023,  1023,  1023,  1023,  1023,  1023, 1023, 1023};

float p = 5.0f;
float d = 1 000.0f;
float i = 0.01f;
float y = 0;
float yPrev = 0.0f;
float maxInt = 10000;
float minInt = -10000;
float integral = 0;
float y_j[maxJ];
int j = 0;
int currJ = 0;


int V0 = 120;
int U = 0;
byte blackNum;
String var = "";

int STATE;

unsigned long int t1, t2, T;

void setup() {

  delay(3000);
  
  startMotors();

  go(0, 0);
  
  Serial.begin(9600);

  T = millis();

  for (j = 0; j < maxJ; j++) {
    y_j[j] = 0.0f;
  }

#ifdef TESTMOTORS
  go(100, 100);
  delay(1000);
  go(-100, -100);
  delay(1000);
  go(-100, 100);
  delay(1000);
  go(100, -100);
  delay(1000);
  go(0, 0);
  delay(1000);
  while (1);
#endif // TESTMOTORS
  scan();
  go(0, 0);
}

void loop() {
  readLine();
  y = calcY();
  t2 = micros();
  U = p * y + d * (y - yPrev) / (t2 - t1) + i * integral;
#ifdef TESTSENS
  writeLine();
#else
  if (blackNum > 0)  go(V0 + U, V0 - U);
#endif
  t1 = t2;
  yPrev = y;
}

void readLine() {
  blackNum = 0;
  for (int i = 0; i < N; i++) {
    sensVal[i] = analogRead(sensPin[i]);
  }
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < N; i++) {
      sensVal[i] += analogRead(sensPin[i]);
    }
  }
  for (int i = 0; i < N; i++) {
    sensVal[i] /= 5;
    if (sensVal[i] > ((maxVal[i] - minVal[i]) / 2)) blackNum++;
  }
}

int calcY() {
  int u = 0;
  y = 0.0f;
  for (int i = 0; i < N; i++) {
    lightVal[i] = float(sensVal[i] - minVal[i]) / float( maxVal[i] - minVal[i]);
  }

  if (lightVal[0] < 0.5) lightVal[0] = 0;
  if (lightVal[N - 1] < 0.5) lightVal[N - 1] = 0;

  for (int i = 0; i < N; i++) {
    y += weight[i] * lightVal[i];
  }
  if (blackNum > 0) y /= blackNum;

  y_j[currJ] = y;
  currJ++;
  if (currJ == maxJ) currJ = 0;

  integral = 0;
  for (j = 0; j < maxJ; j++) integral += y_j[j];

  integral = constrain(integral, minInt, maxInt);

  return y;
}


void writeLine() {
  for (int i = 0; i < N; i++) {
    Serial.print(sensVal[i]);
    Serial.print(" | ");
  }
  Serial.println();
  Serial.println(y);
  Serial.print("t - ");
  Serial.println(t2 - t1);
  delay(200);
}

void go(int lSpeed, int rSpeed) {
  if (lSpeed < 0) {
    digitalWrite(lDirPin, LOW);
  } else {
    digitalWrite(lDirPin, HIGH);
  }
  if (rSpeed < 0) {
    digitalWrite(rDirPin, LOW);
  } else {
    digitalWrite(rDirPin, HIGH);
  }

  analogWrite(lSpeedPin, abs(constrain(lSpeed, -255, 255)));
  analogWrite(rSpeedPin, abs(constrain(rSpeed, -255, 255)));
}

void startMotors() {
  pinMode(lDirPin, OUTPUT);
  pinMode(lSpeedPin, OUTPUT);
  pinMode(rSpeedPin, OUTPUT);
  pinMode(rDirPin, OUTPUT);
}

void scan() {
  for (int i = 0; i < N; i++) {
    minVal[i] = 1023;
    maxVal[i] = 0;
  }
  T = millis();
  int a;
  go(90, -90);
  while (millis() - T <= 5000) {
    for (int i = 0; i < N; i++) {
      a = analogRead(sensPin[i]);
      if (a < minVal[i]) {
        minVal[i] = a;
      }
      if (a > maxVal[i]) {
        maxVal[i] = a;
      }
    }
  }
  go(0, 0);
}


