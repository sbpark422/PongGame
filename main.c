#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/msp432p4111_classic.h"

/* Graphic library context */
Graphics_Context g_sContext; //전역변수
extern int pong();
extern const Graphics_Font g_sFontFixed4x5;

/*
 * Main function
 */
void main(void)
{
    /* Setting our MCLK to 48MHz for faster programming */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    FlashCtl_A_setWaitState(FLASH_A_BANK0, 6);
    FlashCtl_A_setWaitState(FLASH_A_BANK1, 6);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);

    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed4x5);
    Graphics_clearDisplay(&g_sContext);


    /* Configuring S1 & S2 buttons in mainboard */
    P1->DIR &= ~(BIT1|BIT4);
    P1->REN |= (BIT1|BIT4);
    P1->OUT |= (BIT1|BIT4);

    /* Configuring S1 & S2 buttons in boostxl-edumkii */
    P3->DIR &= ~BIT5;
    P5->DIR &= ~BIT1;
    P3->REN |= BIT5;
    P5->REN |= BIT1;
    P3->OUT |= BIT5;
    P5->OUT |= BIT1;

    pong();
    while (1)
    {
    }
}
