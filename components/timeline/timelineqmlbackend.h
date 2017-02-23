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
    QVariant extractValueAtTime(QList<QObject*> keyframes, int startTime) const;
    void updateAvailableItemList();

    PropertyKeyframePair *constructKeyframe(TimelineItem *item, ModelNode modelNode, ModelNode animationNode, int startTime);
    QList<ModelNode> acceptedModelNodeChildren(const ModelNode &parentNode);

  private slots:
    void reloadConnections();
    void setTimeline(QString timelineId);
  };
}
