#include "timelinemodel.h"

#include <QAbstractItemModel>
#include <QDebug>
namespace QmlDesigner {
  PropertyKeyframePair::PropertyKeyframePair(QObject *parent) : QObject(parent) {

  }

  PropertyKeyframePair::PropertyKeyframePair(const QString &property, const int &startTime, const int &duration, const QVariant &startValue, const QVariant &endValue, QObject *parent)
    : QObject(parent),
      m_property(property),
      m_startTime(startTime), 
      m_duration(duration),
      m_startValue(startValue),
      m_endValue(endValue)
  {  
  }

  QString PropertyKeyframePair::propertyName() const {
      return m_property;
  }

  int PropertyKeyframePair::startTime() const {
    return m_startTime;
  }

  int PropertyKeyframePair::duration() const {
    return m_duration;
  }

  QVariant PropertyKeyframePair::endValue() const {
      return m_endValue;
  }

  TimelineItem::TimelineItem(const QString &name, const QString &iconPath, const int &depth)
    : m_name(name),
      m_iconPath(iconPath),
      m_depth(depth),
      m_propertyMap(QMap<QString,QList<QObject*>>()),
      m_keyframes(QList<QObject*>()),
      m_children(QList<TimelineItem>())
  {
  }

  QString TimelineItem::name() const {
    return m_name;
  }

  QString TimelineItem::iconPath() const {
    return m_iconPath;
  }

  int TimelineItem::depth() const {
    return m_depth;
  }

  QList<QObject*> TimelineItem::keyframes() const {
    return m_keyframes;
  }

  QStringList TimelineItem::properties() const {
      return m_propertyMap.uniqueKeys();
  }

  QList<TimelineItem> *TimelineItem::children() {
    return &m_children;
  }

  QMap<QString,QList<QObject*>> TimelineItem::propertyMap() const {
    return m_propertyMap;
  }

  void TimelineItem::addKeyframe(PropertyKeyframePair *keyframe) {
    m_keyframes.append(keyframe);

    QString property = keyframe->propertyName();
    if (!m_propertyMap.contains(property))
        m_propertyMap.insert(property,QList<QObject*>());
    m_propertyMap[property].append(keyframe);
  }

  void TimelineItem::addChild(TimelineItem child) {
      m_children.append(child);
  }

  TimelineModel::TimelineModel(QObject *parent)
    : QAbstractListModel(parent)
  {  
  }
  
  void TimelineModel::addItem(const TimelineItem &item) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_items << item;
    endInsertRows();
  }

  int TimelineModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_items.count();
  }

  QVariant TimelineModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= m_items.count())
      return QVariant();
    const TimelineItem &item = m_items[index.row()];
    if (role == NameRole)
      return item.name();
    else if (role == IconPathRole)
      return item.iconPath();
    else if (role == DepthRole)
      return item.depth();
    else if (role == PropertyRole)
      return QVariant::fromValue(item.properties());
    else if (role == KeyframeRole)
      return QVariant::fromValue(item.keyframes());
    return QVariant();
  }

  QHash<int, QByteArray> TimelineModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[IconPathRole] = "iconPath";
    roles[DepthRole] = "deep";
    roles[PropertyRole] = "properties";
    roles[KeyframeRole] = "keyframes";
    return roles;
  }
}
