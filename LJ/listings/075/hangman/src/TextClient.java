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

import java.net.*;
import java.io.*;

/**
 * Hangman text mode client.
 * @author: Ariel Ortiz
 */
public class TextClient {

    private Socket          socket;
    private BufferedReader  input;
    private PrintWriter     output;

    /**
     * Create client side socket and input/output streams.
     * @param hostName the domain name or IP address of the middle tier host
     * computer.
     * @param portNum the port number.
     */
    public TextClient(String hostName, int port) throws Exception {

        socket  = new Socket(hostName, port);
        input   = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        output  = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));

        System.out.println("Hangman Text Client, Version 1.0");
        System.out.println("Copyright (C) 2000 Ariel Ortiz");
    }

    /**
     * Implementation of the client side protocol.
     */
    public void playHangman() throws Exception {

        // stdin: allows us to read lines of the text from standard input
        BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));

        String inputLine;
        String stdinLine;

        // Server should say "READY"
        inputLine = input.readLine();
        if (! inputLine.equals("READY")) {
            protocolError();
        }

        while (true) {

            write("CHOOSE:all");
            
            // Server should say "OK"
            inputLine = input.readLine();
            if(! inputLine.startsWith("OK:")) {
                protocolError();
            }

            System.out.println(strip(inputLine));

            while (true) {

                System.out.print("Choose a leter: ");
                stdinLine = stdin.readLine();

                // Give up if user types an empty line
                if (stdinLine.equals("")) {
                    write("GIVEUP");
                } else {
                    write("CONTAINS:" + stdinLine.charAt(0));
                }

                inputLine = input.readLine();
                if (inputLine.startsWith("YES:")) {
                    System.out.println(strip(inputLine));

                } else if (inputLine.startsWith("WON:")) {
                    System.out.println(strip(inputLine));
                    System.out.println("You win!");
                    break;

                } else if (inputLine.startsWith("LOST:")) {
                    System.out.println("You loose!");        
                    System.out.println("Mystery word was : " + strip(inputLine));
                    break;

                } else if (inputLine.startsWith("NO:")) {
                    System.out.println("Wrong! Opportunities left: " + strip(inputLine));

                } else {
                    protocolError();
                }
            }

            System.out.print("Do you want to play again? (y/n): ");
            stdinLine = stdin.readLine();
            if (! (stdinLine.length() > 0 && Character.toUpperCase(stdinLine.charAt(0)) == 'Y'))  {
                break;
            }
        }
        write("BYE");
        close();
    }

    /**
     * Close client side socket and input/output streams.
     */
    private void close() throws IOException {
        output.close();
        input.close();
        socket.close();
    }

    /**
     * Write and flush a string to the socket's output stream.
     * @param s the string to write and flush
     */
    private void write(String s) {
        output.println(s);
        output.flush();
    }

    /**
     * Throw an exception to signal a protocol error.
     */
    private void protocolError() throws Exception {
        throw new Exception("Protocol Error!");    
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
     * Create an instance of the TextClient class and start playing 
     * hangman.
     * @param args the domain name or IP address of the middle tier host
     * computer and the port number.
     */
    public static void main(String[] args) throws Exception {
        if (args.length != 2) {
            System.out.println("Must specify host name and port number.");
            System.out.println("For example: java " + TextClient.class.getName() + " 127.0.0.1 2000");
        } else {
            new TextClient(args[0], Integer.parseInt(args[1])).playHangman();
        }
    }
}
