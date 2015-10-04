#include "SubItem.hpp"
#include <QDebug>
#include <quuid.h>
#include "Item.hpp"

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString subPosNrKey = "subPosNr";
static const QString descriptionKey = "description";
static const QString barcodeKey = "barcode";
// no key for item

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString subPosNrForeignKey = "subPosNr";
static const QString descriptionForeignKey = "description";
static const QString barcodeForeignKey = "barcode";
// no key for item

/*
 * Default Constructor if SubItem not initialized from QVariantMap
 */
SubItem::SubItem(QObject *parent) :
        QObject(parent), mUuid(""), mSubPosNr(-1), mDescription(""), mBarcode("")
{
}

/*
 * initialize SubItem from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void SubItem::fillFromMap(const QVariantMap& subItemMap)
{
	mUuid = subItemMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mSubPosNr = subItemMap.value(subPosNrKey).toInt();
	mDescription = subItemMap.value(descriptionKey).toString();
	mBarcode = subItemMap.value(barcodeKey).toString();
	// mItem is parent (Item* containing SubItem)
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void SubItem::fillFromForeignMap(const QVariantMap& subItemMap)
{
	mUuid = subItemMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mSubPosNr = subItemMap.value(subPosNrForeignKey).toInt();
	mDescription = subItemMap.value(descriptionForeignKey).toString();
	mBarcode = subItemMap.value(barcodeForeignKey).toString();
	// mItem is parent (Item* containing SubItem)
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void SubItem::fillFromCacheMap(const QVariantMap& subItemMap)
{
	mUuid = subItemMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mSubPosNr = subItemMap.value(subPosNrKey).toInt();
	mDescription = subItemMap.value(descriptionKey).toString();
	mBarcode = subItemMap.value(barcodeKey).toString();
	// mItem is parent (Item* containing SubItem)
}

void SubItem::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool SubItem::isValid()
{
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	if (mSubPosNr == -1) {
		return false;
	}
	if (mDescription.isNull() || mDescription.isEmpty()) {
		return false;
	}
	if (mBarcode.isNull() || mBarcode.isEmpty()) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from SubItem as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap SubItem::toMap()
{
	QVariantMap subItemMap;
	subItemMap.insert(uuidKey, mUuid);
	subItemMap.insert(subPosNrKey, mSubPosNr);
	subItemMap.insert(descriptionKey, mDescription);
	subItemMap.insert(barcodeKey, mBarcode);
	// mItem points to Item* containing SubItem
	return subItemMap;
}

/*
 * Exports Properties from SubItem as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap SubItem::toForeignMap()
{
	QVariantMap subItemMap;
	subItemMap.insert(uuidForeignKey, mUuid);
	subItemMap.insert(subPosNrForeignKey, mSubPosNr);
	subItemMap.insert(descriptionForeignKey, mDescription);
	subItemMap.insert(barcodeForeignKey, mBarcode);
	// mItem points to Item* containing SubItem
	return subItemMap;
}


/*
 * Exports Properties from SubItem as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap SubItem::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Mandatory: uuid
// Domain KEY: uuid
QString SubItem::uuid() const
{
	return mUuid;
}

void SubItem::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Mandatory: subPosNr
int SubItem::subPosNr() const
{
	return mSubPosNr;
}

void SubItem::setSubPosNr(int subPosNr)
{
	if (subPosNr != mSubPosNr) {
		mSubPosNr = subPosNr;
		emit subPosNrChanged(subPosNr);
	}
}
// ATT 
// Mandatory: description
QString SubItem::description() const
{
	return mDescription;
}

void SubItem::setDescription(QString description)
{
	if (description != mDescription) {
		mDescription = description;
		emit descriptionChanged(description);
	}
}
// ATT 
// Mandatory: barcode
QString SubItem::barcode() const
{
	return mBarcode;
}

void SubItem::setBarcode(QString barcode)
{
	if (barcode != mBarcode) {
		mBarcode = barcode;
		emit barcodeChanged(barcode);
	}
}
// REF
// Opposite: subItems
// Optional: item
// No SETTER for Item - it's the parent
Item* SubItem::item() const
{
	return qobject_cast<Item*>(parent());
}


SubItem::~SubItem()
{
	// place cleanUp code here
}
	
