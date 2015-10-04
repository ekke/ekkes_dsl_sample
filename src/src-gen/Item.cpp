#include "Item.hpp"
#include <QDebug>
#include <quuid.h>
#include "Order.hpp"

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString posNrKey = "posNr";
static const QString nameKey = "name";
static const QString quantityKey = "quantity";
// no key for order
static const QString subItemsKey = "subItems";

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString posNrForeignKey = "posNr";
static const QString nameForeignKey = "name";
static const QString quantityForeignKey = "quantity";
// no key for order
static const QString subItemsForeignKey = "subItems";

/*
 * Default Constructor if Item not initialized from QVariantMap
 */
Item::Item(QObject *parent) :
        QObject(parent), mUuid(""), mPosNr(-1), mName(""), mQuantity(-1.0)
{
}

/*
 * initialize Item from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Item::fillFromMap(const QVariantMap& itemMap)
{
	mUuid = itemMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mPosNr = itemMap.value(posNrKey).toInt();
	mName = itemMap.value(nameKey).toString();
	mQuantity = itemMap.value(quantityKey).toDouble();
	// mOrder is parent (Order* containing Item)
	// mSubItems is List of SubItem*
	QVariantList subItemsList;
	subItemsList = itemMap.value(subItemsKey).toList();
	mSubItems.clear();
	for (int i = 0; i < subItemsList.size(); ++i) {
		QVariantMap subItemsMap;
		subItemsMap = subItemsList.at(i).toMap();
		SubItem* subItem = new SubItem();
		subItem->setParent(this);
		subItem->fillFromMap(subItemsMap);
		mSubItems.append(subItem);
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Item::fillFromForeignMap(const QVariantMap& itemMap)
{
	mUuid = itemMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mPosNr = itemMap.value(posNrForeignKey).toInt();
	mName = itemMap.value(nameForeignKey).toString();
	mQuantity = itemMap.value(quantityForeignKey).toDouble();
	// mOrder is parent (Order* containing Item)
	// mSubItems is List of SubItem*
	QVariantList subItemsList;
	subItemsList = itemMap.value(subItemsForeignKey).toList();
	mSubItems.clear();
	for (int i = 0; i < subItemsList.size(); ++i) {
		QVariantMap subItemsMap;
		subItemsMap = subItemsList.at(i).toMap();
		SubItem* subItem = new SubItem();
		subItem->setParent(this);
		subItem->fillFromForeignMap(subItemsMap);
		mSubItems.append(subItem);
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Item::fillFromCacheMap(const QVariantMap& itemMap)
{
	mUuid = itemMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mPosNr = itemMap.value(posNrKey).toInt();
	mName = itemMap.value(nameKey).toString();
	mQuantity = itemMap.value(quantityKey).toDouble();
	// mOrder is parent (Order* containing Item)
	// mSubItems is List of SubItem*
	QVariantList subItemsList;
	subItemsList = itemMap.value(subItemsKey).toList();
	mSubItems.clear();
	for (int i = 0; i < subItemsList.size(); ++i) {
		QVariantMap subItemsMap;
		subItemsMap = subItemsList.at(i).toMap();
		SubItem* subItem = new SubItem();
		subItem->setParent(this);
		subItem->fillFromCacheMap(subItemsMap);
		mSubItems.append(subItem);
	}
}

void Item::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Item::isValid()
{
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	if (mPosNr == -1) {
		return false;
	}
	if (mName.isNull() || mName.isEmpty()) {
		return false;
	}
	if (mQuantity == -1.0) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Item as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Item::toMap()
{
	QVariantMap itemMap;
	itemMap.insert(uuidKey, mUuid);
	itemMap.insert(posNrKey, mPosNr);
	itemMap.insert(nameKey, mName);
	itemMap.insert(quantityKey, mQuantity);
	// mOrder points to Order* containing Item
	// mSubItems points to SubItem*
	itemMap.insert(subItemsKey, subItemsAsQVariantList());
	return itemMap;
}

/*
 * Exports Properties from Item as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Item::toForeignMap()
{
	QVariantMap itemMap;
	itemMap.insert(uuidForeignKey, mUuid);
	itemMap.insert(posNrForeignKey, mPosNr);
	itemMap.insert(nameForeignKey, mName);
	itemMap.insert(quantityForeignKey, mQuantity);
	// mOrder points to Order* containing Item
	// mSubItems points to SubItem*
	itemMap.insert(subItemsForeignKey, subItemsAsQVariantList());
	return itemMap;
}


/*
 * Exports Properties from Item as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Item::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Mandatory: uuid
// Domain KEY: uuid
QString Item::uuid() const
{
	return mUuid;
}

void Item::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Mandatory: posNr
int Item::posNr() const
{
	return mPosNr;
}

void Item::setPosNr(int posNr)
{
	if (posNr != mPosNr) {
		mPosNr = posNr;
		emit posNrChanged(posNr);
	}
}
// ATT 
// Mandatory: name
QString Item::name() const
{
	return mName;
}

void Item::setName(QString name)
{
	if (name != mName) {
		mName = name;
		emit nameChanged(name);
	}
}
// ATT 
// Mandatory: quantity
double Item::quantity() const
{
	return mQuantity;
}

void Item::setQuantity(double quantity)
{
	if (quantity != mQuantity) {
		mQuantity = quantity;
		emit quantityChanged(quantity);
	}
}
// REF
// Opposite: positions
// Optional: order
// No SETTER for Order - it's the parent
Order* Item::order() const
{
	return qobject_cast<Order*>(parent());
}
// REF
// Opposite: item
// Optional: subItems
QVariantList Item::subItemsAsQVariantList()
{
	QVariantList subItemsList;
	for (int i = 0; i < mSubItems.size(); ++i) {
        subItemsList.append((mSubItems.at(i))->toMap());
    }
	return subItemsList;
}
/**
 * creates a new SubItem
 * parent is this Item
 * if data is successfully entered you must INVOKE addToSubItems()
 * if edit was canceled you must undoCreateElementOfSubItems to free up memory
 */
SubItem* Item::createElementOfSubItems()
{
    SubItem* subItem;
    subItem = new SubItem();
    subItem->setParent(this);
    subItem->prepareNew();
    return subItem;
}

/**
 * if createElementOfSubItems was canceled from UI
 * this method deletes the Object of Type SubItem
 * 
 * to delete a allready into subItems inserted  SubItem
 * you must use removeFromSubItems
 */
void Item::undoCreateElementOfSubItems(SubItem* subItem)
{
    if (subItem) {
        subItem->deleteLater();
        subItem = 0;
    }
}
void Item::addToSubItems(SubItem* subItem)
{
    mSubItems.append(subItem);
    emit addedToSubItems(subItem);
}

bool Item::removeFromSubItems(SubItem* subItem)
{
    bool ok = false;
    ok = mSubItems.removeOne(subItem);
    if (!ok) {
    	qDebug() << "SubItem* not found in subItems";
    	return false;
    }
    emit removedFromSubItemsByUuid(subItem->uuid());
    // subItems are contained - so we must delete them
    subItem->deleteLater();
    subItem = 0;
    return true;
}
void Item::clearSubItems()
{
    for (int i = mSubItems.size(); i > 0; --i) {
        removeFromSubItems(mSubItems.last());
    }
}
void Item::addToSubItemsFromMap(const QVariantMap& subItemMap)
{
    SubItem* subItem = new SubItem();
    subItem->setParent(this);
    subItem->fillFromMap(subItemMap);
    mSubItems.append(subItem);
    emit addedToSubItems(subItem);
}
bool Item::removeFromSubItemsByUuid(const QString& uuid)
{
    for (int i = 0; i < mSubItems.size(); ++i) {
    	SubItem* subItem;
        subItem = mSubItems.at(i);
        if (subItem->uuid() == uuid) {
        	mSubItems.removeAt(i);
        	emit removedFromSubItemsByUuid(uuid);
        	// subItems are contained - so we must delete them
        	subItem->deleteLater();
        	subItem = 0;
        	return true;
        }
    }
    qDebug() << "uuid not found in subItems: " << uuid;
    return false;
}

int Item::subItemsCount()
{
    return mSubItems.size();
}
QList<SubItem*> Item::subItems()
{
	return mSubItems;
}
void Item::setSubItems(QList<SubItem*> subItems) 
{
	if (subItems != mSubItems) {
		mSubItems = subItems;
		emit subItemsChanged(subItems);
	}
}
/**
 * to access lists from QML we're using QDeclarativeListProperty
 * and implement methods to append, count and clear
 * now from QML we can use
 * item.subItemsPropertyList.length to get the size
 * item.subItemsPropertyList[2] to get SubItem* at position 2
 * item.subItemsPropertyList = [] to clear the list
 * or get easy access to properties like
 * item.subItemsPropertyList[2].myPropertyName
 */
QDeclarativeListProperty<SubItem> Item::subItemsPropertyList()
{
    return QDeclarativeListProperty<SubItem>(this, 0, &Item::appendToSubItemsProperty,
            &Item::subItemsPropertyCount, &Item::atSubItemsProperty,
            &Item::clearSubItemsProperty);
}
void Item::appendToSubItemsProperty(QDeclarativeListProperty<SubItem> *subItemsList,
        SubItem* subItem)
{
    Item *itemObject = qobject_cast<Item *>(subItemsList->object);
    if (itemObject) {
		subItem->setParent(itemObject);
        itemObject->mSubItems.append(subItem);
        emit itemObject->addedToSubItems(subItem);
    } else {
        qWarning() << "cannot append SubItem* to subItems " << "Object is not of type Item*";
    }
}
int Item::subItemsPropertyCount(QDeclarativeListProperty<SubItem> *subItemsList)
{
    Item *item = qobject_cast<Item *>(subItemsList->object);
    if (item) {
        return item->mSubItems.size();
    } else {
        qWarning() << "cannot get size subItems " << "Object is not of type Item*";
    }
    return 0;
}
SubItem* Item::atSubItemsProperty(QDeclarativeListProperty<SubItem> *subItemsList, int pos)
{
    Item *item = qobject_cast<Item *>(subItemsList->object);
    if (item) {
        if (item->mSubItems.size() > pos) {
            return item->mSubItems.at(pos);
        }
        qWarning() << "cannot get SubItem* at pos " << pos << " size is "
                << item->mSubItems.size();
    } else {
        qWarning() << "cannot get SubItem* at pos " << pos << "Object is not of type Item*";
    }
    return 0;
}
void Item::clearSubItemsProperty(QDeclarativeListProperty<SubItem> *subItemsList)
{
    Item *item = qobject_cast<Item *>(subItemsList->object);
    if (item) {
        // subItems are contained - so we must delete them
        for (int i = 0; i < item->mSubItems.size(); ++i) {
            item->mSubItems.at(i)->deleteLater();
        }
        item->mSubItems.clear();
    } else {
        qWarning() << "cannot clear subItems " << "Object is not of type Item*";
    }
}


Item::~Item()
{
	// place cleanUp code here
}
	
