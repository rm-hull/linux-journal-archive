#!/bin/sh

function showCard
{
   card=$1

   if [ $card -lt 1 -o $card -gt 52 ] ; then
     echo "Bad card value: $card"
     exit 1
   fi

   suite="$(( ( ( $card - 1) / 13 ) + 1))"
   rank="$(( $card % 13))"

   case $suite in
     1 ) suite="hearts"	  ;;
     2 ) suite="clubs"    ;;
     3 ) suite="spades"   ;;
     4 ) suite="diamonds" ;;
     * ) echo "Bad suite value: $suite"; exit 1
   esac 
    
   case $rank in 
     0 ) rank="king" 	;;
     1 ) rank="ace"	;;
     11) rank="jack"    ;;
     12) rank="queen"   ;;
   esac
 	
   echo "Card $card is the $rank of $suite"
}

while /usr/bin/true
do
  echo -n "Card: "
  read card
  showCard $card
done

exit 0
