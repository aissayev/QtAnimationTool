#include "modelnode.h"
#include "nodemetainfo.h"
#include "metainfo.h"
#include "model.h"

#include "timelineqmlbackend.h"
#include "timelineview.h"
#include "timelinewidget.h"
#include "timelineimageprovider.h"

#include <QQmlContext>
#include "variantproperty.h"
#include "bindingproperty.h"

namespace QmlDesigner {
    TimelineQmlBackend::TimelineQmlBackend(TimelineView *timelineView)
        : m_model(new TimelineModel(this)),
          m_timelineView(timelineView)
    {
        if (!m_widget) {
            m_widget = new TimelineWidget(timelineView);
        }
        m_widget->engine()->addImageProvider(QStringLiteral("timeline"), new TimelineImageProvider());
        context()->setContextProperty(QLatin1String("modelTree"), QVariant::fromValue(m_model));
        m_widget->init();
    }

    void TimelineQmlBackend::setupModel() {
        m_rootModelNode = m_timelineView->rootModelNode();
        qDebug() << "Keyframe debug: [ property  :  start time  :  start value ]";
        TimelineItem rootItem = buildItemTree(m_rootModelNode, 1);
        qDebug() << "Populating timeline model.";
        populateTimelineModel(rootItem);
        qDebug() << "Finished populating timeline model.";
        context()->setContextProperty(QLatin1String("modelTree"), QVariant::fromValue(m_model));
    }

    TimelineItem TimelineQmlBackend::buildItemTree(ModelNode parent, int depth) {
        if (parent.isValid()) {
            QString name = parent.hasId() ? parent.id() : parent.simplifiedTypeName();

            const ModelNode constParent = parent;
            TimelineItem item = TimelineItem(name, getNodeIconUrl(constParent), depth);
            loadKeyframes(&item, parent);

            foreach(ModelNode child, parent.directSubModelNodes()) {
                if(child.metaInfo().isGraphicalItem()) {
                    TimelineItem childItem = buildItemTree(child, depth + 1);
                    if(childItem.name() != "invalid_timelineitem")
                        item.addChild(childItem);
                }
            }
            return item;
        }
        return TimelineItem("invalid_timelineitem","",-1);
    }

    void TimelineQmlBackend::populateTimelineModel(TimelineItem item) {
        m_model->addItem(item);

        QList<TimelineItem> *children = item.children();
        foreach(TimelineItem child, *children) {
            populateTimelineModel(child);
        }
    }

    //Only works right now if animations are sub-models of their targets.
    void TimelineQmlBackend::loadKeyframes(TimelineItem *data, ModelNode node) {
        foreach(ModelNode child, node.directSubModelNodes()) {
            QString name = child.hasId() ? child.id() : child.simplifiedTypeName();
            if (name.contains("Animation"))
                loadKeyframesHelper(data, node, child, name, 0);
        }
    }

    int TimelineQmlBackend::loadKeyframesHelper(TimelineItem *data, ModelNode parentNode, ModelNode node, QString name, int startTime) {
        if(name.contains("ParallelAnimation")) {
            int longestDuration = 0;
            foreach(ModelNode child, node.directSubModelNodes()) {
                QString childName = child.hasId() ? child.id() : child.simplifiedTypeName();
                if (childName.contains("Animation")){
                    int duration = loadKeyframesHelper(data, parentNode, child, childName, startTime);
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
                    currentTime += loadKeyframesHelper(data, parentNode, child, childName, currentTime);
            }
            return currentTime - startTime;
        }
        else if(name.contains("PauseAnimation")) {
            int duration = extractVariantProperty(node.property("duration")).toInt();
            return duration;
        }
        else if(name.contains("Animation")){
            PropertyKeyframePair *keyframe = buildKeyframe(data,parentNode,node,startTime);
            data->addKeyframe(keyframe);
            return keyframe->duration();
        }
        return 0;
    }

    PropertyKeyframePair *TimelineQmlBackend::buildKeyframe(TimelineItem *data, ModelNode parentNode, ModelNode node, int startTime) {
        QString property = node.property("property").toBindingProperty().expression();
        int duration = extractVariantProperty(node.property("duration")).toInt();
        QVariant endValue = extractVariantProperty(node.property("to"));
        QVariant startValue;
        if(node.hasProperty("from"))
            startValue = extractVariantProperty(node.property("from"));
        else if(data->propertyMap().contains(property))
            startValue = extractValueAtTime(data->propertyMap()[property],startTime);
        else
            startValue = parentNode.property(property.toLatin1()).toVariantProperty().value();

        qDebug() << "Keyframe built: [" << property << " : " << startTime << " : " << startValue << "]";

        return new PropertyKeyframePair(property,startTime,duration,startValue,endValue,0);
    }

    QVariant TimelineQmlBackend::extractVariantProperty(AbstractProperty property) const{
        Q_ASSERT(property.isVariantProperty());
        return property.toVariantProperty().value();
    }

    QVariant TimelineQmlBackend::extractValueAtTime(QList<QObject*> keyframes, int startTime) const {
        int latestTime = 0;
        QVariant value = 0;
        foreach(QObject *object, keyframes) {
            PropertyKeyframePair *keyframe = (PropertyKeyframePair*)object;
            int frameStartTime = keyframe->startTime();
            int duration = keyframe->duration();
            if(frameStartTime + duration == startTime)
                return keyframe->endValue();
            else if(frameStartTime + duration > latestTime && frameStartTime < startTime) {
                latestTime = frameStartTime + duration;
                value = keyframe->endValue();
            }
        }
        return value;
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
