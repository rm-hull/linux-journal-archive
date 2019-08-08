import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;

public class UedMenuCtl extends MenuBar {
	UedModel um;
	Frame    uf;
	UedView  uv;

	UedMenuCtl(UedModel um, Frame uf, UedView uv) {
		this.um = um;
		this.uf = uf;
		this.uv = uv;

		Menu m;
		MenuItem mi;

		ResourceBundle b = ResourceBundle.getBundle("Menus");
		String menuLabel;
		try { menuLabel = b.getString("file"+".label"); }
		catch (MissingResourceException e) { menuLabel="file"; }

		Menu fm = new Menu(menuLabel);
		fm.add(mi = mkMenuItem(b, "file", "open"));
		mi.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				System.out.println("Open not written yet");
			}
		});
		fm.add(mi = mkMenuItem(b, "file", "new"));
		mi.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				System.out.println("NewFile not written yet");
			}
		});
		fm.add(mi = mkMenuItem(b, "file", "save"));
		mi.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				System.out.println("Save not written yet");
			}
		});
		fm.add(mi = mkMenuItem(b, "file", "exit"));
		mi.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				// uf.setVisible(false);
				// uf.dispose();
				System.exit(0);
			}
		});
		add(fm);

		// Menu vm = mkMenu(b,  "view");
		// vm.add(mi = mkMenuItem(b, "view", "tree"));
		// vm.add(mi = mkMenuItem(b, "view", "list"));
		// vm.add(mi = mkMenuItem(b, "view", "longlist"));
		// add(vm);

		Menu hm = mkMenu(b,  "help");
		hm.add(mi = mkMenuItem(b, "help", "about"));
		mi.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				JOptionPane.showMessageDialog(
				UedMenuCtl.this.uf,
				"   About UED\n" +
					"\n" +
					"Ued (User Editor) (c) by Ian Darwin\n" +
					"http://www.darwinsys.com/freeware\n",
				"About Ued", JOptionPane.INFORMATION_MESSAGE);
			}
		});
		setHelpMenu(hm);		// needed for portability (Motif, etc.).
	}

	/** Convenience routine to make a Menu */
	public Menu mkMenu(ResourceBundle b, String name) {
		String menuLabel;
		try { menuLabel = b.getString(name+".label"); }
		catch (MissingResourceException e) { menuLabel=name; }
		return new Menu(menuLabel);
	}

	/** Convenience routine to make a MenuItem */
	public MenuItem mkMenuItem(ResourceBundle b, String menu, String name) {
		String miLabel;
		try { miLabel = b.getString(menu + "." + name + ".label"); }
		catch (MissingResourceException e) { miLabel=name; }
		String key = null;
		try { key = b.getString(menu + "." + name + ".key"); }
		catch (MissingResourceException e) { key=null; }

		if (key == null)
			return new MenuItem(miLabel);
		else
			return new MenuItem(miLabel, new MenuShortcut(key.charAt(0)));
	}

}
