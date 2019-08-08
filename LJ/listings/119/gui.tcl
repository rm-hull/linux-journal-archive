#!/bin/sh
# the next line restarts using wish\
exec wish "$0" "$@" 

if {![info exists vTcl(sourcing)]} {

    # Provoke name search
    catch {package require bogus-package-name}
    set packageNames [package names]

    package require Tk
    switch $tcl_platform(platform) {
	windows {
            option add *Button.padY 0
	}
	default {
            option add *Scrollbar.width 10
            option add *Scrollbar.highlightThickness 0
            option add *Scrollbar.elementBorderWidth 2
            option add *Scrollbar.borderWidth 2
	}
    }
    
    # Needs Itcl
    package require Itcl

    # Needs Itk
    package require Itk

    # Needs Iwidgets
    package require Iwidgets

    switch $tcl_platform(platform) {
	windows {
            option add *Pushbutton.padY         0
	}
	default {
	    option add *Scrolledhtml.sbWidth    10
	    option add *Scrolledtext.sbWidth    10
	    option add *Scrolledlistbox.sbWidth 10
	    option add *Scrolledframe.sbWidth   10
	    option add *Hierarchy.sbWidth       10
            option add *Pushbutton.padY         2
        }
    }
    
}

#############################################################################
# Visual Tcl v1.60 Project
#


#################################
# VTCL LIBRARY PROCEDURES
#

if {![info exists vTcl(sourcing)]} {
#############################################################################
## Library Procedure:  Window

proc ::Window {args} {
    ## This procedure may be used free of restrictions.
    ##    Exception added by Christian Gavin on 08/08/02.
    ## Other packages and widget toolkits have different licensing requirements.
    ##    Please read their license agreements for details.

    global vTcl
    foreach {cmd name newname} [lrange $args 0 2] {}
    set rest    [lrange $args 3 end]
    if {$name == "" || $cmd == ""} { return }
    if {$newname == ""} { set newname $name }
    if {$name == "."} { wm withdraw $name; return }
    set exists [winfo exists $newname]
    switch $cmd {
        show {
            if {$exists} {
                wm deiconify $newname
            } elseif {[info procs vTclWindow$name] != ""} {
                eval "vTclWindow$name $newname $rest"
            }
            if {[winfo exists $newname] && [wm state $newname] == "normal"} {
                vTcl:FireEvent $newname <<Show>>
            }
        }
        hide    {
            if {$exists} {
                wm withdraw $newname
                vTcl:FireEvent $newname <<Hide>>
                return}
        }
        iconify { if $exists {wm iconify $newname; return} }
        destroy { if $exists {destroy $newname; return} }
    }
}
#############################################################################
## Library Procedure:  vTcl:DefineAlias

proc ::vTcl:DefineAlias {target alias widgetProc top_or_alias cmdalias} {
    ## This procedure may be used free of restrictions.
    ##    Exception added by Christian Gavin on 08/08/02.
    ## Other packages and widget toolkits have different licensing requirements.
    ##    Please read their license agreements for details.

    global widget
    set widget($alias) $target
    set widget(rev,$target) $alias
    if {$cmdalias} {
        interp alias {} $alias {} $widgetProc $target
    }
    if {$top_or_alias != ""} {
        set widget($top_or_alias,$alias) $target
        if {$cmdalias} {
            interp alias {} $top_or_alias.$alias {} $widgetProc $target
        }
    }
}
#############################################################################
## Library Procedure:  vTcl:DoCmdOption

proc ::vTcl:DoCmdOption {target cmd} {
    ## This procedure may be used free of restrictions.
    ##    Exception added by Christian Gavin on 08/08/02.
    ## Other packages and widget toolkits have different licensing requirements.
    ##    Please read their license agreements for details.

    ## menus are considered toplevel windows
    set parent $target
    while {[winfo class $parent] == "Menu"} {
        set parent [winfo parent $parent]
    }

    regsub -all {\%widget} $cmd $target cmd
    regsub -all {\%top} $cmd [winfo toplevel $parent] cmd

    uplevel #0 [list eval $cmd]
}
#############################################################################
## Library Procedure:  vTcl:FireEvent

proc ::vTcl:FireEvent {target event {params {}}} {
    ## This procedure may be used free of restrictions.
    ##    Exception added by Christian Gavin on 08/08/02.
    ## Other packages and widget toolkits have different licensing requirements.
    ##    Please read their license agreements for details.

    ## The window may have disappeared
    if {![winfo exists $target]} return
    ## Process each binding tag, looking for the event
    foreach bindtag [bindtags $target] {
        set tag_events [bind $bindtag]
        set stop_processing 0
        foreach tag_event $tag_events {
            if {$tag_event == $event} {
                set bind_code [bind $bindtag $tag_event]
                foreach rep "\{%W $target\} $params" {
                    regsub -all [lindex $rep 0] $bind_code [lindex $rep 1] bind_code
                }
                set result [catch {uplevel #0 $bind_code} errortext]
                if {$result == 3} {
                    ## break exception, stop processing
                    set stop_processing 1
                } elseif {$result != 0} {
                    bgerror $errortext
                }
                break
            }
        }
        if {$stop_processing} {break}
    }
}
#############################################################################
## Library Procedure:  vTcl:Toplevel:WidgetProc

proc ::vTcl:Toplevel:WidgetProc {w args} {
    ## This procedure may be used free of restrictions.
    ##    Exception added by Christian Gavin on 08/08/02.
    ## Other packages and widget toolkits have different licensing requirements.
    ##    Please read their license agreements for details.

    if {[llength $args] == 0} {
        ## If no arguments, returns the path the alias points to
        return $w
    }
    set command [lindex $args 0]
    set args [lrange $args 1 end]
    switch -- [string tolower $command] {
        "setvar" {
            foreach {varname value} $args {}
            if {$value == ""} {
                return [set ::${w}::${varname}]
            } else {
                return [set ::${w}::${varname} $value]
            }
        }
        "hide" - "show" {
            Window [string tolower $command] $w
        }
        "showmodal" {
            ## modal dialog ends when window is destroyed
            Window show $w; raise $w
            grab $w; tkwait window $w; grab release $w
        }
        "startmodal" {
            ## ends when endmodal called
            Window show $w; raise $w
            set ::${w}::_modal 1
            grab $w; tkwait variable ::${w}::_modal; grab release $w
        }
        "endmodal" {
            ## ends modal dialog started with startmodal, argument is var name
            set ::${w}::_modal 0
            Window hide $w
        }
        default {
            uplevel $w $command $args
        }
    }
}
#############################################################################
## Library Procedure:  vTcl:WidgetProc

proc ::vTcl:WidgetProc {w args} {
    ## This procedure may be used free of restrictions.
    ##    Exception added by Christian Gavin on 08/08/02.
    ## Other packages and widget toolkits have different licensing requirements.
    ##    Please read their license agreements for details.

    if {[llength $args] == 0} {
        ## If no arguments, returns the path the alias points to
        return $w
    }

    set command [lindex $args 0]
    set args [lrange $args 1 end]
    uplevel $w $command $args
}
#############################################################################
## Library Procedure:  vTcl:toplevel

proc ::vTcl:toplevel {args} {
    ## This procedure may be used free of restrictions.
    ##    Exception added by Christian Gavin on 08/08/02.
    ## Other packages and widget toolkits have different licensing requirements.
    ##    Please read their license agreements for details.

    uplevel #0 eval toplevel $args
    set target [lindex $args 0]
    namespace eval ::$target {set _modal 0}
}
}


if {[info exists vTcl(sourcing)]} {

proc vTcl:project:info {} {
    set base .addDialog
    namespace eval ::widgets::$base {
        set set,origin 0
        set set,size 0
        set runvisible 0
    }
    namespace eval ::widgets::$base.innerFrame {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    set site_3_0 $base.innerFrame
    namespace eval ::widgets::$site_3_0.filenameFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.filenameFrame
    namespace eval ::widgets::$site_4_0.filenameLabel {
        array set save {-disabledforeground 1 -text 1}
    }
    namespace eval ::widgets::$site_4_0.filenameEntry {
        array set save {-textbackground 1 -textvariable 1}
    }
    namespace eval ::widgets::$site_3_0.imageDisplayFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.imageDisplayFrame
    namespace eval ::widgets::$site_4_0.imageLabel {
        array set save {-disabledforeground 1}
    }
    namespace eval ::widgets::$site_3_0.buttonsFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.buttonsFrame
    namespace eval ::widgets::$site_4_0.closeButton {
        array set save {-command 1 -disabledforeground 1 -text 1}
    }
    set base .mainDialog
    namespace eval ::widgets::$base {
        set set,origin 1
        set set,size 0
        set runvisible 1
    }
    namespace eval ::widgets::$base.mainFrame {
        array set save {-borderwidth 1 -height 1 -width 1}
    }
    set site_3_0 $base.mainFrame
    namespace eval ::widgets::$site_3_0.cpd119 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.cpd119
    namespace eval ::widgets::$site_4_0.titleLabel {
        array set save {-anchor 1 -disabledforeground 1 -text 1 -width 1}
    }
    namespace eval ::widgets::$site_4_0.titleEntry {
        array set save {-textbackground 1 -textvariable 1}
    }
    namespace eval ::widgets::$site_3_0.cpd120 {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.cpd120
    namespace eval ::widgets::$site_4_0.statusBarMessage {
        array set save {-anchor 1 -disabledforeground 1 -text 1 -textvariable 1}
    }
    namespace eval ::widgets::$site_3_0.introFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.introFrame
    namespace eval ::widgets::$site_4_0.introLabel {
        array set save {-anchor 1 -disabledforeground 1 -text 1 -width 1}
    }
    namespace eval ::widgets::$site_4_0.introText {
        array set save {-hscrollmode 1 -textbackground 1 -visibleitems 1 -vscrollmode 1 -wrap 1}
    }
    namespace eval ::widgets::$site_3_0.imagesFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.imagesFrame
    namespace eval ::widgets::$site_4_0.imagesInnerFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_5_0 $site_4_0.imagesInnerFrame
    namespace eval ::widgets::$site_5_0.imagesListbox {
        array set save {-hscrollmode 1 -listvariable 1 -selectioncommand 1 -textbackground 1 -visibleitems 1}
    }
    namespace eval ::widgets::$site_5_0.showImageCheckbox {
        array set save {-command 1 -disabledforeground 1 -text 1 -variable 1}
    }
    namespace eval ::widgets::$site_5_0.imageButtonsFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_6_0 $site_5_0.imageButtonsFrame
    namespace eval ::widgets::$site_6_0.addButton {
        array set save {-command 1 -disabledforeground 1 -text 1}
    }
    namespace eval ::widgets::$site_6_0.cpd121 {
        array set save {-command 1 -disabledforeground 1 -text 1}
    }
    namespace eval ::widgets::$site_6_0.upButton {
        array set save {-command 1 -disabledforeground 1 -text 1}
    }
    namespace eval ::widgets::$site_6_0.downButton {
        array set save {-command 1 -disabledforeground 1 -text 1}
    }
    namespace eval ::widgets::$site_4_0.imageLabel {
        array set save {-anchor 1 -disabledforeground 1 -text 1 -width 1}
    }
    namespace eval ::widgets::$site_3_0.conclusionFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.conclusionFrame
    namespace eval ::widgets::$site_4_0.conclusionLabel {
        array set save {-anchor 1 -disabledforeground 1 -text 1 -width 1}
    }
    namespace eval ::widgets::$site_4_0.conclusionText {
        array set save {-hscrollmode 1 -textbackground 1 -visibleitems 1 -vscrollmode 1}
    }
    namespace eval ::widgets::$site_3_0.buttonsFrame {
        array set save {-borderwidth 1 -height 1 -relief 1 -width 1}
    }
    set site_4_0 $site_3_0.buttonsFrame
    namespace eval ::widgets::$site_4_0.publishButton {
        array set save {-command 1 -disabledforeground 1 -text 1}
    }
    namespace eval ::widgets_bindings {
        set tagslist _TopLevel
    }
    namespace eval ::vTcl::modules::main {
        set procs {
            init
            main
        }
        set compounds {
        }
        set projectType single
    }
}
}

#################################
# USER DEFINED PROCEDURES
#
#############################################################################
## Procedure:  main

proc ::main {argc argv} {}

#############################################################################
## Initialization Procedure:  init

proc ::init {argc argv} {}

init $argc $argv

#################################
# VTCL GENERATED GUI PROCEDURES
#

proc vTclWindow. {base} {
    if {$base == ""} {
        set base .
    }
    ###################
    # CREATING WIDGETS
    ###################
    wm focusmodel $top passive
    wm geometry $top 1x1+0+0; update
    wm maxsize $top 1585 1170
    wm minsize $top 1 1
    wm overrideredirect $top 0
    wm resizable $top 1 1
    wm withdraw $top
    wm title $top "vtcl.tcl"
    bindtags $top "$top Vtcl.tcl all"
    vTcl:FireEvent $top <<Create>>
    wm protocol $top WM_DELETE_WINDOW "vTcl:FireEvent $top <<DeleteWindow>>"

    ###################
    # SETTING GEOMETRY
    ###################

    vTcl:FireEvent $base <<Ready>>
}

proc vTclWindow.addDialog {base} {
    if {$base == ""} {
        set base .addDialog
    }
    if {[winfo exists $base]} {
        wm deiconify $base; return
    }
    set top $base
    ###################
    # CREATING WIDGETS
    ###################
    vTcl:toplevel $top -class Toplevel \
        -highlightcolor black 
    wm withdraw $top
    wm focusmodel $top passive
    wm maxsize $top 1585 1170
    wm minsize $top 1 1
    wm overrideredirect $top 0
    wm resizable $top 1 1
    wm title $top "Image"
    vTcl:DefineAlias "$top" "ImageToplevel" vTcl:Toplevel:WidgetProc "" 0
    bindtags $top "$top Toplevel all _TopLevel"
    vTcl:FireEvent $top <<Create>>
    wm protocol $top WM_DELETE_WINDOW "vTcl:FireEvent $top <<DeleteWindow>>"

    frame $top.innerFrame \
        -borderwidth 2 -height 75 -width 125 
    set site_3_0 $top.innerFrame
    frame $site_3_0.filenameFrame \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_4_0 $site_3_0.filenameFrame
    label $site_4_0.filenameLabel \
        -disabledforeground #a1a1a1 -text {Image file:} 
    ::iwidgets::entryfield $site_4_0.filenameEntry \
        -textbackground #ffffff -textvariable ::currentFilename 
    vTcl:DefineAlias "$site_4_0.filenameEntry" "FilenameEntry" vTcl:WidgetProc "ImageToplevel" 0
    pack $site_4_0.filenameLabel \
        -in $site_4_0 -anchor center -expand 0 -fill none -side left 
    pack $site_4_0.filenameEntry \
        -in $site_4_0 -anchor center -expand 1 -fill x -side left 
    frame $site_3_0.imageDisplayFrame \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_4_0 $site_3_0.imageDisplayFrame
    label $site_4_0.imageLabel \
        -disabledforeground #a1a1a1 
    vTcl:DefineAlias "$site_4_0.imageLabel" "ImageLabel" vTcl:WidgetProc "ImageToplevel" 0
    pack $site_4_0.imageLabel \
        -in $site_4_0 -anchor center -expand 1 -fill both -side top 
    frame $site_3_0.buttonsFrame \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_4_0 $site_3_0.buttonsFrame
    button $site_4_0.closeButton \
        -command closeImageShow -disabledforeground #a1a1a1 -text Close 
    pack $site_4_0.closeButton \
        -in $site_4_0 -anchor center -expand 0 -fill none -side top 
    pack $site_3_0.filenameFrame \
        -in $site_3_0 -anchor center -expand 0 -fill x -side top 
    pack $site_3_0.imageDisplayFrame \
        -in $site_3_0 -anchor center -expand 1 -fill both -side top 
    pack $site_3_0.buttonsFrame \
        -in $site_3_0 -anchor center -expand 0 -fill x -side bottom 
    ###################
    # SETTING GEOMETRY
    ###################
    pack $top.innerFrame \
        -in $top -anchor center -expand 1 -fill both -padx 10 -pady 10 \
        -side top 

    vTcl:FireEvent $base <<Ready>>
}

proc vTclWindow.mainDialog {base} {
    if {$base == ""} {
        set base .mainDialog
    }
    if {[winfo exists $base]} {
        wm deiconify $base; return
    }
    set top $base
    ###################
    # CREATING WIDGETS
    ###################
    vTcl:toplevel $top -class Toplevel \
        -highlightcolor black 
    wm focusmodel $top passive
    wm geometry $top +511+335; update
    wm maxsize $top 1585 1170
    wm minsize $top 1 1
    wm overrideredirect $top 0
    wm resizable $top 1 1
    wm deiconify $top
    wm title $top "Publish Image"
    vTcl:DefineAlias "$top" "MainWindow" vTcl:Toplevel:WidgetProc "" 0
    bindtags $top "$top Toplevel all _TopLevel"
    vTcl:FireEvent $top <<Create>>
    wm protocol $top WM_DELETE_WINDOW "vTcl:FireEvent $top <<DeleteWindow>>"

    frame $top.mainFrame \
        -borderwidth 2 -height 75 -width 125 
    set site_3_0 $top.mainFrame
    frame $site_3_0.cpd119 \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_4_0 $site_3_0.cpd119
    label $site_4_0.titleLabel \
        -anchor ne -disabledforeground #a1a1a1 -text Title: -width 13 
    ::iwidgets::entryfield $site_4_0.titleEntry \
        -textbackground #ffffff -textvariable titleText 
    vTcl:DefineAlias "$site_4_0.titleEntry" "TitleEntry" vTcl:WidgetProc "MainWindow" 0
    pack $site_4_0.titleLabel \
        -in $site_4_0 -anchor center -expand 0 -fill both -side left 
    pack $site_4_0.titleEntry \
        -in $site_4_0 -anchor center -expand 0 -fill x -padx 3 -side top 
    frame $site_3_0.cpd120 \
        -borderwidth 2 -relief sunken -height 75 -width 125 
    set site_4_0 $site_3_0.cpd120
    label $site_4_0.statusBarMessage \
        -anchor w -disabledforeground #a1a1a1 -text Ready \
        -textvariable ::status 
    vTcl:DefineAlias "$site_4_0.statusBarMessage" "StatusText" vTcl:WidgetProc "MainWindow" 0
    pack $site_4_0.statusBarMessage \
        -in $site_4_0 -anchor center -expand 1 -fill x -side left 
    frame $site_3_0.introFrame \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_4_0 $site_3_0.introFrame
    label $site_4_0.introLabel \
        -anchor ne -disabledforeground #a1a1a1 -text Introduction: -width 13 
    ::iwidgets::scrolledtext $site_4_0.introText \
        -hscrollmode dynamic -textbackground #ffffff -visibleitems 80x3 \
        -vscrollmode dynamic -wrap word 
    vTcl:DefineAlias "$site_4_0.introText" "IntroText" vTcl:WidgetProc "MainWindow" 0
    pack $site_4_0.introLabel \
        -in $site_4_0 -anchor center -expand 0 -fill both -side left 
    pack $site_4_0.introText \
        -in $site_4_0 -anchor center -expand 0 -fill x -padx 3 -side top 
    frame $site_3_0.imagesFrame \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_4_0 $site_3_0.imagesFrame
    frame $site_4_0.imagesInnerFrame \
        -borderwidth 1 -relief raised -height 75 -width 125 
    set site_5_0 $site_4_0.imagesInnerFrame
    ::iwidgets::scrolledlistbox $site_5_0.imagesListbox \
        -hscrollmode dynamic -listvariable ::imageList \
        -selectioncommand selectImage -textbackground #ffffff \
        -visibleitems 20x5 
    vTcl:DefineAlias "$site_5_0.imagesListbox" "ImagesListbox" vTcl:WidgetProc "MainWindow" 0
    checkbutton $site_5_0.showImageCheckbox \
        -command toggleShowImage -disabledforeground #a1a1a1 \
        -text {Show image} -variable ::showImage 
    vTcl:DefineAlias "$site_5_0.showImageCheckbox" "showImageCheckbox" vTcl:WidgetProc "MainWindow" 0
    frame $site_5_0.imageButtonsFrame \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_6_0 $site_5_0.imageButtonsFrame
    button $site_6_0.addButton \
        -command addImage -disabledforeground #a1a1a1 -text Add 
    button $site_6_0.cpd121 \
        -command deleteImage -disabledforeground #a1a1a1 -text Delete 
    button $site_6_0.upButton \
        -command {moveImage up} -disabledforeground #a1a1a1 -text Up 
    button $site_6_0.downButton \
        -command {moveImage down} -disabledforeground #a1a1a1 -text Down 
    pack $site_6_0.addButton \
        -in $site_6_0 -anchor center -expand 1 -fill none -side left 
    pack $site_6_0.cpd121 \
        -in $site_6_0 -anchor center -expand 1 -fill none -side left 
    pack $site_6_0.upButton \
        -in $site_6_0 -anchor center -expand 1 -fill none -side left 
    pack $site_6_0.downButton \
        -in $site_6_0 -anchor center -expand 1 -fill none -side left 
    pack $site_5_0.imagesListbox \
        -in $site_5_0 -anchor center -expand 1 -fill both -padx 10 -pady 5 \
        -side top 
    pack $site_5_0.showImageCheckbox \
        -in $site_5_0 -anchor center -expand 0 -fill none -side top 
    pack $site_5_0.imageButtonsFrame \
        -in $site_5_0 -anchor center -expand 0 -fill x -ipady 3 -padx 10 \
        -pady 5 -side top 
    label $site_4_0.imageLabel \
        -anchor ne -disabledforeground #a1a1a1 -text Images: -width 13 
    pack $site_4_0.imagesInnerFrame \
        -in $site_4_0 -anchor center -expand 1 -fill both -padx 3 -pady 5 \
        -side right 
    pack $site_4_0.imageLabel \
        -in $site_4_0 -anchor n -expand 0 -fill none -side top 
    frame $site_3_0.conclusionFrame \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_4_0 $site_3_0.conclusionFrame
    label $site_4_0.conclusionLabel \
        -anchor ne -disabledforeground #a1a1a1 -text Conclusion: -width 13 
    ::iwidgets::scrolledtext $site_4_0.conclusionText \
        -hscrollmode dynamic -textbackground #ffffff -visibleitems 80x3 \
        -vscrollmode dynamic 
    vTcl:DefineAlias "$site_4_0.conclusionText" "ConclusionText" vTcl:WidgetProc "MainWindow" 0
    pack $site_4_0.conclusionLabel \
        -in $site_4_0 -anchor center -expand 0 -fill both -side left 
    pack $site_4_0.conclusionText \
        -in $site_4_0 -anchor center -expand 1 -fill both -padx 3 -side left 
    frame $site_3_0.buttonsFrame \
        -borderwidth 2 -relief groove -height 75 -width 125 
    set site_4_0 $site_3_0.buttonsFrame
    button $site_4_0.publishButton \
        -command publishImages -disabledforeground #a1a1a1 -text Publish 
    pack $site_4_0.publishButton \
        -in $site_4_0 -anchor center -expand 0 -fill none -side top 
    pack $site_3_0.cpd119 \
        -in $site_3_0 -anchor center -expand 0 -fill x -side top 
    pack $site_3_0.cpd120 \
        -in $site_3_0 -anchor center -expand 0 -fill x -side bottom 
    pack $site_3_0.introFrame \
        -in $site_3_0 -anchor center -expand 0 -fill x -side top 
    pack $site_3_0.imagesFrame \
        -in $site_3_0 -anchor center -expand 1 -fill both -side top 
    pack $site_3_0.conclusionFrame \
        -in $site_3_0 -anchor center -expand 0 -fill x -side top 
    pack $site_3_0.buttonsFrame \
        -in $site_3_0 -anchor center -expand 0 -fill x -side top 
    ###################
    # SETTING GEOMETRY
    ###################
    pack $top.mainFrame \
        -in $top -anchor center -expand 1 -fill both -padx 10 -pady 10 \
        -side top 

    vTcl:FireEvent $base <<Ready>>
}

#############################################################################
## Binding tag:  _TopLevel

bind "_TopLevel" <<Create>> {
    if {![info exists _topcount]} {set _topcount 0}; incr _topcount
}
bind "_TopLevel" <<DeleteWindow>> {
    if {[set ::%W::_modal]} {
                vTcl:Toplevel:WidgetProc %W endmodal
            } else {
                destroy %W; if {$_topcount == 0} {exit}
            }
}
bind "_TopLevel" <Destroy> {
    if {[winfo toplevel %W] == "%W"} {incr _topcount -1}
}

Window show .
Window show .addDialog
Window show .mainDialog

main $argc $argv
