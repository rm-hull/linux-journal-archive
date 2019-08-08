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

package hangman;

import java.io.*;
import java.net.*;
import java.util.*;

/**
 * Hangman middle tier server.
 * @author: Ariel Ortiz
 */
public class Server {

    private boolean         verbose;
    private ResourceBundle  resources;
    private String          databaseDriver;
    private String          databaseURL;
    private int             port;

    private static int      clientCount;

    /**
     * Initialize the system's resource names (database driver, 
     * database URL, server's port number) from the file 
     * "hangman.properties".
     * @param verbose true to set verbose mode on.
     */
    public Server(boolean verbose) {
        this.verbose    = verbose;
        resources       = ResourceBundle.getBundle("hangman");
        databaseDriver  = resources.getString("database.driver");
        databaseURL     = resources.getString("database.url");
        port            = Integer.parseInt(resources.getString("hangman.port"));

        System.out.println("Hangman Middle Tier Server, Version 1.0");
        System.out.println("Copyright (C) 2000 Ariel Ortiz");
        if (verbose) {
            System.out.println("Verbose mode is ON.");
        } else {
            System.out.println("Verbose mode is OFF. Use -v option to set it ON.");
        }
        System.out.println("Endpoint created: " + port);
        
    }

    /**
     * Obtain the system's database driver.
     * @return the system's database driver.
     */
    public String getDatabaseDriver() {
        return databaseDriver;
    }

    /**
     * Obtain the system's database URL.
     * @return the system's database URL.
     */
    public String getDatabaseURL() {
        return databaseURL;
    }

    /**
     * Obtain the verbose mode of the system.
     * @return true if verbose mode is ON, or false if it is OFF.
     */
    public boolean isVerbose() {
        return verbose;
    }

    /**
     * Start the execution of a hangman Server.
     */
    public void start() {
        ServerSocket server;
        ServerThread thread = null;
        try {
            server = new ServerSocket(port);
            while(true) {
                try {
                    clientCount ++;
                    thread = new ServerThread(this, clientCount, server.accept());
                    // Start a new Thread.
                    thread.start();

                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }


    /**
     * Create an instance of the Server class and starts its
     * execution.
     * @param args command line arguments.
     */
    public static void main(String[] args) {
        new Server(args.length == 1 && args[0].equals("-v")).start();
    }
}
