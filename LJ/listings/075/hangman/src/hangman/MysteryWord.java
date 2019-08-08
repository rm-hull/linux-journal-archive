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

import java.sql.*;
import java.util.*;

/**
 * Hangman "business" rules. 
 * @author: Ariel Ortiz
 */
public class MysteryWord {

    private int             opleft = 6;
    private String          word;
    private StringBuffer    unsolvedWord;


    /**
     * Get a new mystery word from the database.
     * @param category the category of the new mystery word
     */
    public MysteryWord(Server server, String category) {

        try {
            // Load driver and establish connection to the database server
            Class.forName(server.getDatabaseDriver());
            Connection connection = DriverManager.getConnection(server.getDatabaseURL());
            
            Statement statement = connection.createStatement();
            String query = "select * from mystery";

            // Do conditional select if category not "all"
            if (! category.equals("all")) {
                query += " where category = '" + category + "'";
            }

            // Get all mystery words in the same category and put in vector
            ResultSet result = statement.executeQuery(query);
            Vector words = new Vector();
            while(result.next()) {
                words.add(result.getString(1));
            }

            // Select randomly one word
            word = (String) words.get((int) (Math.random() * words.size()));
            unsolvedWord = fillWithUnderscores(word);

            // Close database resources
            statement.close();
            connection.close();
                                               
        } catch (SQLException ex) {
            ex.printStackTrace();
        } catch (ClassNotFoundException ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Attempts to guess if the input char is contained in the mystery word.
     * Decreases opportunities left in case its wrong.
     * @param guessed character
     * @return true if correct, false if wrong
     */
    public boolean guess(char c) {
                
        boolean found = false;

        for (int i = 0; i < word.length(); i ++) {
            if (c == word.charAt(i) && unsolvedWord.charAt(i) == '_') {
                unsolvedWord.setCharAt(i, c);
                found = true;
             }
        }

        if (! found) {
            opleft --;
        }

        return found;
    }

    /**
     * Checks if all letters in the mystery word have been correctly guessed.
     * @return true if all letters have been guessed, false otherwise
     */
    public boolean allGuessed() {
        return unsolvedWord.toString().indexOf('_') == -1;
    }

    /**
     * Obtain the number of opportunities left before being hung.
     * @return opportunities left     
     */
    public int opportunitiesLeft() {
        return opleft;
    }

    /**
     * Obtain the mystery word.
     * @return mystery word
     */
    public String getWord() {
        return word;
    }

    /**
     * Obtain the (maybe partially) unsolved mystery word.
     * @return unsolved mystery word
     */
    public String getUnsolvedWord() {
        return unsolvedWord.toString();
    }

    /**
     * Utility method that returns a new StringBuffer in which al letters 
     * of the input string are replaced with underscores.
     * @param word input string
     * @return resulting StringBuffer
     */
    private StringBuffer fillWithUnderscores(String word) {

        StringBuffer result = new StringBuffer();

        for (int i = 0; i < word.length(); i ++) {
            if (word.charAt(i) == ' ') {
                result.append(" ");
            } else {
                result.append("_");
            }
        }

        return result;
    }
}

