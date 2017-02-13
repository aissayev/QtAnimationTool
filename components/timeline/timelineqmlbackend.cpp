#include "modelnode.h"
#include "nodemetainfo.h"
#include "metainfo.h"
#include "model.h"

#include "timelineqmlbackend.h"
#include "timelineview.h"
#include "timelinewidget.h"

#include <QQmlContext>
#include "variantproperty.h"
#include "bindingproperty.h"

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
            loadKeyframes(&data, parent);
            m_model->addItem(data);

            QList<ModelNode> children = parent.directSubModelNodes();
            foreach(ModelNode node, children) {
                if (node.metaInfo().isGraphicalItem())
                    makeModelFromNode(node, depth + 1);
            }
        }
    }

    void TimelineQmlBackend::loadKeyframes(TimelineItem *data, ModelNode node) {
        foreach(ModelNode child, node.directSubModelNodes()) {
            QString name = child.hasId() ? child.id() : child.simplifiedTypeName();
            if (name.contains("Animation"))
                loadKeyframesHelper(data, child, name, 0);
        }
    }

    //Temporarily Broken
    int TimelineQmlBackend::loadKeyframesHelper(TimelineItem *data, ModelNode node, QString name, int startTime) {
        if(name.contains("ParallelAnimation")) {
            int longestDuration = 0;
            foreach(ModelNode child, node.directSubModelNodes()) {
                QString childName = child.hasId() ? child.id() : child.simplifiedTypeName();
                if (childName.contains("Animation")){
                    int duration = loadKeyframesHelper(data, child, childName, startTime);
                    if(duration > longestDuration)
                        longestDuration = duration;
                }
            }
            return longestDuration;
        }
        else if(name.contains("SequentialAnimation")) {
            int currentTime = startTime;
            foreach(ModelNode child, node.directSubModelNodes()) {
                QString childName = child.hasId() ? child.id() : child.simplifiedTypeName();
                if (childName.contains("Animation"))
                    currentTime += loadKeyframesHelper(data, child, childName, currentTime);
                qDebug() << "Current Time [" << currentTime << "]";
            }
            return currentTime - startTime;
        }
        else if(name.contains("PauseAnimation")) {
            int duration = extractVariantProperty(node.property("duration")).toInt();
            return duration;
        }
        else if(name.contains("Animation")){
            PropertyKeyframePair *keyframe = buildKeyframe(node,startTime);
            data->addKeyframe(keyframe);
            return keyframe->duration();
        }
        return 0;
    }

    // Temporarily Broken
    PropertyKeyframePair *TimelineQmlBackend::buildKeyframe(ModelNode node, int startTime) {
        QString property = node.property("property").toBindingProperty().expression();
        int duration = extractVariantProperty(node.property("duration")).toInt();
        QVariant endValue = extractVariantProperty(node.property("to"));

        return new PropertyKeyframePair(property,startTime,duration,0,endValue,0);
    }

    QVariant TimelineQmlBackend::extractVariantProperty(AbstractProperty property) const{
        Q_ASSERT(property.isVariantProperty());
        return property.toVariantProperty().value();
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
