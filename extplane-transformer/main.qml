import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("ExtPlane Transformer")
    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            width: parent.width
            Layout.alignment: Qt.AlignTop
            Label {
                text: "Data source"
            }
            ComboBox {
                Layout.fillWidth: true
                model: transformer.dataSources
                onCurrentTextChanged: transformer.dataSource = currentText;
            }
        }
    }
    statusBar: StatusBar {
        RowLayout {
//            anchors.fill: parent
            Label { text: transformer.tcpServer.clientCount + " client(s) connected." }
            Label { text: transformer.networkError }
        }
    }
}
