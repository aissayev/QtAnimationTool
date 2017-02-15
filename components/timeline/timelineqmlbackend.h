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
  public:
    TimelineQmlBackend(TimelineView *timelineView);

    TimelineModel *model();
    TimelineWidget *widget();
    QQmlContext *context();

    void setupModel();
    void destroyModel();
    void emitSelectionChanged();
    void emitValueChanged();

  protected:
    static QString getNodeIconUrl(ModelNode modelNode);

  private:
    QPointer<TimelineModel> m_model;
    QPointer<TimelineWidget> m_widget;
    QPointer<TimelineView> m_timelineView;
    ModelNode m_rootModelNode;

    void loadKeyframes(TimelineItem *data, ModelNode node);
    int loadKeyframesHelper(TimelineItem *data, ModelNode parentNode, ModelNode node, QString name, int startTime);
    PropertyKeyframePair *buildKeyframe(TimelineItem *data, ModelNode parentNode, ModelNode node, int startTime);
    QVariant extractVariantProperty(AbstractProperty property) const;
    QVariant extractValueAtTime(QList<QObject*> keyframes, int startTime) const;

    TimelineItem buildItemTree(ModelNode parent, int depth);
    void populateTimelineModel(TimelineItem item);
  };
}
