
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int time = 60;

void setup(void) {
  Serial.begin(9600);
  lcd.begin(16, 2);
}



String format_time(){
  String timestamp = get_timestamp();
}




String get_timestamp(int hh, int mm, int ss)
{

int h = 0;
int m = 0;
int s = 0;
int t = time;
h = t / 3600;
t = t % (3600*h);
m = t / 60;
s = t % (60*m);

  String timestamp = two_digit_format(h) + ":" + two_digit_format(m) + ":" + two_digit_format(s);
  return timestamp;
}

String two_digit_format(int i)
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



void loop(void) {

  String timestamp = format_time();
 
  lcd.setCursor(0, 0);
  
  lcd.print("time:");
  delay(100); 
  
  lcd.setCursor(0, 1);

  lcd.print(timestamp);
  Serial.println(timestamp); 
  time++;
}

