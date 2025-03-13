#include "better_control.h"

// Function to set up CSS to fix GTK warnings
static void setup_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider,
        "image { min-width: 16px; min-height: 16px; }");
    
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    g_object_unref(provider);
}

static void activate(GtkApplication *app, gpointer user_data G_GNUC_UNUSED) {
    AppState *state = g_new(AppState, 1);
    
    // Set up CSS to fix GTK warnings
    setup_css();
    
    // Create main window
    state->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(state->window), "Better Control");
    gtk_window_set_default_size(GTK_WINDOW(state->window), 1000, 700);
    gtk_window_set_resizable(GTK_WINDOW(state->window), TRUE);
    
    // Create main container
    GtkWidget *main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(state->window), main_container);
    
    // Create notebook
    state->notebook = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(state->notebook), TRUE);
    gtk_box_append(GTK_BOX(main_container), state->notebook);
    
    // Initialize pages
    init_wifi_page(state->notebook, state);
    init_bluetooth_page(state->notebook, state);
    init_audio_page(state->notebook, state);
    init_brightness_page(state->notebook, state);
    
    // Present the window instead of using deprecated gtk_widget_show
    gtk_window_present(GTK_WINDOW(state->window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    
    // Use G_APPLICATION_DEFAULT_FLAGS instead of deprecated G_APPLICATION_FLAGS_NONE
    app = gtk_application_new("org.gtk.better-control", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
} 