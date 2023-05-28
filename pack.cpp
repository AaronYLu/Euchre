/* Pack.h
 *
 * Represents a pack of playing cards
 * Project UID 1d9f47bfc76643019cfbf037641defe1
 *
 * by Andrew DeOrio <awdeorio@umich.edu>
 * 2014-12-21
 */


#include "Card.h"
#include <array>
#include <string>
#include "Pack.h"

using namespace std;

  // EFFECTS: Initializes the Pack to be in the following standard order:
  //          the cards of the lowest suit arranged from lowest rank to
  //          highest rank, followed by the cards of the next lowest suit
  //          in order from lowest to highest rank, and so on.
  // NOTE: The standard order is the same as that in pack.in.
  // NOTE: Do NOT use pack.in in your implementation of this function
  Pack::Pack() {
    int count = 0;
    for (int suit=SPADES; suit<=DIAMONDS; suit++){
        for (int rank=NINE; rank<=ACE; rank++){
            cards[count] = Card (static_cast<Rank>(rank),static_cast<Suit>(suit));
            count++;
        }
    }
    next=0;
  }

  // REQUIRES: pack_input contains a representation of a Pack in the
  //           format required by the project specification
  // MODIFIES: pack_input
  // EFFECTS: Initializes Pack by reading from pack_input.
  Pack::Pack(std::istream& pack_input) {
    for (int i=0; i<24; i++){
        std::string str1;
        std::string str2;
        std::string str3;
        pack_input >> str1 >> str2 >> str3;
        cards[i] = Card (string_to_rank(str1),string_to_suit(str3));
    }
    next=0;
  }

  // REQUIRES: cards remain in the Pack
  // EFFECTS: Returns the next card in the pack and increments the next index
  Card Pack::deal_one() {
    next++;
    return cards[next-1];
  }

  // EFFECTS: Resets next index to first card in the Pack
  void Pack::reset() {
    next=0;
  }

  // EFFECTS: Shuffles the Pack and resets the next index. This
  //          performs an in shuffle seven times. See
  //          https://en.wikipedia.org/wiki/In_shuffle.
  void Pack::shuffle(){
    for (int i=0; i<7; i++){
        std::array<Card, 12> firstHalf;
        for (int j=0;j<12;j++){
            firstHalf[j]=cards[j];
        }
        std::array<Card, 12> secondHalf;
        int count=0;
        for (int k=12;k<24;k++){
            secondHalf[count]=cards[k];
            count++;
        }
        int indexOld=0;
        for (int z=0;z<12;z++){
            cards[indexOld]=secondHalf[z];
            indexOld++;
            cards[indexOld]=firstHalf[z];
            indexOld++;
        }
    }
    reset();
  }

  // EFFECTS: returns true if there are no more cards left in the pack
  bool Pack::empty() const {
    if (next==24){
        return true;
    }
    return false;
  }