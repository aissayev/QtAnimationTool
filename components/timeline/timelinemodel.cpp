#include "timelinemodel.h"

#include <QAbstractItemModel>
#include <QDebug>
namespace QmlDesigner {

  // -----------------------------------------------------------------------------
  // Keyframe
  // -----------------------------------------------------------------------------

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

  void PropertyKeyframePair::setStartTime(int startTime) {
      m_startTime = startTime;
      emit startTimeChanged();
  }

  int PropertyKeyframePair::duration() const {
    return m_duration;
  }

  void PropertyKeyframePair::setDuration(int duration) {
      m_duration = duration;
      qDebug() << "[" << m_property << "] duration changed to " << duration;
      emit durationChanged();
  }

  QVariant PropertyKeyframePair::startValue() const {
      return m_startValue;
  }

  void PropertyKeyframePair::setStartValue(QVariant startValue) {
      m_startValue = startValue;
  }

  QVariant PropertyKeyframePair::endValue() const {
      return m_endValue;
  }

  void PropertyKeyframePair::setEndValue(QVariant endValue) {
      m_endValue = endValue;
  }

  // -----------------------------------------------------------------------------
  // Timeline Item
  // -----------------------------------------------------------------------------

  TimelineItem::TimelineItem()
    : m_name(""),
      m_id(""),
      m_iconPath(""),
      m_propertyMap(QMap<QString,QList<QObject*>>()),
      m_keyframes(QList<QObject*>())
  {
  }

  TimelineItem::TimelineItem(const QString &name, const QString &id, const QString &iconPath)
    : m_name(name),
      m_id(id),
      m_iconPath(iconPath),
      m_propertyMap(QMap<QString,QList<QObject*>>()),
      m_keyframes(QList<QObject*>())
  {
  }

  QString TimelineItem::name() const {
    return m_name;
  }

  QString TimelineItem::id() const {
      return m_id;
  }

  QString TimelineItem::iconPath() const {
    return m_iconPath;
  }

  QVariantList TimelineItem::sortedKeyframes() const {
      QVariantList list;
      foreach(QString property, m_propertyMap.uniqueKeys()) {
          list.append(QVariant::fromValue(m_propertyMap[property]));
      }
      qDebug() << list.length();
      return list;
  }

  QList<QObject*> TimelineItem::keyframes() const {
    return m_keyframes;
  }

  QStringList TimelineItem::properties() const {
      return m_propertyMap.uniqueKeys();
  }

  QMap<QString,QList<QObject*>> TimelineItem::propertyMap() const {
    return m_propertyMap;
  }

  void TimelineItem::addKeyframe(PropertyKeyframePair *keyframe) {
    m_keyframes.append(keyframe);

    QString property = keyframe->propertyName();
    if (!m_propertyMap.contains(property))
        m_propertyMap.insert(property,QList<QObject*>());

    int i;
    for(i=0; i<m_propertyMap[property].size(); i++) {
        PropertyKeyframePair *iKeyframe = (PropertyKeyframePair *)(m_propertyMap[property][i]);
        if(keyframe->startTime() < iKeyframe->startTime())
            break;
    }
    m_propertyMap[property].insert(i,keyframe);
  }

  // -----------------------------------------------------------------------------
  // Timeline Model
  // -----------------------------------------------------------------------------

  TimelineModel::TimelineModel(QObject *parent)
    : QAbstractListModel(parent)
  {  
  }
  
  void TimelineModel::addItem(const TimelineItem &item) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_items << item;
    endInsertRows();
  }

  void TimelineModel::reset() {
      beginRemoveRows(QModelIndex(), 0, rowCount());
      m_items.clear();
      endRemoveRows();
  }

  int TimelineModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_items.count();
  }

  QList<TimelineItem> TimelineModel::items() {
      return m_items;
  }

  TimelineItem *TimelineModel::getItemById(QString itemId) {
    int i;
    for(i=0; i<m_items.count(); i++) {
        if(m_items[i].id() == itemId)
            return &m_items[i];
    }
    return NULL;
  }

  void TimelineModel::updateQmlTimelineItem(QString itemId) {
      int i;
      for(i=0; i<m_items.count(); i++) {
          if(m_items[i].id() == itemId){
              emit dataChanged(index(i,0),index(i,0));
              return;
          }
      }
  }

  QVariant TimelineModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= m_items.count())
      return QVariant();
    const TimelineItem &item = m_items[index.row()];
    if (role == NameRole)
      return item.name();
    else if (role == IdRole)
        return item.id();
    else if (role == IconPathRole)
      return item.iconPath();
    else if (role == PropertyRole)
      return QVariant::fromValue(item.properties());
    else if (role == KeyframeRole)
      return QVariant::fromValue(item.keyframes());
    else if (role == SortedKeyframeRole)
        return item.sortedKeyframes();
    return QVariant();
  }

  QHash<int, QByteArray> TimelineModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[IdRole] = "id";
    roles[IconPathRole] = "iconPath";
    roles[PropertyRole] = "properties";
    roles[KeyframeRole] = "keyframes";
    roles[SortedKeyframeRole] = "sortedKeyframes";
    return roles;
  }
}
