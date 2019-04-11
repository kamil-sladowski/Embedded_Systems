#include <LiquidCrystal.h>


const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(8,9,4,5,6,7);

int led = 13 ; 
int analogPin = A0; // linear Hall magnetic sensor analog interface
int analogHall; 
int i =0;
int outputA = 12;
int outputB = 13;

int time = 60; // todo 0
int blinking_mode = 1;
int need_blinking_string = 0;

int watchState = 0; // 0 IDLE, 1 HH,  2 MM

const int MAX_IDLE_TIME = 5;
int timeInNoneIdleState = 0;

int hourState = 0;
int lastHourState = 0;
int minuteState = 0;
int lastMinuteState = 0;
int anyRotationState = 0;
int lastAnyRotationState = 0;

int pushButtonCounter = 0;

int measurements[400];

int buttonState=0;
int prevButtonState=0;

int pushButton = 11;


void setup(void) {
  pinMode (led, OUTPUT); 
  pinMode(analogPin, INPUT);
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT); 
  pinMode(pushButton, INPUT); 
  Serial.begin(9600);
  lcd.begin(16, 2);
  prepare_clock();
}


void loop ()
{
  
  //delay(100);
}


void prepare_clock()
{
  // initialize Timer1
  cli();  // disable global interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  
  // set compare match register to desired timer count:
  OCR1A = 15624;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // enable global interrupts:
  sei();
  
}


ISR(TIMER1_COMPA_vect)
{
  increment_time();
  String timestamp = get_timestamp();
  //read_maesurement(timestamp);
  manage_states();
  display_on_watch(timestamp);
}


void clear_seconds(){
  int* hms = format_time();
  time = time - hms[2];
}


void change_state(){

  if (watchState == 0)
  {
    watchState = 1;
  }

  else if (watchState == 1)
  {
    watchState = 2;
  }
  else if (watchState == 2)
  {
    watchState = 0;
  }
}


void clear_state(){
  watchState = 0;
}


int isInIdleState(){
  return watchState==0;
}


int getRotation(){
  int bState;
  anyRotationState = digitalRead(outputA);
  if (anyRotationState != lastAnyRotationState){
    Serial.println("wasRotation");
  bState = digitalRead(outputB);
    Serial.println("anyRotationState");
  Serial.println(anyRotationState);
  Serial.println("lastAnyRotationState");
  Serial.println(lastAnyRotationState);
  lastAnyRotationState = anyRotationState;
    if (isRightRotation(anyRotationState, bState)){
      return 1;
    }
    return -1;
  
  } 
   lastAnyRotationState = anyRotationState;
   return 0;
}


int isRightRotation(int aState, int bState){
  if (aState == 0 && bState == 1){
    Serial.println("left");
      return 1;
  }
  Serial.println("right");
  return 0;
}


int isPushButonPressed(){
  
    buttonState = digitalRead(pushButton);
  if (buttonState != prevButtonState){
      Serial.println("isPushButonPressed");
    Serial.print("ButtonState: ");
    Serial.println(buttonState);
    pushButtonCounter++;
  prevButtonState = buttonState;
  delay(100);
    return 1;
  }
  prevButtonState = buttonState;
  return 0;
}

int isPushButonPressedTwice(){
  return pushButtonCounter == 2;
}


void increment_hours(){
  
    hourState = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (hourState != lastHourState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != hourState) { 
      if (time < 82800){
          time += 3600;
      }
     } else {
      if (time > 3600){
          time -= 3600;
        }
     }
    
     Serial.print("current time: ");
     Serial.println(time);
   } 
   lastHourState = hourState;
}


void increment_minutes(){
  
    minuteState = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (minuteState != lastMinuteState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != minuteState) { 
       time += 60;
     } else {
       time -= 60;
     }
     Serial.print("current time: ");
     Serial.println(time);
   } 
   lastMinuteState = minuteState;

}


void clearSecondsIfAppropiateState(){
  if(watchState == 2){
    clear_seconds();
  }
}


void increment_time(){
  time++;
}


void read_maesurement(String timestamp){
  int analogHall = analogRead(analogPin); 
    Serial.println("Time  |  Timestamp  |  analogHall");
  Serial.print(time);
  Serial.print("  |  ");
  Serial.print(timestamp);
  Serial.print("  |  ");
  Serial.print(analogHall);
  Serial.println("");
  measurements[time] = analogHall;
}


String get_blinking_timestamp(int h, int m, int s){
  String timestamp;
  switch(watchState){
    case 0:
      timestamp = format_digits(h) + ":" + format_digits(m) + ":" + format_digits(s);
      break;
    case 1:
      timestamp = "  :" + format_digits(m) + ":" + format_digits(s);
      break;
    case 2:
      timestamp = format_digits(h) + ":" + "  " + ":" + format_digits(s);
      break;
    }
  return timestamp;
}


String get_normal_timestamp(int h, int m, int s){
  String timestamp = format_digits(h) + ":" + format_digits(m) + ":" + format_digits(s);
  return timestamp;
}

int* format_time(){
  int* hms = (int *)malloc (sizeof (int) * 3);
  int h = 0;
  int m = 0;
  int s = 0;
  int t = time;
  h = t / 3600;
  t = t % (3600*h);
  m = t / 60;
  s = t % (60*m);
  hms[0] =h;
  hms[1] =m;
  hms[2] =s;
  return hms;
}



String get_timestamp()
{
  int *hms = format_time();
  int h = *hms;
  int m = *(hms+1);
  int s = *(hms+2);

  if(need_blinking_string){
    need_blinking_string = 0;
    return get_blinking_timestamp(h,m,s);
  }
  
  need_blinking_string = 1;
  return get_normal_timestamp(h,m,s);
  
}


String format_digits(int i)
{
  if (i < 10)
  {
    return String(0) + String(i);
  }
  else
  {
    return String(i);
  }
}


void display_on_watch(String timestamp){
  lcd.setCursor(0, 0);
  
  lcd.print("time:");
  delay(100); 
  
  lcd.setCursor(0, 1);

  lcd.print(timestamp);
  Serial.println(timestamp); 
}


void manage_states(){ 
  if (getRotation() != 0 || isPushButonPressed()){
    switch(watchState){
    case 0:
      increment_hours();
      break;
    
    case 1:
      increment_minutes();
      break;
    }
    if(isPushButonPressedTwice()){
    Serial.println("PushButonPressedTwice");
      clearSecondsIfAppropiateState();
      change_state();
      pushButtonCounter=0;
    }
    
  }
  else if (!isInIdleState()){
    if(timeInNoneIdleState < MAX_IDLE_TIME){
          timeInNoneIdleState++;
    }
    else{
      clearSecondsIfAppropiateState();
      change_state();
      timeInNoneIdleState = 0;
    }
  }
}


