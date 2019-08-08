#!/bin/sh
# the next line restarts using wish\
exec wish "$0" "$@"

#
#    Copyright (C) 2003 Derek Fountain
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#    v1.01 - Fix text widget reading - 1.0 not 0.0
#    v1.00 - Initial release
#

# You'll need ActiveTcl if your normal Tcl/Tk distribution doesn't contain
# these extensions. See http://www.activestate.com/Products/ActiveTcl/
#
package require img::jpeg
package require dom::libxml2


# List of images
#
set imageList {}

# Images, internally rendered ready to display
#
array set imageCache {}


# Utility function to save some text out to a file. If the save fails, show
# a dialog with the supplied error message.
#
# Returns 0 for OK, or 1 if there was an error.
#
proc saveText { filename text errorString } {

    set saveStatus [catch {
	set handle [open $filename "w"]
	puts $handle $text
	close $handle
    } ]

    if { $saveStatus } {
	errorDialog "Error writing to $filename: $errorString"
	return 1
    }

    return 0
}


# Utility function to show an error message.
#
proc errorDialog { errorString } {
    tk_dialog .errorDialog "Error publishing" $errorString "" 0 "OK"
    return 0
}


# This does the real work, when the Publish button is clicked
#
proc publishImages {} {
    global widget

    set ::status "Publishing..."
    update idletasks

    # This is the XSLT script used to convert my DOM into an HTML document.
    # It's pretty basic.
    #
    set xslt {<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <xsl:output method="html"/>

  <xsl:template match="/">
    <html>
      <body bgcolor="#FFFFFF">
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="title">
    <head>
      <title>
        <xsl:value-of select="." />
      </title>
    </head>
    <body>
      <h1 align="center">
        <xsl:value-of select="." />
      </h1>
    </body>
  </xsl:template>

  <xsl:template match="intro | conclusion">
    <div align="center">
      <font size="+1">
        <xsl:value-of select="." />
      </font>
    </div>
  </xsl:template>

  <xsl:template match="image">
    <br />
    <div align="center">
      <img src="{@filename}" />
    </div>
  </xsl:template>

</xsl:stylesheet>
    }


    # Pick up the texts from the GUI widgets
    #
    set title $::titleText
    set intro [$widget(IntroText) get 1.0 end]
    set conclusion [$widget(ConclusionText) get 1.0 end]

    # Generate the XML
    #
    if { [generateXML xml $title $intro $::imageList $conclusion] == -1 } {
	set ::status "Ready"
	return [errorDialog "Error creating XML: $xml"]
    }

    # Save the XML to a temporary file
    #
    set xmlname "/tmp/publish.xml"
    if { [saveText $xmlname $xml "Unable to write to $xmlname"] } {
	set ::status "Ready"
	return
    }

    # Run the XSLT processor
    #
    set xsltStatus [catch {
	set html [exec xsltproc /dev/stdin $xmlname << $xslt]
    } xsltError]
    catch { file delete -force $xmlname }
    if { $xsltStatus } {
	set ::status "Ready"
	return [errorDialog "XSLT processing failed: $xsltError"]
    }

    # Now save the HTML in a user specified file
    #
    set types {
	{{HTML}             {.html} }
	{{All Files}        *             }
    }
    set outputName [tk_getSaveFile -parent $::widget(MainWindow) \
		                   -title "Save HTML" -filetypes $types]

    if { $outputName == "" } {
	set ::status "Output canceled"
	after 2000 {set ::status "Ready"}
	return	
    }
    if { [saveText $outputName $html "Unable to write to $outputName"] } {
	set ::status "Ready"
	return
    }

    # Copy the image files to the output directory. If they are already there
    # the copy will fail, which is OK. I silently catch it.
    #
    set outputDir [file dirname $outputName]
    foreach image $::imageList {
	set destImage [file join $outputDir [file tail $image]]
	catch { file copy $image $destImage }
    }

    set ::status "Successfully published webpage $outputName"
    after 5000 {set ::status "Ready"}

    return
}


# This generates the XML. It builds a DOM with the title and introduction,
# followed by each of the images, then the conclusion. The XML is returned
# in the variable named as the first parameter.
#
# Returns -1 if there's an error, in which case the 'xmlVar' contains a 
# brief error explanation.
#
proc generateXML { xmlVar title intro imageList conclusion } {
    upvar $xmlVar xml

    set rootNode [dom::DOMImplementation create]
    set top [::dom::document createElement $rootNode page]

    set titleNode [::dom::document createElement $top title]
    dom::document createTextNode $titleNode $title

    set introNode [::dom::document createElement $top intro]
    dom::document createTextNode $introNode $intro

    foreach image $imageList {
	set imageNode [::dom::document createElement $top image]
	dom::element setAttribute $imageNode filename [file tail $image]
    }

    set conclusionNode [::dom::document createElement $top conclusion]
    dom::document createTextNode $conclusionNode $conclusion

    # This sets the return variable with the XML text
    #
    set xml [dom::DOMImplementation serialize $rootNode -indent 1] 

    return 0
}


# This either shows or hides the image viewer, based on the value of a global
# variable. That global is controlled by the onscreen checkbox.
#
proc toggleShowImage {} {
    if { $::showImage } {
	wm deiconify $::widget(ImageToplevel)
	selectImage
    } else {
	wm withdraw $::widget(ImageToplevel)
    }
}


# This closes the image viewer window. It uses the checkbox mechanism.
#
proc closeImageShow {} {
    set ::showImage 0
    toggleShowImage
}


# This is called when the user want to add an image (or several images) to the
# listbox. It presents a file selector dialog.
#
proc addImage {} {
    set listboxWidget $::widget(ImagesListbox)

    set types {
	{{JPEG}             {.jpg .jpeg}  }
	{{All Files}        *             }
    }
    set selection [tk_getOpenFile -multiple yes \
		                  -title "Image selection" -filetypes $types \
		                  -parent $::widget(MainWindow)]

    # Just update the list variable. Onscreen update is automatic.
    #
    eval lappend ::imageList $selection

    $listboxWidget selection set end    
    selectImage
}


# This is called when the user wants to delete an image from the listbox.
#
proc deleteImage {} {
    set listboxWidget $::widget(ImagesListbox)

    # Just update the list variable. Onscreen update is automatic.
    #
    if { [set deleteIndex [$listboxWidget curselection]] != "" } {
	catch { unset $::imageCache($::currentFilename) }
	set ::imageList [lreplace $::imageList $deleteIndex $deleteIndex]
	selectImage
    }
}


# This is called when the user wants to move the current;y selected list
# item up or down.
#
proc moveImage { direction } {
    set listboxWidget $::widget(ImagesListbox)

    # Check there's an item to move, there's a selected item and the move is
    # possible
    #
    if { ([set numItems [$listboxWidget size]] < 2) ||
	 ([set currentIndex [$listboxWidget curselection]] == "") ||
	 (($currentIndex == 0 && $direction == "up") ||
	  ($currentIndex == [expr $numItems-1] && $direction == "down")) } {
	return
    }

    # Note, then clear the currently selected list item
    #
    set entry [lindex $::imageList $currentIndex]
    $listboxWidget selection clear $currentIndex
    set ::imageList [lreplace $::imageList $currentIndex $currentIndex]

    # Insert the items in it's new place (up or down one), then reset the
    # listbox selection so it stays as the currently selected one
    #
    if { $direction == "up" } {
	set currentIndex [expr $currentIndex-1]
    } else {
	set currentIndex [expr $currentIndex+1]
    }
    set ::imageList [linsert $::imageList $currentIndex $entry]
    $listboxWidget selection set $currentIndex
}


# This is called when a new selection is made in the images listbox. It updates
# the viewer window to show the selected image.
#
proc selectImage {} {
    global currentFilename

    set listboxWidget    $::widget(ImagesListbox)
    set imageLabelWidget $::widget(ImageLabel)

    if { [$listboxWidget curselection] == "" } {
	$imageLabelWidget configure -image ""
	return
    }
    set currentFilename [$listboxWidget get [$listboxWidget curselection]]

    # Only do the image rendering etc if the viewer window is open
    #
    if { [wm state $::widget(ImageToplevel)] == "normal" } {

	# I keep the rendered photo widgets in a cache so they don't have to
	# be recreated each time the use clicks in the list. If the rendering
	# fails I set a status bar message and clear the image area.
	#
	if { ![info exists ::imageCache($currentFilename)] } {
	    if { [catch {
		set ::imageCache($currentFilename) \
		    [image create photo -file $currentFilename]
 	          }] } {
		set ::status "Unable to load image $currentFilename"
		catch { unset ::imageCache($currentFilename) }
		$imageLabelWidget configure -image ""
		return
	    }
	}
	$imageLabelWidget configure -image $::imageCache($currentFilename)
	set ::status "Ready"
    }
}

# Pull in the code created by Visual Tcl. To make a single file script which
# can be placed in /usr/local/bin, replace this line with the entire contents
# of the file (which is exactly what the Tcl 'source' command does anyway).
#
source gui.tcl

# I can't find out how to set this in vTcl!
#
$widget(FilenameEntry) configure -state readonly

# Visual Tcl considers my viewer window as an important toplevel window, and
# won't let the application close until that viewer window is closed. Actually,
# the viewer window isn't that important, so I override the Visual Tcl
# generated code which handles the window manager "close" button.
#
wm protocol $::widget(ImageToplevel) WM_DELETE_WINDOW closeImageShow

bind "_TopLevel" <<DeleteWindow>> {
    if {[set ::%W::_modal]} {
	vTcl:Toplevel:WidgetProc %W endmodal
    } else {
	destroy %W
	if { "%W" == $::widget(MainWindow) } {exit}
    }
}
