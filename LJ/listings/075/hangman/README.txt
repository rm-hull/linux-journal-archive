Three-tier Hangman
==================

This TAR file contains the source code and binary files for a simple three-tier 
"Hangman" game. To get it running, you need to do the following, assuming you 
already have the JDK 1.2.2 or above already installed:

* Download and install miniSQL. http://www.hughes.com.au/

* Download the miniSQL JDBC driver. http://www.imaginary.com/Java/ 

* Copy the miniSQL JDBC driver file (called something like msql-jdbc-2-0a3.jar) 
  to this directory.

* Run the miniSQL daemon as a background process. As root, execute something 
  like: 

	/usr/local/Hughes/bin/msql2d &

* Create the hangman database. As root, type something like: 

	/usr/local/Hughes/bin/msqladmin create hangman

* Insert some words to the database using the SQL monitor:

	/usr/local/Hughes/bin/msql hangman < mystery.sql

* Edit the hangman.properties file if necessary. You may be required to change 
  the attributes hangman.port and database.url to reflect a different 
  configuration on the middle-tier or data servers.

* Run the middle-tier server. Make sure both msql-jdbc-2-0a3.jar and 
  hangman_server.jar JAR files are included in the -cp (classpath) option:

	java -cp .:msql-jdbc-2-0a3.jar:hangman_server.jar hangman.Server -v

* To run the text mode client, open a new shell window and type:

	java TextClient host port

   change "host" to a domain name (for example: localhost) and "port" to a 
   port number (for example: 2000).

* To run the GUI client, copy the files HangmanClient.html and 
  hangman_client.jar to your Web server. Both files must be in the same 
  directory. Because of security constraints, most Web browsers require 
  that the Web server and the middle-tier server must be running in the 
  same host. 
 
  Edit the file HangmanClient.html in order to adjust the applet's
  parameters "middletierhost" and "middletierport".

  To run the applet, use the appropriate URL:

	http://localhost/HangmanClient.html

This program is free software; you can redistribute it and/or modify it 
under the terms of the GNU General Public License. See the file 
licence.txt.

Copyright (C) 2000 Ariel Ortiz
mailto:aortiz@acm.org
