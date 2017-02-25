#pragma once

#include "modelnode.h"

#include <QObject>
#include <QPointer>
#include <QQmlContext>
#include "timelinemodel.h"

namespace QmlDesigner {

  class TimelineView;
  class TimelineModel;
  class TimelineWidget;

  class TimelineQmlBackend : public QObject
  {
      Q_OBJECT
  public:
    explicit TimelineQmlBackend(QObject *parent = 0);
    TimelineQmlBackend(TimelineView *timelineView);

    TimelineModel *model();
    TimelineWidget *widget();
    QQmlContext *context();

    void setupModel();
    void destroyModel();

  protected:
    static QString getNodeIconUrl(ModelNode modelNode);

  private:
    QPointer<TimelineModel> m_timelineModel;
    QList<QString> m_timelineIdList;
    QString m_currentTimeline;
    QStringList m_availableItemList;
    QMap<QString,TimelineItem> m_itemIdMap;
    QMap<QString,ModelNode> m_modelIdMap;
    QPointer<TimelineWidget> m_widget;
    QPointer<TimelineView> m_timelineView;
    ModelNode m_rootModelNode;

    int m_time;

    void fillModelIdMap();
    void constructTimeline(QString timelineId);
    void constructTimelineForItem(ModelNode itemParallelAnimation);
    void constructTimelineForItemProperty(ModelNode itemSequentialAnimation);
    void fetchTimelineIds();
    QStringList fetchAvailableItemProperties(QString itemId);
    QVariant extractValue(ModelNode modelNode, TimelineItem *item, QString property, int startTime) const;
    QVariant extractValueAtTime(QList<QObject*> keyframes, int startTime) const;
    void updateAvailableItemList();

    ModelNode createModelNode(QString type);
    void exportTimeline();
    void exportTimelineItems(ModelNode timelineRoot);
    void exportTimelineItemKeyframes(ModelNode itemAnimationNode, TimelineItem item);

    PropertyKeyframePair *constructKeyframe(TimelineItem *item, ModelNode modelNode, ModelNode animationNode, int startTime);
    QList<ModelNode> acceptedModelNodeChildren(const ModelNode &parentNode);

  private slots:
    void reloadConnections();
    void setCurrentTime(int time);
    void setTimeline(QString timelineId);
    void addTimelineItem(QString itemId);
    void addTimelineItemProperty(QString itemId,QString propertyName);
    void addKeyframe(QString itemId, QString propertyName, int time);
    void playPressed();
  };
}
