import bb.cascades 1.4
import org.ekkescorner.data 1.0
import org.ekkescorner.common 1.0

Page {
    id: orderPage
    property Order orderData
    
    titleBar: TitleBar {
        title: "First data access"
    }

    actions: [
        ActionItem {
            title: "find"
            ActionBar.placement: ActionBarPlacement.Signature
            onTriggered: {
                orderPage.orderData = dataManager.findOrderByNr(42)
            }
        }
    ]

    Container {
        topPadding: ui.du(10.0)
        leftPadding: ui.du(8.0)
        rightPadding: ui.du(8.0)
        Label {
            text: "Title"
            textStyle.color: ui.palette.primary
            textStyle.base: SystemDefaults.TextStyles.BigText
        }
        Container {
            leftPadding: ui.du(4.0)
            Label {
                id: titleLabel
                text: orderData.title
                textStyle.base: SystemDefaults.TextStyles.TitleText
            }
        }
        
        Label {
            text: "Nr"
            textStyle.color: ui.palette.primary
            textStyle.base: SystemDefaults.TextStyles.BigText
        }
        Container {
            leftPadding: ui.du(4.0)
            Label {
                id: nrLabel
                text: orderData.nr
                textStyle.base: SystemDefaults.TextStyles.TitleText
            }
        }
    }

    onCreationCompleted: {
        //
    }
}