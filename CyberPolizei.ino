#include <Adafruit_GFX.h>

#include <GPNBadge.hpp>

#include <FS.h>

#include "rboot.h"
#include "rboot-api.h"

#include "Colors.h"

uint32_t PIXEL_WHITE = pixels.Color(255, 255, 255);
uint32_t PIXEL_BLUE = pixels.Color(0, 0, 255);
uint32_t PIXEL_RED = pixels.Color(255, 0, 0);
uint32_t PIXEL_ORANGE = pixels.Color(255, 100, 0);
uint32_t PIXEL_OFF = pixels.Color(0, 0, 0);


Badge badge;

int pixelBrightness = 64;
int mode = 0;

void setup(){
	badge.init();
	badge.setBacklight(true);

	tft.begin(); //initialize the tft. This also sets up SPI to 80MHz Mode 0
	tft.setRotation(2); //turn screen
	tft.scroll(32); //move down by 32 pixels (needed)
	tft.fillScreen(TFT_BLACK);  //make it Black

	tft.writeFramebuffer();

	pixels.setBrightness(255);

	rboot_config rboot_config = rboot_get_config();
	SPIFFS.begin();
	File f = SPIFFS.open("/rom"+String(rboot_config.current_rom),"w");
	f.println("Cyber Polizei\n");
}

int status = 0;
int lmDisplay = 0;
int lmButton = 0;

void loop(){
	if(millis() - lmDisplay > 200){
		lmDisplay = millis();
		tft.setCursor(0, 20);
		tft.setTextSize(3);
		switch(status){
			case 0:
				tft.fillScreen(TFT_BLACK);
				tft.setTextColor(TFT_YELLOW);
				status = 1;
				break;
			case 1:
				tft.fillScreen(TFT_YELLOW);
				tft.setTextColor(TFT_BLACK);
				status = 0;
				break;
		}
		tft.print(" CYBER\n CYBER\n CYBER\n");
		tft.setCursor(2, 91);
		tft.print("POLIZEI");
		tft.writeFramebuffer();
	}

	if(millis() - lmButton > 350){
		lmButton = millis();
		if(badge.getJoystickState() == JoystickState::BTN_UP){
			pixelBrightness += 32;
			if(pixelBrightness > 255){
				pixelBrightness = 255;
			}
			pixels.setBrightness(pixelBrightness);		
			pixels.show();	
		}else if(badge.getJoystickState() == JoystickState::BTN_DOWN){
			pixelBrightness -= 32;
			if(pixelBrightness < 0){
				pixelBrightness = 0;
			}
		}
	}

	switch(mode){
		case 0:
			polBlink();
			break;
	}


	delay(10);
}

void setPixels(int from, int until, uint32_t c){
	int r, g, b;
	r = pixelBrightness*getRedValueFromColor(c)/255;
	g = pixelBrightness*getGreenValueFromColor(c)/255;
	b = pixelBrightness*getBlueValueFromColor(c)/255;
	for(int i = from; i <= until; i++){
		pixels.setPixelColor(i, r, g, b);
	}
	pixels.show();
}

uint8_t getRedValueFromColor(uint32_t c) {
    return c >> 16;
}

uint8_t getGreenValueFromColor(uint32_t c) {
    return c >> 8;
}

uint8_t getBlueValueFromColor(uint32_t c) {
    return c;
}


//Pixel Effects
int lmPol = 0;
int polCounter = 0;
int polColor = 0;
int polDelay = 30;

void polBlink(){
	uint32_t color1, color2;
	if(millis() - lmPol > polDelay){
		switch(polColor){
			case 0:
				color1 = PIXEL_WHITE;
				color2 = PIXEL_BLUE;
				break;
			case 1:
				color1 = PIXEL_RED;
				color2 = PIXEL_WHITE;
				break;
			case 2:
				color1 = PIXEL_BLUE;
				color2 = PIXEL_RED;
				break;
		}
		switch(polCounter){
			case 0:
  			setPixels(0, 1, color1); //0
				polCounter++;
				polDelay = 30;
				break;
			case 1:
  			setPixels(0, 1, PIXEL_OFF); //1
				polCounter++;
				break;
			case 2:
  			setPixels(0, 1, color1); //2
				polCounter++;
				break;
			case 3:
  			setPixels(0, 1, PIXEL_OFF); //3
				polCounter++;
				break;
			case 4:
  			setPixels(2, 3, color2); //4
				polCounter++;
				break;
			case 5:
  			setPixels(2, 3, PIXEL_OFF); //5
				polCounter++;
				break;
			case 6:
  			setPixels(2, 3, color2); //6
				polCounter++;
				break;
			case 7:
  			setPixels(2, 3, PIXEL_OFF); //7
				polCounter = 0;
				polColor ++;
				if(polColor == 3){
					polColor = 0;
				}
				polDelay = 100;
				break;
			}
			lmPol = millis();
		}
}