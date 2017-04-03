int reading;
int initializationStep; // we take 10 measurements to initialize
int listeningStep;
int defaultReading; // idle reading
int defaultReadingSum;
int slopeSum;
int rawBuffer[3];
int filteredReading;
int listeningBuffer[2];
bool sentSingleCommand; // used for debouncing mute and play buttons

// State machine
int state;
const int INITIALIZING = 0; // Spend 10 cycles establishing defaultReading
const int READY = 1; // When filteredReading < defaultReading * GESTURE_FACTOR
const int LISTENING = 2; // else

int gesture;
const int NONE = 0;
const int VOLUMEDOWN = 1;
const int VOLUMEUP = 2;
const int PLAYPAUSE = 3;
//const int VOLUMEMUTE = 4; // Not implemented yet

// Parameters
const int SPIKE_THRESHOLD = 50;
const float GESTURE_FACTOR = 1.25;
const int LISTEN_IGNORE_COUNT = 10;
const int LISTEN_MEASURE_COUNT = 10;
const float SLOPE_FACTOR = 2;

// Pins
const int ledPin = 11;
const int dataPin = 22;

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(dataPin, INPUT);
  reading = analogRead(dataPin);
  initializationStep = 0;
  listeningStep = 0;
  state = INITIALIZING;
  rawBuffer[0] = rawBuffer[1] = rawBuffer[2] = reading;
  filteredReading = reading;
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  reading = analogRead(dataPin);
  filter();
  setState();
  sendKeystroke();
  delay(10);
}

// Step 1: Remove spikes from the reading
void filter() {
  rawBuffer[2] = rawBuffer[1];
  rawBuffer[1] = rawBuffer[0];
  rawBuffer[0] = reading;

  int magnitude = rawBuffer[0]-rawBuffer[1] + rawBuffer[1]-rawBuffer[2];
  int absolute = abs(rawBuffer[0]-rawBuffer[1]) + abs(rawBuffer[1]-rawBuffer[2]);
  int delta = absolute - abs(magnitude);

  if (delta < SPIKE_THRESHOLD) {
    filteredReading = rawBuffer[1];
  } else {
    //Serial.print("!");
    filteredReading = (rawBuffer[0] + rawBuffer[2])/2;
  }
}

// Step 2: Determine what to do
void setState() {
  // If we're initializing, take 10 measurements to find the baseline
  if (state == INITIALIZING) {
    defaultReadingSum += filteredReading;
    Serial.println(filteredReading);
    initializationStep++;
    if (initializationStep == 10) {
      defaultReading = defaultReadingSum / 10;
      state = READY;
      Serial.print("AVG ");
      Serial.print(defaultReading);
      Serial.println(".");
    }
    return;
  }
  if (filteredReading > defaultReading * GESTURE_FACTOR) {
    state = LISTENING;
    listeningStep++;
  } else {
    if (listeningStep > 0) {
      //Serial.print("STOP: ");
      //Serial.println(filteredReading);
    }
    state = READY;
    listeningStep = 0;
    gesture = NONE;
  }

  if (state == LISTENING) {
    listeningBuffer[1] = listeningBuffer[0];
    listeningBuffer[0] = filteredReading;
    int slope = listeningBuffer[0] - listeningBuffer[1];
    if (listeningStep < LISTEN_IGNORE_COUNT) {
      if (listeningStep == 1) Serial.println("Listening");
      slopeSum = 0;
      return;
    } else if (listeningStep < LISTEN_MEASURE_COUNT + LISTEN_IGNORE_COUNT) {
      Serial.println(slope);
      slopeSum += slope;
    } else if (listeningStep == LISTEN_MEASURE_COUNT + LISTEN_IGNORE_COUNT) {
      Serial.print("Decide ");
      Serial.println(slopeSum);
      if (slopeSum > LISTEN_MEASURE_COUNT * SLOPE_FACTOR) {
        Serial.println("Up");
        gesture = VOLUMEUP;
      }
      else if (slopeSum < LISTEN_MEASURE_COUNT * SLOPE_FACTOR * (-1)) {
        Serial.println("Down");
        gesture = VOLUMEDOWN;
      }
      else {
        Serial.println("Play pause");
        gesture = PLAYPAUSE;
      }
    }
  }
  
}

void sendKeystroke()
{
  digitalWrite(ledPin, HIGH);
  switch (gesture)
  {
    case PLAYPAUSE:
      if (!sentSingleCommand)
      {
        //Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
        //Keyboard.release(KEY_MEDIA_PLAY_PAUSE);
        sentSingleCommand = true;
      }
      break;
      /*
    case VOLUMEMUTE:
      if (!sentSingleCommand)
      {
        //Serial.println("Mute");
        sentSingleCommand = true;
      }
      break;*/ 
    case VOLUMEUP:
      //Keyboard.press(KEY_MEDIA_VOLUME_INC);
      //Keyboard.release(KEY_MEDIA_VOLUME_INC);
      break;
    case VOLUMEDOWN:
      //Keyboard.press(KEY_MEDIA_VOLUME_DEC);
      //Keyboard.release(KEY_MEDIA_VOLUME_DEC);
      break;
  }
}

