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
    QMap<QString,TimelineItem> m_itemIdMap;
    QMap<QString,ModelNode> m_modelIdMap;
    QPointer<TimelineWidget> m_widget;
    QPointer<TimelineView> m_timelineView;
    ModelNode m_rootModelNode;

    //----------------------------------------------------------------------------
    void loadKeyframes(TimelineItem *data, ModelNode node);
    int loadKeyframesHelper(TimelineItem *data, ModelNode parentNode, ModelNode node, int startTime);
    QVariant extractValueAtTime(QList<QObject*> keyframes, int startTime) const;

    TimelineItem buildItemTree(ModelNode parent, int depth);
    void populateTimelineModel(TimelineItem item);
    //----------------------------------------------------------------------------

    void fillModelIdMap();
    void constructTimeline(QString timelineId);
    void constructTimelineForItem(ModelNode itemParallelAnimation);
    void constructTimelineForItemProperty(ModelNode itemSequentialAnimation);
    void fetchTimelineIds();
    QStringList fetchAvailableItemIds();

    PropertyKeyframePair *constructKeyframe(TimelineItem *item, ModelNode modelNode, ModelNode animationNode, int startTime);

    QList<ModelNode> acceptedModelNodeChildren(const ModelNode &parentNode);

  private slots:
    void reloadConnections();
    void setTimeline(QString timelineId);
  };
}
