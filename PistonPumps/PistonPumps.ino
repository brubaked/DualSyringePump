int StepPinA = 2;
int DirPinA = 3;
int DirA; 
int StepPinB = 6;
int DirPinB = 7;
int DirB;
int Pause = 10;
int Stage1 = 1;
int Reverse = 0;
int Reset = 11;
int Flip = 0;
float PumpFrac = 0.9;
unsigned long Time;
unsigned long TimeA = 0;
unsigned long TimeB = 0;
unsigned long TimeC = 0;
unsigned long TimeInitial;
float DelayTime;
float DelayTimeA;
float DelayTimeB;
float SyrArea = 0;
float SyringeDiameter = 0;
float StepsA = 0;
float StepsB = 0;
float TotalSteps = 0;
float SyringeVolume = 0;
float Rate = 0;

// Use of floats vs. integers is necessary to allow decimal calculation and large enough register to avoid integer overflow
// the setup routine runs once when you press reset:

void setup() {
  pinMode(StepPinA, OUTPUT);
  pinMode(DirPinA, OUTPUT);
  pinMode(StepPinB, OUTPUT);
  pinMode(DirPinB, OUTPUT);
  pinMode(Pause, INPUT_PULLUP);
  pinMode(Reset, INPUT_PULLUP);
  
  Serial.begin(250000); // Set up serial for debugging
  while (! Serial);
  
  SyringeDiameter = 1.59; // inner diameter of syringe in cm
  SyringeVolume = 2; // volume of the syringe in ml (cm^3)
  Rate = 5; // dispensing rate in ml/min
  
  SyrArea = (SyringeDiameter / 2) * (SyringeDiameter / 2) * 3.1459;
  DelayTime = (SyrArea * 9.454 / Rate - 0.150);
  TotalSteps = (SyringeVolume * 0.8) / (SyrArea * 0.0001576);
  
  }

  // function to tell X stepper motor to take a step
  int TakeStep(int DirPinX, int &DirX, int StepPinX, float &StepsX, float DelayTimeX, unsigned long &TimeX){ 
    if ((Time - TimeX) >= DelayTimeX && TotalSteps > StepsX){
         digitalWrite(DirPinX, DirX);
         delay(0.05); // delay for the stepper controller to register
         digitalWrite(StepPinX, HIGH);
         delay(0.100); // delay for the stepper controller to register
         digitalWrite(StepPinX, LOW);
         StepsX = StepsX + 1;
         TimeX = millis();
    }
    
  }


// calculating the delay between steps for a given flow rate / syringe diameter
  float DelayTimes(){
    if (DirA == 0){
      DelayTimeA = DelayTime;
      
    }
    else if (DirA == 1){
      DelayTimeA = DelayTime * 0.5;
    }
    
    if (DirB == 0){
      DelayTimeB = DelayTime;
    }
    else if (DirB == 1){
      DelayTimeB = DelayTime * 0.5;
    }
  }

    float DelayTimes2(){ // smoothly transitions the pumping rate of the two syringe pumps to avoid major spikes or drops in flow rate. 
    if (DirA == 0){
      DelayTimeA = SyrArea * 9.454 / (0.95 * Rate - (Time - TimeInitial) * Rate * 0.9 / (TotalSteps * DelayTime * (1 - PumpFrac))) - 0.15;
    }
    else if (DirA == 1){
      DelayTimeA = SyrArea * 9.454 / (0.1 * Rate + (Time - TimeInitial) * Rate * 0.9 / (TotalSteps * DelayTime * (1 - PumpFrac))) - 0.15;
    }
    if (DirB == 0){
      DelayTimeB = SyrArea * 9.454 / (0.95 * Rate - (Time - TimeInitial) * Rate * 0.9 / (TotalSteps * DelayTime * (1 - PumpFrac))) - 0.15;
    }
    else if (DirB == 1){
      DelayTimeB = SyrArea * 9.454 / (0.1 * Rate + (Time - TimeInitial) * Rate * 0.9 / (TotalSteps * DelayTime * (1 - PumpFrac))) - 0.15;
    }
  }

 
void loop() {

  // controls the pause and reset functionality
  while (digitalRead(Pause) == LOW){
         Time = millis();
         digitalWrite(StepPinA, LOW);
         digitalWrite(StepPinB, LOW);
         TimeInitial = millis() - (TimeC - TimeInitial);
         while (digitalRead(Reset) == LOW){
                StepsB = 0;
                StepsA = 0;
                Reverse = 0;
                delay(500);         
         }
  }

  //Reverse is defined with respect to stepper motor A
  
  if (Reverse == 0){
    DirA = 0;
    DirB = 1;
    digitalWrite(DirPinA, DirA);
    digitalWrite(DirPinB, DirB);
  }
  else if (Reverse == 1){
    DirA = 1;
    DirB = 0;
    digitalWrite(DirPinA, DirA);
    digitalWrite(DirPinB, DirB);
  }

  
  if (Stage1 == 1){
    if (DirA == 0 && (0.9*PumpFrac * TotalSteps) <= StepsA){
       if (Flip == 0){
        StepsB = 0;
        Flip = 1;
       }
       DirB = 0;
    }
    if (DirB == 0 && (0.9*PumpFrac * TotalSteps) <= StepsB){
       if (Flip == 0){
        StepsA = 0;
        Flip = 1;
       }
       DirA = 0;
    }
    DelayTimes();
    Time = millis();
    TakeStep(DirPinA, DirA, StepPinA, StepsA, DelayTimeA, TimeA);
    Time = millis();
    TakeStep(DirPinB, DirB, StepPinB, StepsB, DelayTimeB, TimeB);
    if (DirA == 0 && (PumpFrac * TotalSteps) <= StepsA){
       Stage1 = 0; 
       TimeInitial = millis(); 
       DirB = 1;
       Flip = 0;
    }
    if (DirB == 0 && (PumpFrac * TotalSteps) <= StepsB){
       Stage1 = 0;  
       TimeInitial = millis();
       DirB = 1;
       Flip = 0;
    }
//    Serial.print("Stage1 A:B, ");
//    Serial.print(Time);
//    Serial.print(" , ");
//    Serial.print(StepsA);
//    Serial.print(" , ");
//    Serial.print(StepsB);
//    Serial.print(" , ");
//    Serial.print(DelayTimeA);
//    Serial.print(" , ");
//    Serial.print(DelayTimeB);
//    Serial.print(" , ");
//    Serial.print(Reverse);
//    Serial.print(" , ");
//    Serial.print(Stage1);  
//    Serial.print(" , ");
//    Serial.println(TotalSteps); 
  }

  TimeC = millis();
  if (Stage1 == 0 && DirA == 0 && PumpFrac * TotalSteps <= StepsA){
    Time = millis();
    DelayTimes2();
    Time = millis();
    TakeStep(DirPinA, DirA, StepPinA, StepsA, DelayTimeA, TimeA);
    Time = millis();
    TakeStep(DirPinB, DirA, StepPinB, StepsB, DelayTimeB, TimeB); 
    while ((TotalSteps * (DelayTime + 0.150) * (1 - PumpFrac)) - (TimeC - TimeInitial)){
      StepsA = 0;
      Stage1 = 1;
      if (Reverse == 0){
           Reverse = 1;
           Stage1 = 1;
           break;
      }
      if (Reverse == 1) {
          Reverse = 0;
          Stage1 = 1;
          break;
      }
      }
//    Serial.print("Stage2 A:B, ");
//    Serial.print(Time);
//    Serial.print(" , ");
//    Serial.print(StepsA);
//    Serial.print(" , ");
//    Serial.print(StepsB);
//    Serial.print(" , ");
//    Serial.print(DelayTimeA);
//    Serial.print(" , ");
//    Serial.print(DelayTimeB);
//    Serial.print(" , ");
//    Serial.print(TimeC - TimeInitial);
//    Serial.print(" , ");
//    Serial.print((TotalSteps * (DelayTime + 0.150) * (1 - PumpFrac)));  
//    Serial.print(" , ");
//    Serial.println((TimeC - TimeInitial)); 
  }

  TimeC = millis();
  if (DirB == 0 && PumpFrac * TotalSteps <= StepsB){
    Time = millis();
    DelayTimes2();
    Time = millis();
    TakeStep(DirPinA, DirB, StepPinA, StepsA, DelayTimeA, TimeA);
    Time = millis();
    TakeStep(DirPinB, DirB, StepPinB, StepsB, DelayTimeB, TimeB);
    while ((TotalSteps * (DelayTime + 0.150) * (1 - PumpFrac)) - (TimeC - TimeInitial)){ // Reverses the pump directions
      StepsB = 0;
      Stage1 = 1;
      if (Reverse == 0){
           Reverse = 1;
           Stage1 = 1;
           break;
      }
      if (Reverse == 1) {
          Reverse = 0;
          Stage1 = 1;
          break;
      }
      }
//    Serial.print("Stage3 A:B, ");
//    Serial.print(Time);
//    Serial.print(" , ");
//    Serial.print(StepsA);
//    Serial.print(" , ");
//    Serial.print(StepsB);
//    Serial.print(" , ");
//    Serial.print(DelayTimeA);
//    Serial.print(" , ");
//    Serial.print(DelayTimeB);
//    Serial.print(" , ");
//    Serial.print(TimeC - TimeInitial);
//    Serial.print(" , ");
//    Serial.print(Stage1);  
//    Serial.print(" , ");
//    Serial.println(TotalSteps); 
  }
  
}
