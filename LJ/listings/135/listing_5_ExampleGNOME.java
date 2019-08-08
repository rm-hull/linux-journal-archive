import java.io.IOException;

import org.gnu.glade.LibGlade;
import org.gnu.gtk.Gtk;
import org.gnu.gtk.event.GtkEvent;
import org.gnu.pango.FontDescription;

public class ExampleGNOME {
    private LibGlade libglade;
    private static final String GLADE_FILE = "ExampleGNOME.glade";

    public ExampleGNOME () throws IOException {
	libglade = new LibGlade(GLADE_FILE, this);
    }

    public void on_noButton_released(GtkEvent event) {
	Gtk.mainQuit();
        System.exit(1);
    }

    public void on_yesButton_released(GtkEvent event) {
	Gtk.mainQuit();
        System.exit(0);
    }

    public static void main(String args[]) {
	ExampleGNOME gui;

	Gtk.init(args);
	try {
	    gui = new ExampleGNOME();
	} catch (IOException e) {
	    System.err.println(e);
	    System.exit(1);
	}
	Gtk.main();
    }
}
