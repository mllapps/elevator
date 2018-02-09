/**
 * @file btn.h
 * @author fl0mll
 * @date 2018/01/08
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief Button interface
 * 
 * <code>
 * // Check if the button is pressed.
 * btnRc_t ret;
 * if ( (ret = btn_isPressed() ) == BTN_PRESSED_SHORT)
 * {
 *    // Clear flag
 *    btn_clearAll();
 *    
 *    // Do something
 *    doSomething();
 * }else if(ret == BTN_PRESSED_LONG) {
 *    // Clear flag
 *    btn_clearAll();
 * 
 *    // Do something
 *    doSomething();
 * }
 * </code>
 */
#ifndef BTN_H
#define BTN_H

#include "stm32f1xx_hal.h"
#include "main.h"

#define BTN_DEBOUNCE_CNT_DEFAULT            (4)
#define BTN_DEBOUNCE_CNT_MIN                (1)
#define BTN_DEBOUNCE_CNT_MAX                (10)

typedef enum btnRc_e {
    BTN_OK,
    BTN_PRESSED_SHORT,
    BTN_PRESSED_LONG,
    BTN_PRESSED_LONG_REPEAT
} btnRc_t;


void btn_init();

extern void btn_clearAll(void);
extern void btn_clearLongPress(void);
extern btnRc_t btn_isPressed(void);
extern void btn_handler(void);

#endif /* BTN_H */
