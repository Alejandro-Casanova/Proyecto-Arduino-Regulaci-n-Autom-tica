int gatepin=9;
int tacpin=2;
int gate=0;           //Valor de tensión inicial en la puerta(PWM)
int wref=1500;        //Vel. angular de referencia
int fact=25;          //Factor de reducción de la señal de error
int sampletime=250;   //Tiempo de muestreo del tacómetro

//Variables del tacómetro
volatile float rev = 0;
volatile int rpm;
int time;
int oldtime=0;

//Variables del display
#include <Wire.h>
#include <LiquidCrystal_I2C.h> 

const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
const int i2c_addr = 0x27;
 
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

void setup() {
  Serial.begin(9600);
  pinMode(tacpin, INPUT); 
  pinMode(gatepin, OUTPUT); 
  lcd.begin(20,4);
  attachInterrupt(0,count,RISING); //Rutina de Interrupción del tacómetro
}

void count() //Función de interrupción
{
rev++; //Aumenta el contador de rpm por cada parpadeo del led
}

void display(float pwr, int spd){ //Función de control del display lcd
                                 
  //Muestra la potencia del vent. en la fila 1
  lcd.setCursor(0,0);
  lcd.print("Fan Power:");
  lcd.setCursor(13,0);
  lcd.print(pwr, 0);
  lcd.print("%  ");

  //Muestra la vel. del vent. en la fila 2
  lcd.setCursor(0,1);
  lcd.print("Fan Speed:");
  lcd.setCursor(11,1);
  lcd.print(spd);
  lcd.print(" rpm    ");

}

void loop() {
  
  attachInterrupt(0,count,RISING);     //Empieza a contar parpadeos
  delay(sampletime);                   //Espera
  detachInterrupt(0);                  //Para de contar
  time=millis()-oldtime;               //Tiempo pasado contando            
  rpm=(rev/time)*60000/7;              //Calcula rpm (el ventilador tiene 7 aspas)
  
  if(rpm<wref-25) gate=gate+((wref-rpm)/fact+1);
  if(rpm>wref+25) gate=gate+((wref-rpm)/fact-1);
  if(gate>255) gate=255;        //Evita que la variable supere su valor máximo
  
  display(gate/2.55, rpm);      //Muestra los dos parámetros del vent. en el display
  analogWrite(gatepin, gate);   //Actualiza la tensión (PWM) en la puerta
  oldtime=millis();             //Actualiza el cronómetro             
  rev=0;                        //Reinicia el contador
  Serial.println(rpm);

}
