#pragma once

#include <QString>
#include <QAbstractItemModel>

#include "modelnode.h"

namespace QmlDesigner {
  class PropertyKeyframePair: public QObject {
    Q_OBJECT

    Q_PROPERTY(int startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QVariant startValue READ startValue WRITE setStartValue NOTIFY startValueChanged)
    Q_PROPERTY(QVariant endValue READ endValue WRITE setEndValue NOTIFY endValueChanged)

  public:
    explicit PropertyKeyframePair(QObject *parent=0);
    PropertyKeyframePair(const QString &property, const int &startTime, const int &duration, const QVariant &startValue, const QVariant &endValue, QObject *parent=0);

    QString propertyName() const;
    // QProperty startTime
    int startTime() const ;
    void setStartTime(int startTime);
    // QProperty duration
    int duration() const ;
    void setDuration(int duration);
    // QProperty startValue
    QVariant startValue() const ;
    void setStartValue(QVariant startValue);
    // QProperty endValue
    QVariant endValue() const ;
    void setEndValue(QVariant endValue);

  private:
    QString m_property;
    int m_startTime;
    int m_duration;
    QVariant m_startValue;
    QVariant m_endValue;

  signals:
    void startTimeChanged();
    void durationChanged();
    void startValueChanged();
    void endValueChanged();
  };

  class TimelineItem
  {
  public: 
    TimelineItem();
    TimelineItem(const QString &name, const QString &id, const QString &iconPath);

    QString name() const ;
    QString id() const;
    QString iconPath() const;

    QMap<QString,QList<QObject*>> propertyMap() const;
    QStringList properties() const ;
    QList<QObject*> keyframes() const ;
    QVariantList sortedKeyframes() const;

    void addKeyframe(PropertyKeyframePair *keyframe);

  private:
    QString m_name;
    QString m_id;
    QString m_iconPath;
    QMap<QString,QList<QObject*>> m_propertyMap;
    QList<QObject*> m_keyframes;
  };

  class TimelineModel : public QAbstractListModel
  {
    Q_OBJECT

  public:
    enum TimelineRoles {
      NameRole = Qt::UserRole +1,
      IdRole,
      IconPathRole,
      PropertyRole,
      KeyframeRole,
      SortedKeyframeRole
    };

    TimelineModel(QObject *parent = 0);
    void addItem(const TimelineItem &item);
    void reset();
    TimelineItem *getItemById(QString itemId);
    void updateQmlTimelineItem(QString itemId);
    QList<TimelineItem> items();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  protected:
    QHash<int, QByteArray> roleNames() const;
  private:
    QList<TimelineItem> m_items;
  };
}
