

currentTime[] = {0,0,0}; // HH:MM:SS


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

int time = 0;
int measurements[10000];


void clear_seconds(){
	currentTime[2] = 0;
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
		bState = digitalRead(outputB)
		
		if (isRightRotation(int anyRotationState, int bState)){
			return 1;
		}
		return -1;
	
	} 
   lastAnyRotationState = anyRotationState;
   return 0;
}


int isRightRotation(int aState, int bState){
	if (aState == 0 && bState == 1){
	    return 1;
	}
	return 0;
}


int isPushButonPressed(){
    buttonState = digitalRead(pushButton);
	if (buttonState != prevButtonState){
	    
		Serial.print("ButtonState: ");
		Serial.println(buttonState);
		pushButtonCounter++;
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
       currentTime[0]++;
     } else {
       currentTime[0]--;
     }
     Serial.print("currentTime[0]: ");
     Serial.println(currentTime[0]);
   } 
   lastHourState = hourState;
}


void increment_minutes(){
	
    minuteState = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (minuteState != lastMinuteState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != minuteState) { 
       currentTime[1]++;
     } else {
       currentTime[1]--;
     }
     Serial.print("currentTime[1]: ");
     Serial.println(currentTime[1]);
   } 
   lastMinuteState = minuteState;

}


void clearSecondsIfAppropiateState(){
	if(watchState == 2){
		clear_seconds();
	}
}


void manage_state(){ 

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

void main(){
	increment_time();
	read_mesurement();
	manage_state();
	display_result();
}




