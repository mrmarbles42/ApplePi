//declare values to respective analog pins
int humid = ;
int lit = ;
int temp = ;

//global values
int dataH, int dataL, int dataT;
float sensorVal[] = {0,0,0};
int freq = 10000; //collect data every 10 seconds

void setup() {
	//initial setup code (baud, pinmodes, etc...)
	serial.begin(9600)
	pinMode(humid, INPUT);
	pinMode(lit, INPUT);
	pinMode(temp, INPUT);
}

void loop() {
	//Read values
	dataT = analogRead(temp);
	dataH = analogRead(humid);
	dataL = analogRead(lit);
	
	delay(freq);

}	