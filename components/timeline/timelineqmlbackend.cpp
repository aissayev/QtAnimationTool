#include "modelnode.h"
#include "nodemetainfo.h"
#include "metainfo.h"
#include "model.h"

#include "timelineqmlbackend.h"
#include "timelineview.h"
#include "timelinewidget.h"
#include "timelineimageprovider.h"

#include <QQmlContext>
#include <QDebug>
#include "variantproperty.h"
#include "bindingproperty.h"
#include "nodeproperty.h"
#include "nodelistproperty.h"
#include "nodeabstractproperty.h"

namespace QmlDesigner {

//Class

TimelineQmlBackend::TimelineQmlBackend(TimelineView *timelineView)
    : m_timelineModel(new TimelineModel(this)),
      m_timelineIdList(),
      m_itemIdMap(),
      m_modelIdMap(),
      m_timelineView(timelineView)
{
    if (!m_widget) {
        m_widget = new TimelineWidget(timelineView);
    }
    m_widget->engine()->addImageProvider(QStringLiteral("timeline"), new TimelineImageProvider());
    context()->setContextProperty(QLatin1String("modelTree"), m_timelineModel);
    context()->setContextProperty(QLatin1String("timelineList"), QVariant::fromValue(m_timelineIdList));
    context()->setContextProperty(QLatin1String("availableItemList"), QVariant::fromValue(fetchAvailableItemIds()));

    m_widget->init();
}

void TimelineQmlBackend::setupModel() {
    // Load root model node
    m_rootModelNode = m_timelineView->rootModelNode();

    // Log all timelines present in qml
    fetchTimelineIds();

    // Log all model nodes that have an id that are present in qml
    fillModelIdMap();

    // Load timeline from qml if there is one
    if(!m_timelineIdList.isEmpty())
        constructTimeline(m_timelineIdList.first());

    // Pass model to context
    context()->setContextProperty(QLatin1String("modelTree"), m_timelineModel);
    context()->setContextProperty(QLatin1String("timelineList"), QVariant::fromValue(m_timelineIdList));
}

void TimelineQmlBackend::destroyModel() {
    qDebug() << "Called";
    delete(m_timelineModel);
    m_timelineModel = new TimelineModel(this);
    m_itemIdMap = QMap<QString,TimelineItem>();
    m_modelIdMap = QMap<QString,ModelNode>();
    m_timelineIdList = QList<QString>();
}

void TimelineQmlBackend::fetchTimelineIds() {
    foreach(ModelNode child, m_rootModelNode.directSubModelNodes()) {
        if(child.metaInfo().isSubclassOf("QtQuick.ParallelAnimation")) {
            if(child.hasId())
                m_timelineIdList.append(child.id());
        }
        else
            qDebug() << "Timeline root found with no id. Ignored";
    }
}

void TimelineQmlBackend::fillModelIdMap() {
    foreach(ModelNode child, m_rootModelNode.allSubModelNodes()) {
        if(child.hasId())
           m_modelIdMap.insert(child.id(),child);
    }
}

void TimelineQmlBackend::constructTimeline(QString timelineId) {
    delete(m_timelineModel);
    m_timelineModel = new TimelineModel(this);
    m_itemIdMap = QMap<QString,TimelineItem>();

    if(m_modelIdMap.contains(timelineId)){
        foreach(ModelNode child, m_modelIdMap[timelineId].directSubModelNodes()) {
            constructTimelineForItem(child);
        }
    }
    else
        qDebug() << "Timeline [" << timelineId << "] was not found and could not be constructed";

    context()->setContextProperty(QLatin1String("availableItemlist"), QVariant::fromValue(fetchAvailableItemIds()));
}

void TimelineQmlBackend::constructTimelineForItem(ModelNode itemParallelAnimation) {
    foreach(ModelNode child, itemParallelAnimation.directSubModelNodes()) {
        constructTimelineForItemProperty(child);
    }
}

void TimelineQmlBackend::constructTimelineForItemProperty(ModelNode itemSequentialAnimation) {
    int time = 0;
    foreach(ModelNode child, itemSequentialAnimation.directSubModelNodes()) {
        if(child.metaInfo().isSubclassOf("QtQuick.PauseAnimation"))
            time += child.variantProperty("duration").value().toInt();
        else if(child.metaInfo().isSubclassOf("QtQuick.PropertyAnimation")) {
            QString targetId = child.bindingProperty("target").expression();

            if(!m_itemIdMap.contains(targetId)) {
                if(!m_modelIdMap.contains(targetId)) {
                    qDebug() << "Animation could not be linked to model[" << targetId << "]";
                    continue;
                }
                const ModelNode targetModel = m_modelIdMap[targetId];
                TimelineItem item(targetModel.simplifiedTypeName(),targetId,getNodeIconUrl(targetModel));
                m_itemIdMap.insert(targetId,item);
                m_timelineModel->addItem(m_itemIdMap[targetId]);
            }

            PropertyKeyframePair *keyframe = constructKeyframe(&m_itemIdMap[targetId],m_modelIdMap[targetId],child, time);
            time += keyframe->duration();
            m_itemIdMap[targetId].addKeyframe(keyframe);
        }
    }
}

PropertyKeyframePair *TimelineQmlBackend::constructKeyframe(TimelineItem *item, ModelNode modelNode, ModelNode animationNode, int startTime) {
    QString propertyName;
    QVariant startValue;
    QVariant endValue;

    int duration = 0;

    qDebug() << "Parsing Binding Properties";
    foreach (BindingProperty property, animationNode.bindingProperties()) {
        qDebug() << "Property" << property.name();
        if (property.name() == "property") {
            propertyName = property.expression();
        }
        else if (property.name() == "from") {
            startValue = property.expression();
        }
        else if (property.name() == "to") {
            endValue = property.expression();
        }
        else {
            qDebug() << "Property" << property.name() << "is not recognized yet";
        }
    }

    qDebug() << "Parsing Variant Properties";
    foreach(VariantProperty property, animationNode.variantProperties()) {
        qDebug() << "Property" << property.name();
        if (property.name() == "duration") {
            duration = property.value().toInt();
        }
        else if (property.name() == "to") {
            endValue = property.value();
        }
        else if (property.name() == "from") {
            startValue = property.value();
        }
        else {
            qDebug() << "Property" << property.name() << "is not recognized yet";
        }
    }

    if (animationNode.metaInfo().isSubclassOf("QtQuick.Animator") && propertyName.isEmpty())
    {
        qDebug() << animationNode.metaInfo().typeName();
        if (animationNode.metaInfo().isSubclassOf("QtQuick.YAnimator")) {
            propertyName = "y";
        }
        else if (animationNode.metaInfo().isSubclassOf("QtQuick.XAnimator")) {
            propertyName = "x";
        }
        else if (animationNode.metaInfo().isSubclassOf("QtQuick.ScaleAnimator")) {
            propertyName = "scale";
        }
        else if (animationNode.metaInfo().isSubclassOf("QtQuick.OpacityAnimator")) {
            propertyName = "opacity";
        }
        else if (animationNode.metaInfo().isSubclassOf("QtQuick.RotationAnimator")) {
            propertyName = "rotation";
        }
        else if (animationNode.metaInfo().isSubclassOf("QtQuick.UniformAnimator")) {
            qDebug() << "Uniforms not supported yet";
            propertyName = "unsupported";
        }
    }

    if (startValue.isNull()) {
        if(item->propertyMap().contains(propertyName))
            startValue = extractValueAtTime(item->propertyMap()[propertyName],startTime);
        else {
            AbstractProperty modelProp = modelNode.property(propertyName.toLatin1());
            if (modelProp.isValid()) {
                if (modelProp.isBindingProperty()) {
                    startValue = modelProp.toBindingProperty().expression();
                }
                else if (modelProp.isVariantProperty()) {
                    startValue = modelProp.toVariantProperty().value();
                }
            }
            else {
                qDebug() << "invalid property " << propertyName;
            }
        }
    }

    qDebug() << "Keyframe built: [" << propertyName << " : " << startTime << " : " << startValue << "]";

    return new PropertyKeyframePair(propertyName,startTime,duration,startValue,endValue,0);
}

QList<QString> TimelineQmlBackend::fetchAvailableItemIds() {
    QList<QString> availableItemIds = QList<QString>();
    foreach(QString id, m_modelIdMap.keys()) {
        if(!m_itemIdMap.keys().contains(id) && !m_modelIdMap[id].metaInfo().isSubclassOf("QtQuick.Animation"))
            availableItemIds.append(id);
    }
    return availableItemIds;
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

QList<ModelNode> TimelineQmlBackend::acceptedModelNodeChildren(const ModelNode &parentNode)
{
    QList<ModelNode> children;
    PropertyNameList properties;

    if (parentNode.metaInfo().hasDefaultProperty())
        properties.append(parentNode.metaInfo().defaultPropertyName());

    foreach (const PropertyName &propertyName, properties) {
        AbstractProperty property(parentNode.property(propertyName));
        if (property.isNodeAbstractProperty())
            children.append(property.toNodeAbstractProperty().directSubNodes());
    }

    return children;
}

TimelineModel *TimelineQmlBackend::model() {
    return m_timelineModel;
}

TimelineWidget *TimelineQmlBackend::widget() {
    return m_widget;
}

QQmlContext *TimelineQmlBackend::context() {
    return m_widget->rootContext();
}

}
