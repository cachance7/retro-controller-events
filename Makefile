all: joystick_events

release: CCFLAGS += -DNDEBUG
release: joystick_events

debug: CCFLAGS += -g3
debug: joystick_events

clean:
	-rm joystick_events

joystick_events: events.c codes.h
	$(CC) $(CCFLAGS) -o joystick_events events.c
