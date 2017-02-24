#include "modelnode.h"
#include "nodemetainfo.h"
#include "metainfo.h"
#include "model.h"

#include "timelineqmlbackend.h"
#include "timelineview.h"
#include "timelinewidget.h"
#include "timelineimageprovider.h"

#include <QQuickItem>
#include <QQmlContext>
#include <QDebug>
#include "variantproperty.h"
#include "bindingproperty.h"
#include "nodeproperty.h"
#include "nodelistproperty.h"
#include "nodeabstractproperty.h"

namespace QmlDesigner {

//Class

TimelineQmlBackend::TimelineQmlBackend(QObject *parent) : QObject(parent) {

}

TimelineQmlBackend::TimelineQmlBackend(TimelineView *timelineView)
    : m_timelineModel(new TimelineModel(this)),
      m_timelineIdList(),
      m_availableItemList(),
      m_itemIdMap(),
      m_modelIdMap(),
      m_timelineView(timelineView)
{
    m_time = 0;

    if (!m_widget) {
        m_widget = new TimelineWidget(timelineView);
    }
    m_widget->engine()->addImageProvider(QStringLiteral("timeline"), new TimelineImageProvider());
    context()->setContextProperty(QLatin1String("modelTree"), m_timelineModel);
    context()->setContextProperty(QLatin1String("timelineList"), QVariant::fromValue(m_timelineIdList));
    context()->setContextProperty(QLatin1String("availableItemList"), m_availableItemList);
    context()->setContextProperty(QLatin1String("currentTime"), m_time);


    m_widget->init();

    // Do the business
    connect(m_widget,SIGNAL(qmlReloaded()),this,SLOT(reloadConnections()));
    reloadConnections();

}

void TimelineQmlBackend::setupModel() {
    // Load root model node
    m_rootModelNode = m_timelineView->rootModelNode();

    // Log all timelines present in qml
    fetchTimelineIds();

    // Log all model nodes that have an id that are present in qml
    fillModelIdMap();

    // Load timeline from qml if there is one
    if(!m_timelineIdList.isEmpty()){
        m_currentTimeline = m_timelineIdList.first();
        constructTimeline(m_currentTimeline);
    }

    // Pass model to context
    context()->setContextProperty(QLatin1String("modelTree"), m_timelineModel);
    context()->setContextProperty(QLatin1String("timelineList"), QVariant::fromValue(QStringList(m_timelineIdList)));
    context()->setContextProperty(QLatin1String("availableItemList"), m_availableItemList);
    context()->setContextProperty(QLatin1String("currentTime"), m_time);
}

void TimelineQmlBackend::destroyModel() {
    qDebug() << "Called";
    m_timelineModel->reset();
    m_itemIdMap = QMap<QString,TimelineItem>();
    m_availableItemList = QStringList();
    m_modelIdMap = QMap<QString,ModelNode>();
    m_timelineIdList = QList<QString>();
}

void TimelineQmlBackend::fetchTimelineIds() {
    foreach(ModelNode child, m_rootModelNode.directSubModelNodes()) {
        if(child.metaInfo().isSubclassOf("QtQuick.ParallelAnimation")) {
            if(child.hasId())
                m_timelineIdList.append(child.id());
            else
                qDebug() << "Timeline root found with no id. Ignored";
        }
    }
}

void TimelineQmlBackend::fillModelIdMap() {
    foreach(ModelNode child, m_rootModelNode.allSubModelNodesAndThisNode()) {
        if(child.hasId())
           m_modelIdMap.insert(child.id(),child);
    }
}

void TimelineQmlBackend::setCurrentTime(int time) {
    qDebug() << "[backend] setCurrentTime(" << time << ")";
    m_time = time;
    context()->setContextProperty(QLatin1String("currentTime"), m_time);
}

void TimelineQmlBackend::setTimeline(QString timelineId) {
    qDebug() << "[Backend] setTimeline(" << timelineId << ")";

    this->exportTimeline();
    this->constructTimeline(timelineId);
}

void TimelineQmlBackend::exportTimeline() {
    bool running = false;

    if(m_modelIdMap.contains(m_currentTimeline)) {
        running = m_modelIdMap[m_currentTimeline].variantProperty("running").value().toBool();
        //destroy the model node
    }

    ModelNode timelineNode = createModelNode("QtQuick.ParallelAnimation");
    m_rootModelNode.nodeAbstractProperty(m_rootModelNode.metaInfo().defaultPropertyName()).reparentHere(timelineNode);
    timelineNode.setIdWithRefactoring(m_currentTimeline + "_new"); // will remove new tag after model destruction is implemented
    timelineNode.variantProperty("running").setValue(running);

    exportTimelineItems(timelineNode);
}

void TimelineQmlBackend::exportTimelineItems(ModelNode timelineRoot) {
    foreach(TimelineItem item, m_timelineModel->items()) {
        ModelNode itemNode = createModelNode("QtQuick.ParallelAnimation");
        //timelineRoot.nodeAbstractProperty(m_rootModelNode.metaInfo().defaultPropertyName()).reparentHere(itemNode);
    }
}

ModelNode TimelineQmlBackend::createModelNode(QString type) {
    return m_timelineView->createModelNode(type.toLatin1(),m_rootModelNode.majorVersion(),m_rootModelNode.minorVersion());
}

void TimelineQmlBackend::addTimelineItem(QString timelineItemId) {
    if(m_modelIdMap.contains(timelineItemId)) {
        const ModelNode node = m_modelIdMap[timelineItemId];
        TimelineItem item(node.simplifiedTypeName(),timelineItemId,getNodeIconUrl(node));
        m_itemIdMap.insert(timelineItemId,item);
        m_timelineModel->addItem(item);
    }
    else
        qDebug() << "Item [" << timelineItemId << "] could not be added as it does not exist.";
}

void TimelineQmlBackend::addTimelineItemProperty(QString itemId, QString propertyName) {
    if(m_itemIdMap.contains(itemId)) {
        TimelineItem *item = m_timelineModel->getItemById(itemId);
        item->propertyMap().insert(propertyName,QList<QObject*>());
        m_itemIdMap.insert(itemId,*item);
    }
    else
        qDebug() << "Property [" << propertyName << "] could not be added to Item [" << itemId << "] because item does not exist.";
}

void TimelineQmlBackend::addKeyframe(QString itemId, QString propertyName, int time) {
    qDebug() << "[backend] addKeyframe(" << itemId << ", " << propertyName << ", " << time << ")";
    if(m_itemIdMap.contains(itemId)) {
        TimelineItem *item = m_timelineModel->getItemById(itemId);
        if(!item->propertyMap().contains(propertyName))
            addTimelineItemProperty(itemId,propertyName);
        QVariant startValue = extractValue(m_modelIdMap[itemId],item,propertyName,time);
        PropertyKeyframePair *keyframe = new PropertyKeyframePair(propertyName,time,0,startValue,startValue,0);
        item->addKeyframe(keyframe);
        m_itemIdMap.insert(itemId,*item);
        m_timelineModel->updateQmlTimelineItem(itemId);
        context()->setContextProperty(QLatin1String("modelTree"), m_timelineModel);
    }
    else
        qDebug() << "Keyframe for Property [" << propertyName << "] could not be added to Item [" << itemId << "] because item does not exist.";
}

void TimelineQmlBackend::reloadConnections() {
    connect(m_widget->rootObject(), SIGNAL(setCurrentTime(int)), this, SLOT(setCurrentTime(int)));
    connect(m_widget->rootObject(), SIGNAL(setTimeline(QString)), this, SLOT(setTimeline(QString)));
    connect(m_widget->rootObject(), SIGNAL(addTimelineItem(QString)), this, SLOT(addTimelineItem(QString)));
    connect(m_widget->rootObject(), SIGNAL(addTimelineItemProperty(QString,QString)), this, SLOT(addTimelineItemProperty(QString,QString)));
    connect(m_widget->rootObject(), SIGNAL(addKeyframe(QString,QString,int)), this, SLOT(addKeyframe(QString,QString,int)));
}

void TimelineQmlBackend::constructTimeline(QString timelineId) {
    m_timelineModel->reset();
    m_itemIdMap = QMap<QString,TimelineItem>();

    // If timeline already exists, load previous data from QML
    if(m_modelIdMap.contains(timelineId)){
        foreach(ModelNode child, m_modelIdMap[timelineId].directSubModelNodes()) {
            constructTimelineForItem(child);
        }
    }

    foreach(TimelineItem item, m_itemIdMap.values())
        m_timelineModel->addItem(item);
    updateAvailableItemList();
    qDebug() << "[backend-126] Available items: " << m_availableItemList.size();
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

    foreach (BindingProperty property, animationNode.bindingProperties()) {
        if (property.name() == "property") {
            propertyName = property.expression();
        }
        else if (property.name() == "from") {
            startValue = property.expression();
        }
        else if (property.name() == "to") {
            endValue = property.expression();
        }
    }

    foreach(VariantProperty property, animationNode.variantProperties()) {
        if (property.name() == "duration") {
            duration = property.value().toInt();
        }
        else if (property.name() == "to") {
            endValue = property.value();
        }
        else if (property.name() == "from") {
            startValue = property.value();
        }
        else if (property.name() == "property") {
            propertyName = property.value().toString();
        }
    }

    if (animationNode.metaInfo().isSubclassOf("QtQuick.Animator") && propertyName.isEmpty())
    {
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
            propertyName = "unsupported";
        }
    }

    if (startValue.isNull())
        startValue = extractValue(modelNode,item,propertyName,startTime);

    qDebug() << "Keyframe built: [" << propertyName << " : " << startTime << " : " << startValue << "]";

    return new PropertyKeyframePair(propertyName,startTime,duration,startValue,endValue,0);
}

void TimelineQmlBackend::updateAvailableItemList() {
    m_availableItemList = QStringList();
    foreach(QString id, m_modelIdMap.uniqueKeys()) {
        if(!m_itemIdMap.uniqueKeys().contains(id) && !m_modelIdMap[id].metaInfo().isSubclassOf("QtQuick.Animation")) {
            m_availableItemList.append(id);
        }
    }

    context()->setContextProperty(QLatin1String("availableItemlist"), m_availableItemList);
}

QString TimelineQmlBackend::getNodeIconUrl(ModelNode modelNode) {
    if (modelNode.isValid()) {
        // if node has no own icon, search for it in the itemlibrary
        const ItemLibraryInfo *libraryInfo = modelNode.model()->metaInfo().itemLibraryInfo();
        QList <ItemLibraryEntry> itemLibraryEntryList = libraryInfo->entriesForType(
                    modelNode.type(), modelNode.majorVersion(), modelNode.minorVersion());
        if (!itemLibraryEntryList.isEmpty()) {
            return QStringLiteral("image://qmldesigner_itemlibrary/") + itemLibraryEntryList.first().libraryEntryIconPath().replace(".png", "16.png");
        }
        else if (modelNode.metaInfo().isValid())
            return QStringLiteral("image://qmldesigner_itemlibrary/");
    }
    return QStringLiteral("image://qmldesigner_itemlibrary/");
}

QVariant TimelineQmlBackend::extractValue(ModelNode modelNode, TimelineItem *item, QString propertyName, int startTime) const {
    if(item->propertyMap().contains(propertyName))
        return extractValueAtTime(item->propertyMap()[propertyName],startTime);
    else {
        AbstractProperty modelProp = modelNode.property(propertyName.toLatin1());
        if (modelProp.isValid()) {
            if (modelProp.isBindingProperty()) {
                return modelProp.toBindingProperty().expression();
            }
            else if (modelProp.isVariantProperty()) {
                return modelProp.toVariantProperty().value();
            }
        }
    }
    return "";
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
