int reading;
int smoothReading;
int initializationStep; // we take 10 measurements to initialize
int defaultReading; // idle reading
int defaultReadingSum; // used to calculate defaultReading
bool sentSingleCommand; // used for debouncing mute and play buttons

// State machine
int state;
const int INITIALIZING = 0; // go to 1
const int IDLE = 1; // go to 2,3,4,5
const int VOLUMEDOWN = 2; // go to 1
const int VOLUMEUP = 3; // go to 1
const int VOLUMEMUTE = 4; // go to 1
const int PLAYPAUSE = 5; // go to 1

// Pins
const int ledPin = 11;
const int dataPin = 22;

// the setup() method runs once, when the sketch starts

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(dataPin, INPUT);
  Serial.begin(9600);
  smoothReading = reading = analogRead(dataPin);
  initializationStep = 0;
  state = INITIALIZING;
  delay(1000);
}

// the loop() methor runs over and over again,
// as long as the board has power

void loop() {
  reading = analogRead(dataPin);
  smoothReading = (smoothReading + reading) / 2;
  if (state == INITIALIZING)
  {
    initialize();
    digitalWrite(ledPin, initializationStep % 2 == 0 ? HIGH : LOW);
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
  Serial.println(reading);
}

void initialize()
{
  defaultReadingSum += reading;
  initializationStep++;
  if (initializationStep == 10)
  {
    defaultReading = defaultReadingSum / 10;
    Serial.print("Default reading: ");
    Serial.println(defaultReading);
    state = IDLE;
  }
  delay(50);
}

void measure()
{
  Serial.println(smoothReading);
  if (smoothReading > defaultReading + 150)
  {
    state = PLAYPAUSE;
    sentSingleCommand = false;
  }
  else if (smoothReading > defaultReading + 50)
  {
    state = VOLUMEUP;
  }
  else if (smoothReading < defaultReading - 150)
  {
    state = VOLUMEMUTE;
    sentSingleCommand = false;
  }
  else if (smoothReading < defaultReading - 50)
  {
    state = VOLUMEDOWN;
  }
}

void sendKeystroke()
{
  switch (state)
  {
    case PLAYPAUSE:
      if (!sentSingleCommand)
      {
        Serial.println("Play pause");
        sentSingleCommand = true;
      }
      break;
    case VOLUMEMUTE:
      if (!sentSingleCommand)
      {
        Serial.println("Mute");
        sentSingleCommand = true;
      }
      break;
    case VOLUMEUP:
      Serial.println("Up");
      break;
    case VOLUMEDOWN:
      Serial.println("Down");
      break;
  }
  
  if ((smoothReading < defaultReading + 40) 
  || (smoothReading > defaultReading - 40))
  {
    state = IDLE;
  }
  delay(150);
}

