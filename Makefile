CC = gcc
CFLAGS = -Wall -Wextra -g $(shell pkg-config --cflags gtk4 dbus-1 dbus-glib-1)
LDFLAGS = $(shell pkg-config --libs gtk4 dbus-1 dbus-glib-1) -lbluetooth

SRCS = src/main.c src/wifi.c src/bluetooth.c src/audio.c src/brightness.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = better-control

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install:
	install -Dm755 $(TARGET) $(DESTDIR)/usr/bin/$(TARGET)
	install -Dm644 src/control.desktop $(DESTDIR)/usr/share/applications/control.desktop

uninstall:
	rm -f $(DESTDIR)/usr/bin/$(TARGET)
	rm -f $(DESTDIR)/usr/share/applications/control.desktop
