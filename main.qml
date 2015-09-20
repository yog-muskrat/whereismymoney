import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Where is my money!?")

    ScrollView {
        anchors.fill: parent

        ListView {
            id: listView

            anchors.fill: parent
            anchors.margins: 10
            spacing: 10
            orientation: ListView.Horizontal


            model: wimmmodel

            delegate: MonthDelegate {
                header: year+' '+month
                dataModel: moneyModel
                height: parent.height
                width: 504
            }
        }
    }
}
