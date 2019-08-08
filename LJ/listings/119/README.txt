The script is supplied as two files: publish.tcl and gui.tcl.

publish.tcl is the main script, the one which is actually run. gui.tcl is
the output from (and the input to) Visual Tcl, and is used as a library
by publish.tcl.


To try the script, copy the two files to a single directory, change into
that directory, then run publish.tcl:

 ./publish.tcl

If you get an error message saying a library isn't found, ensure you are using
the latest ActiveTcl distribution from here:

 http://www.activestate.com/Products/ActiveTcl/

The script engine used is the Tcl "Windowing Shell", called 'wish'. Therefore
the ActiveTcl wish should be the first in your PATH:

 >which wish
 /opt/ActiveTcl-8.4.4.0/bin/wish

If you can't arrange that for some reason, change the she-bang lines in
publish.tcl (which find and run the first wish in the user's path) to
point to the ActiveTcl wish directly:

 #!/path/to/ActiveTcl-8.4.4.0/bin/wish


If you want to "install" the script in a normal place so you can use it,
you'll want to merge the two files into one big script. This is simple. The
instructions are in the publish.tcl script, near the bottom where the gui.tcl
file is 'source'ed.


Bug fixes, and implemented suggestions and improvements to this script can be
found in the version kept here:

 http://derekfountain.webhop.org/hacks/publish/
