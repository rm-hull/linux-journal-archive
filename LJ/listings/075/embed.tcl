frame .fs
frame .bl
frame .mode
label .fs.labfs  -text "sample rate\n\[Hz\]"
label .bl.labbl  -text "block length\n\[s\]"
foreach f {48000 44100 32000 22050 16000 11025 8000} {
  radiobutton .fs.f$f -text $f -value $f \
    -variable fs -command "NewValue fs $f" -anchor w
}
foreach l {  100   500  1000  2000  5000  10000 20000} {
  radiobutton .bl.l$l -text [expr $l/1000.0] -value $l\
    -variable bl -command "NewValue bl $l" -anchor w
}
scale .tau -label tau     -from 100 -to 1 -variable tau\
      -orient vertical    -command "NewValue tau"
scale .de  -label de      -from  20 -to 1 -variable de\
      -orient vertical    -command "NewValue de"
scale .steps -label steps -from  20 -to 1 -variable steps\
      -orient vertical    -command "NewValue steps"

button .reset -text Reset                       \
      -command "puts stdout Reset" -anchor w
button .quit  -text Quit                        \
      -command "puts stdout Quit; exit" -anchor w

radiobutton .mode.single -text single -value 0  \
       -variable mode -command "NewValue mode 0" -anchor w
radiobutton .mode.accu   -text accumulate -value 1\
       -variable mode -command "NewValue mode 1" -anchor w

set fs   8000
set bl   1000
set tau     1
set de      3
set steps  20
set mode    0
pack .fs -side left -fill y
pack .bl -side left -fill y
pack .fs.labfs  .fs.f48000 .fs.f44100 .fs.f32000 .fs.f22050\
     .fs.f16000 .fs.f11025 .fs.f8000 -side top -anchor w
pack .bl.labbl  .bl.l20000 .bl.l10000 .bl.l5000  .bl.l2000\
     .bl.l1000  .bl.l500   .bl.l100  -side top -anchor w
pack .tau .de .steps .reset .quit -side left -fill y
pack .reset .quit -side top -fill x
pack .mode -side top
pack .mode.single .mode.accu -side top -fill x

proc NewValue {Name Value} { puts stdout "$Name  [expr $Value]"; }

NewValue fs   $fs
NewValue bl   $bl
NewValue mode $mode
