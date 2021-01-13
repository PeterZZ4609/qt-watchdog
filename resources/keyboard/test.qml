import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.VirtualKeyboard 2.2
import QtQuick.VirtualKeyboard.Settings 2.2
import "content"


/*  Keyboard input panel.

            The keyboard is anchored to the bottom of the application.
        */
InputPanel {
    id: inputPanel
    z: 200
    y: 480
    anchors.left: 0
    anchors.right: 800
    states: State {
        name: "visible"
        /*  The visibility of the InputPanel can be bound to the Qt.inputMethod.visible property,
                    but then the handwriting input panel and the keyboard input panel can be visible
                    at the same time. Here the visibility is bound to InputPanel.active property instead,
                    which allows the handwriting panel to control the visibility when necessary.
                */
        when: inputPanel.active
        PropertyChanges {
            target: inputPanel
            y: 240
        }
    }
    transitions: Transition {
        id: inputPanelTransition
        from: ""
        to: "visible"
        reversible: true
        enabled: !VirtualKeyboardSettings.fullScreenMode
        ParallelAnimation {
            NumberAnimation {
                properties: "y"
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
    }
    Binding {
        target: InputContext
        property: "animating"
        value: inputPanelTransition.running
    }
    AutoScroller {}
}

