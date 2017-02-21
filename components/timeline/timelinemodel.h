#pragma once

#include <QString>
#include <QAbstractItemModel>

#include "modelnode.h"

namespace QmlDesigner {
  class PropertyKeyframePair: public QObject {
    Q_OBJECT

    Q_PROPERTY(int startTime READ startTime NOTIFY startTimeChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)

  public:
    explicit PropertyKeyframePair(QObject *parent=0);
    PropertyKeyframePair(const QString &property, const int &startTime, const int &duration, const QVariant &startValue, const QVariant &endValue, QObject *parent=0);

    QString propertyName() const;
    int startTime() const ;
    int duration() const ;
    QVariant endValue() const ;

  private:
    QString m_property;
    int m_startTime;
    int m_duration;
    QVariant m_startValue;
    QVariant m_endValue;

  signals:
    void startTimeChanged();
    void durationChanged();
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
      IconPathRole,
      PropertyRole,
      KeyframeRole,
    };

    TimelineModel(QObject *parent = 0);
    void addItem(const TimelineItem &item);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  protected:
    QHash<int, QByteArray> roleNames() const;
  private:
    QList<TimelineItem> m_items;
  };
}
