## Flight Route Planner
A C++ project that uses Dijkstra's algorithm to find the shortest route from a given airport to another given airport within Europe, given a maximum range. Route can be displayed via a Qt Quick map.

## Features
- Reads airport information from a CSV file
- Calculates optimal route with Dijkstra's algorithm
- Accepts user defined airports via ICAO code as well a maximum range per hop
- Displays route on a map

## Requirements
- C++ 20 or later
- Qt 6.12.0 configured for MSVC
    - Core, Gui, Widgets, Qml, Quick, Location, Positioning modules installed
- Internet access

## How to Run
Open the solution file ```FlightRoutePlanner.slnx ``` in Visual Studio.

Select the Qt configuration for the project.

Select Build > Build Solution to run the project.