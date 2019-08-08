import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/** Ued is the simple main program for Ued */
public class Ued {

	/** This main program builds and connects Model, View and Controller */
    public static void main(String[] args) {

		// Construct a Frame to contain the View
		JFrame f = new JFrame("Ued: A Unix User Editor Wannabe");

		// Start with the Java look-and-feel, if possible
		String lnfName;
		//lnfName = "com.sun.java.swing.plaf.metal.MetalLookAndFeel";
		lnfName = "com.sun.java.swing.plaf.windows.WindowsLookAndFeel";
		try {
			UIManager.setLookAndFeel(lnfName);
			SwingUtilities.updateComponentTreeUI(f);
		} catch (Exception e) {
			System.out.println(e);
			// we don't care, program still works with default Look-n-feel
		}

		// Construct the data model
		UedModel m = new UedModel();

		// Construct the User View
		UedView uv = new UedView(m.getUserVector());

		// Construct the Group View
		// GroupView gv = new GroupView(??);

		// Build the Menu Controller and install it
		UedMenuCtl mc = new UedMenuCtl(m, f, uv);
		f.setMenuBar(mc);

		m.addObserver(uv);	// tell model about its view

		f.addWindowListener(new WindowAdapter() {
	        public void windowClosing(WindowEvent e) {
				System.exit(0);
			}
	    });

		// The Main Screen is a tabbed panel with several views
		Container c = f.getContentPane();

		JTabbedPane mainPane = new JTabbedPane();
		c.add(BorderLayout.CENTER, mainPane);
		mainPane.addTab("Users", uv);
		mainPane.addTab("Groups", new JLabel("Not Written Yet", JLabel.CENTER));
		mainPane.addTab("Properties", new JLabel("Not Written Yet", JLabel.CENTER));
		f.pack();
		f.setVisible(true);
		mainPane.setSelectedIndex(mainPane.indexOfTab("Users"));
    }
}
