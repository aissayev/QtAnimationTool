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

//Helper functions

QList<ModelNode> acceptedModelNodeChildren(const ModelNode &parentNode)
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

//Class

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

void TimelineQmlBackend::destroyModel() {
    qDebug() << "Called";
    delete(m_model);
    m_model = new TimelineModel(this);
}

TimelineItem TimelineQmlBackend::buildItemTree(ModelNode node, int depth) {
    if (node.isValid()) {
        QString name = node.hasId() ? node.id() : node.simplifiedTypeName();
        const ModelNode constParent = node;
        TimelineItem item = TimelineItem(name, getNodeIconUrl(constParent), depth);
        loadKeyframes(&item, node);

        foreach(ModelNode child, node.directSubModelNodes()) {
            if( child.metaInfo().isGraphicalItem() && (child.isRootNode() || acceptedModelNodeChildren(child.parentProperty().parentModelNode()).contains(child))) {
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

//Only works right now if animations are sub-nodes of their targets.
void TimelineQmlBackend::loadKeyframes(TimelineItem *data, ModelNode node) {
    foreach(ModelNode child, node.directSubModelNodes()) {
        if (child.metaInfo().isSubclassOf("QtQuick.Animation")) {
            loadKeyframesHelper(data, node, child, 0);
        }
    }
}

int TimelineQmlBackend::loadKeyframesHelper(TimelineItem *data, ModelNode parentNode, ModelNode node, int startTime) {
    qDebug() << "Node:" << node.simplifiedTypeName();
    if(node.metaInfo().isSubclassOf("QtQuick.ParallelAnimation")) {
        int longestDuration = 0;
        foreach(ModelNode child, node.directSubModelNodes()) {
            if (child.metaInfo().isSubclassOf("QtQuick.Animation")){
                int duration = loadKeyframesHelper(data, parentNode, child, startTime);
                if(duration > longestDuration)
                    longestDuration = duration;
            }
        }
        return longestDuration;
    }
    else if(node.metaInfo().isSubclassOf("QtQuick.SequentialAnimation")) {
        int currentTime = startTime;
        foreach(ModelNode child, node.directSubModelNodes()) {
            if (child.metaInfo().isSubclassOf("QtQuick.Animation"))
                currentTime += loadKeyframesHelper(data, parentNode, child, currentTime);
        }
        return currentTime - startTime;
    }
    else if(node.metaInfo().isSubclassOf("QtQuick.PauseAnimation")) {
        int duration = node.variantProperty("duration").value().toInt();
        return duration;
    }
    else if (node.metaInfo().isSubclassOf("QtQuick.PathAnimation")) {
        qDebug() << "PathAnimation is currently unsupported";
        return 0;
    }
    else if(node.metaInfo().isSubclassOf("QtQuick.Animation")){
        PropertyKeyframePair *keyframe = buildKeyframe(data, parentNode, node, startTime);
        data->addKeyframe(keyframe);
        return keyframe->duration();
    }
    return 0;
}

PropertyKeyframePair *TimelineQmlBackend::buildKeyframe(TimelineItem *data, ModelNode parentNode, ModelNode node, int startTime) {
    QString propertyName;
    QVariant startValue;
    QVariant endValue;

    int duration = 0;

    qDebug() << "Parsing Binding Properties";
    foreach (BindingProperty property, node.bindingProperties()) {
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
    foreach(VariantProperty property, node.variantProperties()) {
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

    if (node.metaInfo().isSubclassOf("QtQuick.Animator") && propertyName.isEmpty())
    {
        qDebug() << node.metaInfo().typeName();
        if (node.metaInfo().isSubclassOf("QtQuick.YAnimator")) {
            propertyName = "y";
        }
        else if (node.metaInfo().isSubclassOf("QtQuick.XAnimator")) {
            propertyName = "x";
        }
        else if (node.metaInfo().isSubclassOf("QtQuick.ScaleAnimator")) {
            propertyName = "scale";
        }
        else if (node.metaInfo().isSubclassOf("QtQuick.OpacityAnimator")) {
            propertyName = "opacity";
        }
        else if (node.metaInfo().isSubclassOf("QtQuick.RotationAnimator")) {
            propertyName = "rotation";
        }
        else if (node.metaInfo().isSubclassOf("QtQuick.UniformAnimator")) {
            qDebug() << "Uniforms not supported yet";
            propertyName = "unsupported";
        }
    }

    if (startValue.isNull()) {
        if(data->propertyMap().contains(propertyName))
            startValue = extractValueAtTime(data->propertyMap()[propertyName],startTime);
        else {
            AbstractProperty parentProp = parentNode.property(propertyName.toLatin1());
            if (parentProp.isValid()) {
                if (parentProp.isBindingProperty()) {
                    startValue = parentProp.toBindingProperty().expression();
                }
                else if (parentProp.isVariantProperty()) {
                    startValue = parentProp.toVariantProperty().value();
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
