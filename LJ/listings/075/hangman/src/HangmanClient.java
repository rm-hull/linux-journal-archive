/*

Three-tier Hangman
Copyright (C) 2000 Ariel Ortiz mailto:aortiz@acm.org

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
02111-1307, USA.

*/

import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.io.*;

/**
 * Hangman GUI client.
 * @author: Ariel Ortiz
 */
public class HangmanClient extends Applet {
	private Choice ivjcategoryChoice = null;
	private Button ivjgiveUpButton = null;
	private Label ivjmysteryLabel = null;
	private Panel ivjPanel1 = null;
	private Panel ivjPanel2 = null;
	private GridLayout ivjPanel2GridLayout = null;
	private Panel ivjPanel3 = null;
	private FlowLayout ivjPanel3FlowLayout = null;
	private Label ivjremainingOpportunitiesLabel = null;
	private Button ivjstartButton = null;
	private GridLayout ivjPanel1GridLayout = null;
	private StickMan stickMan;
	private Socket socket = null;
	private BufferedReader input = null;
	private PrintWriter output = null;
	private Button[] buttons = null;
	IvjEventHandler ivjEventHandler = new IvjEventHandler();
	private Label ivjcategoriesLabel = null;
	private BorderLayout ivjHangmanClientBorderLayout = null;

class IvjEventHandler implements java.awt.event.ActionListener {
		public void actionPerformed(java.awt.event.ActionEvent e) {
			if (e.getSource() == HangmanClient.this.getstartButton()) 
				connEtoC1(e);
			if (e.getSource() == HangmanClient.this.getgiveUpButton()) 
				connEtoC2(e);
		};
	};
/**
 * Close client side socket and input/output streams.
 */
private void close() {
	try {
		if (output != null) {
			output.close();
		}
		if (input != null) {
			input.close();
		}
		if (socket != null) {
			socket.close();
		}
	} catch (IOException ex) {
	}
}
/**
 * connEtoC1:  (startButton.action.actionPerformed(java.awt.event.ActionEvent) --> HangmanClient.startButton_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC1(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.startButton_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC2:  (giveUpButton.action.actionPerformed(java.awt.event.ActionEvent) --> HangmanClient.giveUpButton_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC2(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.giveUpButton_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * Enable the appropriate components in order to start a new game.
 */
private void enableGame() {
	getstartButton().setEnabled(false);
	getgiveUpButton().setEnabled(true);
	getcategoriesLabel().setEnabled(false);
	getcategoryChoice().setEnabled(false);
	for (int i = 0; i < buttons.length; i++) {
		buttons[i].setEnabled(true);
	}
}
/**
 * Enable the appropriate components prior to the start of the game.
 */
private void enablePreGame() {
	getstartButton().setEnabled(true);
	getgiveUpButton().setEnabled(false);
	getcategoriesLabel().setEnabled(true);
	getcategoryChoice().setEnabled(true);
	for (int i = 0; i < buttons.length; i++) {
		buttons[i].setEnabled(false);
	}
}
/**
 * Returns information about this applet.
 * @return a string of information about this applet
 */
public String getAppletInfo() {
	return "HangmanClient\n" + "\n" + "Applet client for the Three-tier Hangman.\n" + "Copyright (C) 2000 Ariel Ortiz mailto:aortiz@acm.org\n"; }
/**
 * Return the Label1 property value.
 * @return java.awt.Label
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Label getcategoriesLabel() {
	if (ivjcategoriesLabel == null) {
		try {
			ivjcategoriesLabel = new java.awt.Label();
			ivjcategoriesLabel.setName("categoriesLabel");
			ivjcategoriesLabel.setAlignment(java.awt.Label.RIGHT);
			ivjcategoriesLabel.setText("Categories:");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjcategoriesLabel;
}
/**
 * Return the categoryChoice property value.
 * @return java.awt.Choice
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Choice getcategoryChoice() {
	if (ivjcategoryChoice == null) {
		try {
			ivjcategoryChoice = new java.awt.Choice();
			ivjcategoryChoice.setName("categoryChoice");
			ivjcategoryChoice.setFont(new java.awt.Font("dialog", 0, 14));
			// user code begin {1}
			ivjcategoryChoice.add("all");
			ivjcategoryChoice.add("animals");
			ivjcategoryChoice.add("movies");
			ivjcategoryChoice.add("computer people");
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjcategoryChoice;
}
/**
 * Return the giveUpButton property value.
 * @return java.awt.Button
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Button getgiveUpButton() {
	if (ivjgiveUpButton == null) {
		try {
			ivjgiveUpButton = new java.awt.Button();
			ivjgiveUpButton.setName("giveUpButton");
			ivjgiveUpButton.setFont(new java.awt.Font("dialog", 0, 14));
			ivjgiveUpButton.setEnabled(false);
			ivjgiveUpButton.setLabel("Give Up!");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjgiveUpButton;
}
/**
 * Return the HangmanClientBorderLayout property value.
 * @return java.awt.BorderLayout
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.BorderLayout getHangmanClientBorderLayout() {
	java.awt.BorderLayout ivjHangmanClientBorderLayout = null;
	try {
		/* Create part */
		ivjHangmanClientBorderLayout = new java.awt.BorderLayout();
		ivjHangmanClientBorderLayout.setVgap(10);
		ivjHangmanClientBorderLayout.setHgap(10);
	} catch (java.lang.Throwable ivjExc) {
		handleException(ivjExc);
	};
	return ivjHangmanClientBorderLayout;
}
/**
 * Return the mysteryLabel property value.
 * @return java.awt.Label
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Label getmysteryLabel() {
	if (ivjmysteryLabel == null) {
		try {
			ivjmysteryLabel = new java.awt.Label();
			ivjmysteryLabel.setName("mysteryLabel");
			ivjmysteryLabel.setFont(new java.awt.Font("monospaced", 0, 18));
			ivjmysteryLabel.setAlignment(java.awt.Label.CENTER);
			ivjmysteryLabel.setText("");
			ivjmysteryLabel.setBackground(java.awt.Color.lightGray);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjmysteryLabel;
}
/**
 * Return the Panel1 property value.
 * @return java.awt.Panel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Panel getPanel1() {
	if (ivjPanel1 == null) {
		try {
			ivjPanel1 = new java.awt.Panel();
			ivjPanel1.setName("Panel1");
			ivjPanel1.setLayout(getPanel1GridLayout());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPanel1;
}
/**
 * Return the Panel1GridLayout property value.
 * @return java.awt.GridLayout
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.GridLayout getPanel1GridLayout() {
	java.awt.GridLayout ivjPanel1GridLayout = null;
	try {
		/* Create part */
		ivjPanel1GridLayout = new java.awt.GridLayout(4, 7);
	} catch (java.lang.Throwable ivjExc) {
		handleException(ivjExc);
	};
	return ivjPanel1GridLayout;
}
/**
 * Return the Panel2 property value.
 * @return java.awt.Panel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Panel getPanel2() {
	if (ivjPanel2 == null) {
		try {
			ivjPanel2 = new java.awt.Panel();
			ivjPanel2.setName("Panel2");
			ivjPanel2.setLayout(getPanel2GridLayout());
			getPanel2().add(getremainingOpportunitiesLabel(), getremainingOpportunitiesLabel().getName());
			getPanel2().add(getPanel3(), getPanel3().getName());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPanel2;
}
/**
 * Return the Panel2GridLayout property value.
 * @return java.awt.GridLayout
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.GridLayout getPanel2GridLayout() {
	java.awt.GridLayout ivjPanel2GridLayout = null;
	try {
		/* Create part */
		ivjPanel2GridLayout = new java.awt.GridLayout(2, 1);
		ivjPanel2GridLayout.setHgap(10);
	} catch (java.lang.Throwable ivjExc) {
		handleException(ivjExc);
	};
	return ivjPanel2GridLayout;
}
/**
 * Return the Panel3 property value.
 * @return java.awt.Panel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Panel getPanel3() {
	if (ivjPanel3 == null) {
		try {
			ivjPanel3 = new java.awt.Panel();
			ivjPanel3.setName("Panel3");
			ivjPanel3.setLayout(getPanel3FlowLayout());
			ivjPanel3.setBackground(java.awt.Color.lightGray);
			getPanel3().add(getstartButton(), getstartButton().getName());
			getPanel3().add(getgiveUpButton(), getgiveUpButton().getName());
			getPanel3().add(getcategoriesLabel(), getcategoriesLabel().getName());
			getPanel3().add(getcategoryChoice(), getcategoryChoice().getName());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPanel3;
}
/**
 * Return the Panel3FlowLayout property value.
 * @return java.awt.FlowLayout
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.FlowLayout getPanel3FlowLayout() {
	java.awt.FlowLayout ivjPanel3FlowLayout = null;
	try {
		/* Create part */
		ivjPanel3FlowLayout = new java.awt.FlowLayout();
		ivjPanel3FlowLayout.setVgap(5);
		ivjPanel3FlowLayout.setHgap(15);
	} catch (java.lang.Throwable ivjExc) {
		handleException(ivjExc);
	};
	return ivjPanel3FlowLayout;
}
/**
 * Returns parameters defined by this applet.
 * @return an array of descriptions of the receiver's parameters
 */
public java.lang.String[][] getParameterInfo() {
	String[][] info = {
		{"middletierhost", "URL", "IP address or domain name of middle tier server"},
		{"middletierport", "Number", "Port number"}
	};
	return info;
}
/**
 * Return the remainingOpportunitiesLabel property value.
 * @return java.awt.Label
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Label getremainingOpportunitiesLabel() {
	if (ivjremainingOpportunitiesLabel == null) {
		try {
			ivjremainingOpportunitiesLabel = new java.awt.Label();
			ivjremainingOpportunitiesLabel.setName("remainingOpportunitiesLabel");
			ivjremainingOpportunitiesLabel.setAlignment(java.awt.Label.CENTER);
			ivjremainingOpportunitiesLabel.setFont(new java.awt.Font("dialog", 0, 14));
			ivjremainingOpportunitiesLabel.setText("Press start button to begin playing...");
			ivjremainingOpportunitiesLabel.setBackground(java.awt.Color.lightGray);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjremainingOpportunitiesLabel;
}
/**
 * Return the Button2 property value.
 * @return java.awt.Button
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private java.awt.Button getstartButton() {
	if (ivjstartButton == null) {
		try {
			ivjstartButton = new java.awt.Button();
			ivjstartButton.setName("startButton");
			ivjstartButton.setFont(new java.awt.Font("dialog", 0, 14));
			ivjstartButton.setLabel("Start");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjstartButton;
}
/**
 * Called when the giveUp button is pressed.
 */
public void giveUpButton_ActionPerformed() {
	if (output != null) {
		write("GIVEUP");
		try {
			// Check server's response.
			String inputLine = input.readLine();
			if (inputLine.startsWith("LOST:")) {
				getremainingOpportunitiesLabel().setText("You give up! The answer was: " + strip(inputLine).toUpperCase());
				enablePreGame();
			} else {
				throw new Exception("Protocol Error!");
			}
		} catch (Exception ex) {
			getremainingOpportunitiesLabel().setText(ex.toString());
		}
	}
}
/**
 * Called whenever the part throws an exception.
 * @param exception java.lang.Throwable
 */
private void handleException(java.lang.Throwable exception) {

	/* Uncomment the following lines to print uncaught exceptions to stdout */
	// System.out.println("--------- UNCAUGHT EXCEPTION ---------");
	// exception.printStackTrace(System.out);
}
/**
 * Initializes the applet.
 * @see #start
 * @see #stop
 */
public void init() {
	try {
		super.init();
		setName("HangmanClient");
		setLayout(getHangmanClientBorderLayout());
		setBackground(java.awt.Color.lightGray);
		setSize(426, 240);
		add(getmysteryLabel(), "North");
		add(getPanel1(), "Center");
		add(getPanel2(), "South");
		initConnections();
		
		// user code begin {1}
		stickMan = new StickMan();
		add(stickMan, "West");
		buttons = new Button[26];
		Font f = new Font("dialog", Font.BOLD, 14);
		ActionListener buttonsListener = new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				letterButton_ActionPerformed(e);
			}
		};
		for (int i = 0; i < buttons.length; i++) {
			buttons[i] = new Button((char) (i + 'A') + "");
			buttons[i].setFont(f);
			buttons[i].addActionListener(buttonsListener);
			getPanel1().add(buttons[i]);
		}
		// user code end
		
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {2}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * Initializes connections
 * @exception java.lang.Exception The exception description.
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void initConnections() throws java.lang.Exception {
	// user code begin {1}
	// user code end
	getstartButton().addActionListener(ivjEventHandler);
	getgiveUpButton().addActionListener(ivjEventHandler);
}
/**
 * Inserts a space between each character inside a String. 
 * Also converts letters to uppercase.
 * @param s the string in which to insert spaces
 * @return the resulting string
 */
private static String insertSpaces(String s) {
	StringBuffer result = new StringBuffer();
	for (int i = 0; i < s.length(); i++) {
		result.append(Character.toUpperCase(s.charAt(i)));
		result.append(' ');
	}
	return result.toString();
}
/**
 * Called when a letter button is pressed.
 * @param event the produced event.
 */
private void letterButton_ActionPerformed(ActionEvent event) {
	if (output != null) {
		write("CONTAINS:" + event.getActionCommand().toLowerCase());
		((Button) event.getSource()).setEnabled(false);
		try {

			// Check server's response.
			String inputLine = input.readLine();
			if (inputLine.startsWith("YES:")) {
				getmysteryLabel().setText(insertSpaces(strip(inputLine)));
			} else
				if (inputLine.startsWith("WON:")) {
					getmysteryLabel().setText(insertSpaces(strip(inputLine)));
					getremainingOpportunitiesLabel().setText("You win!");
					enablePreGame();
				} else
					if (inputLine.startsWith("LOST:")) {
						stickMan.setOpportunitiesLeft(0);
						getremainingOpportunitiesLabel().setText("You loose! The answer was: " + strip(inputLine).toUpperCase());
						enablePreGame();
					} else
						if (inputLine.startsWith("NO:")) {
							int opLeft = Integer.parseInt(strip(inputLine));
							stickMan.setOpportunitiesLeft(opLeft);
							getremainingOpportunitiesLabel().setText("Opportunities left: " + opLeft);
						} else {
							throw new Exception("Protocol Error!");
						}
		} catch (Exception ex) {
			getremainingOpportunitiesLabel().setText(ex.toString());
		}
	}
}
/**
 * Start applet execution. Connects with the middle-tier server.
 */
public void start() {
	String hostName = getParameter("middletierhost");
	int port = Integer.parseInt(getParameter("middletierport"));
	try {
		socket = new Socket(hostName, port);
		input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		output = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));

		// Server should say "READY"
		String inputLine = input.readLine();
		if (!inputLine.equals("READY")) {
			throw new Exception("Protocol Error!");
		}
		enablePreGame();		
	} catch (Exception ex) {
		getremainingOpportunitiesLabel().setText(ex.toString());
		setEnabled(false);
	}
}
/**
 * Called when start button is pressed. Asks for a new word to guess.
 */
public void startButton_ActionPerformed() {
	try {
		// Ask for a word in the selected category
		write("CHOOSE:" + getcategoryChoice().getSelectedItem());

		// Server should say "OK"
		String inputLine = input.readLine();
		if (!inputLine.startsWith("OK:")) {
			throw new Exception("Protocol Error!");
		}

		// Display the mystery (slahed word) string
		getmysteryLabel().setText(insertSpaces(strip(inputLine)));
		getremainingOpportunitiesLabel().setText("You may start choosing letters...");
		stickMan.setOpportunitiesLeft(-1);
		enableGame();
	} catch (Exception ex) {
		socket = null;
		input = null;
		output = null;
		getremainingOpportunitiesLabel().setText(ex.toString());
	}
}
/**
 * Stop applet execution. Terminates middle-tier connection.
 */
public void stop() {
	write("BYE");
	close();
}
/**
 * From a given string, take away any text before the first
 * semicolon.
 * @param s the string to strip
 */
private static String strip(String s) {
	return s.substring(s.indexOf(':') + 1);
}
/**
 * Write and flush a string to the socket's output stream.
 * @param s the string to write and flush
 */
private void write(String s) {
	if (output != null) {
		output.println(s);
		output.flush();
	}
}
}
