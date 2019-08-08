#!/bin/bash
#
# Title  :   Monitor_smtp.sh 
# Author :   John_Ouellette@yahoo.com
# Files  :   monitor_smtp.sh, smtp.pl 
# Pupose :   Check SMTP servers and save a history
#
#
# Submit as 
#         
#Monitor_smtp
# 0,5,10,15,20,25,30,35,40,45,50,55 * * * * * \
#              /usr/local/admin/bin/monitor_smtp.sh
#
# in crontab to monitor every 5 minutes 
#
#
#################################
#
# 1.  Type in SMTP Servers here:
#
#################################

SMTPSERVERS="
SMTP1.somewhere
SMTP2.somewhere
SMTP3.somewhere
SMTP4.somewhere
SMTP5.somewhere
SMTP6.somewhere
SMTP7.somewhere
SMTP8.somewhere
SMTP9.somewhere
SMTP10.somewhere
"



##################################
#
# 2. Set Web Server Root
#    and Script Home
# 
##################################

WROOT="/var/www/html"
SHOME="smtpmon"

##################################################
# Start Script - Do not touch below this line !!!
##################################################
#
# System Variables
#
PING="/bin/ping"
GREP="/bin/grep"
AWK="/bin/awk"
HOSTNAME="/bin/hostname"
MAIL="/bin/mail"
NETSTAT="/bin/netstat"
TAIL="/usr/bin/tail"
CUT="/bin/cut"
TR="/usr/bin/tr"
RM="/bin/rm"
DATE="/bin/date"


HOUR=$($DATE +%H)
MIN=$($DATE +%M)
DAY=$($DATE +%d)
YEAR=$($DATE +%EY)
MNTH=$($DATE +%B)
WEBSRV=$($HOSTNAME)

WPAGE=$WROOT/$SHOME/smtp.html
PFILE=$WROOT/$SHOME/ping.out



#######################
#Are we on the network?
#######################
mkdir -p /tmp

#OK to assume one default gateway
$PING -c 2 $($NETSTAT -nr | $GREP UG |\
                   $AWK '{ print $2 }') 

if (( $? != 0 )); then
    echo "Not on Network - Exiting" > $WPAGE
    cat $PFILE  >> $WPAGE
    rm  $PFILE
    exit 1
fi


#######################
#Create basic HTML Page
#######################

cd $WROOT/$SHOME

$DATE > $WPAGE 
cat >> $WPAGE <<EOF
- <A HREF="history">History</A>
<P>
EOF



LTAG="</A><BR>"
#RTAG="<font color="#FF0000">" 
RTAG="<IMG SRC="../images/critical.gif">" 
GTAG="<IMG SRC="../images/ok.gif">" 
#GTAG="<font color="##4CC417">" 
EF="</font>"

################################
# Loop through SMTP servers and 
# send mail
################################

for SRV in  $SMTPSERVERS

do
mkdir -p $WROOT/$SHOME/history/$YEAR/$MNTH/$DAY/$SRV
WLOG="history/$YEAR/$MNTH/$DAY/$SRV/\
 $SRV.$HOUR.$MIN.log"
LINK="<A HREF="http://$WEBSRV/$SHOME/$WLOG">"
$DATE          > $WLOG

smtp.pl $SRV   2>> $WLOG

if (( $? != 0 ));then 
     echo $SRV is $RTAG   down  $EF $LINK \
              Current Log $LTAG  >> $WPAGE
else 
     echo $SRV is $GTAG    up   $EF $LINK \
              Current Log $LTAG  >> $WPAGE
fi 
        

done
