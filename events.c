#include "dbg.h"
#include "codes.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
 * Extracts a single event from the input stream and assigns
 * the two char reference arguments.
 */
void get_event(FILE *, unsigned char *, unsigned char *);

/*
 * The button message has an additional 16byte payload.
 * This function reads it from the input.
 */
void get_button_event(FILE *, unsigned char *, unsigned char *);

/*
 * Writes event description to provided stream.
 */
void emit_event(FILE *, unsigned char, unsigned char);

/*
 * Both button and d-pad messages have a 16byte footer.
 * This function reads it from the input to progress the stream.
 */
void flush_footer(FILE *);

int main(int argc, char *argv[]) {
    FILE *file, *out = stdout;
    if (argc > 1) {
        debug("attempting to open arg");
        file = fopen(argv[1], "r");
        if (file == NULL) {
            exit(1); // Could not open file for reading
        }
    } else {
        debug("using stdin");
        file = stdin;
    }

    /* Extract and emit events one at a time */
    unsigned char btn, dir;
    for(;;) {
        get_event(file, &btn, &dir);
        emit_event(out, btn, dir);
    }
}

void get_event(FILE * file, unsigned char *btn, unsigned char *dir) {
    unsigned char btn_code;
    unsigned char btn_dir;
    size_t bytes_read;
    short pos = 0;
    struct js_event_unknown event;
    // Get first message in event
    bytes_read = fread(&event, 1, MESSAGE_SIZE - pos, file);
    pos += bytes_read;
    if (pos == MESSAGE_SIZE) {
        debug("Got enough bytes");
        // Decode event type
        unsigned char ev_type = event.ev_type;
        debug("ev_type is %d", ev_type);
        if (ev_type == EV_TYPE_BUTTON) {
            debug("Button event");
            get_button_event(file, btn, dir);
            flush_footer(file);
        } else if (ev_type == EV_TYPE_DPAD) {
            debug("D-Pad event");
            struct js_event_dpad *dpad_event = &event;
            *btn = dpad_event->axis;
            *dir = dpad_event->action_type;
            flush_footer(file);
        } else {
            log_warn("Event type %c not recognized", ev_type);
        }
        return;
    } else {
        sleep(50);
    }
}

void get_button_event(FILE *file, unsigned char *btn, unsigned char *dir) {
    short message_read = 0;
    size_t bytes_read;
    char pos = 0;
    struct js_event_button_2 event;
    for(;;) {
        // Get first message in event
        bytes_read = fread(&event, 1, MESSAGE_SIZE - pos, file);
        pos += bytes_read;
        if (pos == MESSAGE_SIZE) {
            *btn = event.button_id;
            *dir = event.action_type;
            return;
        } else {
            sleep(50);
        }
    }
}

void flush_footer(FILE *file) {
    size_t bytes_read;
    char pos = 0;
    unsigned char buffer[MESSAGE_SIZE];
    for(;;) {
        // Get first message in event
        bytes_read = fread(&buffer[pos], 1, MESSAGE_SIZE - pos, file);
        pos += bytes_read;
        if (pos == MESSAGE_SIZE) {
            return;
        }
    }
}

void emit_event(FILE *file, unsigned char code, unsigned char dir) {
    char * btn = NULL, *action;

    /* Straightforward button code mappings */
    if (code == BTN_X) { btn = "X"; }
    else if (code == BTN_A) { btn = "A"; }
    else if (code == BTN_B) { btn = "B"; }
    else if (code == BTN_Y) { btn = "Y"; }
    else if (code == BTN_L) { btn = "L"; }
    else if (code == BTN_R) { btn = "R"; }
    else if (code == BTN_SELECT) { btn = "Select"; }
    else if (code == BTN_START)  { btn = "Start"; }

    /*
     * This is ugly but the /dev/input messages aren't exactly pretty either
     * May want to emit something else later but this works for a start.
     */
    if (dir == BTN_DOWN) {
        action = "pressed";
    } else if (dir == DPAD_NEUTRAL) {
        if (code == DPAD_X_AXIS) {
            btn = "X_Axis";
        } else if (code == DPAD_Y_AXIS) {
            btn = "Y_Axis";
        }
        action = "released";
    } else if (dir == DPAD_POS) {
        if (code == DPAD_X_AXIS) {
            btn = "Right";
        } else if (code == DPAD_Y_AXIS) {
            btn = "Up";
        }
        action = "pressed";
    } else if (btn == NULL && dir == DPAD_NEG) {
        if (code == DPAD_X_AXIS) {
            btn = "Left";
        } else if (code == DPAD_Y_AXIS) {
            btn = "Down";
        }
        action = "pressed";
    } else { /* Button was released */
        action = "released";
    }

    fprintf(file, btn);
    fprintf(file, " ");
    fprintf(file, action);
    fprintf(file, "\n");
}

