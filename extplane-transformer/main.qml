import QtQuick 2.6
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
                model: ["None", "Condor"]
            }
        }
    }
    statusBar: StatusBar {
        RowLayout {
//            anchors.fill: parent
            Label { text: tcpserver.clientCount + " client(s) connected." }
        }
    }
}
