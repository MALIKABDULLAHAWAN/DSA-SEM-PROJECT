#include <iostream>
#include <vector>
#include <cstring>
#include <stack>
#include <climits>
#include <queue>
#include <algorithm>

using namespace std;

// Constants
const int MAXCITY = 25;
const int INF = 10000;
#define ROUNDTRIP 0
#define ONEWAY 1
struct CityType;

struct FlightType {
    int FlightNo;
    int CityIndex;
    int timeCost;
    char* startCity;
    int timeDepart;
    char* endCity;
    int timeArrival;
    int noOfPassengers;
    int date;  // Add a date field for departure
    FlightType* nextDeparture;
    FlightType* nextArrival;
};

// Definition of CityType
struct CityType {
    char* cityName;
    int timeCost;
    vector<FlightType*> nextDeparture;
    vector<FlightType*> nextArrival;

    // Function to check string equality
      bool check_string_equality(const char ch[], const char city[]) const {
        return (strcmp(ch, city) == 0);
    }

    int return_array_index(const vector<CityType>& arry, const char city[]) const {
        for (int i = 0; i < arry.size(); i++) {
            if (arry[i].cityName != nullptr) {
                if (check_string_equality(arry[i].cityName, city)) {
                    return i;
                }
            }
        }
        return -1;
    }
};
struct RouteType {
    int Day; /* day of travel: mmdd */
    int nHops; /* Number of hops (1 or 2) */
    int FlightNo1; /* Flight number of first hop */
    int FlightNo2; /* Flight number of second hop (if needed) */
};
struct ReservationType {
    char *firstName; /* first name of passenger */
    char *lastName; /* last name of passenger */
    int tripType; /* ROUNDTRIP or ONEWAY */
    RouteType route1; /* first route */
    RouteType route2; /* second route (only if ROUNDTRIP) */
    ReservationType *nextReserve; /* next reservation in linked list */
};

ReservationType MakeReserveNode(const char *firstName, const char *lastName, int tripType, RouteType route1, RouteType route2) {
    ReservationType newReservation;
    newReservation.firstName = new char[strlen(firstName) + 1];
    strcpy(newReservation.firstName, firstName);
    newReservation.lastName = new char[strlen(lastName) + 1];
    strcpy(newReservation.lastName, lastName);
    newReservation.tripType = tripType;
    newReservation.route1 = route1;
    newReservation.route2 = route2;
    newReservation.nextReserve = nullptr;
    return newReservation;
}

vector<CityType> cityList(MAXCITY);
vector<FlightType*> flightList(MAXCITY);
ReservationType* reserveHead = nullptr;
ReservationType* reserveTail = nullptr;

// Function to make a flight node
FlightType* MakeFlightNode(const int flightNo, const char* startCity, const int timeDepart,
                           const char* endCity, const int timeArrival, const int noOfPassengers, const int date) {
    FlightType* newFlight = new FlightType;
    newFlight->FlightNo = flightNo;

    newFlight->startCity = new char[strlen(startCity) + 1];
    strcpy(newFlight->startCity, startCity);
    newFlight->timeDepart = timeDepart;
    newFlight->endCity = new char[strlen(endCity) + 1];
    strcpy(newFlight->endCity, endCity);
    newFlight->timeArrival = timeArrival;
    newFlight->noOfPassengers = noOfPassengers;
    newFlight->date = date;  // Set the date
    newFlight->nextDeparture = nullptr;
    newFlight->nextArrival = nullptr;
    return newFlight;
}

// Function to add a city to the graph
void AddCityToGraph(const char* cityName) {
    int index = cityList[0].return_array_index(cityList, cityName);
    if (index == -1) {
        CityType newCity;
        newCity.cityName = new char[strlen(cityName) + 1];
        strcpy(newCity.cityName, cityName);
        cityList.push_back(newCity);
    }
}

// Function to add a flight to the graph
void AddFlightToGraph(FlightType* newFlight) {
    int startCityIndex = cityList[0].return_array_index(cityList, newFlight->startCity);
    int endCityIndex = cityList[0].return_array_index(cityList, newFlight->endCity);

    // Update the time cost information
    newFlight->timeCost = newFlight->timeArrival - newFlight->timeDepart;

    // Update the time cost in the CityNode
    cityList[startCityIndex].timeCost = 0; // Time cost from source city is 0
    cityList[endCityIndex].timeCost = INF; // Initialize time cost to infinity

    // Add to the departure and arrival vectors
    cityList[startCityIndex].nextDeparture.push_back(newFlight);
    cityList[endCityIndex].nextArrival.push_back(newFlight);
}

// Function to display all cities
void DisplayAllCities() {
    for (const auto& city : cityList) {
        if (city.cityName != nullptr) {
            cout << city.cityName << endl;
        }
    }
}
void ShowFlightDetails(const vector<FlightType*>& flightList, int numFlights) {
    // Function body remains unchanged
    printf("Flight Details:\n");
    for (int i = 0; i < numFlights; ++i) {
        printf("Flight Number: %d\n", flightList[i]->FlightNo);
        printf("Departure City: %s\n", flightList[i]->startCity);
        printf("Departure Time: %d\n", flightList[i]->timeDepart);
        printf("Arrival City: %s\n", flightList[i]->endCity);
        printf("Arrival Time: %d\n", flightList[i]->timeArrival);
        printf("Capacity: %d\n", flightList[i]->noOfPassengers);
        printf("---------------------------\n");
    }
}




// Function to display flight departures for a city
bool CompareFlightsByTime(const FlightType* flight1, const FlightType* flight2) {
    return flight1->timeArrival < flight2->timeArrival;
}

// Function to display flight arrivals for a city, sorted by time
void DisplayArrivalListSorted(const char* cityName) {
    int numFlights = cityList.size();

    cout << "Arrivals to " << cityName << " sorted by time:" << endl;

    for (int i = 0; i < numFlights; ++i) {
        vector<FlightType*> sortedFlights = cityList[i].nextArrival;
        sort(sortedFlights.begin(), sortedFlights.end(), CompareFlightsByTime);

        for (const auto& flight : sortedFlights) {
            if (strcmp(cityName, flight->endCity) == 0) {
                cout << flight->FlightNo << " arriving at " << cityName << " at " << flight->timeArrival << endl;
            }
        }
    }
}

// Function to display flight departures for a city, sorted by time
void DisplayDepartureListSorted(const CityType& city) {
    vector<FlightType*> sortedFlights = city.nextDeparture;
    sort(sortedFlights.begin(), sortedFlights.end(), CompareFlightsByTime);

    for (const auto& flight : sortedFlights) {
        cout << flight->FlightNo << " departing from " << city.cityName << " at " << flight->timeDepart << endl;
    }
}

// Function to display cities reachable from a particular city
// Function to display cities reachable from a particular city
void DisplayCitiesReachableFrom(const char* startCity) {
    int startCityIndex = cityList[0].return_array_index(cityList, startCity);

    if (startCityIndex == -1) {
        cout << "Start city not found in the list." << endl;
        return;
    }

    cout << "Cities reachable from " << startCity << ":" << endl;

    for (const auto& flight : cityList[startCityIndex].nextDeparture) {
        cout << flight->endCity << endl;
    }
}

void DisplayShortestPath(const char* startCity, const char* endCity, const vector<CityType>& cityList)
 {
    int startCityIndex = cityList[0].return_array_index(cityList, startCity);
    int endCityIndex = cityList[0].return_array_index(cityList, endCity);

    if (startCityIndex == -1 || endCityIndex == -1) {
        cout << "Invalid city names." << endl;
        return;
    }

    vector<int> distance(cityList.size(), INT_MAX);
    vector<int> parent(cityList.size(), -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    distance[startCityIndex] = 0;
    pq.push({0, startCityIndex});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        for (const auto& flight : cityList[u].nextDeparture) {
            int v = cityList[0].return_array_index(cityList, flight->endCity);
            int weight = flight->timeCost;

            if (distance[v] > distance[u] + weight) {
                distance[v] = distance[u] + weight;
                parent[v] = u;
                pq.push({distance[v], v});
            }
        }
    }

    // Reconstruct the path
    vector<int> path;
    for (int v = endCityIndex; v != -1; v = parent[v]) {
        path.push_back(v);
    }

    // Display the shortest path
    cout << "Shortest path from " << startCity << " to " << endCity << ": ";
    for (int i = path.size() - 1; i >= 0; --i) {
        cout << cityList[path[i]].cityName;
        if (i > 0) cout << " -> ";
    }
    cout << endl;

    cout << "Total time cost: " << distance[endCityIndex] << endl;
}

// Function to find a route between two cities using DFS
bool DFSFindRoute(const char* startCity, const char* endCity, vector<bool>& visited) {
    int startCityIndex = -1;
    int endCityIndex = -1;

    for (int i = 0; i < cityList.size(); ++i) {
        if (cityList[i].cityName != nullptr) {
            if (cityList[i].check_string_equality(cityList[i].cityName, startCity)) {
                startCityIndex = i;
            }
            if (cityList[i].check_string_equality(cityList[i].cityName, endCity)) {
                endCityIndex = i;
            }
        }
    }

    if (startCityIndex == -1 || endCityIndex == -1) {
        cout << "Invalid city names." << endl;
        return false;
    }

    fill(visited.begin(), visited.end(), false);

    stack<int> cityStack;
    cityStack.push(startCityIndex);
    visited[startCityIndex] = true;

    cout << "Route from " << startCity << " to " << endCity << ": ";

    while (!cityStack.empty()) {
        int currentCityIndex = cityStack.top();
        cityStack.pop();

        cout << cityList[currentCityIndex].cityName << " ";

        if (currentCityIndex == endCityIndex) {
            cout << endl;
            return true;
        }

        for (const auto& flight : cityList[currentCityIndex].nextDeparture) {
            int neighborIndex = cityList[0].return_array_index(cityList, flight->endCity);
            if (!visited[neighborIndex]) {
                cityStack.push(neighborIndex);
                visited[neighborIndex] = true;
            }
        }
    }

    cout << endl;
    return false;
}

void MakeReservation() {
    // Gather passenger information
    char firstName[100], lastName[100];
    int numFlights = MAXCITY;
    int tripType;
    RouteType route1, route2;

    cout << "Enter passenger's first name: ";
    cin >> firstName;
    cout << "Enter passenger's last name: ";
    cin >> lastName;

    cout << "Enter trip type (0 for ROUNDTRIP, 1 for ONEWAY): ";
    cin >> tripType;
    ShowFlightDetails(flightList, numFlights);

    cout << "Enter details for the first route:" << endl;
    cout << "Enter day (mmdd): ";
    cin >> route1.Day;
    cout << "Enter number of hops (1 or 2): ";
    cin >> route1.nHops;
    cout << "Enter flight number for hop 1: ";
    cin >> route1.FlightNo1;

    // Check the availability of the first flight
    if (flightList[route1.FlightNo1 - 1] == nullptr) {
        cout << "Invalid flight number. Reservation not made." << endl;
        return;
    }

    // Update flight information (e.g., decrease available seats)
    flightList[route1.FlightNo1 - 1]->noOfPassengers++;

    if (route1.nHops == 2) {
        cout << "Enter flight number for hop 2: ";
        cin >> route1.FlightNo2;

        // Check the availability of the second flight
        if (flightList[route1.FlightNo2 - 1] == nullptr) {
            cout << "Invalid flight number. Reservation not made." << endl;
            // Revert the changes made to the first flight
            flightList[route1.FlightNo1 - 1]->noOfPassengers--;
            return;
        }

        // Update flight information (e.g., decrease available seats)
        flightList[route1.FlightNo2 - 1]->noOfPassengers++;
    }

    // Initialize route2 for ROUNDTRIP
    if (tripType == ROUNDTRIP) {
        cout << "Enter details for the return route:" << endl;
        cout << "Enter day (mmdd): ";
        cin >> route2.Day;
        cout << "Enter number of hops (1 or 2): ";
        cin >> route2.nHops;
        cout << "Enter flight number for hop 1: ";
        cin >> route2.FlightNo1;

        // Check the availability of the first return flight
        if (flightList[route2.FlightNo1 - 1] == nullptr) {
            cout << "Invalid flight number. Reservation not made." << endl;
            // Revert the changes made to the first flight
            flightList[route1.FlightNo1 - 1]->noOfPassengers--;
            // Revert the changes made to the second flight (if applicable)
            if (route1.nHops == 2) {
                flightList[route1.FlightNo2 - 1]->noOfPassengers--;
            }
            return;
        }

        // Update flight information (e.g., decrease available seats)
        flightList[route2.FlightNo1 - 1]->noOfPassengers++;

        if (route2.nHops == 2) {
            cout << "Enter flight number for hop 2: ";
            cin >> route2.FlightNo2;

            // Check the availability of the second return flight
            if (flightList[route2.FlightNo2 - 1] == nullptr) {
                cout << "Invalid flight number. Reservation not made." << endl;
                // Revert the changes made to the first flight
                flightList[route1.FlightNo1 - 1]->noOfPassengers--;
                // Revert the changes made to the second flight
                flightList[route2.FlightNo1 - 1]->noOfPassengers--;
                return;
            }

            // Update flight information (e.g., decrease available seats)
            flightList[route2.FlightNo2 - 1]->noOfPassengers++;
        }
    }

    // Create a new reservation
    ReservationType newReservation = MakeReserveNode(firstName, lastName, tripType, route1, route2);

    // Add the new reservation to the linked list
    if (reserveHead == nullptr) {
        reserveHead = reserveTail = new ReservationType(newReservation);
    } else {
        reserveTail->nextReserve = new ReservationType(newReservation);
        reserveTail = reserveTail->nextReserve;
    }

    cout << "Reservation made successfully!" << endl;
}




void DeleteReserve() {
    // Gather passenger information
    char firstName[100], lastName[100];

    cout << "Enter passenger's first name: ";
    cin >> firstName;
    cout << "Enter passenger's last name: ";
    cin >> lastName;

    // Search for the reservation to delete
    ReservationType *currentReservation = reserveHead;
    ReservationType *prevReservation = nullptr;

    while (currentReservation != nullptr) {
        if (strcmp(currentReservation->firstName, firstName) == 0 &&
            strcmp(currentReservation->lastName, lastName) == 0) {
            // Found the reservation to delete
            if (prevReservation != nullptr) {
                // If it's not the first node in the list
                prevReservation->nextReserve = currentReservation->nextReserve;
                if (currentReservation == reserveTail) {
                    reserveTail = prevReservation;
                }
            } else {
                // If it's the first node in the list
                reserveHead = currentReservation->nextReserve;
                if (currentReservation == reserveTail) {
                    reserveTail = nullptr;
                }
            }

            // Free memory for firstName and lastName
            delete[] currentReservation->firstName;
            delete[] currentReservation->lastName;

            // Free memory for the reservation node
            delete currentReservation;

            cout << "Reservation deleted successfully!" << endl;
            return;
        }

        prevReservation = currentReservation;
        currentReservation = currentReservation->nextReserve;
    }

    cout << "Reservation not found for the given passenger." << endl;
}

void PrintReservation(ReservationType *pReserve) {
   cout << "Passenger: " << pReserve->firstName << " " << pReserve->lastName <<endl;
   cout << "Trip Type: " << (pReserve->tripType == ROUNDTRIP ? "ROUNDTRIP" : "ONEWAY") <<endl;
   cout << "Route Information:" <<endl;
    
    // Display the first route
   cout << "  Day: " << pReserve->route1.Day <<endl;
   cout << "  Number of Hops: " << pReserve->route1.nHops <<endl;
   cout << "  Flight No 1: " << pReserve->route1.FlightNo1 <<endl;
    if (pReserve->route1.nHops == 2) {
       cout << "  Flight No 2: " << pReserve->route1.FlightNo2 <<endl;
    }

    // Display the second route for ROUNDTRIP
    if (pReserve->tripType == ROUNDTRIP) {
       cout << "Return Route Information:" <<endl;
       cout << "  Day: " << pReserve->route2.Day <<endl;
       cout << "  Number of Hops: " << pReserve->route2.nHops <<endl;
       cout << "  Flight No 1: " << pReserve->route2.FlightNo1 <<endl;
        if (pReserve->route2.nHops == 2) {
           cout << "  Flight No 2: " << pReserve->route2.FlightNo2 <<endl;
        }
    }

   cout << "-----------------------------" <<endl;
}

void PrintSchedule() {
    // Gather passenger information
    char firstName[100], lastName[100];

   cout << "Enter passenger's first name: ";
   cin >> firstName;
   cout << "Enter passenger's last name: ";
   cin >> lastName;

    // Search for the passenger's reservations
    ReservationType *currentReservation = reserveHead;

    while (currentReservation != nullptr) {
        if (strcmp(currentReservation->firstName, firstName) == 0 &&
            strcmp(currentReservation->lastName, lastName) == 0) {
            // Found the passenger's reservation
           cout << "Schedule for " << firstName << " " << lastName << ":" <<endl;
            PrintReservation(currentReservation);
            return;
        }

        currentReservation = currentReservation->nextReserve;
    }

   cout << "No reservations found for the given passenger." <<endl;
}

// Function to print a list of passengers of a particular flight
// Function to print a list of passengers of a particular flight
void PrintPassengersOfFlightSorted(int flightNumber) {
    vector<pair<string, string>> passengers;  // Pair of first name and last name

    for (ReservationType* currentReservation = reserveHead; currentReservation != nullptr; currentReservation = currentReservation->nextReserve) {
        if (currentReservation->route1.FlightNo1 == flightNumber || currentReservation->route1.FlightNo2 == flightNumber ||
            (currentReservation->tripType == ROUNDTRIP && (currentReservation->route2.FlightNo1 == flightNumber || currentReservation->route2.FlightNo2 == flightNumber))) {
            passengers.push_back({currentReservation->lastName, currentReservation->firstName});
        }
    }

    // Sort passengers by last name
    sort(passengers.begin(), passengers.end());

    // Print the sorted list of passengers
    cout << "Passengers for Flight " << flightNumber << " in order of last name:" << endl;
    for (const auto& passenger : passengers) {
        cout << passenger.second << " " << passenger.first << endl;
    }
}


// Function to display the menu
void DisplayMenu() {
    cout << "\nAirline Reservation System Menu\n";
    cout << "1. Show a list of all cities serviced by the airline\n";
    cout << "2. Show flight departures for a city\n";
    cout << "3. Show flight arrivals for a city\n";
    cout << "4. Show cities that can be reached from a particular city\n";
    cout << "5. Show the list of cities in the shortest path between two cities\n";
    cout << "6. Find a route between two cities\n";
    cout << "7. Make an airline reservation for a passenger\n";
    cout << "8. Print a passenger's reservation schedule\n";
    cout << "9. Delete a passenger's reservation\n";
    cout << "10. Print a list of passengers of a particular flight\n";
    cout << "0. Exit\n";
}

int main() {
    int choice;
    vector<bool> visited(MAXCITY);
    cityList[0].cityName = new char[strlen("Lahore")+1];
    strcpy(cityList[0].cityName, "Lahore\0");
    cityList[1].cityName = new char[strlen("Karachi")+1];
    strcpy(cityList[1].cityName, "Karachi\0");
    cityList[2].cityName = new char[strlen("Nowshera")+1];
    strcpy(cityList[2].cityName, "Nowshera\0");
    cityList[3].cityName = new char[strlen("Islamabad")+1];
    strcpy(cityList[3].cityName, "Islamabad\0");
    cityList[4].cityName = new char[strlen("Larkana")+1];
    strcpy(cityList[4].cityName, "Larkana\0");
    cityList[5].cityName = new char[strlen("Pindi")+1];
    strcpy(cityList[5].cityName, "Pindi\0");
    cityList[6].cityName = new char[strlen("Sargodha")+1];
    strcpy(cityList[6].cityName, "Sargodha\0");
    cityList[7].cityName = new char[strlen("Khushab")+1];
    strcpy(cityList[7].cityName, "Khushab\0");
    cityList[8].cityName = new char[strlen("Faisalabad")+1];
    strcpy(cityList[8].cityName, "Faisalabad\0");
    cityList[9].cityName = new char[strlen("Rawalpindi")+1];
    strcpy(cityList[9].cityName, "Rawalpindi\0");
    cityList[10].cityName = new char[strlen("Peshawar")+1];
    strcpy(cityList[10].cityName, "Peshawar\0");
    cityList[11].cityName = new char[strlen("Multan")+1];
    strcpy(cityList[11].cityName, "Multan\0");
    cityList[12].cityName = new char[strlen("Hyderabad")+1];
    strcpy(cityList[12].cityName, "Hyderabad\0");
    cityList[13].cityName = new char[strlen("Quetta")+1];
    strcpy(cityList[13].cityName, "Quetta\0");
    cityList[14].cityName = new char[strlen("Murree")+1];
    strcpy(cityList[14].cityName, "Murree\0");
    cityList[15].cityName = new char[strlen("Gwadar")+1];
    strcpy(cityList[15].cityName, "Gwadar\0");
    cityList[16].cityName = new char[strlen("Swat")+1];
    strcpy(cityList[16].cityName, "Swat\0");
    cityList[17].cityName = new char[strlen("Sialkot")+1];
    strcpy(cityList[17].cityName, "Sialkot\0");
    cityList[18].cityName = new char[strlen("Chitral")+1];
    strcpy(cityList[18].cityName, "Chitral\0");
    cityList[19].cityName = new char[strlen("Gujranwala")+1];
    strcpy(cityList[19].cityName, "Gujranwala\0");
    cityList[20].cityName = new char[strlen("WahCantonment")+1];
    strcpy(cityList[20].cityName, "WahCantonment\0");
    cityList[21].cityName = new char[strlen("Attock")+1];
    strcpy(cityList[21].cityName, "Attock\0");
    cityList[22].cityName = new char[strlen("Swabi")+1];
    strcpy(cityList[22].cityName, "Swabi\0");
    cityList[23].cityName = new char[strlen("Mianwali")+1];
    strcpy(cityList[23].cityName, "Mianwali\0");
    cityList[24].cityName = new char[strlen("Bhalwal")+1];
    strcpy(cityList[24].cityName, "Bhalwal\0");
   flightList[0] = MakeFlightNode(1, "Nowshera\0", 1200, "Karachi\0", 1400, 100, 1015);
flightList[1] = MakeFlightNode(2, "Karachi\0", 1500, "Nowshera\0", 1700, 120, 1016);
flightList[2] = MakeFlightNode(3, "Sargodha\0", 1300, "Nowshera\0", 1500, 80, 1017);
flightList[3] = MakeFlightNode(4, "Islamabad\0", 1100, "Sialkot\0", 1330, 90, 1018);
flightList[4] = MakeFlightNode(5, "Sialkot\0", 1400, "Islamabad\0", 1630, 110, 1019);
flightList[5] = MakeFlightNode(6, "Nowshera\0", 1030, "Karachi\0", 1230, 80, 1020);
flightList[6] = MakeFlightNode(7, "Multan\0", 1030, "Hyderabad\0", 1230, 80, 1111);
flightList[7] = MakeFlightNode(8, "Hyderabad\0", 1600, "Quetta\0", 1800, 150, 1112);
flightList[8] = MakeFlightNode(9, "Islamabad\0", 1400, "Sialkot\0", 1630, 100, 1023);
flightList[9] = MakeFlightNode(10, "Sialkot\0", 1100, "Nowshera\0", 1330, 90, 1024);
flightList[10] = MakeFlightNode(11, "Nowshera\0", 1230, "Karachi\0", 1430, 120, 1025);
flightList[11] = MakeFlightNode(12, "Karachi\0", 1700, "Sargodha\0", 1900, 140, 1026);
flightList[12] = MakeFlightNode(13, "Sargodha\0", 1300, "Islamabad\0", 1530, 110, 1027);
flightList[13] = MakeFlightNode(14, "Islamabad\0", 1600, "Sialkot\0", 1830, 90, 1028);
flightList[14] = MakeFlightNode(15, "Sialkot\0", 1200, "Nowshera\0", 1430, 80, 1029);
flightList[15] = MakeFlightNode(16, "Nowshera\0", 1330, "Karachi\0", 1530, 100, 1030);
flightList[16] = MakeFlightNode(17, "Karachi\0", 1800, "Sargodha\0", 2000, 130, 1101);
flightList[17] = MakeFlightNode(18, "Sargodha\0", 1400, "Islamabad\0", 1630, 100, 1102);
flightList[18] = MakeFlightNode(19, "Islamabad\0", 1630, "Sialkot\0", 1900, 110, 1103);
flightList[19] = MakeFlightNode(20, "Sialkot\0", 1130, "Nowshera\0", 1330, 70, 1104);
flightList[20] = MakeFlightNode(21, "Nowshera\0", 1300, "Karachi\0", 1500, 90, 1105);
flightList[21] = MakeFlightNode(22, "Karachi\0", 1830, "Sargodha\0", 2030, 120, 1106);
flightList[22] = MakeFlightNode(23, "Sargodha\0", 1500, "Islamabad\0", 1730, 110, 1107);
flightList[23] = MakeFlightNode(24, "Islamabad\0", 1700, "Sialkot\0", 1930, 100, 1108);
flightList[24] = MakeFlightNode(25, "Sialkot\0", 1230, "Nowshera\0", 1430, 80, 1109);
    // Add cities and flights to the graph
  for (int i = 0; i < MAXCITY; ++i) {
        if (flightList[i] != nullptr) {
            AddFlightToGraph(flightList[i]);
        }
    }
for (int i = 0; i < MAXCITY; ++i) {
        if (flightList[i] != nullptr) {
            AddCityToGraph(cityList[i].cityName);
        }
    }
    do {
        DisplayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                DisplayAllCities();
                cout << endl << "Enter any key to continue" << endl;
                // getch(); // Uncomment if needed
                break;
          case 2: {
                // Option 2: Show flight departures for a city
                char defaultDepartureCity[100];
                char departureCity[100];
                cout << "Enter the departure city (or press Enter to use the default): ";
                cin.ignore();  // Ignore newline character from previous input
                cin.getline(departureCity, sizeof(departureCity));
                if (strlen(departureCity) == 0) {
                    strcpy(departureCity, defaultDepartureCity);
                }
                int x = cityList[0].return_array_index(cityList, departureCity);
                if (x != -1)
                    DisplayDepartureListSorted(cityList[x]);
                else
                    cout << "City not found in the list" << endl;
                break;
            }
            case 3:
                // Option 3: Show flight arrivals for a city
                char arrivalCity[100];
                cout << "Enter the arrival city: ";
                cin >> arrivalCity;
                DisplayArrivalListSorted(arrivalCity);
                break;
            case 4:
                // Option 4: Show cities that can be reached from a particular city
                char reachableCity[100];
                cout << "Enter the city to find reachable cities from: ";
                cin >> reachableCity;
                DisplayCitiesReachableFrom(reachableCity);
                break;
           case 5:
    // Option 5: Show the list of cities in the shortest path between two cities
    char startCitySP[100], endCitySP[100];
    cout << "Enter the start city: ";
    cin >> startCitySP;
    cout << "Enter the end city: ";
    cin >> endCitySP;
    DisplayShortestPath(startCitySP, endCitySP, cityList);
    break;

            case 6:
                // Option 6: Find a route between two cities
                char startCityRoute[100], endCityRoute[100];
                cout << "Enter the start city: ";
                cin >> startCityRoute;
                cout << "Enter the end city: ";
                cin >> endCityRoute;
                DFSFindRoute(startCityRoute, endCityRoute, visited);
                break;
                case 7:
                MakeReservation();
                break;
            case 8:
                PrintSchedule();
                break;
            case 9:
                DeleteReserve();
                break;
            case 10:
                int flightNumber;
                cout << "Enter the flight number: ";
                cin >> flightNumber;
                PrintPassengersOfFlightSorted(flightNumber);
                break;
            case 0:
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    } while (choice != 0);

    return 0;
}
