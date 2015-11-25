#ifndef CODES_H
#define CODES_H

/* The controller has 8 buttons total */
#define BTN_X 0x30
#define BTN_A 0x31
#define BTN_B 0x32
#define BTN_Y 0x33
#define BTN_L 0x34
#define BTN_R 0x35
#define BTN_SELECT 0x38
#define BTN_START 0x39

/* Buttons are either pressed (DOWN) or unpressed (UP) */
#define BTN_DOWN 0x01
#define BTN_UP 0x00

/* D-pad has two axes, X and Y */
/*
 *    Y-
 * X-    X+
 *    Y+
 */
#define DPAD_Y_AXIS 0x01
#define DPAD_X_AXIS 0x00

/* D-pad axes have 3 positions: -, 0, + */
#define DPAD_NEG 0x00
#define DPAD_NEUTRAL 0x7F
#define DPAD_POS 0xFF

/* Only two message types */
#define EV_TYPE_BUTTON 0x04
#define EV_TYPE_DPAD 0x03

/* Messages come through in chunks of 16 bytes */
#define MESSAGE_SIZE 16

struct js_event_unknown {
    unsigned char timecode[8];
    unsigned char ev_type;
    unsigned char filler[7];
};

struct js_event_button_1 {
    unsigned char timecode[8];
    unsigned char ev_type;
    unsigned char filler[7];
};

struct js_event_button_2 {
    unsigned char timecode[8];
    unsigned char unknown1[2];
    unsigned char button_id;
    unsigned char unknown2;
    unsigned char action_type;
    unsigned char unknown3[3];
};

struct js_event_dpad {
    unsigned char timecode[8];
    unsigned char ev_type;
    unsigned char unknown1;
    unsigned char axis;
    unsigned char unknown2;
    unsigned char action_type;
    unsigned char unknown3[3];
};

struct js_event_end {
    unsigned char timecode[8];
    unsigned char filler[8];
};
#endif
