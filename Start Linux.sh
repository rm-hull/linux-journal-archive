#Get the directory the script was run from (launching from shell doesn't use that as the working dir)
#script name passed to sed must be same name as this script
SCRIPTDIR=`echo $0 | sed 's/Start Linux.sh//'`
echo $SCRIPTDIR
#relative path to server executable
AUTORUN=autorun.sh

"$SCRIPTDIR$AUTORUN" &

