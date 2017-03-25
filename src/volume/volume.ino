const int ledPin = 11;
const int dataPin = 22;
int smoothReading;
int[] buffer = new int[10];
int bufferPosition = 0;

// State machine
const int INITIALIZING = 0; // go to 1
const int IDLE = 1; // go to 2,3,4,5
const int VOLUMEDOWN = 2; // go to 1
const int VOLUMEUP = 3; // go to 1
const int VOLUMEMUTE = 4; // go to 1
const int PLAYPAUSE = 5; // go to 1
int state = INITIALIZING;

// the setup() method runs once, when the sketch starts

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(dataPin, INPUT);
  Serial.begin(9600);
  smoothReading = smoothReading = analogRead(dataPin);
}

// the loop() methor runs over and over again,
// as long as the board has power

void loop() {
  int proximity = analogRead(dataPin);
  smoothReading = (smoothReading + proximity) / 2;
  if (state == INITIALIZING)
  {
    initialize();
    digitalWrite(ledPin, bufferPosition % 2 == 0 ? HIGH : LOW);
  }
  else if (state == IDLE)
  {
    digitalWrite(ledPin, LOW);
    measure();
  }
  else
  {
    digitalWrite(ledPin, HIGH);
    sendKeystroke();
  }
  Serial.println(proximity);
}

void initialize()
{
  buffer[bufferPosition] = smoothReading;
  buffeerPosition++;
  if (bufferPosition = 10)
  {
    bufferPosition = 0;
    state = IDLE;
  }
  delay(50);
}

void measure()
{

}
}


