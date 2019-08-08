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

/**
 * A server connection to a client running as an independent thread.
 * @author: Ariel Ortiz
 */
public class ServerThread extends Thread {

    private Server          server;
    private int             id;
    private Socket          socket;
    private BufferedReader  input;
    private PrintWriter     output;

    /**
     * Initialize a server connection with a client.
     * @param clientSocket the socket that connects to the client
     */
    public ServerThread(Server server, int id, Socket socket) throws IOException {
        this.id     = id;
        this.socket = socket;
        this.server = server;
        input = new BufferedReader(
                    new InputStreamReader(
                        socket.getInputStream()));
        output = new PrintWriter(
                    new OutputStreamWriter(
                        socket.getOutputStream()));
    }

    /**
     * Implementation of the server side protocol.
     */
    public void run() {

        try {        
            if (server.isVerbose()) {
                System.out.println("(" + id + ") connection begin");
            }
    
            MysteryWord word = null;
    
            write("READY");
    
            while (true) {
                String command;
    
                try {
                    command = input.readLine();
                } catch (IOException ex) {
                    command = null;
                }
    
                if (command == null || command.equals("BYE")) {
                    break;
                }
    
                if (server.isVerbose()) {
                    System.out.println("from client (" + id + ")... " + command);
                }
    
                if (word == null) {
                    if (command.startsWith("CHOOSE:")) {
                        word = new MysteryWord(server, command.substring(command.indexOf(':') + 1));
                        write("OK:" + word.getUnsolvedWord());

                    } else {
                        write("ERROR");
                    }

                } else {
                    if (command.startsWith("CONTAINS:")) {
                        if (word.guess(command.charAt(command.indexOf(':') + 1))) {
                            if (word.allGuessed()) {
                                write("WON:" + word.getUnsolvedWord());
                                word = null;
                            } else {
                                write("YES:" + word.getUnsolvedWord());
                            }

                        } else {
                            if (word.opportunitiesLeft() == 0) {
                                write("LOST:" + word.getWord());
                                word = null;
                            } else {
                                write("NO:" + word.opportunitiesLeft());
                            }
                        }
    
                    } else if (command.equals("GIVEUP")) {
                        write("LOST:" + word.getWord());
                        word = null;

                    } else {
                        write("ERROR");
                    }
                }
            }

        } finally {
            close();
            if (server.isVerbose()) {
                System.out.println("(" + id + ") connection end");
            }
        }
    }

    /**
     * Write and flush a string to the socket's output stream.
     * @param s the string to write and flush
     */
    private void write(String s) {
        output.println(s);
        output.flush();
        if(server.isVerbose()) {
            System.out.println("to client (" + id + ")...   " + s);
        }
    }

    /**
     * Close server connection with a client, including its asociated
     * streams and socket.
     */
    public void close() {
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
            ex.printStackTrace();
        }
    }
}
