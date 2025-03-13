#include "better_control.h"
#include <string.h>

// Forward declarations
static void refresh_bluetooth_list(GtkWidget *listbox);
static void toggle_bluetooth(GtkSwitch *switch_widget, gboolean state, gpointer user_data);
static void connect_to_device(GtkListBoxRow *row, gpointer user_data);
static char* get_device_name(void);

void connect_bluetooth_device(GtkWidget *button G_GNUC_UNUSED, const char *mac) {
    char *cmd = g_strdup_printf("bluetoothctl connect %s", mac);
    char *output = execute_command(cmd);
    g_free(cmd);
    
    if (output) {
        show_error_dialog(GTK_WIDGET(gtk_widget_get_root(button)), output);
        free(output);
    }
}

void disconnect_bluetooth_device(GtkWidget *button G_GNUC_UNUSED, const char *mac) {
    char *cmd = g_strdup_printf("bluetoothctl disconnect %s", mac);
    char *output = execute_command(cmd);
    g_free(cmd);
    
    if (output) {
        show_error_dialog(GTK_WIDGET(gtk_widget_get_root(button)), output);
        free(output);
    }
}

// Get the device name that will be visible to others
static char* get_device_name(void) {
    char *output = execute_command("bluetoothctl show | grep 'Name:' | cut -d ':' -f 2");
    if (!output) {
        return g_strdup("Unknown Device");
    }
    
    // Trim whitespace
    char *trimmed = g_strstrip(output);
    char *result = g_strdup(trimmed);
    free(output);
    return result;
}

// Check if Bluetooth is powered on
static gboolean is_bluetooth_powered(void) {
    char *output = execute_command("bluetoothctl show | grep 'Powered:' | cut -d ':' -f 2");
    if (!output) {
        return FALSE;
    }
    
    gboolean powered = (strstr(output, "yes") != NULL);
    free(output);
    return powered;
}

static void toggle_bluetooth(GtkSwitch *switch_widget, gboolean state, gpointer user_data) {
    GtkWidget *listbox = GTK_WIDGET(user_data);
    GtkWidget *visibility_label = GTK_WIDGET(g_object_get_data(G_OBJECT(switch_widget), "visibility_label"));
    
    char *output = execute_command(state ? "bluetoothctl power on" : "bluetoothctl power off");
    if (output) {
        free(output);
    }
    
    if (state) {
        // Update visibility label with device name
        char *device_name = get_device_name();
        char *visibility_text = g_strdup_printf("Your device will be visible as: %s", device_name);
        gtk_label_set_text(GTK_LABEL(visibility_label), visibility_text);
        g_free(visibility_text);
        g_free(device_name);
        
        // Refresh the device list
        refresh_bluetooth_list(listbox);
    } else {
        // Clear the list and update visibility label
        gtk_label_set_text(GTK_LABEL(visibility_label), "Bluetooth is turned off");
        
        // Clear existing items
        GtkWidget *child = gtk_widget_get_first_child(listbox);
        while (child) {
            GtkWidget *next = gtk_widget_get_next_sibling(child);
            gtk_list_box_remove(GTK_LIST_BOX(listbox), child);
            child = next;
        }
    }
}

static void connect_to_device(GtkListBoxRow *row, gpointer user_data G_GNUC_UNUSED) {
    // Get the MAC address from the row data
    const char *mac = g_object_get_data(G_OBJECT(row), "mac_address");
    if (!mac) return;
    
    // Check if already connected
    char *cmd = g_strdup_printf("bluetoothctl info %s | grep 'Connected:' | cut -d ':' -f 2", mac);
    char *output = execute_command(cmd);
    g_free(cmd);
    
    if (output) {
        gboolean is_connected = (strstr(output, "yes") != NULL);
        free(output);
        
        if (is_connected) {
            // Disconnect if already connected
            disconnect_bluetooth_device(GTK_WIDGET(row), mac);
        } else {
            // Connect if not connected
            connect_bluetooth_device(GTK_WIDGET(row), mac);
        }
        
        // Refresh the list to update connection status
        GtkWidget *listbox = gtk_widget_get_parent(GTK_WIDGET(row));
        refresh_bluetooth_list(listbox);
    }
}

static void refresh_bluetooth_list(GtkWidget *listbox) {
    // Clear existing items
    GtkWidget *child = gtk_widget_get_first_child(listbox);
    while (child) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(listbox), child);
        child = next;
    }
    
    // Get Bluetooth devices using bluetoothctl
    char *output = execute_command("bluetoothctl devices");
    if (!output) return;
    
    char *line = strtok(output, "\n");
    while (line) {
        // Format is "Device XX:XX:XX:XX:XX:XX DeviceName"
        char *device_str = strstr(line, "Device ");
        if (device_str) {
            device_str += 7; // Skip "Device "
            
            // Extract MAC address
            char mac[18] = {0};
            strncpy(mac, device_str, 17);
            mac[17] = '\0';
            
            // Extract device name
            char *name = device_str + 18; // Skip MAC address and space
            
            // Check if device is connected
            char *cmd = g_strdup_printf("bluetoothctl info %s | grep 'Connected:' | cut -d ':' -f 2", mac);
            char *conn_output = execute_command(cmd);
            g_free(cmd);
            
            gboolean is_connected = FALSE;
            if (conn_output) {
                is_connected = (strstr(conn_output, "yes") != NULL);
                free(conn_output);
            }
            
            // Create row
            GtkWidget *row = gtk_list_box_row_new();
            g_object_set_data_full(G_OBJECT(row), "mac_address", g_strdup(mac), g_free);
            
            // Create box for the row content
            GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_widget_set_margin_start(box, 10);
            gtk_widget_set_margin_end(box, 10);
            gtk_widget_set_margin_top(box, 5);
            gtk_widget_set_margin_bottom(box, 5);
            
            // Device name
            GtkWidget *name_label = gtk_label_new(name);
            gtk_widget_set_hexpand(name_label, TRUE);
            gtk_label_set_xalign(GTK_LABEL(name_label), 0.0); // Left-align
            gtk_box_append(GTK_BOX(box), name_label);
            
            // Connection status
            GtkWidget *status_label = gtk_label_new(is_connected ? "Connected" : "Not connected");
            gtk_label_set_xalign(GTK_LABEL(status_label), 1.0); // Right-align
            
            // Set color based on connection status
            if (is_connected) {
                GtkCssProvider *provider = gtk_css_provider_new();
                gtk_css_provider_load_from_string(provider, "label { color: #2ecc71; }");
                
                // Use the display-wide provider instead of the deprecated style context
                GdkDisplay *display = gtk_widget_get_display(status_label);
                gtk_style_context_add_provider_for_display(display,
                                                        GTK_STYLE_PROVIDER(provider),
                                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
                g_object_unref(provider);
            }
            
            gtk_box_append(GTK_BOX(box), status_label);
            
            // Set the box as the row's child
            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);
            gtk_list_box_append(GTK_LIST_BOX(listbox), row);
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(output);
}

void init_bluetooth_page(GtkWidget *notebook, AppState *state G_GNUC_UNUSED) {
    GtkWidget *bluetooth_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(bluetooth_box, 10);
    gtk_widget_set_margin_bottom(bluetooth_box, 10);
    gtk_widget_set_margin_start(bluetooth_box, 10);
    gtk_widget_set_margin_end(bluetooth_box, 10);
    
    // Create header with toggle switch
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    
    GtkWidget *bluetooth_label = gtk_label_new("Bluetooth");
    gtk_widget_set_hexpand(bluetooth_label, TRUE);
    gtk_label_set_xalign(GTK_LABEL(bluetooth_label), 0.0); // Left-align
    gtk_box_append(GTK_BOX(header_box), bluetooth_label);
    
    GtkWidget *bluetooth_toggle = gtk_switch_new();
    gboolean is_powered = is_bluetooth_powered();
    gtk_switch_set_active(GTK_SWITCH(bluetooth_toggle), is_powered);
    gtk_box_append(GTK_BOX(header_box), bluetooth_toggle);
    
    gtk_box_append(GTK_BOX(bluetooth_box), header_box);
    
    // Create visibility label
    GtkWidget *visibility_label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(visibility_label), 0.0); // Left-align
    gtk_widget_set_margin_bottom(visibility_label, 10);
    gtk_box_append(GTK_BOX(bluetooth_box), visibility_label);
    
    // Store visibility label reference in toggle switch
    g_object_set_data(G_OBJECT(bluetooth_toggle), "visibility_label", visibility_label);
    
    // Set initial visibility text
    if (is_powered) {
        char *device_name = get_device_name();
        char *visibility_text = g_strdup_printf("Your device will be visible as: %s", device_name);
        gtk_label_set_text(GTK_LABEL(visibility_label), visibility_text);
        g_free(visibility_text);
        g_free(device_name);
    } else {
        gtk_label_set_text(GTK_LABEL(visibility_label), "Bluetooth is turned off");
    }
    
    // Create devices list with title
    GtkWidget *devices_label = gtk_label_new("Available Devices");
    gtk_label_set_xalign(GTK_LABEL(devices_label), 0.0); // Left-align
    gtk_box_append(GTK_BOX(bluetooth_box), devices_label);
    
    // Create Bluetooth list
    GtkWidget *bt_listbox = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(bt_listbox), GTK_SELECTION_SINGLE);
    gtk_widget_set_vexpand(bt_listbox, TRUE);
    gtk_box_append(GTK_BOX(bluetooth_box), bt_listbox);
    
    // Connect signals
    g_signal_connect(bluetooth_toggle, "state-set", G_CALLBACK(toggle_bluetooth), bt_listbox);
    g_signal_connect(bt_listbox, "row-activated", G_CALLBACK(connect_to_device), NULL);
    
    // Create refresh button
    GtkWidget *refresh_button = gtk_button_new_with_label("Refresh Devices");
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(refresh_bluetooth_list), bt_listbox);
    gtk_box_append(GTK_BOX(bluetooth_box), refresh_button);
    
    // Create scrolled window
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), bluetooth_box);
    
    // Add to notebook
    GtkWidget *label = gtk_label_new("Bluetooth");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled, label);
    
    // Initial refresh if Bluetooth is powered on
    if (is_powered) {
        refresh_bluetooth_list(bt_listbox);
    }
} 