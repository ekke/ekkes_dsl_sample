#include "Info.hpp"
#include <QDebug>
#include <quuid.h>
#include "Order.hpp"

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString remarksKey = "remarks";
// no key for order

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString remarksForeignKey = "remarks";
// no key for order

/*
 * Default Constructor if Info not initialized from QVariantMap
 */
Info::Info(QObject *parent) :
        QObject(parent), mUuid(""), mRemarks("")
{
}

/*
 * initialize Info from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Info::fillFromMap(const QVariantMap& infoMap)
{
	mUuid = infoMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mRemarks = infoMap.value(remarksKey).toString();
	// mOrder is parent (Order* containing Info)
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Info::fillFromForeignMap(const QVariantMap& infoMap)
{
	mUuid = infoMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mRemarks = infoMap.value(remarksForeignKey).toString();
	// mOrder is parent (Order* containing Info)
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Info::fillFromCacheMap(const QVariantMap& infoMap)
{
	mUuid = infoMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mRemarks = infoMap.value(remarksKey).toString();
	// mOrder is parent (Order* containing Info)
}

void Info::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Info::isValid()
{
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Info as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Info::toMap()
{
	QVariantMap infoMap;
	infoMap.insert(uuidKey, mUuid);
	infoMap.insert(remarksKey, mRemarks);
	// mOrder points to Order* containing Info
	return infoMap;
}

/*
 * Exports Properties from Info as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Info::toForeignMap()
{
	QVariantMap infoMap;
	infoMap.insert(uuidForeignKey, mUuid);
	infoMap.insert(remarksForeignKey, mRemarks);
	// mOrder points to Order* containing Info
	return infoMap;
}


/*
 * Exports Properties from Info as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Info::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Mandatory: uuid
// Domain KEY: uuid
QString Info::uuid() const
{
	return mUuid;
}

void Info::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Optional: remarks
QString Info::remarks() const
{
	return mRemarks;
}

void Info::setRemarks(QString remarks)
{
	if (remarks != mRemarks) {
		mRemarks = remarks;
		emit remarksChanged(remarks);
	}
}
// REF
// Opposite: info
// Optional: order
// No SETTER for Order - it's the parent
Order* Info::order() const
{
	return qobject_cast<Order*>(parent());
}


Info::~Info()
{
	// place cleanUp code here
}
	
