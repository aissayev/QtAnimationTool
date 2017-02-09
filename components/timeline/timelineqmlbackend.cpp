#include "modelnode.h"
#include "nodemetainfo.h"
#include "metainfo.h"
#include "model.h"

#include "timelineqmlbackend.h"
#include "timelineview.h"
#include "timelinewidget.h"
#include "timelinemodel.h"

#include <QQmlContext>

namespace QmlDesigner {
    TimelineQmlBackend::TimelineQmlBackend(TimelineView *timelineView)
        : m_model(new TimelineModel(this)),
          m_widget(new TimelineWidget(timelineView)),
          m_timelineView(timelineView)
    {
        //please give the model to model tree
    }

    void TimelineQmlBackend::setupModel() {
        m_rootModelNode = m_timelineView->rootModelNode();
        makeModelFromNode(m_rootModelNode, 1);
        context()->setContextProperty(QLatin1String("modelTree"), QVariant::fromValue(m_model));
    }

    void TimelineQmlBackend::makeModelFromNode(ModelNode parent, int depth) {
        if (parent.isValid()) {
            QString name;
            if (parent.hasId()) {
                name = parent.id();
            }
            else {
                name = parent.simplifiedTypeName();
            }
            const ModelNode constParent = parent;
            TimelineItem data = TimelineItem(name, getNodeIconUrl(constParent), depth);
            Keyframe *key = new Keyframe("hi",depth,10,0,10,0);
            data.addKeyframe(key);
            m_model->addItem(data);
            QList<ModelNode> children = parent.directSubModelNodes();
            foreach(ModelNode node, children) {
                if (node.metaInfo().isGraphicalItem())
                    makeModelFromNode(node, depth + 1);
            }
        }
    }

    TimelineModel *TimelineQmlBackend::model() {
        return m_model;
    }

    TimelineWidget *TimelineQmlBackend::widget() {
        return m_widget;
    }

    QQmlContext *TimelineQmlBackend::context() {
        return m_widget->rootContext();
    }

    void TimelineQmlBackend::emitSelectionChanged() {

    }

    void TimelineQmlBackend::emitValueChanged() {

    }

    QString TimelineQmlBackend::getNodeIconUrl(ModelNode modelNode) {
        if (modelNode.isValid()) {
            // if node has no own icon, search for it in the itemlibrary
            const ItemLibraryInfo *libraryInfo = modelNode.model()->metaInfo().itemLibraryInfo();
            QList <ItemLibraryEntry> itemLibraryEntryList = libraryInfo->entriesForType(
                modelNode.type(), modelNode.majorVersion(), modelNode.minorVersion());
            if (!itemLibraryEntryList.isEmpty()) {
                return QStringLiteral("image://qmldesigner_itemlibrary/") + itemLibraryEntryList.first().libraryEntryIconPath();
            }
            else if (modelNode.metaInfo().isValid())
                return QStringLiteral("image://qmldesigner_itemlibrary/");
        }
        return QStringLiteral("image://qmldesigner_itemlibrary/");
    }
}