import bb.cascades 1.4
import org.ekkescorner.common 1.0

TabbedPane {
    id: rootPane
    showTabsOnActionBar: false

    attachedObjects: [
        QTimer {
            id: startupDelayedTimer
            interval: 100
            singleShot: true
            onTimeout: {
                // delayed init of data
                dataManager.init()
            }
        }
    ]

    Tab {
        id: orderTab
        title: "order"
        content: OrderPage {
        }
    }
    Tab {
        id: itemsTab
        title: "item"
        content: Page {
            Container {
                Label {
                    text: "dummi"
                }
            }
        }
    }

    onCreationCompleted: {
        // init dataManager, connect(), SceneCover
        startupDelayedTimer.start()
    }
}
