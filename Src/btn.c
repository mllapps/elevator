/**
 * @file btn.c
 * @author fl0mll
 * @date 2018/01/08
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Button implementation
 */
#include "btn.h"

/**
 * Button data struct type
 */
typedef struct btnData_s {
    /* Variable to tell main that the button is pressed (and debounced). 
    Main will clear it after a detected button press.
    */
    uint8_t buttonDown;
    uint8_t currentState;
    uint8_t longPressActive;
    uint8_t longPressActiveRepeat;

    uint16_t buttonDebCnt;

    uint32_t timestamp;
} btnData_t;

/**
 * Modul data
 */
volatile btnData_t btnData;

/**
 * Basic initialization for the button module
 */
void btn_init()
{
    btnData.buttonDown = 0;
    btnData.longPressActive = 0;
    btnData.longPressActiveRepeat = 0;
    btnData.buttonDebCnt = BTN_DEBOUNCE_CNT_DEFAULT;
}

/**
 * @brief Post initialization function for the button module
 * 
 * @return void
 */
__weak void btn_initPost(void)
{
    /* Implement this function outside the module to load values 
     * from persistent storage like eeprom.
     */
}

/**
 * Clear all button flags
 */
void btn_clearAll(void)
{
    btnData.buttonDown = 0;
    btnData.longPressActive = 0;
    btnData.longPressActiveRepeat = 0;
}

/**
 * Clear all button flags
 */
void btn_clearLongPress(void)
{
//    btnData.longPressActive = 0;
    btnData.longPressActiveRepeat = 0;
}

/**
 * @brief Check if a button is pressed
 * 
 * @retrun BTN_OK if nothing pressed otherwise 
 *         BTN_PRESSED_SHORT or BTN_PRESSED_LONG
 */
btnRc_t btn_isPressed(void)
{
    btnRc_t ret = BTN_OK;

    if(btnData.buttonDown == 1 && /* button pressed since last... */
        btnData.currentState == GPIO_PIN_SET && /* button released */
        btnData.longPressActive == 0) { /* no long press detected */
            /* Short press detected */
            ret = BTN_PRESSED_SHORT;
    }else if (btnData.buttonDown == 1 && /* */
        btnData.currentState == GPIO_PIN_RESET && /* button pressed */
        btnData.longPressActive == 1 &&
        btnData.longPressActiveRepeat == 0) {
            /* Long press detected */
            ret = BTN_PRESSED_LONG;
    }else if (btnData.buttonDown == 1 && /* */
        btnData.currentState == GPIO_PIN_RESET && /* button pressed */
        btnData.longPressActive == 1 && 
        btnData.longPressActiveRepeat == 1) {
            /* Long press detected */
            ret = BTN_PRESSED_LONG_REPEAT;
    }

    return ret;
}

/**
 * @brief Button handler with debouncing feature
 * 
 * @info Call this function at main running loop
 * 
 * @param none
 * @return void
 */
void btn_handler(void)
{
	uint32_t curTimeStamp;
    
    curTimeStamp = HAL_GetTick();
    
    /* Counter for number of equal states */
    static uint8_t count = 0;
    /* Keeps track of current (debounced) state */
    static uint8_t btnState = GPIO_PIN_SET;

    //uint32_t stateChanged = ~SW1_IN_GPIO_Port->IDR ^ btnState;

    /* Check if button is high or low for the moment */
    if ( (btnData.currentState = HAL_GPIO_ReadPin(SW1_IN_GPIO_Port, SW1_IN_Pin) ) != btnState) {
        /* Button state is about to be changed, increase counter */
        count++;
        if (count >= btnData.buttonDebCnt) {
            /* The button have not bounced for N checks, change state */
            btnState = btnData.currentState;

            /* Start the timestamp to detect the press time but do this only once */
            if(btnData.buttonDown == 0) {
                btnData.timestamp = HAL_GetTick();                
            }

            /* If the button was pressed (not released), tell main so */
            if (btnData.currentState != GPIO_PIN_SET) {
                btnData.buttonDown = 1;
            }
            count = 0;
        }
    } else {
        /* Button released or debouncing so we reset the counter counter */
        count = 0;

        /* If the button was pressed (not released) */
        if(btnData.buttonDown == 1 && btnData.currentState == GPIO_PIN_RESET) {

        	if(btnData.longPressActive == 0 && curTimeStamp - btnData.timestamp >= 1000) {
        		btnData.timestamp = curTimeStamp;
        		btnData.longPressActive = 1;
        	}else if(btnData.longPressActive == 1 && btnData.longPressActiveRepeat == 0 && curTimeStamp - btnData.timestamp >= 200) {
        		btnData.timestamp = curTimeStamp;
        		btnData.longPressActiveRepeat = 1;
        	}
        }else {
        	btn_clearAll();
        }
    }
}
