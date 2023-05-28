/* Player.h
 *
 * Euchre player interface
 * Project UID 1d9f47bfc76643019cfbf037641defe1
 *
 * by Andrew DeOrio
 * awdeorio@umich.edu
 * 2014-12-21
 */


#include "Card.h"
#include <string>
#include <vector>
#include "Player.h"
#include <cassert>
#include <algorithm>
using namespace std;

class SimplePlayer : public Player{
  public: 

    SimplePlayer (std::string str) : name(str){
    }
    //EFFECTS returns player's name
    virtual const std::string & get_name() const {
      return name;
    }

    //REQUIRES player has less than MAX_HAND_SIZE cards
    //EFFECTS  adds Card c to Player's hand
    virtual void add_card(const Card &c) {
        //DOES c dereference itself?
      Cards.push_back(c);
    }

    //REQUIRES round is 1 or 2
    //MODIFIES order_up_suit
    //EFFECTS If Player wishes to order up a trump suit then return true and
    //  change order_up_suit to desired suit.  If Player wishes to pass, then do
    //  not modify order_up_suit and return false.
    virtual bool make_trump(const Card &upcard, bool is_dealer,
                            int round, Suit &order_up_suit) const {
    Suit sameColor = Suit_next(upcard.get_suit());
    if (round==1){
      int count=0;
      Card lowest = Card(TEN, upcard.get_suit());
      for (int i=0; i<Cards.size(); i++){
        if (Card_less(lowest, Cards[i], upcard.get_suit()) == true){
          count++;
        }
      }
      
      if (count>=2){
        order_up_suit=upcard.get_suit();
        return true;
      }
      return false;
    }
    else if (is_dealer==false){
      int count=0;
        Card lowest = Card(TEN, sameColor);
        for (int i=0; i<Cards.size(); i++){
          if (Card_less(lowest, Cards[i], sameColor) == true){
            count++;
          }
        }
        //SHOULDN"T THIS BE count >= 1?
        if (count>=1){
          order_up_suit=sameColor;
          return true;
        }
        return false;
    }
    else{
      order_up_suit=sameColor;
      return true;
    }                            
  }

    //REQUIRES Player has at least one card
    //EFFECTS  Player adds one card to hand and removes one card from hand.
    virtual void add_and_discard(const Card &upcard) {
      Cards.push_back(upcard);
      Card min = Cards[0];
      int num=0;
      for (int i=0; i<Cards.size(); i++){
        if (Card_less(Cards[i], min, upcard.get_suit())==true){
          min=Cards[i];
          num=i;
        }
      }
      Cards.erase(Cards.begin() + num);
    }

    //REQUIRES Player has at least one card
    //EFFECTS  Leads one Card from Player's hand according to their strategy
    //  "Lead" means to play the first Card in a trick.  The card
    //  is removed the player's hand.
    virtual Card lead_card(Suit trump) {
      bool allTrump=true;
      vector<Card> noTrump;
      for (int i=0; i<Cards.size(); i++){
        if (Cards[i].is_trump(trump)!=true){
            
          noTrump.push_back(Cards[i]);
          allTrump=false;
        }
      }
      if (allTrump==true){
        Card max = Cards[0];
        int num=0;
        for (int i=0; i<Cards.size();i++){
            //How to know when to use CardLess and when not to
          if (Card_less(max, Cards[i],trump)){
            max=Cards[i];
            num=i;
          }
        }
        Cards.erase(Cards.begin() + num);
        return max;
      }
      else{
        Card max = noTrump[0];
        for (int i=0; i<noTrump.size();i++){
          if (Card_less(max, noTrump[i],trump)){
            max=noTrump[i];
          }
        }
        //not efficient
        for (int i=0; i<Cards.size();i++){
          if (Cards[i] == max){
            Cards.erase(Cards.begin() + i);
          }
        }
        return max;
      }
    }

    //REQUIRES Player has at least one card
    //EFFECTS  Plays one Card from Player's hand according to their strategy.
    //  The card is removed from the player's hand.
    virtual Card play_card(const Card &led_card, Suit trump) {   
      bool canFollowSuit=false;
      vector<Card> FollowSuit;
      for (int i=0; i<Cards.size();i++){
        if (Cards[i].is_left_bower(trump)==false
        && led_card.is_left_bower(trump) == false && 
        Cards[i].get_suit()==led_card.get_suit()){
          canFollowSuit=true;
          FollowSuit.push_back(Cards[i]);         
        }
        else if (led_card.is_left_bower(trump)==true && 
        Suit_next(led_card.get_suit()) == Cards[i].get_suit()){
          canFollowSuit=true;
          FollowSuit.push_back(Cards[i]);
        }
        else if (Cards[i].is_left_bower(trump)==true && 
        led_card.get_suit()==trump){
          canFollowSuit=true;
          FollowSuit.push_back(Cards[i]);
        }
      }
      if (canFollowSuit==true){
        Card max = FollowSuit[0];
        for (int i=0; i<FollowSuit.size(); i++){
          if (Card_less(max, FollowSuit[i], led_card, trump) == true){
            max=FollowSuit[i];
          }
        }
        for(int i = 0; i < Cards.size(); i++){
          if(Cards[i] == max){
            Cards.erase(Cards.begin() + i);
          }
        }
        return max;
      }
      else{
        Card min = Cards[0];
        int num=0;
        for (int i=0; i<Cards.size(); i++){
          if (Card_less(Cards[i], min, trump)==true){
            min=Cards[i];
            num=i;
          }
        }
        Cards.erase(Cards.begin()+num);
        return min;
      }
    }

    // Maximum number of cards in a player's hand
    static const int MAX_HAND_SIZE = 5;

  private:
    string name;
    vector<Card> Cards;
};

class HumanPlayer : public Player{

  public:
    HumanPlayer (string str) : name(str){
    }

    //EFFECTS returns player's name
    virtual const std::string & get_name() const {
      return name;
    }

    //REQUIRES player has less than MAX_HAND_SIZE cards
    //EFFECTS  adds Card c to Player's hand
    virtual void add_card(const Card &c) {
      hand.push_back(c);
    }

    //REQUIRES round is 1 or 2
    //MODIFIES order_up_suit
    //EFFECTS If Player wishes to order up a trump suit then return true and
    //  change order_up_suit to desired suit.  If Player wishes to pass, then do
    //  not modify order_up_suit and return false.
    virtual bool make_trump(const Card &upcard, bool is_dealer,
                            int round, Suit &order_up_suit) const {
    //DO we assume the player will enter the correct suit (the upturned card)
      vector<Card> altHand = hand;
      std::sort(altHand.begin(), altHand.end());
      print_hand(altHand);
      cout << "Human player " << name << ", please enter a suit, or \"pass\":\n"; 
      string decision;
      cin >> decision;
      if (decision != "pass") {
        order_up_suit = string_to_suit(decision);
        return true;
      }
      else {
        return false;
      }
    }

    //REQUIRES Player has at least one card
    //EFFECTS  Player adds one card to hand and removes one card from hand.
    virtual void add_and_discard(const Card &upcard) {
      std::sort(hand.begin(), hand.end());
      print_hand();
      cout << "Discard upcard: [-1]\n";
      cout << "Human player " << name << ", please select a card to discard:\n";
      cout <<endl;

      //where is the implementation for push_back?
      hand.push_back(upcard);

      string decision;
      cin >> decision; 

      if(decision == "-1"){
        hand.erase(hand.begin() + hand.size() - 1);
      }
      else {
        //currently 6 cards in hand
        for(int i = 0; i < hand.size() - 1; i++){
            if(i == stoi(decision)){
                hand.erase(hand.begin() + i);
            }
        }
      }
    }

    //REQUIRES Player has at least one card
    //EFFECTS  Leads one Card from Player's hand according to their strategy
    //  "Lead" means to play the first Card in a trick.  The card
    //  is removed the player's hand.
    virtual Card lead_card(Suit trump) {
        std::sort(hand.begin(), hand.end());
        print_hand();
        cout << "Human player " << name << ", please select a card:\n";

        string decision;
        cin >> decision;
        
        Card c1;
        for(int i = 0; i < hand.size(); i++){
            if(i == stoi(decision)){
              c1 = hand[i];
              hand.erase(hand.begin() + i);               
            }
        } 
        return c1;
    }

    //REQUIRES Player has at least one card
    //EFFECTS  Plays one Card from Player's hand according to their strategy.
    //  The card is removed from the player's hand.
    virtual Card play_card(const Card &led_card, Suit trump) {
        std::sort(hand.begin(), hand.end());
        print_hand();
        cout << "Human player " << name << ", please select a card:\n";
        string decision;
        cin >> decision;
        Card c1;
        for(int i = 0; i < hand.size(); i++){
            if(i == stoi(decision)){
              c1 = hand[i];
              hand.erase(hand.begin() + i);
            }
        } 
        return c1;
    }

    // Maximum number of cards in a player's hand
    static const int MAX_HAND_SIZE = 5;

  private:
   string name;
   vector<Card> hand;

   void print_hand() const {
      for (size_t i=0; i < hand.size(); ++i){
        cout << "Human player " << name << "'s hand: "
        << "[" << i << "] " << hand[i] << "\n";
      }
    }

    void print_hand(vector<Card> altHand) const { //for sort in makeTrump
      for (size_t i=0; i < altHand.size(); ++i){
        cout << "Human player " << name << "'s hand: "
        << "[" << i << "] " << altHand[i] << "\n";
      }
    }
};




//EFFECTS: Returns a pointer to a player with the given name and strategy
//To create an object that won't go out of scope when the function returns,
//use "return new Simple(name)" or "return new Human(name)"
//Don't forget to call "delete" on each Player* after the game is over
Player * Player_factory(const std::string &name, const std::string &strategy){
  // We need to check the value of strategy and return 
  // the corresponding player type.
  if (strategy == "Simple") {
    // The "new" keyword dynamically allocates an object.
    return new SimplePlayer(name);
  }
  // Repeat for each other type of Player
  else if (strategy == "Human"){
    return new HumanPlayer(name);
  }
  // Invalid strategy if we get here
  assert(false);
  return nullptr;

}

//EFFECTS: Prints player's name to os
std::ostream & operator<<(std::ostream &os, const Player &p) {
  os<<p.get_name();
  return os;
}