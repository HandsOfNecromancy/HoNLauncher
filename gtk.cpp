#include <string>
#include <gtk/gtk.h>
#include "strings.h"
#include "launcher.h"

void on_combo_changed(GtkComboBox *combo, gpointer user_data)
{
	gint index = gtk_combo_box_get_active(combo);
	vidmode = (int)index;
}

void on_radio1_toggled(GtkRadioButton *radio, gpointer user_data)
{
	userlang = 0;
}

void on_radio2_toggled(GtkRadioButton *radio, gpointer user_data)
{
	userlang = 1;
}

void on_radio3_toggled(GtkRadioButton *radio, gpointer user_data)
{
	userlang = 2;
}

void on_radio4_toggled(GtkRadioButton *radio, gpointer user_data)
{
	userlang = 3;
}

void on_radio5_toggled(GtkRadioButton *radio, gpointer user_data)
{
	userlang = 4;
}

void on_radio6_toggled(GtkRadioButton *radio, gpointer user_data)
{
	userlang = 5;
}

static void on_button_clicked(GtkWidget *widget, gpointer data)
{
	LaunchGame();
	gtk_main_quit();
}

static void on_image_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button == 1) // Left mouse button
	{
		LaunchExtras();
	}
}

int main(int argc, char *argv[]) {
	LoadSettings();

	std::string commandLine;
	for (int i = 0; i < argc; ++i)
	{
		commandLine += argv[i];
		if (i < argc - 1)
		{
			commandLine += " ";
		}
	}
	commandline = new char[commandLine.length() + 1];
	strcpy(commandline, commandLine.c_str());

	AdjustSettings();

	if (skiplauncher)
	{
		// short circuit window creation, it's not necessary
		LaunchGame();
		return 0;
	}

	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), GAMENAME);
	gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DIALOG);

	GtkWidget *fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);

	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("startup.png", NULL);
	GtkWidget *background = gtk_image_new_from_pixbuf(pixbuf);
	gtk_widget_set_halign(background, GTK_ALIGN_FILL);
	gtk_widget_set_valign(background, GTK_ALIGN_FILL);
	gtk_widget_set_hexpand(background, TRUE);
	gtk_widget_set_vexpand(background, TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), background, 0, 0);

	GtkWidget *combo = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), VIDMODE1);
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), VIDMODE2);
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), VIDMODE3);
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), VIDMODE4);
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), vidmode);
	gtk_widget_set_size_request(combo, 200, -1);
	gtk_fixed_put(GTK_FIXED(fixed), combo, 14, 10);

	g_signal_connect(combo, "changed", G_CALLBACK(on_combo_changed), NULL);

	GtkWidget *button1 = gtk_button_new_with_label(BTPLAY);
	gtk_widget_set_size_request(button1, 100, -1);
	gtk_fixed_put(GTK_FIXED(fixed), button1, 398, 10);

	GtkWidget *button2 = gtk_button_new_with_label(BTQUIT);
	gtk_widget_set_size_request(button2, 100, -1);
	gtk_fixed_put(GTK_FIXED(fixed), button2, 526, 10);

	GtkWidget *radio1 = gtk_radio_button_new_with_label(NULL, LANGEN);
	gtk_fixed_put(GTK_FIXED(fixed), radio1, 10, 300);

	GtkWidget *radio2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), LANGFR);
	gtk_fixed_put(GTK_FIXED(fixed), radio2, 10, 330);

	GtkWidget *radio3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), LANGDE);
	gtk_fixed_put(GTK_FIXED(fixed), radio3, 10, 360);

	GtkWidget *radio4 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), LANGRU);
	gtk_fixed_put(GTK_FIXED(fixed), radio4, 10, 390);

	GtkWidget *radio5 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), LANGES);
	gtk_fixed_put(GTK_FIXED(fixed), radio5, 10, 420);

	GtkWidget *radio6 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), LANGPT);
	gtk_fixed_put(GTK_FIXED(fixed), radio6, 10, 450);

	// Get the screen and its resolution
	GdkScreen *dpiscreen = gdk_screen_get_default();
	gdouble dpi = gdk_screen_get_resolution(dpiscreen);

	// Calculate the size of the image based on the DPI
	int width = (int)(160 * dpi / 96); // 96 is the standard DPI
	int height = (int)(120 * dpi / 96);

	// Create a GdkPixbuf from the image file
	GdkPixbuf *pixbuf_extras = gdk_pixbuf_new_from_file_at_scale("extras.png", width, height, TRUE, NULL);
	if (pixbuf_extras == NULL)
	{
		//g_printerr("Failed to load image file 'extras.png'\n");
		//return 1;
	}
	else
	{
		// Create a GtkImage from the GdkPixbuf
		GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf_extras);

		// Create a GtkEventBox to handle the click event
		GtkWidget *event_box = gtk_event_box_new();
		gtk_container_add(GTK_CONTAINER(event_box), image);

		// Connect the "button-press-event" signal to the handler function
		g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_image_clicked), NULL);

		// Add the GtkEventBox to the GtkFixed container
		gtk_fixed_put(GTK_FIXED(fixed), event_box, 470, 350); // Adjust these values to position the image
	}

	if (userlang == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio1), TRUE);
	else if (userlang == 1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio2), TRUE);
	else if (userlang == 2)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio3), TRUE);
	else if (userlang == 3)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio4), TRUE);
	else if (userlang == 4)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio5), TRUE);
	else if (userlang == 5)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio6), TRUE);

	g_signal_connect(radio1, "toggled", G_CALLBACK(on_radio1_toggled), NULL);
	g_signal_connect(radio2, "toggled", G_CALLBACK(on_radio2_toggled), NULL);
	g_signal_connect(radio3, "toggled", G_CALLBACK(on_radio3_toggled), NULL);
	g_signal_connect(radio4, "toggled", G_CALLBACK(on_radio4_toggled), NULL);
	g_signal_connect(radio5, "toggled", G_CALLBACK(on_radio5_toggled), NULL);
	g_signal_connect(radio6, "toggled", G_CALLBACK(on_radio6_toggled), NULL);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect(button1, "clicked", G_CALLBACK(on_button_clicked), NULL);
	g_signal_connect(button2, "clicked", G_CALLBACK(gtk_main_quit), NULL);

	GtkCssProvider *provider = gtk_css_provider_new();
	GdkDisplay *display = gdk_display_get_default();
	GdkScreen *screen = gdk_display_get_default_screen(display);
	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

	const char *css = "radiobutton label { color: white; }";

	gtk_css_provider_load_from_data(provider, css, -1, NULL);

	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
