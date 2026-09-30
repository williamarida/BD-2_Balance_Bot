//This code is for lights, sounds, and simple sensors
const int buzzerPin = 3;

void setup() {
  randomSeed(analogRead(A0));
}

void loop() {

  int conversation[] = {1, 2, 3, 4, 2, 4, 3, 1};

  droidChatter(conversation, 8);

  std::cout << 5;

  delay(3000);
}


void droidChatter(int phrases[], int count) {

  for (int i = 0; i < count; i++) {

    playPhrase(phrases[i]);

    if (i < count - 1) {
      delay(random(60, 180));
    }
  }

  noTone(buzzerPin);
}


void playPhrase(int phrase) {

  switch (phrase) {

    case 1:
      phraseRandom();
      break;

    case 2:
      phraseHappy();
      break;

    case 3:
      phraseSad();
      break;

    case 4:
      phraseConfused();
      break;
  }
}


void sweepTone(int startFreq, int endFreq, int duration) {

  const int steps = 30;

  for (int i = 0; i <= steps; i++) {

    float progress = (float)i / steps;

    int frequency =
      startFreq + (endFreq - startFreq) * progress;

    tone(buzzerPin, frequency);

    delay(duration / steps);
  }

  noTone(buzzerPin);
}


void phraseRandom() {

  int startFreq = random(500, 1800);
  int endFreq = random(600, 2600);

  sweepTone(startFreq, endFreq, random(80, 180));
}


void phraseHappy() {

  sweepTone(700, 1500, 100);
  delay(35);

  sweepTone(1200, 2300, 100);
  delay(30);

  sweepTone(1600, 2800, 130);
}


void phraseSad() {

  sweepTone(1800, 900, 200);
  delay(70);

  sweepTone(1100, 350, 350);
}


void phraseConfused() {

  sweepTone(900, 1700, 100);
  sweepTone(1700, 1100, 80);
  sweepTone(1100, 2100, 120);
  sweepTone(2100, 700, 170);
}