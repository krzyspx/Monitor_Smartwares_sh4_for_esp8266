//2800 540 1540
const int interruptPin = 12;
volatile bool intfired = false;
volatile unsigned long pulseWidth = 0;
volatile unsigned long pulseWidthmem = 0;
volatile unsigned long pulseWidthmema = 0;
volatile unsigned long pulseWidthmemc = 0;
volatile boolean started = false;
volatile boolean read_a = false;
volatile boolean read_b = false;
volatile boolean read_c = false;

int error = 25;
int _numBits = 32;
int remoteId;
int _channel;
boolean state;
unsigned long _message;
bool _messageReady = false;
int pulsoPre;

boolean ICACHE_RAM_ATTR _checkPulse(long pulseLength, long comp) {
  if ((pulseLength < comp + error) && (pulseLength > comp - error)) {

    return true;
  }
  else {
    return false;
  }
}




void ICACHE_RAM_ATTR handleInterrupt() {
  intfired = true;

  unsigned long _now = micros();
  static unsigned long lastChange = 0;
  static unsigned int lastPulseLength = 0;
  static int  pulseCounter = 0;
  static int bitCounter = 0;
  //  static boolean started = false;
  static unsigned long message = 0;

  pulseWidth =  _now - lastChange;

  if (started) {
    if (pulseCounter == 0) {
      pulseCounter++;

      if (_checkPulse(pulseWidth, 265 * 2)) { // 0
        read_b = 1;
        pulseWidthmem = pulseWidth;
        message <<= 1;
      }
      else if (_checkPulse(pulseWidth, 220 * 7)) { // 1
        read_c = 1;
        pulseWidthmemc = pulseWidth;
        message <<= 1;
        message |= 1;
      }
      else  {
        started = false;
        bitCounter = 0;
        lastChange = _now;
        pulseCounter = 0;
        message = 0;
        _messageReady = false;
        return;
      }

      if (bitCounter == _numBits - 1) {
        remoteId = message >> 16;
        state = message >> 4 & 1;
        _channel = message & 15;
        bitCounter++;
      } else {
        //rawPulse[bitCounter] = pulseWidth;
        bitCounter++;
      }
    }
    else {
      if (bitCounter == 32) {
        started = false;
        bitCounter = 0;
        message = 0;
        _messageReady = true;
      }
      lastChange = _now;
      pulseCounter = 0;
      return;
    }

  }
  else {
    if (_checkPulse(pulseWidth, 200 * 14)) {
      started = true;
        read_a = 1;
        pulseWidthmema = pulseWidth;
 
      lastChange = _now;
    }
    else {
      started = false;
      lastChange = _now;
    }
  }

}








void setup() {
  pinMode(2, OUTPUT);
  // pinMode(interruptPin, INPUT_PULLUP);


  // attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, RISING);
  Serial.begin(115200);
  Serial.println();

}

void loop() {
/*
  delay(500);
  if (intfired) {
    Serial.println("Interrupt Detected");
    //   Serial.println(pulseWidthmem);
    intfired = false;
  }
  if (read_a) {
    Serial.println(pulseWidthmema);
    read_a = 0;
  }
  if (read_b) {
    Serial.println(pulseWidthmem);
    read_b = 0;
  }
  if (read_c) {
    Serial.println(pulseWidthmemc);
    read_c = 0;
  }
 */
  if (_messageReady) {
    _messageReady = false;
    enviardatos();
  }
}

void enviardatos() {
  Serial.print("Id : ");
  Serial.print(remoteId);
  Serial.print(" / Canal : ");
  Serial.print(_channel);
  Serial.print(" / State: ");
  Serial.println(state);
}
