// Project UID 1d9f47bfc76643019cfbf037641defe1

#include <iostream>
#include "Card.h"
#include "Player.h"
#include "Pack.h"
#include <fstream>

using namespace std;
//change lead each trick
class Game{
public:
    Game(char **argv){
        Player *player1 = Player_factory (argv[4], argv[5]);
        Player *player2 = Player_factory (argv[6], argv[7]);
        Player *player3 = Player_factory (argv[8], argv[9]);
        Player *player4 = Player_factory (argv[10], argv[11]);
        players.push_back(player2);
        players.push_back(player3);
        players.push_back(player4);
        players.push_back(player1);
        //eldest hand is player 2 
        points=atoi(argv[3]);
        ifstream pack_filename (argv[1]);
        pack = Pack(pack_filename);
        dealer = 3;
        doShuffle=argv[2];
        round=1;
        lead=0;
        hand=0;
        t1Points=0;
        t2Points=0;
        firstTrump=0;      
    }
    void play(){
        while (t1Points<points && t2Points<points){
            if (doShuffle=="shuffle"){
                shuffle();
            }
            pack.reset();
            deal();
            cout << "Hand " << hand << endl;
            cout << (*players[dealer]).get_name() << " deals" << endl;
            cout << upcard << " turned up" << endl;
            make_trump();
            play_hand();
            hand++;
            dealerShift();
            cout << (*players[3]).get_name() << " and " <<
            (*players[1]).get_name() << " have " << t1Points << 
            " points" <<endl;
            cout << (*players[0]).get_name() << " and " <<
            (*players[2]).get_name() << " have " << t2Points << 
            " points" <<endl;
            cout <<endl;
        }       
        if (t2Points>=points){
            cout << (*players[0]).get_name() << " and " <<
            (*players[2]).get_name() << " win!" <<endl;
        }
        else if (t1Points>=points){
             cout << (*players[3]).get_name() << " and " <<
            (*players[1]).get_name() << " win!" <<endl;
        }
        for (int i=0;i<players.size();i++){
            delete players[i];
        }
    } 

private:
    std::vector<Player*> players;
    Pack pack;
    int points;
    int t1Points; //points of team1, player index 1,3
    int t2Points; //points of team2, player index 0,2
    bool t1ordered;
    int dealer;
    int firstTrump; //first to pass/order up trump
    string doShuffle;
    int round; //round for making trump
    int hand;
    int lead; //index of the player who leads a hand
    Card upcard; //do i need to initialize this in constructor
    //or is it ok to leave for later in deal
    Suit trumpSuit; //initialize in constructor? this might be broken

    void shuffle(){
        pack.shuffle();
    }

    void deal() {
        deal3(players[(dealer+1)%4]);
        deal2(players[(dealer+2)%4]);
        deal3(players[(dealer+3)%4]);
        deal2(players[dealer]);
        deal2(players[(dealer+1)%4]);
        deal3(players[(dealer+2)%4]);
        deal2(players[(dealer+3)%4]);
        deal3(players[dealer]);
        upcard=pack.deal_one();
        trumpSuit = upcard.get_suit();
    }

    void deal3(Player * player){
        for (int i=0;i<=2;i++){
            (*player).add_card(pack.deal_one());
        }
    }

    void deal2(Player * player){
        for (int i=0;i<=1;i++){
            (*player).add_card(pack.deal_one());
        }
    }

    void dealerShift(){ //shifts the dealer
        if (dealer!=3){
            dealer++;
        }
        else{
            dealer=0;
        }
    }

    void make_trump(){   
        int count=0;
        while (round<3){
            make_trump_helper();
            count++;  
            round++;         
        }
        if (count==1){ 
            (*players[dealer]).add_and_discard(upcard);
        }
        round=1;
        firstTrump++;
    }

    void make_trump_helper(){
        //essentially here, our i always starts at 0 (the first guy)
        //in reality, our i should move up by one each hand. 
        for (int i=0;i<=3;i++){
            int num=(firstTrump+i)%4;
            if ((*players[num]).make_trump(upcard, dealer==num, round, trumpSuit)==true){
                cout << (*players[num]).get_name() << " orders up " << trumpSuit <<endl;
                if (dealer!=num){
                    cout << endl;
                }
                if (num==1||num==3){
                    t1ordered=true;
                }
                else{
                    t1ordered=false;
                }
                round=5;
                return;
            }
            else{
                cout << (*players[num]).get_name() << " passes" <<endl;
            }
        }
    }

    void play_hand() {
        int t1=0;
        int t2=0;
        lead = (dealer+1)%4;
        for (int i=0;i<=4;i++){
            int winner = play_trick();
            if (winner==1 || winner==3){
                t1++;
            }
            else{
                t2++;
            }
            lead=winner;
        }
        if (t1>t2){
            //t1 index 1 and 3
            cout << (*players[3]).get_name() << " and " <<
            (*players[1]).get_name() << " win the hand" <<endl;
            if (t1ordered==true){
                if (t1==3 || t1==4){
                    t1Points+=1;
                }
                else if (t1==5) {
                    t1Points+=2; //march
                    cout << "march!" << endl;
                }
            }
            else{
                t1Points+=2; //euchred
                cout << "euchred!" << endl;
            }
        }
        else{
            cout << (*players[0]).get_name() << " and " <<
            (*players[2]).get_name() << " win the hand" <<endl;
            if (t1ordered==false){
                if (t2==3 || t2==4){
                    t2Points+=1;
                }
                else if (t2==5) {
                    t2Points+=2; 
                    cout << "march!" <<endl;
                }
            }
            else{
                t2Points+=2; 
                cout << "euchred!" <<endl;
            }
        }        
    }

    int play_trick() {
        int count=lead;
        vector <Card> trick; 
        trick.push_back((*players[lead]).lead_card(trumpSuit));
        cout << trick[0] << " led by " << (*players[lead]).get_name() <<endl;
        for (int i=1;i<=3;i++){
            if (count+1==4){
                count=0;
                trick.push_back((*players[count]).play_card(trick[0], trumpSuit));
            }
            else{
                count++;
                trick.push_back((*players[count]).play_card(trick[0], trumpSuit));
            }
            cout << trick[i] << " played by " << (*players[count]).get_name() << endl; 
        }
        Card *max = &trick[0];
        int ind = 0;
        for (int i=0;i<trick.size();i++){
            if (Card_less(trick[i], *max, trick[0], trumpSuit)==false){
                max=&trick[i];
                ind=i;
            }
        }
        int winner = (lead+ind)%4;
        cout << (*players[winner]).get_name() << " takes the trick" << endl;
        cout <<endl;
        return winner; //returns winner through original index of players
    }
};

int main(int argc, char **argv) {
    if (argc!=12){
    cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
    << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
    << "NAME4 TYPE4" << endl;
    return 1;
    }
  ifstream pack_filename (argv[1]);
  int points = atoi (argv[3]);
  string shuffle = argv[2];
  string name1=argv[4];
  string type1=argv[5];
  string name2=argv[6];
  string type2=argv[7];
  string name3=argv[8];
  string type3=argv[9];
  string name4=argv[10];
  string type4=argv[11];
  if (points<1 || points > 100 || 
  (shuffle != "shuffle" && shuffle != "noshuffle") || 
  (type1!="Simple"&&type1!="Human") || 
  (type2!="Simple"&&type2!="Human") || 
  (type3!="Simple"&&type3!="Human") || 
  (type4!="Simple"&&type4!="Human")){
    cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
    << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
    << "NAME4 TYPE4" << endl;
    return 1;
  }
  if (!pack_filename.is_open()){
            cout << "Error opening " << argv[1] << endl;
            return 1;
  }
  for (int i=0;i<argc; i++){
    cout << argv[i] << " ";
  }
  cout<<endl;
  Game game1(argv);
  game1.play(); 
  return 0;
}