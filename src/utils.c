#include "better_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void show_error_dialog(GtkWidget *parent, const char *message) {
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gtk_widget_get_root(parent)));
    gtk_window_set_title(GTK_WINDOW(dialog), "Error");
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(box, 10);
    gtk_widget_set_margin_end(box, 10);
    gtk_widget_set_margin_top(box, 10);
    gtk_widget_set_margin_bottom(box, 10);
    
    GtkWidget *label = gtk_label_new(message);
    gtk_box_append(GTK_BOX(box), label);
    
    GtkWidget *button = gtk_button_new_with_label("OK");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_box_append(GTK_BOX(box), button);
    
    gtk_window_set_child(GTK_WINDOW(dialog), box);
    gtk_window_present(GTK_WINDOW(dialog));
}

char *execute_command(const char *command) {
    if (!command || strlen(command) == 0) {
        return NULL;
    }
    
    FILE *pipe = popen(command, "r");
    if (!pipe) {
        return strdup("Error executing command");
    }
    
    char *output = NULL;
    char buffer[1024];
    size_t total_size = 0;
    
    while (fgets(buffer, sizeof(buffer), pipe)) {
        size_t len = strlen(buffer);
        char *new_output = realloc(output, total_size + len + 1);
        if (!new_output) {
            if (output) {
                free(output);
            }
            pclose(pipe);
            return strdup("Memory allocation error");
        }
        output = new_output;
        memcpy(output + total_size, buffer, len + 1);
        total_size += len;
    }
    
    int status = pclose(pipe);
    if (status != 0 && !output) {
        return strdup("Command execution failed");
    }
    
    // If no output was captured but command succeeded, return empty string
    if (!output) {
        return strdup("");
    }
    
    return output;
}

int get_current_volume(void) {
    char *output = execute_command("pactl get-sink-volume @DEFAULT_SINK@");
    if (!output) return 50;
    
    int volume = 50;
    char *percent = strstr(output, "%");
    if (percent) {
        *percent = '\0';
        volume = atoi(strrchr(output, '/') + 1);
    }
    
    free(output);
    return volume;
}

int get_current_mic_volume(void) {
    char *output = execute_command("pactl get-source-volume @DEFAULT_SOURCE@");
    if (!output) return 50;
    
    int volume = 50;
    char *percent = strstr(output, "%");
    if (percent) {
        *percent = '\0';
        volume = atoi(strrchr(output, '/') + 1);
    }
    
    free(output);
    return volume;
}

int get_current_brightness(void) {
    char *output = execute_command("brightnessctl get");
    if (!output) return 50;
    
    int brightness = atoi(output);
    free(output);
    
    // Convert raw brightness to percentage
    char *max_output = execute_command("brightnessctl max");
    if (!max_output) return 50;
    
    int max_brightness = atoi(max_output);
    free(max_output);
    
    return (brightness * 100) / max_brightness;
} 