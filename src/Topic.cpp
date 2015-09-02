#include "Topic.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString idKey = "id";
static const QString classificationKey = "classification";
static const QString subTopicKey = "subTopic";
static const QString parentTopicKey = "parentTopic";

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString idForeignKey = "id";
static const QString classificationForeignKey = "classification";
static const QString subTopicForeignKey = "subTopic";
static const QString parentTopicForeignKey = "parentTopic";

/*
 * Default Constructor if Topic not initialized from QVariantMap
 */
Topic::Topic(QObject *parent) :
        QObject(parent), mUuid(""), mId(-1), mClassification("")
{
	// set Types of DataObject* to NULL:
	mParentTopic = 0;
}

/*
 * initialize Topic from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Topic::fillFromMap(const QVariantMap& topicMap)
{
	mUuid = topicMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mId = topicMap.value(idKey).toInt();
	mClassification = topicMap.value(classificationKey).toString();
	// mParentTopic points to Topic*
	if (topicMap.contains(parentTopicKey)) {
		QVariantMap parentTopicMap;
		parentTopicMap = topicMap.value(parentTopicKey).toMap();
		if (!parentTopicMap.isEmpty()) {
			mParentTopic = 0;
			mParentTopic = new Topic();
			mParentTopic->setParent(this);
			mParentTopic->fillFromMap(parentTopicMap);
		}
	}
	// mSubTopic is List of Topic*
	QVariantList subTopicList;
	subTopicList = topicMap.value(subTopicKey).toList();
	mSubTopic.clear();
	for (int i = 0; i < subTopicList.size(); ++i) {
		QVariantMap subTopicMap;
		subTopicMap = subTopicList.at(i).toMap();
		Topic* topic = new Topic();
		topic->setParent(this);
		topic->fillFromMap(subTopicMap);
		mSubTopic.append(topic);
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Topic::fillFromForeignMap(const QVariantMap& topicMap)
{
	mUuid = topicMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mId = topicMap.value(idForeignKey).toInt();
	mClassification = topicMap.value(classificationForeignKey).toString();
	// mParentTopic points to Topic*
	if (topicMap.contains(parentTopicForeignKey)) {
		QVariantMap parentTopicMap;
		parentTopicMap = topicMap.value(parentTopicForeignKey).toMap();
		if (!parentTopicMap.isEmpty()) {
			mParentTopic = 0;
			mParentTopic = new Topic();
			mParentTopic->setParent(this);
			mParentTopic->fillFromForeignMap(parentTopicMap);
		}
	}
	// mSubTopic is List of Topic*
	QVariantList subTopicList;
	subTopicList = topicMap.value(subTopicForeignKey).toList();
	mSubTopic.clear();
	for (int i = 0; i < subTopicList.size(); ++i) {
		QVariantMap subTopicMap;
		subTopicMap = subTopicList.at(i).toMap();
		Topic* topic = new Topic();
		topic->setParent(this);
		topic->fillFromForeignMap(subTopicMap);
		mSubTopic.append(topic);
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Topic::fillFromCacheMap(const QVariantMap& topicMap)
{
	mUuid = topicMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mId = topicMap.value(idKey).toInt();
	mClassification = topicMap.value(classificationKey).toString();
	// mParentTopic points to Topic*
	if (topicMap.contains(parentTopicKey)) {
		QVariantMap parentTopicMap;
		parentTopicMap = topicMap.value(parentTopicKey).toMap();
		if (!parentTopicMap.isEmpty()) {
			mParentTopic = new Topic();
			mParentTopic->setParent(this);
			mParentTopic->fillFromCacheMap(parentTopicMap);
		}
	}
	// mSubTopic is List of Topic*
	QVariantList subTopicList;
	subTopicList = topicMap.value(subTopicKey).toList();
	mSubTopic.clear();
	for (int i = 0; i < subTopicList.size(); ++i) {
		QVariantMap subTopicMap;
		subTopicMap = subTopicList.at(i).toMap();
		Topic* topic = new Topic();
		topic->setParent(this);
		topic->fillFromCacheMap(subTopicMap);
		mSubTopic.append(topic);
	}
}

void Topic::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Topic::isValid()
{
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	if (mId == -1) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Topic as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Topic::toMap()
{
	QVariantMap topicMap;
	topicMap.insert(uuidKey, mUuid);
	topicMap.insert(idKey, mId);
	topicMap.insert(classificationKey, mClassification);
	// mSubTopic points to Topic*
	topicMap.insert(subTopicKey, subTopicAsQVariantList());
	// mParentTopic points to Topic*
	if (mParentTopic) {
	topicMap.insert(parentTopicKey, mParentTopic->toMap());
	}
	return topicMap;
}

/*
 * Exports Properties from Topic as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Topic::toForeignMap()
{
	QVariantMap topicMap;
	topicMap.insert(uuidForeignKey, mUuid);
	topicMap.insert(idForeignKey, mId);
	topicMap.insert(classificationForeignKey, mClassification);
	// mSubTopic points to Topic*
	topicMap.insert(subTopicForeignKey, subTopicAsQVariantList());
	// mParentTopic points to Topic*
	if (mParentTopic) {
		topicMap.insert(parentTopicForeignKey, mParentTopic->toMap());
	}
	return topicMap;
}


/*
 * Exports Properties from Topic as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Topic::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Optional: uuid
QString Topic::uuid() const
{
	return mUuid;
}

void Topic::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Mandatory: id
// Domain KEY: id
int Topic::id() const
{
	return mId;
}

void Topic::setId(int id)
{
	if (id != mId) {
		mId = id;
		emit idChanged(id);
	}
}
// ATT 
// Optional: classification
QString Topic::classification() const
{
	return mClassification;
}

void Topic::setClassification(QString classification)
{
	if (classification != mClassification) {
		mClassification = classification;
		emit classificationChanged(classification);
	}
}
// REF
// Optional: parentTopic
Topic* Topic::parentTopic() const
{
	return mParentTopic;
}
/**
 * creates a new Topic
 * parent is this Topic
 * if data is successfully entered you must INVOKE setParentTopic()
 * if edit was canceled you must undoCreateParentTopic to free up memory
 */
Topic* Topic::createParentTopic()
{
    Topic* topic;
    topic = new Topic();
    topic->setParent(this);
    topic->prepareNew();
    return topic;
}

/**
 * if createParentTopic was canceled from UI
 * this method deletes the Object of Type Topic
 * 
 * to delete a  parentTopic allready set to  Topic
 * you must use deleteParentTopic
 */
void Topic::undoCreateParentTopic(Topic* topic)
{
    if (topic) {
        topic->deleteLater();
        topic = 0;
    }
}

void Topic::setParentTopic(Topic* parentTopic)
{
	if (!parentTopic) {
	    return;
	}
	if (parentTopic != mParentTopic) {
		if (mParentTopic) {
			mParentTopic->deleteLater();
		}
		mParentTopic = parentTopic;
		mParentTopic->setParent(this);
		emit parentTopicChanged(parentTopic);
	}
}
void Topic::deleteParentTopic()
{
	if (mParentTopic) {
		emit parentTopicDeleted(mParentTopic->uuid());
		mParentTopic->deleteLater();
		mParentTopic = 0;
	}
}
bool Topic::hasParentTopic()
{
	if (mParentTopic) {
        return true;
    } else {
        return false;
    }
}
// REF
// Optional: subTopic
QVariantList Topic::subTopicAsQVariantList()
{
	QVariantList subTopicList;
	for (int i = 0; i < mSubTopic.size(); ++i) {
        subTopicList.append((mSubTopic.at(i))->toMap());
    }
	return subTopicList;
}
/**
 * creates a new Topic
 * parent is this Topic
 * if data is successfully entered you must INVOKE addToSubTopic()
 * if edit was canceled you must undoCreateElementOfSubTopic to free up memory
 */
Topic* Topic::createElementOfSubTopic()
{
    Topic* topic;
    topic = new Topic();
    topic->setParent(this);
    topic->prepareNew();
    return topic;
}

/**
 * if createElementOfSubTopic was canceled from UI
 * this method deletes the Object of Type Topic
 * 
 * to delete a allready into subTopic inserted  Topic
 * you must use removeFromSubTopic
 */
void Topic::undoCreateElementOfSubTopic(Topic* topic)
{
    if (topic) {
        topic->deleteLater();
        topic = 0;
    }
}
void Topic::addToSubTopic(Topic* topic)
{
    mSubTopic.append(topic);
    emit addedToSubTopic(topic);
}

bool Topic::removeFromSubTopic(Topic* topic)
{
    bool ok = false;
    ok = mSubTopic.removeOne(topic);
    if (!ok) {
    	qDebug() << "Topic* not found in subTopic";
    	return false;
    }
    emit removedFromSubTopicByUuid(topic->uuid());
    // subTopic are independent - DON'T delete them
    return true;
}
void Topic::clearSubTopic()
{
    for (int i = mSubTopic.size(); i > 0; --i) {
        removeFromSubTopic(mSubTopic.last());
    }
}
void Topic::addToSubTopicFromMap(const QVariantMap& topicMap)
{
    Topic* topic = new Topic();
    topic->setParent(this);
    topic->fillFromMap(topicMap);
    mSubTopic.append(topic);
    emit addedToSubTopic(topic);
}
bool Topic::removeFromSubTopicByUuid(const QString& uuid)
{
    for (int i = 0; i < mSubTopic.size(); ++i) {
    	Topic* topic;
        topic = mSubTopic.at(i);
        if (topic->uuid() == uuid) {
        	mSubTopic.removeAt(i);
        	emit removedFromSubTopicByUuid(uuid);
        	// subTopic are independent - DON'T delete them
        	return true;
        }
    }
    qDebug() << "uuid not found in subTopic: " << uuid;
    return false;
}

bool Topic::removeFromSubTopicById(const int& id)
{
    for (int i = 0; i < mSubTopic.size(); ++i) {
    	Topic* topic;
        topic = mSubTopic.at(i);
        if (topic->id() == id) {
        	mSubTopic.removeAt(i);
        	emit removedFromSubTopicById(id);
        	// subTopic are independent - DON'T delete them
        	return true;
        }
    }
    qDebug() << "id not found in subTopic: " << id;
    return false;
}
int Topic::subTopicCount()
{
    return mSubTopic.size();
}
QList<Topic*> Topic::subTopic()
{
	return mSubTopic;
}
void Topic::setSubTopic(QList<Topic*> subTopic) 
{
	if (subTopic != mSubTopic) {
		mSubTopic = subTopic;
		emit subTopicChanged(subTopic);
	}
}
/**
 * to access lists from QML we're using QDeclarativeListProperty
 * and implement methods to append, count and clear
 * now from QML we can use
 * topic.subTopicPropertyList.length to get the size
 * topic.subTopicPropertyList[2] to get Topic* at position 2
 * topic.subTopicPropertyList = [] to clear the list
 * or get easy access to properties like
 * topic.subTopicPropertyList[2].myPropertyName
 */
QDeclarativeListProperty<Topic> Topic::subTopicPropertyList()
{
    return QDeclarativeListProperty<Topic>(this, 0, &Topic::appendToSubTopicProperty,
            &Topic::subTopicPropertyCount, &Topic::atSubTopicProperty,
            &Topic::clearSubTopicProperty);
}
void Topic::appendToSubTopicProperty(QDeclarativeListProperty<Topic> *subTopicList,
        Topic* topic)
{
    Topic *topicObject = qobject_cast<Topic *>(subTopicList->object);
    if (topicObject) {
		topic->setParent(topicObject);
        topicObject->mSubTopic.append(topic);
        emit topicObject->addedToSubTopic(topic);
    } else {
        qWarning() << "cannot append Topic* to subTopic " << "Object is not of type Topic*";
    }
}
int Topic::subTopicPropertyCount(QDeclarativeListProperty<Topic> *subTopicList)
{
    Topic *topic = qobject_cast<Topic *>(subTopicList->object);
    if (topic) {
        return topic->mSubTopic.size();
    } else {
        qWarning() << "cannot get size subTopic " << "Object is not of type Topic*";
    }
    return 0;
}
Topic* Topic::atSubTopicProperty(QDeclarativeListProperty<Topic> *subTopicList, int pos)
{
    Topic *topic = qobject_cast<Topic *>(subTopicList->object);
    if (topic) {
        if (topic->mSubTopic.size() > pos) {
            return topic->mSubTopic.at(pos);
        }
        qWarning() << "cannot get Topic* at pos " << pos << " size is "
                << topic->mSubTopic.size();
    } else {
        qWarning() << "cannot get Topic* at pos " << pos << "Object is not of type Topic*";
    }
    return 0;
}
void Topic::clearSubTopicProperty(QDeclarativeListProperty<Topic> *subTopicList)
{
    Topic *topic = qobject_cast<Topic *>(subTopicList->object);
    if (topic) {
        // subTopic are independent - DON'T delete them
        topic->mSubTopic.clear();
    } else {
        qWarning() << "cannot clear subTopic " << "Object is not of type Topic*";
    }
}


Topic::~Topic()
{
	// place cleanUp code here
}
	
