/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        D10  //Serial Receive pin
#define SSerialTX        D11  //Serial Transmit pin

#define SSerialTxControl D12  //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         LED_BUILTIN


/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
int iSendBuffer = 0;
byte sendBuffer[10];
byte recivedBuffer[10];
void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(115200);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Hello and welcome to this early development and testing...");
  Serial.println("Todays project is : valvontajärjestelmä");
  Serial.println("Use Serial Monitor, type in upper window, ENTER");

  
  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);    
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver   
  
  // Start the software serial port, to another device
  RS485Serial.begin(4800);   // set the data rate 


}//--(end setup )---



void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  dataReciver(false,true);
  delay(10);
}//--(end main loop )---




byte hashCreator(int iBuffer, char sendOrRecive){
  byte r = 0;
  byte Buffer[10];
  if(sendOrRecive =='r'){
  for(int i=0; i < iBuffer;i++){
    r = r + recivedBuffer[i];
    }}
  if(sendOrRecive =='s'){
    for(int i=0; i < iBuffer;i++){
    r = r + sendBuffer[i];
    }}  
  return r;
}

  
void sender(int i){//Takes sendBuffer size as input
char feedback;
  do{
    bufferSend(i);
    feedback = getFeedback();
  }while(feedback=='E');
  if(feedback == 'R'){
    dataReciver(true,true);
    }
}

void bufferSend(int iBuffer){
  sendBuffer[iBuffer] = hashCreator(iBuffer,'s');
  enableTransmit();
  for(int i=0; i < iBuffer+1;i++){
    Serial.write(sendBuffer[i]);
    RS485Serial.write(sendBuffer[i]);
  }
  disableTransmit();
}

void dataReciver(bool exeptAnswerNow, bool sendResponse){
  while(checker(bufferRecive()) == false){
    if(exeptAnswerNow == true)sendFeedback('E');
  }    
    Serial.print("recived: ");
     for(int j =0; j<10;j++)Serial.print(recivedBuffer[j]);
    if(sendResponse){sendFeedback('R');
        enableTransmit();
        for(int j =0; j<10;j++)RS485Serial.write(recivedBuffer[j]);
        disableTransmit();  
}else sendFeedback('O');
}
bool bufferRecive(){
  int i = 0;
  int delayTimer;
  int timeOutTimer = millis();
  while(millis() - timeOutTimer < 1000){
    if (RS485Serial.available()) {
      delayTimer = millis();    
      byte a = RS485Serial.read();
      recivedBuffer[i] = a;
      i++;
    }
    if((i != 0) and (i == 9 or millis()-delayTimer > 100)){return i;}
  }
  return false;
}

bool checker(int iBuffer){
  if(iBuffer == 0)return false;
  if(recivedBuffer[iBuffer-1] == hashCreator(iBuffer,'r'))return true;
}

char getFeedback(){
  if (checker(bufferRecive()))return recivedBuffer[0];
  else return 'E';
  }
void sendFeedback(char charToSend){
  sendBuffer[0]= charToSend;
  bufferSend(1);
}
void enableTransmit() {digitalWrite(SSerialTxControl, RS485Transmit);}  // Enable RS485 Transmit
void disableTransmit() {digitalWrite(SSerialTxControl, RS485Receive); } // Disable RS485 Transmit    

