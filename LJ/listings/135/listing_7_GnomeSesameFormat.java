//   FILE: GnomeSesameFormat.java -- A GUI for sesame-format
// AUTHOR: W. Michael Petullo <mike@flyn.org>
//   DATE: 26 January 2005

// Copyright (C) 2005 W. Michael Petullo <mike@flyn.org>
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.File;
import java.io.DataOutputStream;

import java.lang.Runtime;
import java.lang.Process;
import java.lang.System;
import java.lang.InterruptedException;

import gnu.getopt.Getopt;
import gnu.getopt.LongOpt;

import org.gnu.glade.LibGlade;
import org.gnu.gtk.Gtk;
import org.gnu.gtk.Button;
import org.gnu.gtk.CheckButton;
import org.gnu.gtk.Entry;
import org.gnu.gtk.Label;
import org.gnu.gtk.ProgressBar;
import org.gnu.gtk.Window;
import org.gnu.gtk.FileSelection;
import org.gnu.gtk.event.GtkEvent;
import org.gnu.pango.FontDescription;

interface Cipher {
	public String name();
	public int keyBitCount();
}

class AES256 implements Cipher {
	public String name() { return "aes"; }
	public int keyBitCount() { return 256; }
}

class AES128 implements Cipher {
	public String name() { return "aes"; }
	public int keyBitCount() { return 128; }
}

interface FS {
	public String name();
}

class Ext3 implements FS {
	public String name() { return "ext3"; }
}

class Ext2 implements FS {
	public String name() { return "ext2"; }
}

class ProgressBarUpdater extends Thread {
	boolean stop;
	ProgressBar progressBar;

	private ProgressBarUpdater() {};

	ProgressBarUpdater(ProgressBar p) {
		stop = false;
		progressBar = p;
	}

	public void run() {
		while (stop == false) {
			while (Gtk.eventsPending()) {
				Gtk.mainIteration();
			}
			progressBar.pulse();
			try {
				Thread.sleep(100);
			} catch (java.lang.InterruptedException e) {}
		}
	}

	void stopReq() {
		stop = true;
	}
}

public class GnomeSesameFormat {
	private LibGlade glade;

	private boolean isDryRun;
	private Cipher cipher;
	private FS fs;
	private String passphrase;
	private String volName;
	private String device;

	private Window topLevel;
	private FileSelection devSelUI;
	private Window errUI;
	private Window progressUI;

	private void init() throws IOException {
	        glade = new LibGlade(System.getProperty("GLADE_FILE"), this);

		// Default values.
		isDryRun = false;
		cipher = new AES256();
		fs = new Ext3();
		passphrase = null;
		volName = null;

		// References to various Windows used by application.
		topLevel = (Window) glade.getWidget("topLevel");
		devSelUI = (FileSelection) glade.getWidget("devSelUI");
		errUI = (Window) glade.getWidget("errUI");
		progressUI = (Window) glade.getWidget("progressUI");
	}

	public GnomeSesameFormat() throws IOException {
		init();
		device = null;

		Label l = (Label) glade.getWidget("displayedDevice");
		l.setText("none selected");
	}

	public GnomeSesameFormat(String d) throws IOException {
		init();
		device = d;

		Label l = (Label) glade.getWidget("labelDevice");
		l.setSensitive(false);

		Button b = (Button) glade.getWidget("buttonOpen");
		b.setSensitive(false);

		l = (Label) glade.getWidget("displayedDevice");
		l.setText(d);
		l.setSensitive(false);
	}

	private void error(String msg) {
		Label l = (Label) glade.getWidget("labelErr");
		l.setText(msg);
		topLevel.setSensitive(false);
		errUI.show();
	}

	public void onTopLevelDeleteEvent(GtkEvent event) {
		Gtk.mainQuit();
		System.exit(0);
	}

	public void onAES128Activate(GtkEvent event) {
		cipher = new AES128();
	}

	public void onAES256Activate(GtkEvent event) {
		cipher = new AES256();
	}

	public void onExt2Activate(GtkEvent event) {
		fs = new Ext2();
	}

	public void onExt3Activate(GtkEvent event) {
		fs = new Ext3();
	}

	public void onCloseButtonClicked(GtkEvent event) {
		Gtk.mainQuit();
		System.exit(0);
	}

	public void onHelpButtonClicked(GtkEvent event) {
		// FIXME
	}

	public void onErrOkButtonClicked(GtkEvent event) {
		errUI.hide();
		topLevel.setSensitive(true);
	}

	public void onDevSelCancelButtonClicked(GtkEvent event) {
		devSelUI.hide();
		topLevel.setSensitive(true);
	}

	private boolean topLevelInputOk() {
		boolean fnval = false;

		if (device == null)
			error("Device not selected");
		else if (passphrase.equals(""))
			error("Passphrase not entered");
		else if (volName.equals(""))
			error("Volume name not entered");
		else
			fnval = true;

		return fnval;
	}

	public void onFormatButtonClicked(GtkEvent event) {
		Entry entry;
		
		entry = (Entry) glade.getWidget("entryPassphrase");
		passphrase = entry.getText();

		entry = (Entry) glade.getWidget("entryVolumeName");
		volName = entry.getText();

		if (topLevelInputOk ()) {
			ProgressBar p = (ProgressBar) 
				glade.getWidget("progressBarFormat");
			Label l = (Label) glade.getWidget("labelFormat");
			ProgressBarUpdater pU = new ProgressBarUpdater(p);

			topLevel.setSensitive(false);
			progressUI.show();

			if (! isDryRun) {
				l.setText("Formatting " + device);	
				pU.start();
				execSesameFormat();
				pU.stopReq();
				try {
					pU.join();
				} catch (java.lang.InterruptedException e) {}
			} else {
				l.setText("[Simulated] Formatting " + device);	
				pU.start();
				try {
					Thread.sleep(1000);
				} catch (java.lang.InterruptedException e) {}
				pU.stopReq();
				try {
					pU.join();
				} catch (java.lang.InterruptedException e) {}
			}

			progressUI.hide();
			topLevel.setSensitive(true);
		}
	}

	public void onOpenButtonClicked(GtkEvent event) {
		topLevel.setSensitive(false);
		devSelUI.show();
	}

	private boolean devSelInputOk(String path) {
		boolean fnval = false;

		if (! new File (path).exists())
			error("Device " + path + " does not exist");
		else
			fnval = true;

		return fnval;
	}

	public void onDevSelOkButtonClicked(GtkEvent event) {
		String tmp = devSelUI.getFilename();
		if (devSelInputOk(tmp)) {
			device = tmp;

			Label l = (Label) glade.getWidget("displayedDevice");
			l.setText(device);

			devSelUI.hide();
			topLevel.setSensitive(true);
		}
	}

	private String buildCommand() {
		CheckButton c = (CheckButton) glade.getWidget
		                              ("checkButtonBadBlocks");

		StringBuffer cmd = new StringBuffer();
		String progpath = System.getProperty("SESAME_FORMAT");

		cmd.append(progpath);
		cmd.append(" --fs-type=");
		cmd.append(fs.name());
		cmd.append(" --fs-cipher=");
		cmd.append(cipher.name());
		cmd.append(" --fs-keylen=");
		cmd.append(cipher.keyBitCount());
		cmd.append(" ");
		if (c.getState()) {
			cmd.append("-c");
			cmd.append(" ");
		}
		cmd.append("--volume-name=");
		cmd.append(volName);
		cmd.append(" ");
		cmd.append(device);

		return cmd.toString();
	}

	private void execSesameFormat() {
		if (! isDryRun) 
			try {
				// Execute format process and provide password.
				Process p = Runtime.getRuntime().exec
							(buildCommand());
				DataOutputStream o = new DataOutputStream
							(p.getOutputStream());

				o.writeBytes(passphrase);
				o.flush();
				o.close();

				if (p.waitFor() != 0)
					throw new InterruptedException ();

			} catch (SecurityException e) {
				error("Could not find sesame-format");
			} catch (NullPointerException e) {
				error("Could not find sesame-format");
			} catch (IllegalArgumentException e) {
				error("Could not find sesame-format");
			} catch (IOException e) {
				error("Error executing sesame-format");
			} catch (InterruptedException e) {
				error("Error executing sesame-format");
			} 
	}

	private void setDryRun(boolean setting) {
		isDryRun = setting;
	}

	private static void printUsage(int status, String error, String more) {
		System.err.println("gnome-sesame-setup [options] [device]\n\n" +
		  "-h, --help\n" +
		  "      print a list of options\n\n" +
		  "-d, --dry-run\n" +
		  "      do not actually perform any operations on device\n");
		if (error != null)
			System.err.println(error + ": " + more);
		System.exit(status);
	}

	private static void main() {}

	public static void main(String args[]) {
		GnomeSesameFormat gui = null;

		Gtk.init(args);

		LongOpt[] longOpt = new LongOpt[2];
		longOpt[0] = new LongOpt("help", LongOpt.NO_ARGUMENT, null, 
					 'h');
		longOpt[1] = new LongOpt("dry-run", LongOpt.NO_ARGUMENT, null,
					 'd');

		Getopt g = new Getopt("gnome-sesame-format", args, "hd", 
				      longOpt);
		
		int c;
		boolean optDryRun = false;
		while ((c = g.getopt()) != -1)
			switch (c) {
			case 'h':
				printUsage(0, null, null);
			case 'd':
				optDryRun = true;
				break;
			default:
				printUsage(1, null, null);
		}

		try {
			int i = g.getOptind();
			if (i == 1)
				gui = new GnomeSesameFormat(args[i]);
			else if (i > 1)
				printUsage(1, null, null);
			else
				gui = new GnomeSesameFormat();
			gui.setDryRun(optDryRun);

			Gtk.main();
		} catch (Exception e) {
			System.err.println(e);
			System.exit(1);
		}
	}

}
