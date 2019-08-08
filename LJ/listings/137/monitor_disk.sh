
#!/bin/ksh
#
# Title      :   Monitor_disk.sh 
# Author     :   John_Ouellette@yahoo.com
# Files      :   monitor_disk.sh 
# Purpose    :   Check servers disk,alert if 
#                above threshold
#
# NOTES: 
#
#
# Submit as 
#
#         
# Monitor_disk
# 0,15 * * * * * /usr/local/admin/bin/ \
#                        monitor_disk.sh
#
# in crontab to monitor every 15 minutes 
#
##################################
#
# 1. Set proper recipient(s).
#
##################################

RECIPIENT_MAX="mypager"
RECIPIENT_WARN="mygroup"

##################################
#
# 2. Type in your thresholds here:
#       
##################################

typeset -i THRESHOLD_MAX="95"
typeset -i THRESHOLD_WARN="90"

#################################################
# Start Script - Do not touch below this line !!!
#################################################
#
# System Variables
#
GREP="/sbin/grep"
MOUNT="/sbin/mount"
AWK="/usr/bin/awk"
DF="/usr/bin/df"
TAIL="/usr/bin/tail"
CUT="/usr/bin/cut"
MAILX="/usr/bin/mailx"
TR="/usr/bin/tr"
HOSTNAME=$(/usr/bin/hostname| $TR '[a-z]' '[A-Z]')
SED="/sbin/sed"
PING="/sbin/ping"
########################
#Are we on the network?


mkdir -p /tmp

#OK to assume one default gateway
$PING -c 2 $($NETSTAT -nr | $GREP UG \
                | $AWK '{ print $2 }') 

if (( $? != 0 )); then
echo "Not on Network - Exiting" 
    exit 1
fi


FILESYSTEMS=$($MOUNT | $GREP -iv proc | \
     $GREP -iv cdrom |  $AWK '{print $3}') 


for FS in $FILESYSTEMS
                                                                  
  do




typeset -i  UTILIZED=$($DF -k $FS | $TAIL -1 | \
             $AWK '{print $5}' | $CUT -d"%" -f1)
        ((UTILIZED+=0))

SUBJECT_MAX="Urgent:Filesystem $FS on $HOSTNAME \
 is $UTILIZED % full!"
SUBJECT_WARN="Warning: Filesystem $FS on $HOSTNAME\
 is $UTILIZED % full."
SUBJECT_OK="OK: Filesystem $FS on $HOSTNAME is \
 $UTILIZED % now."


MESSAGE_MAX="$SUBJECT_MAX Max  Threshold is \
 $THRESHOLD_MAX %.\nImmediate System Administrator\
 intervention is required."
MESSAGE_WARN="$SUBJECT_WARN Warn Threshold is \
 $THRESHOLD_WARN %. \nPotential problems if not\
 acted on."
MESSAGE_OK="$SUBJECT_OK"
        
FILE_MAX=max$(echo $FS| $SED s#/#_# )
FILE_WARN=warn$(echo $FS| $SED s#/#_# )
        

 mail_alert () 

 {
 STATE=$1

 case $STATE in

  MAX) echo "$MESSAGE_MAX"  | /usr/bin/mailx -s \
       "$SUBJECT_MAX"  "$RECIPIENT_MAX"  "$OTHER";;
 WARN) echo "$MESSAGE_WARN" | /usr/bin/mailx -s \
       "$SUBJECT_WARN" "$RECIPIENT_WARN" "$OTHER";;
   OK) echo "$MESSAGE_OK"   | /usr/bin/mailx -s \
       "$SUBJECT_OK"   "$RECIPIENT_MAX"  "$OTHER";;
 esac

 } 



 #Compare the utilized value against the threshold:
 if  (( "$UTILIZED"  >   "$THRESHOLD_MAX" )); then

                  #Check if alert sent
                  if [[ ! -a full.$FS ]];then
                    touch $FILE_MAX 
                    mail_alert MAX
                  else 
                    echo Alert already sent for\
                    $FS on $HOSTNAME   - waiting 
                   fi

 elif (( "$UTILIZED"  > "$THRESHOLD_WARN" )); then

    #Check if alert sent
    if [[ ! -a warn.$FS ]];then
        touch $FILE_WARN 
        mail_alert WARN 
    else
        echo Alert already sent for $FS\ 
        on $HOSTNAME   - waiting   
     fi

  else 
  #Remove file if needed 
    if [[  -a $FILE_WARN ||  -a $FILE_MAX ]];then
       rm -e $FILE_WARN  
       rm -e $FILE_MAX  
       mail_alert OK
    fi 

 fi


  done

exit 0

