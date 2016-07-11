/**
  License

  THE BEER-WARE LICENSE" (Revision 42):
  [phk@FreeBSD.ORG]() wrote this file. As long as you retain this notice you
  can do whatever you want with this stuff. If we meet some day, and you think
  this stuff is worth it, you can buy me a beer in return Poul-Henning Kamp
**/
#include <SoftwareSerial.h>

#define pinLDR 0
#define pinRed 4
#define pinGreen 3
#define pinBlue 2

int ambientLight;
int thresholdLDR = 0;
int red = 0;
int green = 0;
int blue = 0;

char bRed[] = "000";
char bGreen[] = "000";
char bBlue[] = "000";
char bLDR = '0';
char bThresholdLDR[] = "000";

SoftwareSerial blu(11, 10); // RX TX

void setup()
{
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);

  digitalWrite(pinRed, LOW);
  digitalWrite(pinGreen, LOW);
  digitalWrite(pinBlue, LOW);

  blu.begin(9600);
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("# Arduino Started #");
  blu.println("!! Bluetooth Started !!");

  printVars();
}

void loop()
{
  readBluetooth();
  writeBluetooth();
  
  printVars();
  
  useLDR();

//  serialRW();
  delay(3000);
}

void printVars()
{
  Serial.println("##################");
  Serial.print("RED         ");
  Serial.println(red);
  Serial.print("GREEN       ");
  Serial.println(green);
  Serial.print("BLUE        ");
  Serial.println(blue);
  Serial.print("LDR         ");
  Serial.println(bLDR);
  Serial.print("Threshold   ");
  Serial.println(thresholdLDR);
  Serial.println("##################");
}

void serialRW()
{
  if (blu.available()) {
    while (blu.available()) {
      Serial.write(blu.read());
    }
  }

  if (Serial.available()) {
    while (Serial.available()) {
      blu.write(Serial.read());
    }
  }    
}

void readBluetooth()
{
  char currentValue[3];
  boolean hasData = false;
  int i = 0;
  int j = 0;

  if (blu.available())
  {
    while (blu.available())
    {
      char c = blu.read();
      
      if (c == '<') // begin data
      {
        hasData = true;
      }
      else if (hasData && c == '>') // end data
      {
        setValue(j, currentValue);
        hasData = false;
      }
      else if (hasData && c == '|') // next data
      {
        setValue(j++, currentValue);        
        char currentValue[3];
        i = 0;
      }
      else if (hasData)
      {
        currentValue[i++] = c;
      }
    }
  }
}

void writeBluetooth()
{
  String vars = "<";
  vars += ambientLight;
  vars += ">";

  blu.println(vars);
}

void setValue(int position, char value[])
{  
  switch(position)
  {
    case 0:
      char bRed[3];
      for(int i = 2; i >= 0; i--) {
        bRed[i] = value[i];
      }
      red = atoi(bRed);
    break;

    case 1:
      char bGreen[3];
      for (int i = 2; i >= 0; i--) {
        bGreen[i] = value[i];
      }
      green = atoi(bGreen);
    break;

    case 2:
      char bBlue[3];
      for (int i = 2; i >= 0; i--) {
        bBlue[i] = value[i];
      }
      blue = atoi(bBlue);
    break;    

    case 3:
      bLDR = value[0];
    break;

    case 4:
      char bThresholdLDR[3];
      for (int i = 2; i >= 0; i--) {
        bThresholdLDR[i] = value[i];
      }
      thresholdLDR = atoi(bThresholdLDR);
    break;
  }  
}

void readLDR()
{
  int total = 0;
  int average = 0;
  int thisReading = 0;

  for (int i = 0; i < 10; i++)
  {
    thisReading = analogRead(pinLDR);
    total = total + thisReading;
  }

  ambientLight = total / 10;
  delay(1);
}

void useLDR()
{
  if (bLDR == '1')
  {
    readLDR();
    
    if (ambientLight > thresholdLDR)
    {
      analogWrite(pinRed, red);
      analogWrite(pinGreen, green);
      analogWrite(pinBlue, blue);
    }
    else
    {
      digitalWrite(pinRed, LOW);
      digitalWrite(pinGreen, LOW);
      digitalWrite(pinBlue, LOW);
    }
  }
  else
  {
    analogWrite(pinRed, red);
    analogWrite(pinGreen, green);
    analogWrite(pinBlue, blue);
  }
}
