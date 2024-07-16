#include <Ultrasonic.h>
#include "pitches.h"
#include "DFRobot_RGBLCD1602.h"
#include <DHT.h>;
//Constants
#define DHTPIN 6     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
const int potPin = A3; // initialize potentialmeter analog pin
//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
char dtaUart[15];
char dtaLen = 0;
DFRobot_RGBLCD1602 lcd(/*RGBAddr*/0x60 ,/*lcdCols*/16,/*lcdRows*/2);
Ultrasonic ultrasonic(8, 9);
int distance;
int pump_open_duration = 100;

int melody[] = {
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, 
  NOTE_A4, NOTE_REST, NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_REST, NOTE_E4,
  NOTE_GS4, NOTE_B4, NOTE_REST, NOTE_E4, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_D5, 
  NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4, NOTE_REST, NOTE_C4, NOTE_E4,
  NOTE_A4, NOTE_B4, NOTE_REST, NOTE_E4, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_REST
};
int noteDurations[] = {
  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9
};

const int AirValue = 570;   //you need to change this value that you had recorded in the air
const int WaterValue = 0;  //you need to change this value that you had recorded in the water
const int BUTTON_PIN = 12;

int intervals = (AirValue - WaterValue)/3;
int soilMoistureValue1 = 0;
int soilMoistureValue2 = 0;
int buttonState = 0;
int waterlevel  = (20 - distance) ;

void setup() {
  pinMode(7,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(3, OUTPUT); //green led
  pinMode(2,OUTPUT); //red led
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  dht.begin();
  lcd.init(); // Print a message to the LCD.
  lcd.print("Program Begin !");
  lcd.setCursor(0,1);
  lcd.print("=W=");
  delay(1000);
  lcd.clear();
  lcd.print("KEIHAU G13 !");
  lcd.setCursor(0,1);
  lcd.print("Home gardening");

}
void serialEvent() {
    while(Serial.available()) {
        dtaUart[dtaLen++] = Serial.read();
    }
}
void loop() {
  int potValue = analogRead(potPin);
  pump_open_duration = map(potValue, 0, 1023, 100, 2000);
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  lcd.print("Watering ");
  lcd.setCursor(0,1);
  lcd.print("duration:");
  lcd.print(pump_open_duration/100);
  lcd.print(" sec");
  delay(1000);
  lcd.clear();
  
  distance = ultrasonic.read();
  float time = micros();
  int soilMoistureValue1 = analogRead(A1);
  buttonState = digitalRead(BUTTON_PIN); //put Sensor insert into soil
  delay(100);
  Serial.print("Distance in CM: ");
  Serial.println(distance);
  if (distance > 19 && distance < 300){
     Serial.println("no water");
    digitalWrite(3,HIGH);
    digitalWrite(2,LOW);
    delay(1000);
    digitalWrite(3,LOW);
    delay(500);
    lcd.print("No Water");
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
      int noteDuration = 1000 / noteDurations[i];
      tone(4, melody[i], noteDuration);
      delay(noteDuration * 1.3);
      noTone(4);
      digitalWrite(3,HIGH);
      delay(100);
      digitalWrite(3,LOW);
      
    }
    delay(1000);
    lcd.clear();
  }else{
    lcd.print("Water level: ");
    lcd.print(int(20-distance));
    lcd.print("cm");
    lcd.setCursor(0, 1);
    lcd.print(":)");
    delay(1000);
    lcd.clear();
    digitalWrite(2,HIGH);
    digitalWrite(3,LOW);
  }
if(soilMoistureValue1 > WaterValue && soilMoistureValue1 < (WaterValue + intervals))
  {
    Serial.println("Sensor1 : Very Wet");
    Serial.print("Data: " );
    Serial.println(soilMoistureValue1);
    lcd.print("Moisture:");
    lcd.print(soilMoistureValue1);
    lcd.setCursor(0,1);
    lcd.print("Very Wet");
    delay(2000);
    lcd.clear();
    lcd.print("Temp:");
    lcd.print(temp);
    lcd.setCursor(0,1);
    lcd.print("Humidity:");
    lcd.print(hum);
    delay(2000);
    lcd.clear();
    digitalWrite(7,LOW);
  }
  else if(soilMoistureValue1 > (WaterValue + intervals) && soilMoistureValue1 < (AirValue - intervals))
  {
    Serial.println("Sensor1 : Wet");
    Serial.print("Data: " );
    Serial.println(soilMoistureValue1);
    lcd.print("Moisture:");
    lcd.print(soilMoistureValue1);
    lcd.setCursor(0,1);
    lcd.print("Condition: Wet");
    delay(2000);
    lcd.clear();
    lcd.print("Temp:");
    lcd.print(temp);
    lcd.setCursor(0,1);
    lcd.print("Humidity:");
    lcd.print(hum);
    delay(2000);
    lcd.clear();
    digitalWrite(7,LOW);
    digitalWrite(6,HIGH);
  }
  else if(soilMoistureValue1 < AirValue && soilMoistureValue1 > (AirValue - intervals)){
    Serial.println("Sensor1: Dry"); 
    Serial.print("Data: " );
    Serial.println(soilMoistureValue1);
    lcd.print("Moisture:");
    lcd.print(soilMoistureValue1);
    lcd.setCursor(0,1);
    lcd.print("Condition: Dry");
    delay(2000);
    lcd.clear();
    lcd.print("Temp:");
    lcd.print(temp);
    lcd.setCursor(0,1);
    lcd.print("Humidity:");
    lcd.print(hum);
    delay(2000);
    lcd.clear();
    
    digitalWrite(7,HIGH);
    delay(pump_open_duration);
    digitalWrite(7,LOW);
    delay(60000);

  }
    if(buttonState == LOW){          //如果按鍵按了
      digitalWrite(7, HIGH);
      delay(pump_open_duration);          //start pumping water 
      digitalWrite(7,LOW);
      }else{                           //如果按鍵是未按下
      digitalWrite(7, LOW);    //no pumping water
      }
  lcd.setRGB(255, 255, 255);

  delay(1000);

}