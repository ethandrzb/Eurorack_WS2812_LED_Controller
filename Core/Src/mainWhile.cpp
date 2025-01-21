#include "mainWhile.hpp"
#include "WS2812FX/SimpleBreathingEffect.hpp"

extern "C"
{
	void mainWhileC(void)
	{
		mainWhileCpp();
	}
}

extern uint8_t LEDIndex;

colorRGB rgb = {.red = 0, .green = 0, .blue = 0};
colorHSV hsv = {.hue = 200, .saturation = 1.0, .value = 1.0};


void mainWhileCpp(void)
{
	SimpleBreathingEffect sbe = SimpleBreathingEffect(10, 0.005, hsv, 0.25);
	while(1)
	{
		switch(LEDIndex)
		{
			case 0:
				sbe.updateEffect();
//				WS2812FX_SimpleBreathingEffect(10, 0.005, hsv, 0.25);
				break;
			case 1:
				rgb.red = 32;
				rgb.green = 0;
				rgb.blue = 32;
				WS2812_ClearLEDs();
				WS2812_SimpleMeterEffect(rgb, 45, true);
				WS2812_SendAll();
				break;
			case 2:
				rgb.red = 0;
				rgb.green = 32;
				rgb.blue = 16;
				WS2812_ClearLEDs();
				WS2812_MirroredMeterEffect(rgb, 17, false);
				WS2812_SendAll();
				break;
			case 3:
				static colorHSV hsvRainbow = {.hue = 0, .saturation = 1.0f, .value = 0.2f};
				hsvRainbow.hue = (hsvRainbow.hue < 360) ? hsvRainbow.hue + 1 : 0;
				WS2812_FillRainbow(hsvRainbow, 3);
				WS2812_SendAll();
				break;
		}
	}
}
