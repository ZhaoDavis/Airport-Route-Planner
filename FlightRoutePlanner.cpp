#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <format>
#include <cmath>
#include <numbers>
#include <optional>
#include <queue>
#include <algorithm>
#include <cctype>
#include <direct.h>
#include "marker.h"

//supress Qt errors
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QAbstractListModel>
#include <QQmlContext>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

using namespace std;

struct airport {
    string name;
    string city;
    string country;
    string icao;
    double latitude;
    double longitude;

    airport(string name, string city, string country, string icao, double latitude, double longitude) 
    : name(name), city(city), country(country), icao(icao), latitude(latitude), longitude(longitude) {}
};

struct route {
    double cost;
    vector<int> path;  //stores indices of objects in airports vector

    route(double cost, vector<int> path) : cost(cost), path(path) {}
};

double toRadians(double degrees) {
    return degrees * numbers::pi/180.0;
}

double haversine(airport& x, airport& y) {
    double lat1 = toRadians(x.latitude);
    double lat2 = toRadians(y.latitude);
    double lon1 = toRadians(x.longitude);
    double lon2 = toRadians(y.longitude);

    double a = pow(sin((lat1 - lat2) / 2), 2) + cos(lat1) * cos(lat2) * pow(sin((lon1 - lon2) / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double dist = 6371.0088 * c; //estimate earth radius to be 6371.0088km
    return dist;
}

//Find index of an airport in the airports vector, given the icao code
int findInd(vector<airport>& airports, string& toFind) {
    for (int i = 0; i < airports.size(); i++) {
        if (airports[i].icao == toFind) {
            return i; 
        }
    }
    return -1;
}

airport getAirport(vector<airport> airports) {
    string input;
    cin >> input;
    for (char& c : input) {
        c = toupper(c);
    }
    
    int airportInd = findInd(airports, input);

    while (airportInd < 0) {
        cout << "Could not find airport with code: " << input << "\nPlease enter a valid ICAO airport code" << endl;
        cin >> input;
        for (char& c : input) {
            c = toupper(c);
        }

        airportInd = findInd(airports, input);
    }

    return airports[airportInd];
}

optional<route> dijkstra(vector<airport> airports, string start, string end, int maxRange = 750, double layoverPenalty = 100) {
    int startInd = findInd(airports, start);
    int endInd = findInd(airports, end);
    if (startInd < 0 || endInd < 0) {
        cout << "Could not find start/end indices" << endl;
    }

    int n = airports.size();
    int INF = pow(2, 31) - 1;
    vector<double> dist(n, INF);
    vector<int> prev(n, -1);

    priority_queue < pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    dist[startInd] = 0;
    pq.push({0, startInd});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;
        if (u == endInd) break;

        for (int v = 0; v < n; v++) {
            if (v == u) continue;
            double km = haversine(airports[u], airports[v]);
            if (km > maxRange) continue;

            double nd = d + km + layoverPenalty;
            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
                pq.push({nd, v});
            }
        }
    }

    if (dist[endInd] == INF) return nullopt;

    vector<int> path;
    for (int at = endInd; at != -1; at = prev[at]) path.push_back(at);
    reverse(path.begin(), path.end());

    return route{dist[endInd], path };
}

int main(int argc, char** argv)
{
    string filepath = "data/EU_airports.csv";
    ifstream file(filepath);

    if (!file) {
        cerr << format("Could not find file at {}", filepath);
    }

    string line;
    //skip row with column names
    getline(file, line);

    vector<airport> airports;
    while (getline(file, line)) {
        stringstream ss(line);
        string parsed;
        vector<string> fields;

        //extract fields from line with comma as delimiter
        while (getline(ss, parsed, ',')) {
            fields.push_back(parsed);
        }

        airport temp = airport(fields[0], fields[1], fields[2], fields[3], stod(fields[4]), stod(fields[5]));
        airports.push_back(temp);
    }

    cout << "Enter ICAO code for starting airport:";
    airport start = getAirport(airports);

    cout << "Enter ICAO code for destination airport:";
    airport end = getAirport(airports);

    cout << "Enter a maximum radius for each hop (kilometers):";
    int range;
    cin >> range;

    auto result = dijkstra(airports, start.icao, end.icao, range);

    MarkerModel markers;

    if (!result) {
        cout << "No route found" << endl;
        return -1;
    }
    else {
        cout << "\nFlight path for " << start.city << " (" << start.icao << ")" 
            << " to " << end.city << " (" << end.icao <<  ")" << ":\n" << endl;
        int step = 1;
        for (int ind : result->path) {
            airport airport = airports[ind];
            cout << step << " | " << airport.icao << " | " << airport.city << endl;
            markers.addMarker(QString::fromStdString(airport.icao), step, airport.latitude, airport.longitude);
            step++;
        }
        cout << endl;
    }

    //Initialize Qt app
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("markerModel", &markers);
    engine.load(QUrl(QStringLiteral("main.qml")));

    return app.exec();
}