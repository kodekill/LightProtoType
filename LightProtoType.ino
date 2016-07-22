#include <Adafruit_NeoPixel.h>
#include <IRremote.h>

#define PIN 2
#define RECV_PIN 10

#define Button0 0xFF4AB5
#define Button1 0xFF6897
#define Button2 0xFF9867
#define Button3 0xFFB04F
#define Button4 0xFF30CF
#define Button5 0xFF18E7
#define Button6 0xFF7A85
#define Button7 0xFF10EF
#define Button8 0xFF38C7
#define Button9 0xFF5AA5
#define ButtonUp 0xFF629D
#define ButtonDn 0xFFA857
#define ButtonLt 0xFF22DD
#define ButtonRt 0xFFC23D
#define ButtonOK 0xFF02FD
#define ButtonStr 0xFF42BD
#define ButtonHsh 0xFF52AD

IRrecv irrecv(RECV_PIN);
decode_results results;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);

int flag = 0;  //flag for determining if the pulse effect is going to run
int input = 0; //records remote input
unsigned int intensity = 250;
unsigned int rate = 5; 

enum AnimationMode{
  ON,
  BREATHE,
  BLINK,
  CYCLE,
} mode = ON;

typedef struct {
  int R;
  int G;
  int B;
  int intensity;
} RGB;
  
RGB color, colorPrime;



void setup() {
  color = {0, 0, 0, 5};
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  irrecv.enableIRIn(); // Start the receiver
  Serial.begin(9600);
}

int Clamp(int x) {
  if (x < 0) 
  return 0;
  
  else if (x > 255)
  return 255;
  
  else 
  return x;
}

void increaseIntensity() {
  color.intensity = Clamp(color.intensity + 25);
}

void decreaseIntensity() {
  color.intensity = Clamp(color.intensity - 25);
}


void loop() 
{
 //                                                                      Serial.print("Take Input : ");
  TakeInput();
  AnimationStep();
  delay(10);
  SetPixels();
}

void AnimationStep(){
  switch (mode){
	  case ON:
		  Full_On();
          Serial.println("AnimationStep = on");
	 break;

     case BREATHE:
         Breathe();
	     Serial.println("AnimationStep = Breathe");
     break;

     case BLINK:
         Blink();
         Serial.println("AnimationStep = BLINK");
     break;

     case CYCLE:
         Serial.println("AnimationStep = CYCLE");
     break;

     default:
		 Full_On();
         Serial.println("AnimationStep = default");
     break;
  }
}

void TakeInput(){
 if (irrecv.decode(&results)) {
    Serial.print("got input: ");
    input = (results.value);
    switch (input) {
      
      case Button1: // Red
      color.R = 1,
      color.G = color.B = 0;
      break;
      
      case Button2: // Green
      color.R = color.B = 0,
      color.G = 1;
      break;
      
      case Button3:// Blue
      color.R = color.G = 0,
      color.B = 1;
      break;
      
      case Button4: // Yellow
      color.R = color.G = 1,
      color.B = 0;
      break;
      
      case Button5: // Purple
      color.R = color.B = 1,
      color.G = 0;
      break;
      
      case Button6: // Aqua
      color.R = 0,
      color.G = color.B = 1;
      break;
      
      case Button7:  // White
      color.R = color.G = color.B = 1;
      break;
      
      case Button8:
      
      break;
      
      case Button9:
      
      break;
      
      case Button0: // Off
        color.R = color.G = color.B = 0;
      break;
      
      case ButtonUp:
        increaseIntensity();
      break;
      
      case ButtonDn:
        decreaseIntensity();
      break;

      case ButtonLt:
        rate -= 2;
        if (rate <=0){ rate = 1;}
      break;

      case ButtonRt:
        rate += 2;
        if (rate >=50){ rate = 50;}
      break;

      case ButtonOK: //Turn lights on
        mode = ON;
      break;
      
      case ButtonStr:
        mode = BLINK;
      break;
      
      case ButtonHsh: //breath
	     mode = BREATHE;
      break;   
    }
    Serial.println(input);
    irrecv.resume(); // Receive the next value 
  }
  else {
  //                                                 Serial.println("no input :(");
  }
}

void Full_On(){
  static int intensity = color.intensity;
//  colorPrime = color;
//  colorPrime.intensity = brightness;
}

void Breathe(){
  
  static enum {
     inhale,
     exhale 
    }respiration = exhale;

  static int brightness = 255;

 if (brightness > color.intensity){
      brightness = color.intensity;
      respiration = exhale;
 }
  
  if (respiration == exhale){
    if (brightness == 0){
      respiration = inhale;
      brightness++;
    }
    else 
      brightness--;
  }

  else{
    if (brightness == color.intensity){
      respiration = exhale;
      brightness--;
    }
    else brightness++;
  }

  // use the value of colorPrime for output 
  colorPrime = color;
  colorPrime.intensity = brightness;
  Serial.print("brightness = ");
  Serial.println(brightness);
}


void Blink(){
  static enum{
    on,
    off,
    middle
  }type = on;

  static int brightness = 64;

  if (type == on){
    delay(100*rate);
    brightness = 64;
    type = middle;
    
  }
  
  else if (type == middle){
    type = off;
  }
  
  else {
    delay(100*rate);
    brightness = 0;
    type = on;
  }
  
  colorPrime = color;
  colorPrime.intensity = brightness;

}

void SetPixels(){
  SetLED(strip.Color(colorPrime.R * colorPrime.intensity, colorPrime.G * colorPrime.intensity, colorPrime.B * colorPrime.intensity), 0);
}

void SetLED(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
