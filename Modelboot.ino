
#include <EnableInterrupt.h>
#include <AccelStepper.h>
#include <Servo.h>
#include "WiFiEsp.h"
#include <ShiftRegister74HC595.h>
#define HC06 Serial2
unsigned long dur;
ShiftRegister74HC595 <2>sr ( 28,27,29); //dataPin,latchPin,clockPin
 byte a,b,c,d,e,f,g,h,k,j,l,i,m,n,p,r,s;
const int SERIAL_BUFFER_SIZE = 8;
char serialBuffer[SERIAL_BUFFER_SIZE];

boolean newData = false;
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

byte leds = 0;
//----------------servo----------------------
const int servo1 = 24;  // roer servo
int servoVal;   
Servo myservo1;           
const int servo2 = 25; //kraan SB
int servoVal2=105;   
Servo myservo2;
const int servo3 = 31; //kraan BB
int servoVal3=125;   
Servo myservo3;
//--------------------RC KANALEN __________________________________ 
#define SERIAL_PORT_SPEED 57600
#define RC_NUM_CHANNELS  14

#define CH1  0
#define CH2  1
#define CH3  2
#define CH4  3
#define CH5  4
#define CH6  5
#define CH7  6
#define CH8  7
#define CH9  8
#define CH10  9
#define CH11 10
#define CH12  11
#define CH13  12
#define CH14  13



#define CH1_INPUT A9 //  linker joystick Xas         
#define CH2_INPUT A10 //  rechter joystick y as   
#define CH3_INPUT  A11//  linker joystick Yas       
#define CH4_INPUT  A12// ln298 rechter joystick x as  
#define CH5_INPUT A13// lier kraan                
#define CH6_INPUT  10// servo kraan              
#define CH7_INPUT  11//Bluspomp /pin22              
#define CH8_INPUT 13  //      ch 8  pin 6                         
#define CH9_INPUT  12//stappenmotor  kraan         
#define CH10_INPUT  15// relais pin 24,33,34,38      
#define CH11_INPUT  18// relais pin 38,39           
#define CH12_INPUT  19//  relais pin 35,36,37,26     
#define CH13_INPUT  20// relais pin30  /              
#define CH14_INPUT 21// 2xrelais pin34/pin35        

//-----------------------------stappenmotor--------------------------------
#define FULLSTEP 4 
#define HALFSTEP 8
#define motorPin1 A0      
#define motorPin2  A1    
#define motorPin3  A2   
#define motorPin4  A3     
#define motorPin5 A4    
#define motorPin6  A5     
#define motorPin7  A6    
#define motorPin8  A7 
#define MotorInterfaceType 8
AccelStepper stepper(FULLSTEP , motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(FULLSTEP , motorPin5, motorPin7, motorPin6, motorPin8);

//------------------------Hoofdmotoren en boeg hekschroef-----------------------
int MotorSpeed1= 0;//hoofdmoterA
int MotorSpeed2 = 0;//hoofdmotorB
int MotorSpeed3= 0;//hkopschroef
int MotorSpeed4 = 0;//hekschroef
// Relay
int Buttoncounter=0;
unsigned long millis_relays = 0;
//--------------------------wifi bluetooth-----------
int   Blt = atol(receivedChars);

//------------------failsave-------------------------------

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];
volatile word ch10_count =0;
unsigned long previousMillis = 0;   
const long interval = 25;
int RC_teller;
int count=0;
 unsigned long lastMillis=millis();
unsigned long time_now = 0;
 
void rc_read_values() {
noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
 rc_start[0] = micros();
    rc_start[1] = micros();
    rc_start[2] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
}}

void calc_ch1() { calc_input(CH1, CH1_INPUT); }
void calc_ch2() { calc_input(CH2, CH2_INPUT); }
void calc_ch3() { calc_input(CH3, CH3_INPUT); }
void calc_ch4() { calc_input(CH4, CH4_INPUT); }
void calc_ch5() { calc_input(CH5, CH5_INPUT); } 
void calc_ch6() { calc_input(CH6, CH6_INPUT); }
void calc_ch7() { calc_input(CH7, CH7_INPUT); }
void calc_ch8()  {  calc_input(CH8, CH8_INPUT);}
void calc_ch9() { calc_input(CH9, CH9_INPUT); }
void calc_ch10() {{ calc_input(CH10, CH10_INPUT); }if ( ch10_count < 10000 )ch10_count++;}
void calc_ch11() { calc_input(CH11, CH11_INPUT); }
void calc_ch12() { calc_input(CH12, CH12_INPUT); }
void calc_ch13() { calc_input(CH13, CH13_INPUT); }
void calc_ch14() { calc_input(CH14, CH14_INPUT); }


void setup() {
    sr.setAllLow();
myservo1.attach(servo1);  // attaches the servo


int a=0;
 for(int i=22;i<54;i++){
 pinMode(i, OUTPUT);//relay pins
 digitalWrite(i, LOW);
}
pinMode(31,INPUT);//relay pins
  stepper.setMaxSpeed(500.0);
  stepper.setAcceleration(50.0);
  stepper2.setMaxSpeed(500.0);
  stepper2.setAcceleration(50.0);
Serial.begin(9600);
Serial3.begin(115200);
HC06.begin(9600);
  pinMode(CH1_INPUT, INPUT);
  pinMode(CH2_INPUT, INPUT);
  pinMode(CH3_INPUT, INPUT);
  pinMode(CH4_INPUT, INPUT);
  pinMode(CH5_INPUT, INPUT_PULLUP);
  pinMode(CH6_INPUT, INPUT);
  pinMode(CH7_INPUT, INPUT);
  pinMode(CH8_INPUT, INPUT_PULLUP);
  pinMode(CH9_INPUT, INPUT);
  pinMode(CH10_INPUT, INPUT_PULLUP);
  pinMode(CH11_INPUT, INPUT);
  pinMode(CH12_INPUT, INPUT);
  pinMode(CH13_INPUT, INPUT);
  pinMode(CH14_INPUT, INPUT);

  enableInterrupt(CH1_INPUT, calc_ch1, CHANGE);
  enableInterrupt(CH2_INPUT, calc_ch2, CHANGE);
  enableInterrupt(CH3_INPUT, calc_ch3, CHANGE);
  enableInterrupt(CH4_INPUT, calc_ch4, CHANGE);
  enableInterrupt(CH5_INPUT, calc_ch5, CHANGE);
  enableInterrupt(CH6_INPUT, calc_ch6, CHANGE);
  enableInterrupt(CH7_INPUT, calc_ch7, CHANGE);
  enableInterrupt(CH8_INPUT, calc_ch8, CHANGE);
  enableInterrupt(CH9_INPUT, calc_ch9, CHANGE);
  enableInterrupt(CH10_INPUT, calc_ch10, CHANGE);
  enableInterrupt(CH11_INPUT, calc_ch11, CHANGE);
  enableInterrupt(CH12_INPUT, calc_ch12, CHANGE);
  enableInterrupt(CH13_INPUT, calc_ch13, CHANGE);
  enableInterrupt(CH14_INPUT, calc_ch14, CHANGE);
  }

void loop() {

//-------Failsafe-------------------------------------------
 unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= interval) {previousMillis = currentMillis;}
    noInterrupts();
  RC_teller = ch10_count;
  ch10_count = 0;
    interrupts(); 
 //----------Timer relais----------------------------- 
if (currentMillis - lastMillis >500){
count=0;}
 //-------------RC Beturing-------------------------- 
 if (RC_teller>0){ 
 rc_read_values();
Serial.print(" RC_teller"); Serial.print( RC_teller);     Serial.print ("MotorSpeed1");Serial.println ( MotorSpeed1);

if ((rc_values[CH5]>1200)&&(rc_values[CH5]<1750)){rc_values[CH5]=1500;}
if ((rc_values[CH5]>500)&&(rc_values[CH5]<1200)){rc_values[CH5]=1000;}
if ((rc_values[CH5]>1750)){rc_values[CH5]=2000;}
if ((rc_values[CH6]>1200)&&(rc_values[CH6]<1750)){rc_values[CH6]=1500;}
if ((rc_values[CH6]>500)&&(rc_values[CH6]<1200)){rc_values[CH6]=1000;}
if ((rc_values[CH6]>1750)){rc_values[CH6]=2000;}
if ((rc_values[CH7]>1200)&&(rc_values[CH7]<1750)){rc_values[CH7]=1500;}
if ((rc_values[CH7]>500)&&(rc_values[CH7]<1200)){rc_values[CH7]=1000;}
if ((rc_values[CH7]>1750)){rc_values[CH7]=2000;}
if ((rc_values[CH8]>1200)&&(rc_values[CH8]<1750)){rc_values[CH8]=1500;}
if ((rc_values[CH8]>500)&&(rc_values[CH8]<1200)){rc_values[CH8]=1000;}
if ((rc_values[CH8]>1750)){rc_values[CH8]=2000;}
if ((rc_values[CH9]>1200)&&(rc_values[CH9]<1750)){rc_values[CH9]=1500;}
if ((rc_values[CH9]>500)&&(rc_values[CH9]<1200)){rc_values[CH9]=1000;}
if ((rc_values[CH9]>1750)){rc_values[CH9]=2000;}
if ((rc_values[CH10]>1200)&&(rc_values[CH10]<1750)){rc_values[CH10]=1500;}
if ((rc_values[CH10]>500)&&(rc_values[CH10]<1200)){rc_values[CH10]=1000;}
if ((rc_values[CH10]>1750)){rc_values[CH10]=2000;}
if ((rc_values[CH11]>1200)&&(rc_values[CH11]<1750)){rc_values[CH11]=1500;}
if ((rc_values[CH11]>500)&&(rc_values[CH11]<1200)){rc_values[CH11]=1000;}
if ((rc_values[CH11]>1750)){rc_values[CH11]=2000;}
if ((rc_values[CH12]>1200)&&(rc_values[CH12]<1750)){rc_values[CH12]=1500;}
if ((rc_values[CH12]>500)&&(rc_values[CH12]<1200)){rc_values[CH12]=1000;}
if ((rc_values[CH12]>1750)){rc_values[CH12]=2000;}
if ((rc_values[CH13]>1200)&&(rc_values[CH13]<1750)){rc_values[CH13]=1500;}
if ((rc_values[CH13]>500)&&(rc_values[CH13]<1200)){rc_values[CH13]=1000;}
if ((rc_values[CH13]>1750)){rc_values[CH13]=2000;}
if ((rc_values[CH14]>1200)&&(rc_values[CH14]<1750)){rc_values[CH14]=1500;}
if ((rc_values[CH14]>500)&&(rc_values[CH14]<1200)){rc_values[CH14]=1000;}
if ((rc_values[CH14]>1750)){rc_values[CH14]=2000;}

 
if ((rc_values[CH2]>1450)&&(rc_values[CH2]<1550)){digitalWrite(52, LOW);digitalWrite(50, LOW); digitalWrite(51, LOW);digitalWrite(53, LOW);MotorSpeed1 = 0;  analogWrite(3, MotorSpeed1); MotorSpeed2 = 0;  analogWrite(2, MotorSpeed2);}//HOOFDMOTOREN STOP
if ((rc_values[CH3]>1450)&&(rc_values[CH3]<1550)){digitalWrite(46, LOW);digitalWrite(47, LOW); digitalWrite(48, LOW);digitalWrite(49, LOW);MotorSpeed3 =0;  analogWrite(4, MotorSpeed3); MotorSpeed4 = 0;  analogWrite(5, MotorSpeed4);} //HEK- KOPSCHROEF STOP
if ((rc_values[CH4]>1400 && rc_values[CH4]<1500)){  servoVal = 90; myservo1.write(servoVal);}
if (rc_values[CH5]==1500){digitalWrite(40,LOW);digitalWrite(41, LOW);digitalWrite(42,LOW);digitalWrite(43, LOW);digitalWrite(44,LOW);digitalWrite(45, LOW); }//ankerlier stop kraan lieren
if (rc_values[CH6]==1500){digitalWrite(38,LOW);digitalWrite(39, LOW);Blt=0;myservo2.detach();myservo3.detach(); }//ankerlier stop
if (rc_values[CH7]==1500){sr.set(9,LOW);sr.set(10,LOW);digitalWrite (22,LOW); }//sleeplier stop
if (rc_values[CH9]==1500){stepper.stop();stepper2.stop();} // kraan BB stop // kraan sbstop
if ((rc_values[CH4]>1470)&&(rc_values[CH4]<1525)){myservo1.attach(servo1); myservo1.write(90);}//stuur servo
if ((servoVal >85)&& (servoVal<90)){myservo1.detach();}


if ((rc_values[CH2]>1525) ||(rc_values[CH2]>500)&&(rc_values[CH2]<1470)){motor(i);}
if ((rc_values[CH3]>1525) ||(rc_values[CH3]>500)&&(rc_values[CH3]<1470)){motor(i);}
if ((rc_values[CH4]>1525) ||(rc_values[CH4]>500)&&(rc_values[CH4]<1470)){motor(i);}
if ((rc_values[CH1]>1525) ||(rc_values[CH1]>500)&&(rc_values[CH1]<1470)){motor(i);}

//---------RELAIS-----------

 if (count ==0) {
   lastMillis=millis();

if (rc_values[CH13]==2000){if(a==0){Blt=1; }else {Blt=2;}}   //dekverl 1 pulse pin4
if (rc_values[CH13]==1000){if(b==0){Blt=3;}else {Blt=4;}}     //dekverl 2
if ((rc_values[CH14]==2000)&&(count==0)) {if(Buttoncounter==0){Blt=5;}else if (Buttoncounter==1) {Blt=23;}else if(Buttoncounter==2){Blt=6;}}
if (rc_values[CH14]==1000){if(c==0){Blt=7;}else{Blt=8;}}          //Schijnwerpers
if (rc_values[CH12]==2000){if(d==0){ Blt=9;}else{Blt=10;}}       //ROOD GROEN toplicht heklicht
if (rc_values[CH12]==1000){if(e==0){ Blt=13;}else { Blt=14;}}   //sleeplichten ROOD GROEN
if (rc_values[CH11]==2000){if (f==0){ Blt=19;}else{Blt=20;}}      //RWR lichten
if (rc_values[CH11]==1000){if (g==0){Blt=21;}else {Blt=22;}}    //ankerlichten
if (rc_values[CH10]==2000){if(h==0){Blt=17; }else {Blt=18;}}       //  Hoorn
if (rc_values[CH10]==1000){if(j==0){Blt=11;}else {Blt=12;}}//Radar
//if (rc_values[CH7]==1500){digitalWrite (22,LOW);}                          //  BlusPomp uit

}
                   
//--------LIEREN---------------
if (rc_values[CH8]==1000){
if (rc_values[CH5]==2000){Blt=77;}  //ankerlier SB NEER
if (rc_values[CH5]==1000){Blt=75;} //ankerlier SB OP
if (rc_values[CH6]==2000){Blt=81;} //ankerlieren BB OP
if (rc_values[CH6]==1000){Blt=79;} //ankerlieren BB NEER
if (rc_values[CH7]==2000){Blt=16;}     //  BlusPomp aan
//if (rc_values[CH9]==2000){Blt=6000;} 
//if (rc_values[CH9]==1000){Blt=6000;}
//---------------STAPPENMOTOR---------------------------

}else if (rc_values[CH8]==2000){
if (rc_values[CH5]==2000 ){Blt=61;}                      //lier kraan SB op   
if (rc_values[CH5]==1000){Blt=59;} //lier kraan SB neer
if (rc_values[CH6]==2000){Blt=52;}                         //kraan Sb neer
if (rc_values[CH6]==1000){Blt=50;}    //kraan Sb op
if (rc_values[CH7]==2000){Blt=83;}  //SLEEPLIER OP
if (rc_values[CH7]==1000){Blt=85;} //SLEEPLIER NEER
if (rc_values[CH9]==2000){Blt=56;} //kraan SB rechtsomdraaien
if (rc_values[CH9]==1000){Blt=55;}//kraan SB linksomdraaien

}else if(rc_values[CH8]==1500){
  if (rc_values[CH7]==2000){Blt=83;}  //SLEEPLIER OP
if (rc_values[CH7]==1000){Blt=85;} //SLEEPLIER NEER
if (rc_values[CH5]==2000){Blt=71;}   //lier kraan BB op   
if (rc_values[CH5]==1000){Blt=73;}    //lier kraan BB neer
if (rc_values[CH6]==2000){Blt=63;}   //kraan BB neer
if (rc_values[CH6]==1000){Blt=65;}    //kraan BB op
if (rc_values[CH9]==2000){Blt=67;}   //kraan BB rechtsom draaien
if (rc_values[CH9]==1000){Blt=69;}   //kraan BB linksom draaien
}

}


if (RC_teller ==0){
  
  //------------WIFI-----------------------------I
    static byte ndx = 0;
   long now = millis();
        char rc;
        char endMarker = '>'; 
  static long lastReceived= millis();
/*
while (Serial3.available() && newData == false) {
     rc = Serial3.read();
     
        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) { ndx = numChars - 1; }   
        } else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
               lastReceived = now;
       Blt = atol(receivedChars); } 
} 
   */ 
//------------BLUETOOTH-----------------------------I
 while( (HC06.available()>0) &&( newData == false)) {
        rc = HC06.read();
       
        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {  ndx = numChars - 1; }  
        } else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
               lastReceived = now;
       Blt = atol(receivedChars);   
    }          
  }
  if (now-lastReceived >2500){ failsave();}
}

Serial.println(Blt);



for(int i=1;i<24;i++)
if(Blt ==i){relais(i);}
 for(int i=100;i<130;i++)
if(Blt ==i){motor(i);}
  for(int i=1000;i<6000;i++)
if(Blt ==i){motor(i);}
 for(int i=50;i<75;i++)
if (Blt==i){stappenmotor(i);}
 for(int i=75;i<91;i++)
if(Blt ==i){lieren(i);}


if (RC_teller ==0 && Blt==0 ){failsave();}


newData = false;
}

//-----------------------------------------------------------------------------------------
void motor(int Blt){
//Serial.print("Motor");
//------------Hoofd motoren ----------------------------------------------------------------
 //if (rc_values[CH4]>500){Blt=0;}
 
 //in1=52 in2=50 in3=51 in4=53
   if (RC_teller >0){ 
if (rc_values[CH8] ==2000){
      if ((rc_values[CH2]>800)&&(rc_values[CH2] < 1400)){digitalWrite(50, HIGH);digitalWrite(51, LOW); digitalWrite(52, LOW); digitalWrite(53, HIGH);}   //  A achteruit B achteruit
     if (rc_values[CH2] >1550){digitalWrite(50, LOW);digitalWrite(51, HIGH);digitalWrite(52, HIGH);digitalWrite(53, LOW); }                          //A vooruit B vooruit
       if ((rc_values[CH4]>800)&&(rc_values[CH4] < 1400)){digitalWrite(50, HIGH);   digitalWrite(51, LOW);digitalWrite(52, LOW);digitalWrite(53, LOW);}  //A achteruit B stop
      if (rc_values[CH4] > 1550 ){digitalWrite(50, LOW);digitalWrite(51, LOW);digitalWrite(52, LOW);digitalWrite(53, HIGH);}                         // A stop B achteruit
      if ((rc_values[CH4] < 1400)&& (rc_values[CH2]<1400)){digitalWrite(50, HIGH);    digitalWrite(51, LOW);digitalWrite(52, LOW);digitalWrite(53, LOW);}  // A achteruit B stop  
      if ((rc_values[CH4] < 1400)&& (rc_values[CH2]>1550)){digitalWrite(50, LOW);    digitalWrite(51, LOW);digitalWrite(52, HIGH);digitalWrite(53, LOW);}   // A vooruit B stop 
      if ((rc_values[CH4] > 1550)&& (rc_values[CH2]>1500)){digitalWrite(50, LOW);digitalWrite(51, HIGH);digitalWrite(52, LOW);digitalWrite(53, LOW); }      // A stop B vooruit  
       if ((rc_values[CH4] > 1550)&& (rc_values[CH2]<1400)){digitalWrite(50, LOW); digitalWrite(51, LOW);digitalWrite(52, LOW);digitalWrite(53, HIGH); }      // A stop B achteruit


}else if (rc_values[CH8]==1500){
      if (rc_values[CH2] < 1400){digitalWrite(50, HIGH);digitalWrite(51, LOW);digitalWrite(52, LOW);digitalWrite(53, HIGH);}                            // A achteruit B achteruit
      if (rc_values[CH2] > 1550){digitalWrite(50, LOW);  digitalWrite(51, HIGH);digitalWrite(52, HIGH);digitalWrite(53, LOW);  }                       // A vooruit B vooruit
       
}else if (rc_values[CH8]==1000){
     if  (rc_values[CH2]>1550){digitalWrite(50, HIGH);digitalWrite(51, HIGH);digitalWrite(52, LOW);digitalWrite(53, LOW); }                         //A achteruit B vooruit 
     if  (rc_values[CH2]<1400){digitalWrite(50, LOW);  digitalWrite(51, LOW);digitalWrite(52, HIGH);digitalWrite(53, HIGH); }                         //A vooruit B achteruit
   }

if ((rc_values[CH2]>800)&&(rc_values[CH2] < 1450)){ 
        MotorSpeed1 = map(rc_values[CH2], 1400, 1020, 0, 255); 
         MotorSpeed2 = MotorSpeed1;}
 else if ((rc_values[CH2]> 1500)&&(rc_values[CH2]<1950)){
         MotorSpeed1 = map(rc_values[CH2],1480, 1950, 0, 255); MotorSpeed2 = MotorSpeed1;}
       
    if (rc_values[CH2]> 1950){
      MotorSpeed1 =255; MotorSpeed2 = MotorSpeed1;}

 
}
       
if (RC_teller ==0){ 
Serial.print("test");
      if ( Blt == 105){digitalWrite(52, LOW);digitalWrite(50, HIGH);}                // A achteruit
      if ( Blt == 107){digitalWrite(51, LOW);digitalWrite(53, HIGH);}                // B achteruit
      if ( Blt == 100){digitalWrite(52, HIGH);digitalWrite(50, LOW);}                //A vooruit
      if ( Blt == 103){digitalWrite(51, HIGH);digitalWrite(53, LOW); }               // B vooruit
      if ( Blt == 101){digitalWrite(52, LOW); digitalWrite(50, LOW);MotorSpeed1=0;}  //A stop
      if ( Blt == 104){digitalWrite(51, LOW);digitalWrite(53, LOW);MotorSpeed2=0;}   // B stop
      if( Blt == 102 || Blt==106 || Blt== 114){digitalWrite(52, LOW);digitalWrite(50, LOW);MotorSpeed1=0;} //A stop
      if ( Blt == 104 || Blt==108 || Blt==113){digitalWrite(51, LOW);digitalWrite(53, LOW);MotorSpeed2=0;} // B stop
int  a;
 
  if ((Blt >= 1000)&& (Blt < 1255)) {  // slider motor A
       MotorSpeed1= map (Blt,1000,1255,60,255);  
 }else if (( Blt>= 2000) && (Blt< 2255 )){
       MotorSpeed2 = map (Blt,2000,2255,60,255); //slider motor B
 }else if (( Blt>= 3001)&& (Blt< 3255 )){
   MotorSpeed1 = map (Blt,3000,3255,40,255);  //slider beide motoren
 MotorSpeed2 = MotorSpeed1;
       }
}  
  if (MotorSpeed1 < 30)MotorSpeed1 = 0;    // Adjust to prevent "buzzing" at very low speed
  if (MotorSpeed2 < 30)MotorSpeed2 = 0;

  analogWrite(3, MotorSpeed1); // Set the motor speeds
  analogWrite(2, MotorSpeed2);

////kop hekschroef      //pin  46=in1 pin47=in2 pin48=in4 pin49=in3 pin4=enA pin5=enB

if (RC_teller >0) { 

//if ((rc_values[CH3]<1450)&&(rc_values[CH1]>1450)&&(rc_values[CH1]<1600)) {digitalWrite(46, LOW); digitalWrite(47, HIGH);digitalWrite(48,HIGH); digitalWrite(49,LOW);} //HEKSCHROEF KOPSCHROEF LIKSOM
//if ((rc_values[CH3]>1520)&&(rc_values[CH1]>1450)&&(rc_values[CH1]<1600)){digitalWrite(46,HIGH); digitalWrite(47,LOW);digitalWrite(48, LOW); digitalWrite(49, HIGH);}  //HEKSCHROEF KOPSCHROEF RECHTSOM
if ((rc_values[CH3]<1450)&&(rc_values[CH1]>1600)) {digitalWrite(46,LOW); digitalWrite(47,LOW);digitalWrite(48, LOW); digitalWrite(49,HIGH);}    // HEKSCHROEF
if ((rc_values[CH3]<1450)&&(rc_values[CH1]<1450)) {digitalWrite(46,LOW ); digitalWrite(47,LOW);digitalWrite(48,HIGH ); digitalWrite(49, LOW);}  // HEKSCHROEF
if ((rc_values[CH3]>1520)&&(rc_values[CH1]>1600)){digitalWrite(46, HIGH); digitalWrite(47, LOW);digitalWrite(48, LOW); digitalWrite(49, LOW);}   // KOPSCHROEF LInKSOM
if ((rc_values[CH3]>1520)&&(rc_values[CH1]<1450)) {digitalWrite(46, LOW); digitalWrite(47, HIGH);digitalWrite(48,LOW); digitalWrite(49, LOW);}  // KOPSCHROEF RECHTSOM

if ((rc_values[CH3]>800)&&(rc_values[CH3] < 1450)){ 
        MotorSpeed3= map(rc_values[CH3], 1450, 1020, 0, 255); 
         MotorSpeed4 = MotorSpeed3;
 }else if (rc_values[CH3]> 1520){
       MotorSpeed3 = map(rc_values[CH3],1500, 1960, 0, 255); 
       MotorSpeed4 = MotorSpeed3;
 }
}
if (RC_teller ==0){
 
if ((Blt==118)||(Blt==116)||(Blt==119)){digitalWrite(46, LOW); digitalWrite(47, LOW);}
if ((Blt==124)||(Blt==121)||(Blt==122)){digitalWrite(48, LOW); digitalWrite(49, LOW);}
if (Blt==117){digitalWrite(46, HIGH); digitalWrite(47,LOW);}
if (Blt==115){digitalWrite(46, LOW); digitalWrite(47, HIGH);}
if (Blt==120){digitalWrite(48, HIGH); digitalWrite(49, LOW);}
if (Blt==123){digitalWrite(48, LOW); digitalWrite(49, HIGH);}
   if ((Blt > 4000)&& (Blt < 4255)) {
       MotorSpeed3= map (Blt,4000,4255,80,255);
 }else if (( Blt>5000) && (Blt< 5255 )){
       MotorSpeed4 = map (Blt,5000,5255,80,255);
      }
}
  if (MotorSpeed3< 30)MotorSpeed3 = 0;    // Adjust to prevent "buzzing" at very low speed
  if (MotorSpeed4 < 30)MotorSpeed4 = 0;
     analogWrite(4, MotorSpeed3); // Set the motor speeds
     analogWrite(5, MotorSpeed4);

//------------ servo roer-------------------------------------------------------------------

 myservo1.attach(servo1);  // attaches the servo
if (RC_teller>0){
if (rc_values[CH4]>1490){//sturen bij syncroon draaien

   servoVal= map(rc_values[CH4], 1485, 1915,90,0);  // sturen met beide motoren knop middenstand

}else if (rc_values[CH4]<1450){
   servoVal= map(rc_values[CH4], 1480,1020, 90, 180);  
}else if (rc_values[CH4]>1400 && rc_values[CH4]<1500){
  servoVal = 90;
}
}
if (RC_teller ==0){
if (Blt ==109){servoVal =178;}
if (Blt ==110 ||  Blt ==112){servoVal =90;}
if (Blt ==111){servoVal =0;}
}
//if ((rc_values[CH4]>1525) ||(rc_values[CH4]>500)&&(rc_values[CH4]<1470)){myservo1.attach(servo1); }
 myservo1.write(servoVal);
 delay(5);
 
 }
 //--------------------------------------------------------------------------------------------------------------------------------
 void relais(int Blt){
 unsigned long millis_relays = millis();
   if (millis_relays - previousMillis >= 20) {previousMillis = millis_relays;

      switch(Blt) {
         case 1  :sr.set(7,HIGH);a=1;count++; break;              //dekverl 1 pulse pin4
         case 2  :sr.set(7,LOW);a=0;count++; break;
         case 3  :sr.set(6, HIGH);b=1;count++;  break;                  //dekverl 2
         case 4  :sr.set(6, LOW);b=0;count++;  break;
         case 5  :sr.set(0, HIGH);sr.set(1,LOW); Buttoncounter=1;count++ ;break;    //Stuurhutlicht
         case 23 :sr.set(1,HIGH);sr.set(0,LOW); Buttoncounter=2 ;count++; break;
         case 6  :sr.set(0, LOW);sr.set(1,LOW); Buttoncounter=0;count++;break;
         case 7  :sr.set(5, HIGH);c=1;count++;break;                     // Schijnwerper
         case 8  :sr.set(5,LOW);c=0;count++;break;
         case 9  :sr.set(2, HIGH); sr.set(4,HIGH); sr.set(3,LOW);d=1;count++;break;  //ROOD GROEN toplicht heklicht
         case 10 :sr.set(2,LOW);sr.set(4,LOW);d=0;count++; break;
         case 11 :sr.set(12,HIGH);j=1;count++ ;break;                           //Radar
         case 12 :sr.set(12,LOW);j=0;count++ ;break;
         case 13 :sr.set(2, HIGH);sr.set(4,LOW); sr.set(3,HIGH);e=1;count++;break;//sleeplichten ROOD GROEN
         case 14 :sr.set(2,LOW); sr.set(3,LOW);e=0;count++;break;
         case 15 : digitalWrite (22,LOW);k=1;count++;break;                 //  BlusPomp
         case 16 : digitalWrite (22,HIGH);k=0;count++;break;
         case 17 :sr.set(15,HIGH);count++ ;h=1;count++; break;                      //  Hoorn
         case 18 :sr.set(15,LOW);count++ ;h=0;count++;break;
         case 19 :sr.set(14,HIGH);f=1;count++;break;                    //RWR lichten
         case 20 :sr.set(14,LOW);f=0;count++;break;
         case 21 :sr.set(13,HIGH);g=1;count++;break;                  //ankerlichten
         case 22 :sr.set(13,LOW);g=0;count++;break;
      }
  } 
}
//---------------------------------------------------------------------------------------------------------------------------
    void stappenmotor(int Blt){ 
int period = 150;

   //IN1=43 IN2=42 IN3=41 IN4=40 LIEREN KRAAN BB/SB
  ///IN1=A0 IN2=A1 IN3=A2 IN4=A3  STAPPENMOTOR SB 
  /// IN1=A4 IN2=A5 IN3=A7 IN4=A6 STAPPENMOTOR BB

if (Blt==61)  {digitalWrite(44,HIGH);digitalWrite(45, LOW);}                                         //lier kraan SB op    
if (Blt==59)   {digitalWrite(44,LOW);digitalWrite(45, HIGH);}                                          //lier kraan SB neer
if ((Blt==60)||(Blt==62))  {digitalWrite(42,LOW);digitalWrite(43, LOW);digitalWrite(44,LOW);digitalWrite(45, LOW);}  //lier kraan SB stop
if  (( Blt==56)&&(stepper.currentPosition()<2096)){stepper.setSpeed(500); stepper.runSpeed();} // SB kraan rechtsom draaien
if ( (Blt==55 )&&(stepper.currentPosition()>0)){   stepper.setSpeed(-500); stepper.runSpeed();} // SB kraan linkom draaien
if  (( Blt==54)||(Blt== 57)){stepper.stop();} // kraan sbstop

if(millis() >= time_now + period){
    time_now += period;
if ((Blt==52 )&& (servoVal2<105))  { myservo2.attach(servo2);servoVal2=servoVal2+1; myservo2.write(servoVal2);}    //kraan Sb op
if ((Blt==50 )&& (servoVal2>78))   { myservo2.attach(servo2);servoVal2=servoVal2-1; myservo2.write(servoVal2);}     //kraan SB neer
if ((Blt==63 )&& (servoVal3<125))  { myservo3.attach(servo3);servoVal3=servoVal3+1; myservo3.write(servoVal3);}    //kraan BB op
if ((Blt==51)||(Blt==53)||(Blt==64)||(Blt==66)){myservo2.detach();myservo3.detach();}
if ((Blt==65 )&& (servoVal3>80))   { myservo3.attach(servo3);servoVal3=servoVal3-1; myservo3.write(servoVal3);}     //kraan BB neer
}
if (Blt==71)  {digitalWrite(42, HIGH);digitalWrite(43, LOW);}                                         //lier kraan BB op
if (Blt==73)   {digitalWrite(42, LOW);digitalWrite(43, HIGH);}                                         //lier kraan BB neer
if ((Blt==72)||(Blt==74))  {digitalWrite(40,LOW);digitalWrite(41, LOW);digitalWrite(42, LOW);digitalWrite(43, LOW);} //lier kraan BB stop
 
if  (( Blt==67)&&(stepper2.currentPosition()>-2096)){stepper2.setSpeed(-500); stepper2.runSpeed();} // BB kraan rechtsom draaien
if ((Blt==69 )&&(stepper2.currentPosition()<0)){   stepper2.setSpeed(500); stepper2.runSpeed();} // BB kraan linkom draaien
if  (( Blt==68)||(Blt== 70)){stepper2.stop();} // kraan BB stop
//Serial.print("servoVal2=");Serial.println( servoVal2);
// Serial.println(stepper2.currentPosition());
 
}
//-------------------------------------------------------------------------------------------------------------------
void lieren(int Blt){
        
// IN1=40 IN2=41IN3=38 IN4=39  ANKERLIEREN
 switch(Blt) {
         case 75 : digitalWrite(40,HIGH);digitalWrite(41, LOW); break;  //Ankerlier  SB op 
         case 76 : digitalWrite(40,LOW);digitalWrite(41, LOW); break;     
         case 77 : digitalWrite(40,LOW);digitalWrite(41, HIGH);  break;   //Ankerlier SBneer           
         case 78 : digitalWrite(40,LOW);digitalWrite(41, LOW);  break;
         case 79 : digitalWrite(38,LOW);digitalWrite(39, HIGH);break;    //Ankerlier BB op
         case 80 : digitalWrite(38,LOW);digitalWrite(39, LOW); break;
         case 81 : digitalWrite(38,HIGH);digitalWrite(39, LOW);break;         //Ankerlier  BB neer;
         case 82 : digitalWrite(38,LOW);digitalWrite(39, LOW);;break;                    
         case 83 : sr.set(9,LOW); sr.set(10,HIGH);break; // SLEEPLier OP
         case 84 : sr.set(9,LOW);sr.set(10,LOW);break;  
         case 85 : sr.set(9,HIGH); sr.set(10,LOW); break;  // SLEEPLier NEER
         case 86 : sr.set(9,LOW);sr.set(10,LOW);break; 
  }
  }
 //--------------------------------------------------------  
    void failsave(){
Serial.println("Failsafe");

Blt=0;
sr.setAllLow();
 for(int i=22;i<54;i++){ digitalWrite(i, LOW);}

MotorSpeed1=0;
MotorSpeed2=0;
MotorSpeed3=0;
MotorSpeed4=0;
  analogWrite(2, MotorSpeed1); // Set the motor speeds
  analogWrite(3, MotorSpeed2);
  analogWrite(4, MotorSpeed3); // Set the motor speeds
  analogWrite(5, MotorSpeed4);


 myservo1.detach();  // sturen
 myservo2.detach();  // kraan sb
 myservo3.detach();  //kraan bb
stepper.stop(); // kraan sb
stepper2.stop(); //kraan bb
 
}
