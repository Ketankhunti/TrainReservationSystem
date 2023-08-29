#include <bits/stdc++.h>
using namespace std;

class Route{
    public:
    pair<string, int> station;
};
class Ticket{
    public:
    string status;//status : Confirmed or Waiting
    string seatNumber; //seat number or waiting number
    pair<string,int> src;
    pair<string,int> dest;
    char coachCategory;//letter of the coach : S,B,A,H
    string date;
    int pnr;
};
class Seat{
    public:
    int number;
    char coachCategory;//letter of the coach : S,B,A,H
    int coachNum;
    //maping booked ticket with pnr number
    unordered_map<int,Ticket*> tickets;
    //unordered_set<string> dates;
    Seat(){
        this->number = 0;
    }
    Seat(int number){
        this->number = number;
    }
};
class UserTicket{
    public:
    int pnrNumber;
    string trainNumber;
    pair<string,int> src;
    pair<string,int> dest;
    string date;
    string coachClass;//sl,3a,2a,1a 
    int totalFare;
    int totalSeatsBooked;
    vector<pair<Ticket*,Seat*>> ticketSeatMap;
    // vector<string> seatNumbers;
    // vector<string> waitingSeatsNumber;
};

class Coach{
    public:
    char category;//letter of the coach : S,B,A,H
    string coachClass; //string of SL,3A,2A,1A
    int coachNum;
    vector<Seat *> seats;
};
// class WaitingTicket{
//     public:
//     string pnrNumber;
//     string src;
//     string dest;
//     string category;
//     string date;
// };
class Train{
    public:
    string number;
    //each train will have waiting number starting from 1
    int waitingCount = 1;
    unordered_map<string,int> routes;
    vector<Coach*> coaches;
    //to map waiting number with a ticket
    map<string,Ticket*> waitingList;
    set<Seat*> cancelledSeats;
};

//function to split string by space
vector<string> split(string s){
    vector<string> ans;
    string temp = "";
    for (auto i : s){
        if (i == ' '){
            ans.push_back(temp);
            temp = "";
        }
        else temp += i;
    }
    ans.push_back(temp);
    return ans;
}
//function to split string by hyphen
vector<string> splitHyphen(string s){
    vector<string> ans;
    string temp = "";
    for (int i = 0; i < s.size(); i++){
        if (s[i] == '-'){
            ans.push_back(temp);
            temp = "";
        }
        else{
            temp += s[i];
        }
    }
    ans.push_back(temp);
    return ans;
}
/*
function to add new route
input : Rajkot-100
*/
pair<string,int> addRoute(string s){
    vector<string> temp = splitHyphen(s); //separate station name and distance
    return make_pair(temp[0], stoi(temp[1]));
}
/*
function to add new coach
input : S1-10
*/
Coach* addCoach(string s){
    unordered_map<char,string> mp;
    mp['S'] = "SL";
    mp['B'] = "3A";
    mp['A'] = "2A";
    mp['H'] = "1A";
    Coach *c = new Coach();
    vector<string> temp = splitHyphen(s);
    c->category = temp[0][0];
    c->coachClass = mp[c->category];
    c->coachNum = stoi(temp[0].substr(1));
    int seatCount = stoi(temp[1]);
    //adding seats in the coach
    vector<Seat *> coachSeats(seatCount);
    for (int i = 0; i < seatCount; i++){
        Seat* currSeat = new Seat(i+1);
        currSeat->coachCategory = c->category;
        currSeat->coachNum = c->coachNum;
        coachSeats[i] = currSeat;   
    }
    c->seats = coachSeats;
    return c;
}
int getStationDistance(Train* t,string station){
    return t->routes[station];
}
//function to calculate total fare
int findFare(UserTicket* userTicket){
    unordered_map<string, int> mp;
    mp["SL"] = 1;
    mp["3A"] = 2;
    mp["2A"] = 3;
    mp["1A"] = 4;
    int srcDistance = userTicket->src.second;
    int destDistance = userTicket->dest.second;
    int distanceTravelled = destDistance - srcDistance;
    return (distanceTravelled * userTicket->totalSeatsBooked * mp[userTicket->coachClass]);
}
bool checkRouteDateCollision(Train* trn,Ticket* tkt,string src,string dest,string d){
    //if dates are different then no collision
    if(tkt->date != d) return true;
    //current source and destination distance
    int srcDistance = trn->routes[src];
    int destDistance = trn->routes[dest];
    
    if(srcDistance >= tkt->dest.second) return true;
    if(destDistance <= tkt->src.second) return true;
    return false;
}
//function to check is the given seat is booked for a particular date
bool isSeatNotReserved(Train* t,Seat *s,string date,string src,string dest){
    //if date is free then no need to check for route clash
    bool status = true;//here true means seat is free
    //traversing all tickets for a given seat
    for(auto tkt:s->tickets){
        status = status && checkRouteDateCollision(t,tkt.second,src,dest,date);
    }
    return status; 
}
//function to check for if vacant seats >= given booking request
bool checkVacancy(string trainNum,unordered_map<string,Train *> trn, string src,string dest,string date,string cate, int quant){
    Train* currTrain = trn[trainNum];
    int vacantSeats = 0;
    for (auto coach : currTrain->coaches){
        if (coach->coachClass == cate){
            for (auto seat : coach->seats){
                if (vacantSeats >= quant) return true;
                if (isSeatNotReserved(currTrain,seat,date,src,dest)){
                    vacantSeats++;
                }
            }
        }
    }
    return (vacantSeats >= quant);
}
/*
function to find the trains based on the given inputed route
input : user request string/array trains map
output: set of suitable train numbers
*/
set<string> getAvailableTrains(vector<string>& userRequest,unordered_map<string, Train*> trains){
    set<string> ans;
    string src = userRequest[0];
    string dest = userRequest[1];
    string coachCategory = userRequest[3];

    for(auto i:trains){
        if( (i.second->routes.find(src) != i.second->routes.end()) && (i.second->routes.find(dest) != i.second->routes.end())){
            //check for reverse journey
            int srcDistance = i.second->routes[src];
            int destDistance = i.second->routes[dest];
            if(destDistance > srcDistance){
                //checking for coach category
                for(auto j:i.second->coaches){
                    if(j->coachClass == coachCategory){
                        ans.insert(i.first);
                        break;
                    }
                }
            }
        }
    }
    return ans;
}
/*
Main book ticket function
this will return a 2d vector
ans = vector<vector<string>>
ans[0] = vector of booked seates number
ans[1] = vector of waiting seates number
*/
vector<pair<Ticket*,Seat*>> bookTicket(UserTicket* currTicket,Train *trn,vector<string>& userRequest,int pnr){
    vector<pair<Ticket*,Seat*>> ans;
    unordered_map<string, char> mp;
    mp["SL"] = 'S';
    mp["3A"] = 'B';
    mp["2A"] = 'A';
    mp["1A"] = 'H';
    string src = userRequest[0];
    string dest = userRequest[1];
    string journeyDate = userRequest[2];
    string coachCategory = userRequest[3];
    int totalSeats = stoi(userRequest[4]);

    int seatsBooked = 0;
    //vector<string> bookSeat;//to store the booked seats (for future use)
    //first booking all the free seats
    for (auto coach : trn->coaches){
        if (coach->category == mp[coachCategory]){
            for (auto seat : coach->seats){
                if (seatsBooked == totalSeats) return ans; 
                //if current seat is not reserved then book the ticket
                if (isSeatNotReserved(trn,seat,journeyDate,src,dest)){
                    seatsBooked++;
                    string seatNumber = coach->category + to_string(coach->coachNum) + "-" + to_string(seat->number);
                    Ticket* newTicket = new Ticket();
                    seat->tickets[pnr] = newTicket;//mapping pnr with ticket
                    newTicket->status = "Confirmed";
                    newTicket->seatNumber = seatNumber;
                    newTicket->src = make_pair(src,trn->routes[src]);
                    newTicket->dest = make_pair(dest,trn->routes[dest]);
                    newTicket->date = journeyDate;
                    newTicket->pnr = pnr;
                    newTicket->coachCategory = coach->category;
                    ans.push_back(make_pair(newTicket,seat));
                }
            }
        }
    }
    //checking for remaining tickes
    int ticketsLeft = totalSeats - seatsBooked;
    for(int i=0;i<ticketsLeft;i++){
        Ticket* newTicket = new Ticket();
        newTicket->date = journeyDate;
        newTicket->src = make_pair(src,trn->routes[src]);
        newTicket->dest = make_pair(dest,trn->routes[dest]); 
        newTicket->status = "Waiting";
        newTicket->coachCategory = mp[coachCategory];
        newTicket->pnr = pnr;
        string waitingNumberString = "WL-" + to_string(trn->waitingCount);
        newTicket->seatNumber = waitingNumberString;
        //adding the current ticket in waiting list of train
        trn->waitingList[waitingNumberString] = newTicket;
        Seat* temp = new Seat();
        ans.push_back(make_pair(newTicket,temp));
        trn->waitingCount++;
    }
    return ans;
}
void getDetailsByPNR(int pnr,unordered_map<int, UserTicket*>& tickets){

    if(tickets.find(pnr) == tickets.end()){
        cout<<"Invalid PNR\n";
        return;
    }
    UserTicket* tkt = tickets[pnr];
        cout<<endl;
        cout<<"1.Train Number: " << tkt->trainNumber <<endl;
        cout<<"2.From: "<< tkt->src.first<<endl;
        cout<<"3.To: "<< tkt->dest.first<<endl;
        cout<<"4.Date: " << tkt->date<<endl;
        cout<<"5.Total Fare: " << tkt->totalFare<<endl;
        cout << "Seat Details(Coach Number-Seat Number): ";
        for(auto mp:tkt->ticketSeatMap) cout<<mp.first->seatNumber<<" ";
        cout<<endl<<endl;
    return;
}
void generateReport(unordered_map<int, UserTicket*>& tickets){
    priority_queue<int,vector<int>, greater<int>> pq;
    for(auto i:tickets) pq.push(i.first);
    cout<<endl;
    cout << "******************************************** Ticket Booking Report ********************************************** "<<endl;
    while(!pq.empty()){                                                         //1. Train Number: 17726
                                                                                //2. From: Ahmedabad
                                                                                //3. To: Surat
                                                                                //4. Date: 2023-03-15
                                                                                //5. Total Fare: 900
                                                                                //6. Seat Details (Coach Number-Seat Number): S1-1 S1-2 S1-3

        UserTicket* tkt = tickets[pq.top()];
        pq.pop();
        cout<<endl;
        cout<<"1.Train Number: " << tkt->trainNumber <<endl;
        cout<<"2.From: "<< tkt->src.first<<endl;
        cout<<"3.To: "<< tkt->dest.first<<endl;
        cout<<"4.Date: " << tkt->date<<endl;
        cout<<"5.Total Fare: " << tkt->totalFare<<endl;
        cout << "Seat Details(Coach Number-Seat Number): ";
        for(auto mp:tkt->ticketSeatMap) cout<<mp.first->seatNumber<<" ";
        cout<<endl<<endl;
    }
    return;
}
void cancelBooking(int pnr,int quant,Train* train,UserTicket* userTicket){
    //start traversing the ticket seat mapping vector of UserTicket class
    int len = userTicket->ticketSeatMap.size();
    for(int i= len - 1;i>= len - quant;i--){
        Ticket* currTicket = userTicket->ticketSeatMap[i].first;
        Seat* currSeat = userTicket->ticketSeatMap[i].second;
        //if the current booking seat is confirmed
        if(currTicket->status == "Confirmed"){
            //remove ticket from current seat
            currSeat->tickets.erase(pnr);//removing the pnr ticket mapping
            userTicket->totalSeatsBooked--;
            userTicket->ticketSeatMap.pop_back();
            train->cancelledSeats.insert(currSeat);
        }else{
            userTicket->ticketSeatMap.pop_back();
            userTicket->totalSeatsBooked--;
            train->waitingList.erase(currTicket->seatNumber);
        }
    }
    return;
}
void bookFromWaitingList(Train* trn,unordered_map<int,UserTicket*> allTicketsBooked){
    for(auto i:trn->waitingList){
        if(trn->waitingList.size() == 0 || trn->cancelledSeats.size() == 0) break;
        string waitingNumber = i.first;
        Ticket* currTicket = i.second;
        //checking the vacany from the cancelled seats only
        for(auto seat:trn->cancelledSeats){
            if(seat->coachCategory == currTicket->coachCategory){
                if(isSeatNotReserved(trn,seat,currTicket->date,currTicket->src.first,currTicket->dest.first)){
                    currTicket->status = "Confirmed";
                    currTicket->seatNumber = seat->coachCategory + to_string(seat->coachNum) + "-" + to_string(seat->number);
                    int pnr = currTicket->pnr;
                    seat->tickets[pnr] = currTicket;
                    UserTicket* currUser = allTicketsBooked[pnr];
                    for(int i=0;i<currUser->ticketSeatMap.size();i++){
                        if(currUser->ticketSeatMap[i].first == currTicket){
                            currUser->ticketSeatMap[i].second = seat;
                        }
                    }
                    trn->cancelledSeats.erase(seat); 
                    trn->waitingList.erase(waitingNumber);
                    break;                 
                }
            }
        }
        if(trn->waitingList.size() == 0) break;
    }
    return;    
}
void resetWaitingList(Train* t){
    map<string,Ticket*> ans;
    int counter = 1;
    for(auto i:t->waitingList){
        string currNumber = "WL-" + to_string(counter);
        //current waiting number is not same as previous waiting number
        if(currNumber != i.first){
            i.second->seatNumber = currNumber;
            ans[currNumber] = i.second;
        }
        counter++;
    }
    t->waitingCount = counter;
    t->waitingList = ans;
}
int main(){
    // map of train : train number -> train details
    unordered_map<string,Train*> trains;

    //maping of pnr number with user ticket
    unordered_map<int,UserTicket*> allTicketsBooked;
   cout <<"Enter no. of trains : ";
    int tt;
    cin >> tt;
    cin.ignore();
     for(int i=1;i<=tt;i++){
        
        cout << "Enter Train Details of Train " <<  (i + 1)  << " in Format of" << endl;
        cout << "TrainNumber StartingFrom-Distance EndinAt-Distance for Example 17726 Rajkot-0 Mumbai-750" << endl;
        string trainDetailStr;
        getline(cin, trainDetailStr);

        //storing all train details
        vector<string> trainDetails = split(trainDetailStr);
        Train* t = new Train();
        t->number = trainDetails[0];
        for(int i=1;i<trainDetails.size();i++){
            pair<string,int> p = addRoute(trainDetails[i]);
            t->routes[p.first] = p.second;
        }

        // for(auto i:t->routes){
        //     cout<<i->station.first<<"--"<<i->station.second<<endl;
        // }
cout << "Enter Coach Details of Train "<< (i + 1) << endl;
        //coach detail input
        string coachDetailStr;
        getline(cin, coachDetailStr);
        vector<string> coachDetails = split(coachDetailStr);
        
        //adding coaches in the train
        for (int i = 1; i < coachDetails.size(); i++){
            t->coaches.push_back(addCoach(coachDetails[i]));
        }
        trains[t->number] = t; 
    }
    //user request start
    int PNR = 100000001;
    int waitingNumber = 1;
    while (true){

        cout << endl;
            cout << "1. Book Ticket"<<endl;
            cout<<"2. Retrive Ticket Details using PNR"<<endl;
            cout<<"3. Generate Report"<<endl;
            cout<<"4. Cancel Ticket"<<endl;
            cout << "5. leave console "<<endl<<endl;
            int s;
            cin>>s;
            cin.ignore();
        //ticket booking request
        if(s==1){
                 //sample input : Ahmedabad Surat 2023-03-15 1A 2
                cout << "Enter Booking Request in Format of : [ src des date coachType noOfSeats]" << endl;
        string requestStr;
        getline(cin, requestStr);
        vector<string> userRequest = split(requestStr);
            set<string> trainsAvailables = getAvailableTrains(userRequest,trains);
            if(trainsAvailables.size() != 0){
                cout << "Available Trains Are : ";
                for(auto i:trainsAvailables){
                    cout<<i<<"  ";
                }cout<<endl;
                cout << "Enter Train Number You Want To Book Tickets In : ";
                string trainNumber;
                getline(cin,trainNumber);
cout<<endl;
                //checking if the inputed train number is valid or not
                if(trainsAvailables.find(trainNumber) == trainsAvailables.end()){
                    cout<<"Invalid train number\n";
                    continue;
                }
                Train* userDemandTrain = trains[trainNumber];
                //book ticket
                UserTicket* currTicket = new UserTicket();
                currTicket->pnrNumber = PNR;
                currTicket->trainNumber = trainNumber;
                currTicket->src = make_pair(userRequest[0],getStationDistance(userDemandTrain,userRequest[0]));
                currTicket->dest = make_pair(userRequest[1],getStationDistance(userDemandTrain,userRequest[1]));
                currTicket->date = userRequest[2];
                currTicket->totalSeatsBooked = stoi(userRequest[4]);
                currTicket->coachClass = userRequest[3];
                currTicket->ticketSeatMap = bookTicket(currTicket,userDemandTrain,userRequest,PNR);
                currTicket->totalFare = findFare(currTicket);
                cout<<"Your PNR Number Is: "<<currTicket->pnrNumber<<endl;
                cout<<"Your Ticket Fare Amount Is: "<<currTicket->totalFare<<endl;

                //adding ticket to pnr->ticket map
                allTicketsBooked[PNR] = currTicket;
                
                PNR++;
            }else cout<<"No Trains Available\n";
        }
        //pnr input request
        else if(s==2){
           // cout<<endl;
            cout << "Enter Your PNR Number: ";
            string requestStr;
            getline(cin, requestStr);
            vector<string> userRequest = split(requestStr);
            int inputPNR = stoi(userRequest[0]);
            getDetailsByPNR(inputPNR,allTicketsBooked);
            
        }
        //report generation request
        else if(s==3){
            // string requestStr;
            // getline(cin, requestStr);
            // vector<string> userRequest = split(requestStr);
            generateReport(allTicketsBooked);
        }
        //cancellation request
        else if(s==4){
            //check if pnr number is valid or not
            //userRequest.size() == 3 && userRequest[0] == "CANCEL"
            cout << "Enter PNR numner followed by number of seats you booked : [PNR Number_Of_Seats]"<<endl;
            string requestStr;
            getline(cin, requestStr);
            vector<string> userRequest = split(requestStr);
            if(allTicketsBooked.find(stoi(userRequest[0])) != allTicketsBooked.end()){
                //checking if seats to cancel is <= booked seats
                UserTicket* currTicket = allTicketsBooked[stoi(userRequest[0])];
                Train* currTrain = trains[currTicket->trainNumber];
                if(currTicket->totalSeatsBooked >= stoi(userRequest[1])){
                    int pnrCancel = stoi(userRequest[0]);
                    cancelBooking(pnrCancel,stoi(userRequest[1]),currTrain,allTicketsBooked[stoi(userRequest[0])]);
                    //finding the fare again
                    currTicket->totalFare = findFare(currTicket);
                    cout<<"Your PNR Number Is: "<<currTicket->pnrNumber<<endl;
                    cout<<"Your New Ticket Fare Amount Is: "<<currTicket->totalFare<<endl;
                    bookFromWaitingList(trains[allTicketsBooked[pnrCancel]->trainNumber],allTicketsBooked);
                    resetWaitingList(currTrain);
                }else{
                    cout<<"Cancellation request is more than booked seats....Try Again!!\n";
                }
            }else{
                cout<<"Invalid PNR\n";
            }
        }
        else{
           break;
        }
    }
    return 0;
}