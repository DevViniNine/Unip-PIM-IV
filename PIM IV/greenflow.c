#include <gtk/gtk.h>
#include <stdlib.h>

GtkBuilder *builder;
GtkWidget *window;


void on_main_window_destroy(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

int main(int argc, char *argv[]){
    gtk_init(&argc, &argv);
    GtkBuilder *builder = gtk_builder_new_from_file("user_interface.glade");

    gtk_builder_add_callback_symbols(builder,

                                     "on_main_window_destroy", G_CALLBACK(on_main_window_destroy),NULL);


gtk_builder_connect_signals(builder, NULL);
GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

gtk_widget_show_all(window);
gtk_main();

return 0;
}
