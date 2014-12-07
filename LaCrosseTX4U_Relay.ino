
byte               Data[11];
byte               TData[11];
byte               HData[11];
unsigned char      receicePin=6;
unsigned char      sendPin=8;
unsigned char      VccPin=4;
unsigned int       lcTemp;
unsigned int       lcHumy;

void setup() {
  Serial.begin(9600);
  Serial.println("Started LACrosseTX4Y_Relay.");
  pinMode(receicePin, INPUT);
  pinMode(sendPin, OUTPUT);
  pinMode(VccPin, OUTPUT);
  digitalWrite(VccPin, HIGH);
}

void getData(byte *Data){
  
  byte              d=0;
  byte              byteCount=0;
  boolean           validBit=false;
  unsigned long     duration;
  boolean           isValid =false;
  
  while(!isValid ){ 
    duration = pulseIn(receicePin, HIGH);
    //DebugL("duration",duration); 
    
    if ((duration>1150) && (duration<1400) ) {
      d=d << 1 ;
      if (byteCount>0) byteCount +=1;
      //if (byteCount/4 >9) DebugB("d0",d);
      validBit=true;
      //DebugL("duration",duration);
      //DebugB("d",d); 
    }
    else if ((duration>400) && (duration<600) ) {
       d = d << 1;
       d +=1; 
       if (byteCount>0) byteCount +=1;
       //if (byteCount/4 >9) DebugB("d1",d);
       validBit=true;
       //DebugL("duration",duration);
       //DebugB("d",d);
       
    }
 
    if (validBit) {
      
      if ((d==0x0A) && (byteCount==0)){
        byteCount=7;
        Data[0]=0x00;
        Data[1]=0x0A;
        //DebugB("SignalFound",d);
        d=0;
      }
      if ((byteCount>7) && (byteCount % 4==3)){
        Data[byteCount/4]=d;
        //DebugB("BiteCount",byteCount/4);
        //DebugB("D=",d);
        d=0;
        if (byteCount/4 >9) return;
      }
      validBit=false;
    }
      
  }

}

boolean Validate (byte *Data){
  byte Sum=0;
  for (int i = 0; i<10; i=i+1) Sum +=Data[i];

  if (Sum % 16 == Data[10]) return true;
  else return false;
}

String DecodeSenzor(byte *Data){
  String senzor;
  byte add;
  String address;
  float Value=100;
  String response;
 

 if (Data[2]==0x00) {
   senzor="Temperature";
   Value = Data[5]*10.0 - 50.0 + Data[6] + 0.1*Data[7];
   
 }
 else if (Data[2]==0x0E) {
   senzor="Humidity";
   Value = Data[5]*10.0  + Data[6] ;
 }
 else senzor="Unknown";
 
 //if (str=="11111111001110011") senzor="Boiler is up";
  add=(Data[3]<< 4) + (Data[4]>>1);
  //DebugB("adresa=",add);

if ((senzor =="Temperature" ) || (senzor =="Humidity")) {
  if (add==0x83) address="External";
}
 
 
 if (Value==100) response=senzor ;
 else response=senzor + " " + address +" " +Value;
  
 return response; 
  
}

void Send_1(){
  digitalWrite(sendPin, HIGH);   
  delayMicroseconds(550);             
  digitalWrite(sendPin, LOW);    
  delayMicroseconds(1050);   
}

void Send_0(){
  digitalWrite(sendPin, HIGH);   
  delayMicroseconds(1300);             
  digitalWrite(sendPin, LOW);    
  delayMicroseconds(1050);   
}


void  SendData(byte *d){
  for (int k = 0; k< 2; k++ ) {
    for (int i = 0; i< 11; i++ ) {
      for (int j = 3; j>= 0; j-- ) {
        //Serial.print(bitRead(d[i],j));
        if (bitRead(d[i],j)==1 ) Send_1();
        else Send_0(); 
      }
    }
    delay(30);
  }
  delay(250);
}


void loop() {
  String a;
  getData(Data);
  if (Validate(Data)) {
    //DebugData(Data);
    if (Data[2]==0x00) {
      lcTemp=millis();
      ArrayAddT();
    }
    else if (Data[2]==0x0E) {
      lcHumy=millis();
      ArrayAddH();
    }
    
    DebugS("",DecodeSenzor(Data));
    if (lcTemp!=0 && lcHumy!=0){
      delay(1500);
      lcTemp=lcHumy=0;
      SendData(TData);
      delay(700);
      SendData(HData);
      //DebugData(TData);
      //DebugData(HData);
    }
      
    
  }

}

void ArrayAddT(){
  for (int i = 0; i<= 10; i=i+1) {
    TData[i]=Data[i];

  }
}


void ArrayAddH(){
  for (int i = 0; i<= 10; i=i+1) {
    HData[i]=Data[i];
  }
}
void DebugS(String te,String t){
  Serial.println("");
  Serial.print(millis());
  Serial.print("  ");
  Serial.print(te);
  Serial.print(" ");
  Serial.print(t);
}

void DebugB(String te,byte t){
  Serial.println("");
  Serial.print(millis());
  Serial.print("  ");
  Serial.print(te);
  Serial.print("=");
  Serial.print(t,DEC);
}

void DebugL(String te,long t){
  Serial.println("");
  Serial.print(millis());
  Serial.print("  ");
  Serial.print(te);
  Serial.print("=");
  Serial.print(t);
}

void DebugData(byte *Data){
  Serial.println("");
  Serial.print(millis());
  Serial.print("  ");
  for (int i = 0; i<= 10; i=i+1) {
    Serial.print(Data[i],DEC);
    Serial.print(" ");
  }
}
