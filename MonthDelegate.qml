import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle {
    property string header
    property var dataModel

    border.width: 1

    ColumnLayout {
        anchors.fill: parent

        Label {
            text: header
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 18
            color: "#424242"
        }

        TableView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: dataModel

            TableViewColumn {
                title: "Категория"
                role: "categoryName"
                width: 100
            }
            TableViewColumn {
                title: "+ (I)"
                role: "firstHalfIncome"
                width: 100
                delegate: TextField {
                    text: styleData.value
                }
            }
            TableViewColumn {
                title: "- (I)"
                role: "firstHalfOutcome"
                width: 100
                delegate: TextField {
                    text: styleData.value
                }
            }
            TableViewColumn {
                title: "+ (II)"
                role: "secondHalfIncome"
                width: 100
                delegate: TextField {
                    text: styleData.value
                }
            }
            TableViewColumn {
                title: "- (II)"
                role: "secondHalfOutcome"
                width: 100
                delegate: TextField {
                    text: styleData.value
                }
            }
        }

    }
}

