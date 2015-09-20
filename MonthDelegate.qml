import QtQuick 2.4
import QtQuick.Controls 1.2
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
                title: "Первая половина +"
                role: "firstHalfIncome"
                width: 100
            }
            TableViewColumn {
                title: "Первая половина -"
                role: "firstHalfOutcome"
                width: 100
            }
            TableViewColumn {
                title: "Вторая половина +"
                role: "secondHalfIncome"
                width: 100
            }
            TableViewColumn {
                title: "Вторая половина -"
                role: "secondHalfOutcome"
                width: 100
            }
        }

    }
}

