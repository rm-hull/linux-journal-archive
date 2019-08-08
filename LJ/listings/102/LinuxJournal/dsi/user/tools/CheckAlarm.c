
/* Set the Node ID of DSI cluster architecture */

#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <unistd.h>
#include <time.h>
#include "dsi.h"

#ifdef GTK_GUI
#include <gtk/gtk.h>
#endif


#define MAX_MSG_CHARS 50

_syscall3(int, security, unsigned int, id, unsigned int, call, unsigned int *, args);

#ifdef GTK_GUI
typedef struct
{
	GtkWidget *view;
	GtkTextBuffer *text;
} mystruct;


/* Every GTK timeout, function is called to check if any alarms are present */

gint update_client( mystruct *data )
{
	int ret;
	unsigned int id	= DSI_MAGIC;
	unsigned int call 	= DSI_ALARM_CHECK;
	GtkTextIter iter;
	gchar entry_text[MAX_MSG_CHARS];
	char *date;
	time_t calendar = time(NULL);
	struct tm *system_time = localtime(&calendar);

	date = asctime(system_time);

	ret = security( id, call, NULL);

	if ( ret > 0 ) {
		sprintf(entry_text, "Alarm trapped at %s", date);
		gtk_text_buffer_insert_at_cursor( data->text, entry_text, -1 );
    
		if ( ret & 0x10 ) {
			sprintf(entry_text, "Unable to receive on socket\n");
			gtk_text_buffer_insert_at_cursor( data->text, entry_text, -1 );
		}

		if ( ret & 0x8 ) {
			sprintf(entry_text, "Unable to send data to socket\n");
			gtk_text_buffer_insert_at_cursor( data->text, entry_text, -1 );
		}

		if ( ret & 0x4 ) {
			sprintf(entry_text, "Unable to connect to socket\n");
			gtk_text_buffer_insert_at_cursor( data->text, entry_text, -1 );
		}

		if ( ret & 0x2 ) {
			sprintf(entry_text, "Unable to create socket\n");
			gtk_text_buffer_insert_at_cursor( data->text, entry_text, -1 );
		}

		if ( ret & 0x1 ) {
			sprintf(entry_text, "Unable to fork task\n");
			gtk_text_buffer_insert_at_cursor( data->text, entry_text, -1 );
		}
      
		gtk_text_buffer_insert_at_cursor( data->text, "\n", -1 );
		gtk_text_buffer_get_end_iter( data->text, &iter);
		gtk_text_view_scroll_to_iter( GTK_TEXT_VIEW( data->view ), &iter, 0, 
					      FALSE, 0, 0);
	}

	return 1;
}


/* GTK interface */

void gtk_gui( int argc, char *argv[] )
{
	static mystruct data;
	static GtkWidget *window, *vbox, *scrolled_window, *client_view, *button;
	static GtkTextBuffer *client_text = NULL;
	static gint timeout = 0;

	gtk_init (&argc, &argv);

	/* create a new window */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (GTK_WIDGET (window), 300, 220);
	gtk_window_set_title (GTK_WINDOW (window), "Alarm monitor");
	g_signal_connect (G_OBJECT (window), "destroy",
			  G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect_swapped (G_OBJECT (window), "delete_event",
				  G_CALLBACK (gtk_widget_destroy), 
				  window);
  
	/* Main text vertic layout window */
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (window), vbox);

	/* Client text window */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 
					10);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW 
					(scrolled_window),
					GTK_POLICY_AUTOMATIC, 
					GTK_POLICY_AUTOMATIC);
  
	client_view = gtk_text_view_new();
	gtk_text_view_set_editable( GTK_TEXT_VIEW( client_view ), FALSE );
	gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW( client_view ), TRUE );
	gtk_text_view_set_cursor_visible( GTK_TEXT_VIEW( client_view ), 
					  FALSE );
  
	client_text = gtk_text_view_get_buffer (GTK_TEXT_VIEW 
						(client_view));
  
	gtk_container_add( GTK_CONTAINER(scrolled_window), client_view );
	gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 2);
	gtk_widget_show (client_view);
	gtk_widget_show (scrolled_window);
  
	/* 'Close' button */
	button = gtk_button_new_with_label ("Close");
	g_signal_connect (G_OBJECT (button), "clicked",
			  G_CALLBACK (gtk_main_quit), NULL);
	gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
	gtk_widget_show (button);
  
	/* Main loop */

	data.view = client_view;
	data.text = client_text;

	timeout = gtk_timeout_add(1000, (GtkFunction)update_client, &data);
	gtk_widget_show (window);

	gtk_main();
	gtk_timeout_remove( timeout );
}

#endif    /* GTK_GUI */


/* Main */

int main( int argc, char *argv[] )
{
#ifdef GTK_GUI
	gtk_gui(argc, argv);
#else
	int ret;
	unsigned int id	= DSI_MAGIC;
	unsigned int call = DSI_ALARM_CHECK;
	char *date;
	time_t calendar = time(NULL);
	struct tm *system_time = localtime(&calendar);

	while (1) {
		ret = security( id, call, NULL);

		if ( ret > 0 ) {
			calendar = time(NULL);
			system_time = localtime(&calendar);
			date = asctime(system_time);
			
			printf("Alarm trapped at %s", date);
    
			if ( ret & 0x10 ) printf("Unable to receive on socket\n");
			if ( ret & 0x8 ) printf("Unable to send data to socket\n");
			if ( ret & 0x4 ) printf("Unable to connect to socket\n");      
			if ( ret & 0x2 ) printf("Unable to create socket\n");      
			if ( ret & 0x1 ) printf("Unable to fork task\n");
      
			printf("\n");

		}
		sleep(1);
	}
    
#endif /* GTK_GUI */

	return 0;
}


