#!/bin/sh

function initializeDeck
{
    card=1
    while [ $card -lt 53 ]
    do
      deck[$card]=$card
      card=$(( $card + 1 ))
    done
}

function pickCard
{
    local errcount randomcard

    threshold=10		# max guesses for a card before we fall through
    errcount=0

    while [ $errcount -lt $threshold ]
    do
      randomcard=$(( ( $RANDOM % 52 ) + 1 ))
      errcount=$(( $errcount + 1 ))

      if [ ${deck[$randomcard]} -ne 0 ] ; then
	picked=${deck[$randomcard]}
	deck[$picked]=0		# picked, remove it
	# echo randomly picked $picked
        return $picked 
      fi
    done

    # If we get here, we've been uanble to randomly pick a card
    # so now we'll just step up until we find an available card

    randomcard=1

    while [ ${deck[$randomcard]} -eq 0 ]
    do
       randomcard=$(( $randomcard + 1 ))
    done

    picked=$randomcard
    deck[$picked]=0		# picked, remove it
    # echo fell through, picked $picked
    return $picked
}

function shuffleDeck
{
    count=1

    while [ $count -le 52 ]
    do
      pickCard
      newdeck[$count]=$picked
      # echo " .. pickCard returned $picked"
      count=$(( $count + 1 ))
    done

    echo "Shuffled the deck"
}


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
     1 ) suite="Hearts"	  ;;
     2 ) suite="Clubs"    ;;
     3 ) suite="Spades"   ;;
     4 ) suite="Diamonds" ;;
     * ) echo "Bad suite value: $suite"; exit 1
   esac 
    
   case $rank in 
     0 ) rank="King" 	;;
     1 ) rank="Ace"	;;
     11) rank="Jack"    ;;
     12) rank="Queen"   ;;
   esac

   cardname="$rank of $suite"
}

#################################################################
####  The main code block ....

initializeDeck
shuffleDeck

count=1
while [ $count -le 52 ] 
do
  showCard ${newdeck[$count]}
  echo "card $count = ${newdeck[$count]} = $cardname"
  count=$(( $count + 1 ))
done

echo -n "** Player's hand: "
  showCard ${newdeck[1]} ; echo -n "$cardname, "
  showCard ${newdeck[3]} ; echo "$cardname"

echo -n "** Dealer's hand: "
  showCard ${newdeck[2]} ; echo -n "$cardname, "
  showCard ${newdeck[4]} ; echo "$cardname"

exit 0
