#!/bin/sh

dealerwin=0
playerwin=0

function initializeDeck
{
    card=1
    while [ $card -lt 53 ]
    do
      deck[$card]=$card
      card=$(( $card + 1 ))
    done
    player[1]=-1; player[2]=-1; player[3]=-1; player[4]=-1; player[5]=-1
    dealer[1]=-1; dealer[2]=-1; dealer[3]=-1; dealer[4]=-1; dealer[5]=-1
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

function handValue
{
   # feed this as many cards as are in the hand
   handvalue=0	# initialize
   for cardvalue
   do
     if [ $cardvalue -ge 0 ] ; then
       rankvalue=$(( $cardvalue % 13 ))
       case $rankvalue in
         0|11|12 ) rankvalue=10 	;;
         1       ) rankvalue=11	;;
       esac

       handvalue=$(( $handvalue + $rankvalue ))
     fi
   done
}

#################################################################
####  The main code block ....

echo "**** Welcome to Blackjack.sh! **** "

while [ /bin/true ] 
do

# technically we should deal a certain number of hands from a deck 
# before we shuffle again, but for the purposes of this game, we'll
# just go ahead and shuffle before each hand.

initializeDeck
shuffleDeck

# Deal!

player[1]=${newdeck[1]}
player[2]=${newdeck[3]}
nextplayercard=3	# player starts with two cards

dealer[1]=${newdeck[2]}
dealer[2]=${newdeck[4]}
nextdealercard=3	# dealer also has two cards

nextcard=5		# we've dealt the first four cards already

# test for dealer or player blackjack

handValue ${player[1]} ${player[2]}
playerhandvalue=$handvalue
handValue ${dealer[1]} ${dealer[2]}
dealerhandvalue=$handvalue

echo ""

if [ $playerhandvalue -eq 21 -a $dealerhandvalue -eq 21 ] ; then
  echo "Extraordinary!  Both the dealer and player were dealt a blackjack!"
  echo "Unfortunately, this means you didn't win: it's a push (or tie)."
  echo ""
  exit 0
fi

if [ $dealerhandvalue -eq 21 ] ; then
  echo "Darn it!  Dealer pulled a blackjack out of their hat. You lose."
  echo ""
  exit 0
elif [ $playerhandvalue -eq 21 ] ; then
  echo "NICE! You got a blackjack and won the game. Payout would be 3:2!"
  echo ""
  exit 0
fi

# no blackjack, game proceeds as usual...

# first off, we need to show one of the dealer's cards, so we'll show card #1

echo -n "Dealer's hand: "
showCard ${dealer[1]} ; echo -n "$cardname, "
echo "(card face down)"
echo ""

# and your hand:

echo -n "You've been dealt: "
showCard ${player[1]} ; echo -n "$cardname, "
showCard ${player[2]} ; echo "$cardname"
echo ""

handValue ${player[1]} ${player[2]}

while [ $handvalue -lt 22 ]
do

  if [ $handvalue -lt 16 ] ; then
    echo -n "H)it or S)tand? (recommended: hit) "
  else
    echo -n "H)it or S)tand? (recommended: stand) "
  fi

  read answer
  if [ "$answer" = "stand" -o "$answer" = "s" ] ; then 
     break
  elif [ "$answer" = "q" -o "$answer" = "quit" ] ; then
    echo "" ; echo -n "Player quits. Standings: dealer wins: $dealerwin "
    echo "and player wins: $playerwin" ; echo ""
    exit 0
  fi

  player[$nextplayercard]=${newdeck[$nextcard]}

  showCard ${player[$nextplayercard]}

  echo "" ; echo "You've been dealt: $cardname" ; echo ""

  handValue ${player[1]} ${player[2]} ${player[3]} ${player[4]} ${player[5]}

  nextcard=$(( $nextcard + 1 ))
  nextplayercard=$(( $nextplayercard + 1 ))
done

if [ $handvalue -gt 21 ] ; then
  echo "Busted!  Your hand is worth $handvalue"
  echo ""
  echo "****** Dealer Wins!"
  dealerwin=$(( $dealerwin + 1 ))
  break
else
  echo ""
  echo "You stand with a hand value of $handvalue"
  echo ""
  playerhandvalue=$handvalue
fi

# now the dealer has to play out their hand, with standard Vegas
# rules...

echo -n "Dealer's hand: "
  showCard ${dealer[1]} ; echo -n "$cardname, "
  showCard ${dealer[2]} ; echo "$cardname"

handValue ${dealer[1]} ${dealer[2]}

while [ $handvalue -lt 17 ]
do
  dealer[$nextdealercard]=${newdeck[$nextcard]}

  showCard ${dealer[$nextdealercard]}

  nextcard=$(( $nextcard + 1 ))
  nextdealercard=$(( $nextdealercard + 1 ))

  echo "" ; echo "Dealer takes: $cardname"

  handValue ${dealer[1]} ${dealer[2]} ${dealer[3]} ${dealer[4]} ${dealer[5]}
done

  echo ""

  if [ $handvalue -gt 21 ] 
  then
    echo "**** Dealer busted!  Player wins with $playerhandvalue!" 
    playerwin=$(( $playerwin + 1 ))
  elif [ $handvalue -eq $playerhandvalue ]
  then
    echo "**** Dealer and player tie with $handvalue points."
  elif [ $handvalue -lt $playerhandvalue ] 
  then
    echo "**** Player wins with $playerhandvalue"
    playerwin=$(( $playerwin + 1 ))
  else 
    echo "**** Dealer wins with $handvalue"
    dealerwin=$(( $dealerwin + 1 ))
  fi

 echo ""
 echo "---------------------------------------------------"

done

exit 0
