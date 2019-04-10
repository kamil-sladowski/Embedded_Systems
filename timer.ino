#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int time = 60;
int blinking_mode = 1;
int need_blinking_string = 0;



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

void setup(void) {
  Serial.begin(9600);
  lcd.begin(16, 2);
}


String get_blinking_timestamp(int h, int m, int s){
    String timestamp = two_digit_format(h) + ":" + "  " + ":" + two_digit_format(s);
  return timestamp;
}

String get_normal_timestamp(int h, int m, int s){
  String timestamp = two_digit_format(h) + ":" + two_digit_format(m) + ":" + two_digit_format(s);
  return timestamp;
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


void display_on_watch(String timestamp){
  lcd.setCursor(0, 0);
  
  lcd.print("time:");
  delay(100); 
  
  lcd.setCursor(0, 1);

  lcd.print(timestamp);
  Serial.println(timestamp); 
}


void loop(void) {

  String timestamp = get_timestamp();
  display_on_watch(timestamp);
  
  time++;
}
