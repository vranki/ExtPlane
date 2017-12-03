import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0

TextArea {
    Layout.fillWidth: true
    Layout.fillHeight: true
    readOnly: true
    text: transformer.dataSource.helpText
}
