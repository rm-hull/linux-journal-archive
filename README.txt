==== Linux Journal Archive ====

Welcome to the Linux Journal Archive for 2018.

This year's Archive includes integrated search capabilities.
This capability is provided by the "FlyingAnt CD Web Server" by
Wrensoft http://www.wrensoft.com.  See below for instructions
specific to the Operating System you use.



==== Usage instuctions ====

Linux:

    After unzipping the Archive you should have a directory/folder
    named LJArchive2018.

    Using your file system browser, find and run the script
    named "Start Linux.sh" in the unzipped directory/folder.
    This should start the Archive web server and open your
    default web browser to the URL:

        http://127.0.0.1:8091/index.html.

    To stop the Archive web server click one of the links on any of
    the Archive web pages, or execute "Stop Linux.sh". The
    web server process is named FlyingAntLinux.

    These scripts can also be run from a shell/terminal/console window.

    Some Linux distributions may unzip files without setting the
    executable bit on the Archive web server executable files.
    If trying to run the script from the command line produces
    a "permission denied" error, set the executable permissions
    on the files:

        autorun.sh
        Start Linux.sh
        Server/Linux/FlyingAntLinux
        Website/Zoom/search_linux.cgi

    Then you should be able to run "Start Linux.sh" normally.


Mac OSX:

    After unzipping the Archive you should have a folder
    named LJArchive2018.

    On OSX Sierra 10.12 and newer you need to open a
    terminal and run the commands below before proceeding.
    Make sure to change "PATH" in the first command to
    match the location where you unzipped the Archive.
    Note that the "$" is the prompt, not part of the command.

        $ cd /PATH/LJArchive2018
        $ xattr -r -d com.apple.quarantine 'Start Mac OSX.app'

    Then, using Mac Finder or any file system browser, find and run
    the application named "Start Mac OSX" in the unzipped folder.
    This should start the Archive web server and open your
    default web browser to the URL:

        http://127.0.0.1:8091/index.html.

    If your encounter problems running the application, press
    the Control key and then click on "Start Mac OSX".  If you
    get an error message that it cannot run, go to Systems Preferences,
    then Security and Privacy, then the General tab and click
    Allow Flying Ant.  See the following URL for more information:

        https://support.apple.com/kb/PH18657?locale=en_US

    To stop the Archive web server click one of the links on any of
    the Archive web pages, or close the terminal window that appeared
    when starting the server.  Make sure that the terminal is no longer
    running, it may still be running even if it has no windows open.

    Please note that only the x86 platform is supported and not PowerPC
    hardware.


Windows:

    After unzipping the Archive you should have a directory/folder
    named LJArchive2018/LJArchive2018.  By default Windows unzips
    files into a directory with the same name as the base name of
    the zip file, however the zip file already contains a top level
    directory by the same name, so you get the doubled name.  When
    unzipping, Windows should give you the option to change the output
    directory, remove the last folder name in the suggested path to
    eliminate the double name.

    Using Windows Explorer or any file system browser, find and run
    the program "Start Windows" (or "Start Windows.exe" if your
    system shows file extensions).  This should start the Archive
    web server and open your default web browser to the URL:

        http://127.0.0.1:8091/index.html.

    To stop the Archive web server click one of the links on any of
    the Archive web pages.


