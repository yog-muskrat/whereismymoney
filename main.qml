import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Where is my money!?")

    ColumnLayout {
        id: mainLayout

        anchors.margins: 10
        anchors.fill: parent

        RowLayout
        {
            Button {
                id: prevButton

                text: "Назад"
            }

            Label {
                id: monthLabel

                Layout.fillWidth: true
                text: "Сентябрь"
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 18
                font.bold: true
                color: "#424242"
            }

            Button {
                id: nextButton

                text: "Вперед"
            }
        }

        TableView {
            id: table

            Layout.fillWidth: true
            Layout.fillHeight: true

            TableViewColumn {
                role: "display"
                title: "Код"
                width: table.parent.width / 2 - 2
            }

            TableViewColumn {
                role: "edit"
                title: "Название"
                width: table.parent.width / 2 - 2
            }

            model: catModel
        }
    }
}
