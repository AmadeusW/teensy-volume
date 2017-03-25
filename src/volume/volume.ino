int reading;
int smoothReading;
int initializationStep; // we take 10 measurements to initialize
int centerReading;
int bottomReading;
int topReading;
int defaultReading; // idle reading
int defaultReadingSum; // used to calculate defaultReading
bool sentSingleCommand; // used for debouncing mute and play buttons

// State machine
int state;
const int INITIALIZING = 0; // go to 6
const int DETECTCENTER = 6; // go to 7
const int DETECTBOTTOM = 7; // go to 8
const int DETECTTOP = 8; // go to 1
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
  }
  else if (state == DETECTCENTER)
  {
    detectCenter();
  }
  else if (state == DETECTBOTTOM)
  {
    detectBottom();
  }
  else if (state == DETECTTOP)
  {
    detectTop();
  }
  else if (state == IDLE)
  {
    measure();
  }
  else
  {
    sendKeystroke();
  }
  //Serial.println(reading);
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
    state = DETECTCENTER;
  }
  delay(100);
}

void detectCenter()
{
  digitalWrite(ledPin, HIGH);
  Serial.print("Hold hand in the center...");
  delay(2000);
  reading = analogRead(dataPin);
  smoothReading = reading;

  for (int i = 4; i > 0; i--)
  {
    digitalWrite(ledPin, LOW);
    delay(i * 100);
    reading = analogRead(dataPin);
    smoothReading = (smoothReading + reading) / 2;
  
    digitalWrite(ledPin, HIGH);
    delay(i * 100);
    reading = analogRead(dataPin);
    smoothReading = (smoothReading + reading) / 2;    
  }

  digitalWrite(ledPin, LOW);
  centerReading = smoothReading;
  topReading = centerReading * 1.2;
  bottomReading = centerReading * 0.8;
  Serial.print("Center reading: ");
  Serial.println(centerReading);
  state = DETECTBOTTOM;
  Serial.println("Hold hand at the bottom...");
  delay(500);
}

void detectBottom()
{
  if (smoothReading < bottomReading)
  {
    digitalWrite(ledPin, HIGH);
    bottomReading = smoothReading;
    Serial.print("Lowest reading:");
    Serial.println(bottomReading);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
  delay(100);

  if (smoothReading > centerReading * 1.2)
  {
    state = DETECTTOP;
    Serial.println("Hold hand at the top...");
  }
}

void detectTop()
{
  if (smoothReading > topReading)
  {
    digitalWrite(ledPin, HIGH);
    topReading = smoothReading;
    Serial.print("Highest reading:");
    Serial.println(topReading);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
  delay(100);

  if (smoothReading < centerReading * 0.8)
  {
    Serial.println("All done!");
    state = IDLE;
  }
}

void measure()
{
  digitalWrite(ledPin, LOW);
  if (smoothReading > (centerReading + topReading * 4) / 5)
  {
    state = PLAYPAUSE;
    sentSingleCommand = false;
  }
  else if (smoothReading > (centerReading + topReading) / 2)
  {
    state = VOLUMEUP;
  }
  else if (smoothReading < (centerReading + bottomReading * 4) / 5)
  {
    state = VOLUMEMUTE;
  }
  else if (smoothReading < (centerReading + bottomReading) / 2)
  {
    state = VOLUMEDOWN;
  }
}

void sendKeystroke()
{
  digitalWrite(ledPin, HIGH);
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

