import QtQuick
import QtQuick.Window
import QtLocation
import QtPositioning

Window {
    width: 1000
    height: 1000
    visible: true
    title: "FlightPlanner"

    Plugin {
        id: osm
        name: "osm"
    }
    Row {
        anchors.fill: parent
        Map {
            id: map
            plugin: osm
            width: parent.width
            height: parent.height

            Component.onCompleted: {
                console.log("Available map types")
                for (let i = 0; i < supportedMapTypes.length; ++i) {
                    console.log("Map type:", supportedMapTypes[i].name)
                    activeMapType = supportedMapTypes[i]
                    break
                }
                console.log("Active map type:", activeMapType.name)
                console.log("Map size", width, height)
            }

            center: QtPositioning.coordinate(52, 10)   
            zoomLevel: 4
            MapItemView {
                model: markerModel 
                delegate: MapQuickItem {
                    coordinate: QtPositioning.coordinate(model.lat, model.lon)
                    anchorPoint.x: 6
                    anchorPoint.y: 6
                    sourceItem: Item {    
                        Rectangle {
                            width: 18; 
                            height: 18; 
                            radius: 9
                            color: "red"; 
                            border.color: "black"
                            Text {
                                anchors.centerIn: parent 
                                text: model.step
                                color: "white"
                                font.bold: true
                                font.pixelSize: 16
                            }
                        }
                        Text {
                            x: -5
                            y: -18
                            text: model.name
                            color: "black"
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }
    }
}

