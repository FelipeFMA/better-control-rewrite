#ifndef BETTER_CONTROL_H
#define BETTER_CONTROL_H

#include <gtk/gtk.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

// Common structures
typedef struct {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *wifi_page;
    GtkWidget *bluetooth_page;
    GtkWidget *audio_page;
    GtkWidget *brightness_page;
} AppState;

// Function declarations
void init_wifi_page(GtkWidget *notebook, AppState *state);
void init_bluetooth_page(GtkWidget *notebook, AppState *state);
void init_audio_page(GtkWidget *notebook, AppState *state);
void init_brightness_page(GtkWidget *notebook, AppState *state);

// Utility functions
void show_error_dialog(GtkWidget *parent, const char *message);
char *execute_command(const char *command);
int get_current_volume(void);
void set_volume(int volume);
int get_current_mic_volume(void);
void set_mic_volume(int volume);
int get_current_brightness(void);
void set_brightness(int brightness);

// Bluetooth functions
void connect_bluetooth_device(GtkWidget *button, const char *mac);
void disconnect_bluetooth_device(GtkWidget *button, const char *mac);

#endif // BETTER_CONTROL_H 