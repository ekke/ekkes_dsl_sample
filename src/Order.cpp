#include "Order.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString nrKey = "nr";
static const QString expressOrderKey = "expressOrder";
static const QString titleKey = "title";
static const QString orderDateKey = "orderDate";
static const QString stateKey = "state";
static const QString processingStateKey = "processingState";
static const QString positionsKey = "positions";
static const QString infoKey = "info";
static const QString topicIdKey = "topicId";
static const QString extrasKey = "extras";
static const QString customerIdKey = "customerId";
static const QString depIdKey = "depId";
static const QString tagsKey = "tags";
static const QString domainsKey = "domains";

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString nrForeignKey = "nr";
static const QString expressOrderForeignKey = "expressOrder";
static const QString titleForeignKey = "title";
static const QString orderDateForeignKey = "orderDate";
static const QString stateForeignKey = "STATUS";
static const QString processingStateForeignKey = "processingState";
static const QString positionsForeignKey = "positions";
static const QString infoForeignKey = "info";
static const QString topicIdForeignKey = "topicId";
static const QString extrasForeignKey = "extras";
static const QString customerIdForeignKey = "customerId";
static const QString depIdForeignKey = "depId";
static const QString tagsForeignKey = "tags";
static const QString domainsForeignKey = "domains";

/*
 * Default Constructor if Order not initialized from QVariantMap
 */
Order::Order(QObject *parent) :
        QObject(parent), mUuid(""), mNr(-1), mExpressOrder(false), mTitle(""), mProcessingState(0)
{
	// set Types of DataObject* to NULL:
	mInfo = 0;
	// lazy references:
	mTopicId = -1;
	mTopicIdAsDataObject = 0;
	mTopicIdInvalid = false;
	mExtras = -1;
	mExtrasAsDataObject = 0;
	mExtrasInvalid = false;
	mCustomerId = -1;
	mCustomerIdAsDataObject = 0;
	mCustomerIdInvalid = false;
	mDepId = -1;
	mDepIdAsDataObject = 0;
	mDepIdInvalid = false;
	// ENUMs:
	mState = OrderState::DEFAULT_VALUE;
	// Date, Time or Timestamp ? construct null value
	mOrderDate = QDate();
	// transient values (not cached)
	// int mProcessingState
		// lazy Arrays where only keys are persisted
		mTagsKeysResolved = false;
}

bool Order::isAllResolved()
{
	if (hasTopicId() && !isTopicIdResolvedAsDataObject()) {
		return false;
	}
	if (hasExtras() && !isExtrasResolvedAsDataObject()) {
		return false;
	}
	if (hasCustomerId() && !isCustomerIdResolvedAsDataObject()) {
		return false;
	}
	if (hasDepId() && !isDepIdResolvedAsDataObject()) {
		return false;
	}
    if(!areTagsKeysResolved()) {
        return false;
    }
    return true;
}

/*
 * initialize Order from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Order::fillFromMap(const QVariantMap& orderMap)
{
	mUuid = orderMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mNr = orderMap.value(nrKey).toInt();
	mExpressOrder = orderMap.value(expressOrderKey).toBool();
	mTitle = orderMap.value(titleKey).toString();
	if (orderMap.contains(orderDateKey)) {
		// always getting the Date as a String (from server or JSON)
		QString orderDateAsString = orderMap.value(orderDateKey).toString();
		mOrderDate = QDate::fromString(orderDateAsString, "yyyy-MM-dd");
		if (!mOrderDate.isValid()) {
			mOrderDate = QDate();
			qDebug() << "mOrderDate is not valid for String: " << orderDateAsString;
		}
	}
	// ENUM
	if (orderMap.contains(stateKey)) {
		bool* ok;
		ok = false;
		orderMap.value(stateKey).toInt(ok);
		if (ok) {
			mState = orderMap.value(stateKey).toInt();
		} else {
			mState = stateStringToInt(orderMap.value(stateKey).toString());
		}
	} else {
		mState = OrderState::NO_VALUE;
	}
	// mProcessingState is transient
	if (orderMap.contains(processingStateKey)) {
		mProcessingState = orderMap.value(processingStateKey).toInt();
	}
	// mInfo points to Info*
	if (orderMap.contains(infoKey)) {
		QVariantMap infoMap;
		infoMap = orderMap.value(infoKey).toMap();
		if (!infoMap.isEmpty()) {
			mInfo = 0;
			mInfo = new Info();
			mInfo->setParent(this);
			mInfo->fillFromMap(infoMap);
		}
	}
	// topicId lazy pointing to Topic* (domainKey: id)
	if (orderMap.contains(topicIdKey)) {
		mTopicId = orderMap.value(topicIdKey).toInt();
		if (mTopicId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// extras lazy pointing to Xtras* (domainKey: id)
	if (orderMap.contains(extrasKey)) {
		mExtras = orderMap.value(extrasKey).toInt();
		if (mExtras != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// customerId lazy pointing to Customer* (domainKey: id)
	if (orderMap.contains(customerIdKey)) {
		mCustomerId = orderMap.value(customerIdKey).toInt();
		if (mCustomerId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// depId lazy pointing to Department* (domainKey: id)
	if (orderMap.contains(depIdKey)) {
		mDepId = orderMap.value(depIdKey).toInt();
		if (mDepId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// mPositions is List of Item*
	QVariantList positionsList;
	positionsList = orderMap.value(positionsKey).toList();
	mPositions.clear();
	for (int i = 0; i < positionsList.size(); ++i) {
		QVariantMap positionsMap;
		positionsMap = positionsList.at(i).toMap();
		Item* item = new Item();
		item->setParent(this);
		item->fillFromMap(positionsMap);
		mPositions.append(item);
	}
	// mTags is (lazy loaded) Array of Tag*
	mTagsKeys = orderMap.value(tagsKey).toStringList();
	// mTags must be resolved later if there are keys
	mTagsKeysResolved = (mTagsKeys.size() == 0);
	mTags.clear();
	mDomainsStringList = orderMap.value(domainsKey).toStringList();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Order::fillFromForeignMap(const QVariantMap& orderMap)
{
	mUuid = orderMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mNr = orderMap.value(nrForeignKey).toInt();
	mExpressOrder = orderMap.value(expressOrderForeignKey).toBool();
	mTitle = orderMap.value(titleForeignKey).toString();
	if (orderMap.contains(orderDateForeignKey)) {
		// always getting the Date as a String (from server or JSON)
		QString orderDateAsString = orderMap.value(orderDateForeignKey).toString();
		mOrderDate = QDate::fromString(orderDateAsString, "yyyy-MM-dd");
		if (!mOrderDate.isValid()) {
			mOrderDate = QDate();
			qDebug() << "mOrderDate is not valid for String: " << orderDateAsString;
		}
	}
	// ENUM
	if (orderMap.contains(stateForeignKey)) {
		bool* ok;
		ok = false;
		orderMap.value(stateForeignKey).toInt(ok);
		if (ok) {
			mState = orderMap.value(stateForeignKey).toInt();
		} else {
			mState = stateStringToInt(orderMap.value(stateForeignKey).toString());
		}
	} else {
		mState = OrderState::NO_VALUE;
	}
	// mProcessingState is transient
	if (orderMap.contains(processingStateForeignKey)) {
		mProcessingState = orderMap.value(processingStateForeignKey).toInt();
	}
	// mInfo points to Info*
	if (orderMap.contains(infoForeignKey)) {
		QVariantMap infoMap;
		infoMap = orderMap.value(infoForeignKey).toMap();
		if (!infoMap.isEmpty()) {
			mInfo = 0;
			mInfo = new Info();
			mInfo->setParent(this);
			mInfo->fillFromForeignMap(infoMap);
		}
	}
	// topicId lazy pointing to Topic* (domainKey: id)
	if (orderMap.contains(topicIdForeignKey)) {
		mTopicId = orderMap.value(topicIdForeignKey).toInt();
		if (mTopicId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// extras lazy pointing to Xtras* (domainKey: id)
	if (orderMap.contains(extrasForeignKey)) {
		mExtras = orderMap.value(extrasForeignKey).toInt();
		if (mExtras != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// customerId lazy pointing to Customer* (domainKey: id)
	if (orderMap.contains(customerIdForeignKey)) {
		mCustomerId = orderMap.value(customerIdForeignKey).toInt();
		if (mCustomerId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// depId lazy pointing to Department* (domainKey: id)
	if (orderMap.contains(depIdForeignKey)) {
		mDepId = orderMap.value(depIdForeignKey).toInt();
		if (mDepId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// mPositions is List of Item*
	QVariantList positionsList;
	positionsList = orderMap.value(positionsForeignKey).toList();
	mPositions.clear();
	for (int i = 0; i < positionsList.size(); ++i) {
		QVariantMap positionsMap;
		positionsMap = positionsList.at(i).toMap();
		Item* item = new Item();
		item->setParent(this);
		item->fillFromForeignMap(positionsMap);
		mPositions.append(item);
	}
	// mTags is (lazy loaded) Array of Tag*
	mTagsKeys = orderMap.value(tagsForeignKey).toStringList();
	// mTags must be resolved later if there are keys
	mTagsKeysResolved = (mTagsKeys.size() == 0);
	mTags.clear();
	mDomainsStringList = orderMap.value(domainsForeignKey).toStringList();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Order::fillFromCacheMap(const QVariantMap& orderMap)
{
	mUuid = orderMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mNr = orderMap.value(nrKey).toInt();
	mExpressOrder = orderMap.value(expressOrderKey).toBool();
	mTitle = orderMap.value(titleKey).toString();
	if (orderMap.contains(orderDateKey)) {
		// always getting the Date as a String (from server or JSON)
		QString orderDateAsString = orderMap.value(orderDateKey).toString();
		mOrderDate = QDate::fromString(orderDateAsString, "yyyy-MM-dd");
		if (!mOrderDate.isValid()) {
			mOrderDate = QDate();
			qDebug() << "mOrderDate is not valid for String: " << orderDateAsString;
		}
	}
	// ENUM
	if (orderMap.contains(stateKey)) {
		bool* ok;
		ok = false;
		orderMap.value(stateKey).toInt(ok);
		if (ok) {
			mState = orderMap.value(stateKey).toInt();
		} else {
			mState = stateStringToInt(orderMap.value(stateKey).toString());
		}
	} else {
		mState = OrderState::NO_VALUE;
	}
	// mProcessingState is transient - don't forget to initialize
	// mInfo points to Info*
	if (orderMap.contains(infoKey)) {
		QVariantMap infoMap;
		infoMap = orderMap.value(infoKey).toMap();
		if (!infoMap.isEmpty()) {
			mInfo = new Info();
			mInfo->setParent(this);
			mInfo->fillFromCacheMap(infoMap);
		}
	}
	// topicId lazy pointing to Topic* (domainKey: id)
	if (orderMap.contains(topicIdKey)) {
		mTopicId = orderMap.value(topicIdKey).toInt();
		if (mTopicId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// extras lazy pointing to Xtras* (domainKey: id)
	if (orderMap.contains(extrasKey)) {
		mExtras = orderMap.value(extrasKey).toInt();
		if (mExtras != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// customerId lazy pointing to Customer* (domainKey: id)
	if (orderMap.contains(customerIdKey)) {
		mCustomerId = orderMap.value(customerIdKey).toInt();
		if (mCustomerId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// depId lazy pointing to Department* (domainKey: id)
	if (orderMap.contains(depIdKey)) {
		mDepId = orderMap.value(depIdKey).toInt();
		if (mDepId != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// mPositions is List of Item*
	QVariantList positionsList;
	positionsList = orderMap.value(positionsKey).toList();
	mPositions.clear();
	for (int i = 0; i < positionsList.size(); ++i) {
		QVariantMap positionsMap;
		positionsMap = positionsList.at(i).toMap();
		Item* item = new Item();
		item->setParent(this);
		item->fillFromCacheMap(positionsMap);
		mPositions.append(item);
	}
	// mTags is (lazy loaded) Array of Tag*
	mTagsKeys = orderMap.value(tagsKey).toStringList();
	// mTags must be resolved later if there are keys
	mTagsKeysResolved = (mTagsKeys.size() == 0);
	mTags.clear();
	mDomainsStringList = orderMap.value(domainsKey).toStringList();
}

void Order::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Order::isValid()
{
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	if (mNr == -1) {
		return false;
	}
	if (mTitle.isNull() || mTitle.isEmpty()) {
		return false;
	}
	if (mOrderDate.isNull() || !mOrderDate.isValid()) {
		return false;
	}
	if (mPositions.size() == 0) {
		return false;
	}
	// customerId lazy pointing to Customer* (domainKey: id)
	if (mCustomerId == -1) {
		return false;
	}
	// depId lazy pointing to Department* (domainKey: id)
	if (mDepId == -1) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Order as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Order::toMap()
{
	QVariantMap orderMap;
	// topicId lazy pointing to Topic* (domainKey: id)
	if (mTopicId != -1) {
		orderMap.insert(topicIdKey, mTopicId);
	}
	// extras lazy pointing to Xtras* (domainKey: id)
	if (mExtras != -1) {
		orderMap.insert(extrasKey, mExtras);
	}
	// customerId lazy pointing to Customer* (domainKey: id)
	if (mCustomerId != -1) {
		orderMap.insert(customerIdKey, mCustomerId);
	}
	// depId lazy pointing to Department* (domainKey: id)
	if (mDepId != -1) {
		orderMap.insert(depIdKey, mDepId);
	}
	// mTags points to Tag*
	// lazy array: persist only keys
	if(mTagsKeysResolved || (mTagsKeys.size() == 0 && mTags.size() != 0)) {
		mTagsKeys.clear();
		for (int i = 0; i < mTags.size(); ++i) {
			Tag* tag;
			tag = mTags.at(i);
			mTagsKeys << tag->uuid();
		}
	}
	orderMap.insert(tagsKey, mTagsKeys);
	orderMap.insert(uuidKey, mUuid);
	orderMap.insert(nrKey, mNr);
	orderMap.insert(expressOrderKey, mExpressOrder);
	orderMap.insert(titleKey, mTitle);
	if (hasOrderDate()) {
		orderMap.insert(orderDateKey, mOrderDate.toString("yyyy-MM-dd"));
	}
	// ENUM always as  int
	orderMap.insert(stateKey, mState);
	orderMap.insert(processingStateKey, mProcessingState);
	// mPositions points to Item*
	orderMap.insert(positionsKey, positionsAsQVariantList());
	// mInfo points to Info*
	if (mInfo) {
	orderMap.insert(infoKey, mInfo->toMap());
	}
	// Array of QString
	orderMap.insert(domainsKey, mDomainsStringList);
	return orderMap;
}

/*
 * Exports Properties from Order as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Order::toForeignMap()
{
	QVariantMap orderMap;
	// topicId lazy pointing to Topic* (domainKey: id)
	if (mTopicId != -1) {
		orderMap.insert(topicIdForeignKey, mTopicId);
	}
	// extras lazy pointing to Xtras* (domainKey: id)
	if (mExtras != -1) {
		orderMap.insert(extrasForeignKey, mExtras);
	}
	// customerId lazy pointing to Customer* (domainKey: id)
	if (mCustomerId != -1) {
		orderMap.insert(customerIdForeignKey, mCustomerId);
	}
	// depId lazy pointing to Department* (domainKey: id)
	if (mDepId != -1) {
		orderMap.insert(depIdForeignKey, mDepId);
	}
	// mTags points to Tag*
	// lazy array: persist only keys
	if(mTagsKeysResolved || (mTagsKeys.size() == 0 && mTags.size() != 0)) {
		mTagsKeys.clear();
		for (int i = 0; i < mTags.size(); ++i) {
			Tag* tag;
			tag = mTags.at(i);
			mTagsKeys << tag->uuid();
		}
	}
	orderMap.insert(tagsKey, mTagsKeys);
	orderMap.insert(uuidForeignKey, mUuid);
	orderMap.insert(nrForeignKey, mNr);
	orderMap.insert(expressOrderForeignKey, mExpressOrder);
	orderMap.insert(titleForeignKey, mTitle);
	if (hasOrderDate()) {
		orderMap.insert(orderDateForeignKey, mOrderDate.toString("yyyy-MM-dd"));
	}
	// ENUM always as  int
	orderMap.insert(stateForeignKey, mState);
	orderMap.insert(processingStateForeignKey, mProcessingState);
	// mPositions points to Item*
	orderMap.insert(positionsForeignKey, positionsAsQVariantList());
	// mInfo points to Info*
	if (mInfo) {
		orderMap.insert(infoForeignKey, mInfo->toMap());
	}
	// Array of QString
	orderMap.insert(domainsForeignKey, mDomainsStringList);
	return orderMap;
}


/*
 * Exports Properties from Order as QVariantMap
 * transient properties are excluded:
 * mProcessingState
 * To export ALL data use toMap()
 */
QVariantMap Order::toCacheMap()
{
	QVariantMap orderMap;
	// topicId lazy pointing to Topic* (domainKey: id)
	if (mTopicId != -1) {
		orderMap.insert(topicIdKey, mTopicId);
	}
	// extras lazy pointing to Xtras* (domainKey: id)
	if (mExtras != -1) {
		orderMap.insert(extrasKey, mExtras);
	}
	// customerId lazy pointing to Customer* (domainKey: id)
	if (mCustomerId != -1) {
		orderMap.insert(customerIdKey, mCustomerId);
	}
	// depId lazy pointing to Department* (domainKey: id)
	if (mDepId != -1) {
		orderMap.insert(depIdKey, mDepId);
	}
	// mTags points to Tag*
	// lazy array: persist only keys
	if(mTagsKeysResolved || (mTagsKeys.size() == 0 && mTags.size() != 0)) {
		mTagsKeys.clear();
		for (int i = 0; i < mTags.size(); ++i) {
			Tag* tag;
			tag = mTags.at(i);
			mTagsKeys << tag->uuid();
		}
	}
	orderMap.insert(tagsKey, mTagsKeys);
	orderMap.insert(uuidKey, mUuid);
	orderMap.insert(nrKey, mNr);
	orderMap.insert(expressOrderKey, mExpressOrder);
	orderMap.insert(titleKey, mTitle);
	if (hasOrderDate()) {
		orderMap.insert(orderDateKey, mOrderDate.toString("yyyy-MM-dd"));
	}
	// ENUM always as  int
	orderMap.insert(stateKey, mState);
	// mPositions points to Item*
	orderMap.insert(positionsKey, positionsAsQVariantList());
	// mInfo points to Info*
	if (mInfo) {
	orderMap.insert(infoKey, mInfo->toMap());
	}
	// Array of QString
	orderMap.insert(domainsKey, mDomainsStringList);
	// excluded: mProcessingState
	return orderMap;
}
// REF
// Lazy: topicId
// Optional: topicId
// topicId lazy pointing to Topic* (domainKey: id)
int Order::topicId() const
{
	return mTopicId;
}
Topic* Order::topicIdAsDataObject() const
{
	return mTopicIdAsDataObject;
}
void Order::setTopicId(int topicId)
{
	if (topicId != mTopicId) {
        // remove old Data Object if one was resolved
        if (mTopicIdAsDataObject) {
            // reset pointer, don't delete the independent object !
            mTopicIdAsDataObject = 0;
        }
        // set the new lazy reference
        mTopicId = topicId;
        mTopicIdInvalid = false;
        emit topicIdChanged(topicId);
        if (topicId != -1) {
            // resolve the corresponding Data Object on demand from DataManager
        }
    }
}
void Order::removeTopicId()
{
	if (mTopicId != -1) {
		setTopicId(-1);
	}
}
bool Order::hasTopicId()
{
    if (!mTopicIdInvalid && mTopicId != -1) {
        return true;
    } else {
        return false;
    }
}
bool Order::isTopicIdResolvedAsDataObject()
{
    if (!mTopicIdInvalid && mTopicIdAsDataObject) {
        return true;
    } else {
        return false;
    }
}

// lazy bound Data Object was resolved. overwrite id if different
void Order::resolveTopicIdAsDataObject(Topic* topic)
{
    if (topic) {
        if (topic->id() != mTopicId) {
            setTopicId(topic->id());
        }
        mTopicIdAsDataObject = topic;
        mTopicIdInvalid = false;
    }
}
void Order::markTopicIdAsInvalid()
{
    mTopicIdInvalid = true;
}
// REF
// Lazy: extras
// Optional: extras
// extras lazy pointing to Xtras* (domainKey: id)
int Order::extras() const
{
	return mExtras;
}
Xtras* Order::extrasAsDataObject() const
{
	return mExtrasAsDataObject;
}
void Order::setExtras(int extras)
{
	if (extras != mExtras) {
        // remove old Data Object if one was resolved
        if (mExtrasAsDataObject) {
            // reset pointer, don't delete the independent object !
            mExtrasAsDataObject = 0;
        }
        // set the new lazy reference
        mExtras = extras;
        mExtrasInvalid = false;
        emit extrasChanged(extras);
        if (extras != -1) {
            // resolve the corresponding Data Object on demand from DataManager
        }
    }
}
void Order::removeExtras()
{
	if (mExtras != -1) {
		setExtras(-1);
	}
}
bool Order::hasExtras()
{
    if (!mExtrasInvalid && mExtras != -1) {
        return true;
    } else {
        return false;
    }
}
bool Order::isExtrasResolvedAsDataObject()
{
    if (!mExtrasInvalid && mExtrasAsDataObject) {
        return true;
    } else {
        return false;
    }
}

// lazy bound Data Object was resolved. overwrite id if different
void Order::resolveExtrasAsDataObject(Xtras* xtras)
{
    if (xtras) {
        if (xtras->id() != mExtras) {
            setExtras(xtras->id());
        }
        mExtrasAsDataObject = xtras;
        mExtrasInvalid = false;
    }
}
void Order::markExtrasAsInvalid()
{
    mExtrasInvalid = true;
}
// REF
// Lazy: customerId
// Mandatory: customerId
// customerId lazy pointing to Customer* (domainKey: id)
int Order::customerId() const
{
	return mCustomerId;
}
Customer* Order::customerIdAsDataObject() const
{
	return mCustomerIdAsDataObject;
}
void Order::setCustomerId(int customerId)
{
	if (customerId != mCustomerId) {
        // remove old Data Object if one was resolved
        if (mCustomerIdAsDataObject) {
            // reset pointer, don't delete the independent object !
            mCustomerIdAsDataObject = 0;
        }
        // set the new lazy reference
        mCustomerId = customerId;
        mCustomerIdInvalid = false;
        emit customerIdChanged(customerId);
        if (customerId != -1) {
            // resolve the corresponding Data Object on demand from DataManager
        }
    }
}
void Order::removeCustomerId()
{
	if (mCustomerId != -1) {
		setCustomerId(-1);
	}
}
bool Order::hasCustomerId()
{
    if (!mCustomerIdInvalid && mCustomerId != -1) {
        return true;
    } else {
        return false;
    }
}
bool Order::isCustomerIdResolvedAsDataObject()
{
    if (!mCustomerIdInvalid && mCustomerIdAsDataObject) {
        return true;
    } else {
        return false;
    }
}

// lazy bound Data Object was resolved. overwrite id if different
void Order::resolveCustomerIdAsDataObject(Customer* customer)
{
    if (customer) {
        if (customer->id() != mCustomerId) {
            setCustomerId(customer->id());
        }
        mCustomerIdAsDataObject = customer;
        mCustomerIdInvalid = false;
    }
}
void Order::markCustomerIdAsInvalid()
{
    mCustomerIdInvalid = true;
}
// REF
// Lazy: depId
// Mandatory: depId
// depId lazy pointing to Department* (domainKey: id)
int Order::depId() const
{
	return mDepId;
}
Department* Order::depIdAsDataObject() const
{
	return mDepIdAsDataObject;
}
void Order::setDepId(int depId)
{
	if (depId != mDepId) {
        // remove old Data Object if one was resolved
        if (mDepIdAsDataObject) {
            // reset pointer, don't delete the independent object !
            mDepIdAsDataObject = 0;
        }
        // set the new lazy reference
        mDepId = depId;
        mDepIdInvalid = false;
        emit depIdChanged(depId);
        if (depId != -1) {
            // resolve the corresponding Data Object on demand from DataManager
        }
    }
}
void Order::removeDepId()
{
	if (mDepId != -1) {
		setDepId(-1);
	}
}
bool Order::hasDepId()
{
    if (!mDepIdInvalid && mDepId != -1) {
        return true;
    } else {
        return false;
    }
}
bool Order::isDepIdResolvedAsDataObject()
{
    if (!mDepIdInvalid && mDepIdAsDataObject) {
        return true;
    } else {
        return false;
    }
}

// lazy bound Data Object was resolved. overwrite id if different
void Order::resolveDepIdAsDataObject(Department* department)
{
    if (department) {
        if (department->id() != mDepId) {
            setDepId(department->id());
        }
        mDepIdAsDataObject = department;
        mDepIdInvalid = false;
    }
}
void Order::markDepIdAsInvalid()
{
    mDepIdInvalid = true;
}
// ATT 
// Optional: uuid
QString Order::uuid() const
{
	return mUuid;
}

void Order::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Mandatory: nr
// Domain KEY: nr
int Order::nr() const
{
	return mNr;
}

void Order::setNr(int nr)
{
	if (nr != mNr) {
		mNr = nr;
		emit nrChanged(nr);
	}
}
// ATT 
// Optional: expressOrder
bool Order::expressOrder() const
{
	return mExpressOrder;
}

void Order::setExpressOrder(bool expressOrder)
{
	if (expressOrder != mExpressOrder) {
		mExpressOrder = expressOrder;
		emit expressOrderChanged(expressOrder);
	}
}
// ATT 
// Mandatory: title
QString Order::title() const
{
	return mTitle;
}

void Order::setTitle(QString title)
{
	if (title != mTitle) {
		mTitle = title;
		emit titleChanged(title);
	}
}
// ATT 
// Mandatory: orderDate
/**
 * in QML set DateTimePicker value this way:
 * myPicker.value = new Date(orderDate)
 */
QDate Order::orderDate() const
{
	return mOrderDate;
}

/**
 * from QML DateTime Picker use as parameter:
 * orderDate = new Date(myPicker.value)
 */
void Order::setOrderDate(QDate orderDate)
{
	if (orderDate != mOrderDate) {
		mOrderDate = orderDate;
		emit orderDateChanged(orderDate);
	}
}
bool Order::hasOrderDate()
{
	return !mOrderDate.isNull() && mOrderDate.isValid();
}
// ATT 
// Optional: state
// ENUM: OrderState::OrderStateEnum
int Order::state() const
{
	return mState;
}
void Order::setState(int state)
{
	if (state != mState) {
		mState = state;
		emit stateChanged(state);
	}
}
void Order::setState(QString state)
{
    setState(stateStringToInt(state));
}
int Order::stateStringToInt(QString state)
{
    if (state.isNull() || state.isEmpty()) {
        return OrderState::NO_VALUE;
    }
    if (state == "NEW") {
        return 0;
    }
    if (state == "CONFIRMED") {
        return 1;
    }
    if (state == "DELIVERED") {
        return 2;
    }
    if (state == "CLOSED") {
        return 3;
    }
    qWarning() << "state wrong enumValue as String: " << state;
    return OrderState::NO_VALUE;
}
// ATT 
// Optional: processingState
int Order::processingState() const
{
	return mProcessingState;
}

void Order::setProcessingState(int processingState)
{
	if (processingState != mProcessingState) {
		mProcessingState = processingState;
		emit processingStateChanged(processingState);
	}
}
// REF
// Opposite: order
// Optional: info
Info* Order::info() const
{
	return mInfo;
}
/**
 * creates a new Info
 * parent is this Order
 * if data is successfully entered you must INVOKE setInfo()
 * if edit was canceled you must undoCreateInfo to free up memory
 */
Info* Order::createInfo()
{
    Info* info;
    info = new Info();
    info->setParent(this);
    info->prepareNew();
    return info;
}

/**
 * if createInfo was canceled from UI
 * this method deletes the Object of Type Info
 * 
 * to delete a  info allready set to  Info
 * you must use deleteInfo
 */
void Order::undoCreateInfo(Info* info)
{
    if (info) {
        info->deleteLater();
        info = 0;
    }
}

void Order::setInfo(Info* info)
{
	if (!info) {
	    return;
	}
	if (info != mInfo) {
		if (mInfo) {
			mInfo->deleteLater();
		}
		mInfo = info;
		mInfo->setParent(this);
		emit infoChanged(info);
	}
}
void Order::deleteInfo()
{
	if (mInfo) {
		emit infoDeleted(mInfo->uuid());
		mInfo->deleteLater();
		mInfo = 0;
	}
}
bool Order::hasInfo()
{
	if (mInfo) {
        return true;
    } else {
        return false;
    }
}
// ATT 
// Optional: domains
void Order::addToDomainsStringList(const QString& stringValue)
{
    mDomainsStringList.append(stringValue);
    emit addedToDomainsStringList(stringValue);
}

bool Order::removeFromDomainsStringList(const QString& stringValue)
{
    bool ok = false;
    ok = mDomainsStringList.removeOne(stringValue);
    if (!ok) {
    	qDebug() << "QString& not found in mDomainsStringList: " << stringValue;
    	return false;
    }
    emit removedFromDomainsStringList(stringValue);
    return true;
}
int Order::domainsCount()
{
    return mDomainsStringList.size();
}
QStringList Order::domainsStringList()
{
	return mDomainsStringList;
}
void Order::setDomainsStringList(const QStringList& domains) 
{
	if (domains != mDomainsStringList) {
		mDomainsStringList = domains;
		emit domainsStringListChanged(domains);
	}
}
// REF
// Opposite: order
// Mandatory: positions
QVariantList Order::positionsAsQVariantList()
{
	QVariantList positionsList;
	for (int i = 0; i < mPositions.size(); ++i) {
        positionsList.append((mPositions.at(i))->toMap());
    }
	return positionsList;
}
/**
 * creates a new Item
 * parent is this Order
 * if data is successfully entered you must INVOKE addToPositions()
 * if edit was canceled you must undoCreateElementOfPositions to free up memory
 */
Item* Order::createElementOfPositions()
{
    Item* item;
    item = new Item();
    item->setParent(this);
    item->prepareNew();
    return item;
}

/**
 * if createElementOfPositions was canceled from UI
 * this method deletes the Object of Type Item
 * 
 * to delete a allready into positions inserted  Item
 * you must use removeFromPositions
 */
void Order::undoCreateElementOfPositions(Item* item)
{
    if (item) {
        item->deleteLater();
        item = 0;
    }
}
void Order::addToPositions(Item* item)
{
    mPositions.append(item);
    emit addedToPositions(item);
}

bool Order::removeFromPositions(Item* item)
{
    bool ok = false;
    ok = mPositions.removeOne(item);
    if (!ok) {
    	qDebug() << "Item* not found in positions";
    	return false;
    }
    emit removedFromPositionsByUuid(item->uuid());
    // positions are contained - so we must delete them
    item->deleteLater();
    item = 0;
    return true;
}
void Order::clearPositions()
{
    for (int i = mPositions.size(); i > 0; --i) {
        removeFromPositions(mPositions.last());
    }
}
void Order::addToPositionsFromMap(const QVariantMap& itemMap)
{
    Item* item = new Item();
    item->setParent(this);
    item->fillFromMap(itemMap);
    mPositions.append(item);
    emit addedToPositions(item);
}
bool Order::removeFromPositionsByUuid(const QString& uuid)
{
    for (int i = 0; i < mPositions.size(); ++i) {
    	Item* item;
        item = mPositions.at(i);
        if (item->uuid() == uuid) {
        	mPositions.removeAt(i);
        	emit removedFromPositionsByUuid(uuid);
        	// positions are contained - so we must delete them
        	item->deleteLater();
        	item = 0;
        	return true;
        }
    }
    qDebug() << "uuid not found in positions: " << uuid;
    return false;
}

int Order::positionsCount()
{
    return mPositions.size();
}
QList<Item*> Order::positions()
{
	return mPositions;
}
void Order::setPositions(QList<Item*> positions) 
{
	if (positions != mPositions) {
		mPositions = positions;
		emit positionsChanged(positions);
	}
}
/**
 * to access lists from QML we're using QDeclarativeListProperty
 * and implement methods to append, count and clear
 * now from QML we can use
 * order.positionsPropertyList.length to get the size
 * order.positionsPropertyList[2] to get Item* at position 2
 * order.positionsPropertyList = [] to clear the list
 * or get easy access to properties like
 * order.positionsPropertyList[2].myPropertyName
 */
QDeclarativeListProperty<Item> Order::positionsPropertyList()
{
    return QDeclarativeListProperty<Item>(this, 0, &Order::appendToPositionsProperty,
            &Order::positionsPropertyCount, &Order::atPositionsProperty,
            &Order::clearPositionsProperty);
}
void Order::appendToPositionsProperty(QDeclarativeListProperty<Item> *positionsList,
        Item* item)
{
    Order *orderObject = qobject_cast<Order *>(positionsList->object);
    if (orderObject) {
		item->setParent(orderObject);
        orderObject->mPositions.append(item);
        emit orderObject->addedToPositions(item);
    } else {
        qWarning() << "cannot append Item* to positions " << "Object is not of type Order*";
    }
}
int Order::positionsPropertyCount(QDeclarativeListProperty<Item> *positionsList)
{
    Order *order = qobject_cast<Order *>(positionsList->object);
    if (order) {
        return order->mPositions.size();
    } else {
        qWarning() << "cannot get size positions " << "Object is not of type Order*";
    }
    return 0;
}
Item* Order::atPositionsProperty(QDeclarativeListProperty<Item> *positionsList, int pos)
{
    Order *order = qobject_cast<Order *>(positionsList->object);
    if (order) {
        if (order->mPositions.size() > pos) {
            return order->mPositions.at(pos);
        }
        qWarning() << "cannot get Item* at pos " << pos << " size is "
                << order->mPositions.size();
    } else {
        qWarning() << "cannot get Item* at pos " << pos << "Object is not of type Order*";
    }
    return 0;
}
void Order::clearPositionsProperty(QDeclarativeListProperty<Item> *positionsList)
{
    Order *order = qobject_cast<Order *>(positionsList->object);
    if (order) {
        // positions are contained - so we must delete them
        for (int i = 0; i < order->mPositions.size(); ++i) {
            order->mPositions.at(i)->deleteLater();
        }
        order->mPositions.clear();
    } else {
        qWarning() << "cannot clear positions " << "Object is not of type Order*";
    }
}
// ATT 
// Optional: tags
QVariantList Order::tagsAsQVariantList()
{
	QVariantList tagsList;
	for (int i = 0; i < mTags.size(); ++i) {
        tagsList.append((mTags.at(i))->toMap());
    }
	return tagsList;
}
// no create() or undoCreate() because dto is root object
// see methods in DataManager
void Order::addToTags(Tag* tag)
{
    mTags.append(tag);
    emit addedToTags(tag);
}

bool Order::removeFromTags(Tag* tag)
{
    bool ok = false;
    ok = mTags.removeOne(tag);
    if (!ok) {
    	qDebug() << "Tag* not found in tags";
    	return false;
    }
    // tags are independent - DON'T delete them
    return true;
}
void Order::clearTags()
{
    for (int i = mTags.size(); i > 0; --i) {
        removeFromTags(mTags.last());
    }
}

/**
 * lazy Array of independent Data Objects: only keys are persited
 * so we get a list of keys (uuid or domain keys) from map
 * and we persist only the keys toMap()
 * after initializing the keys must be resolved:
 * - get the list of keys: tagsKeys()
 * - resolve them from DataManager
 * - then resolveTagsKeys()
 */
bool Order::areTagsKeysResolved()
{
    return mTagsKeysResolved;
}

QStringList Order::tagsKeys()
{
    return mTagsKeys;
}

void Order::resolveTagsKeys(QList<Tag*> tags)
{
    if(mTagsKeysResolved){
        return;
    }
    mTags.clear();
    for (int i = 0; i < tags.size(); ++i) {
        addToTags(tags.at(i));
    }
    mTagsKeysResolved = true;
}

int Order::tagsCount()
{
    return mTags.size();
}
QList<Tag*> Order::tags()
{
	return mTags;
}
void Order::setTags(QList<Tag*> tags) 
{
	if (tags != mTags) {
		mTags = tags;
		emit tagsChanged(tags);
	}
}
/**
 * to access lists from QML we're using QDeclarativeListProperty
 * and implement methods to append, count and clear
 * now from QML we can use
 * order.tagsPropertyList.length to get the size
 * order.tagsPropertyList[2] to get Tag* at position 2
 * order.tagsPropertyList = [] to clear the list
 * or get easy access to properties like
 * order.tagsPropertyList[2].myPropertyName
 */
QDeclarativeListProperty<Tag> Order::tagsPropertyList()
{
    return QDeclarativeListProperty<Tag>(this, 0, &Order::appendToTagsProperty,
            &Order::tagsPropertyCount, &Order::atTagsProperty,
            &Order::clearTagsProperty);
}
void Order::appendToTagsProperty(QDeclarativeListProperty<Tag> *tagsList,
        Tag* tag)
{
    Order *orderObject = qobject_cast<Order *>(tagsList->object);
    if (orderObject) {
        orderObject->mTags.append(tag);
        emit orderObject->addedToTags(tag);
    } else {
        qWarning() << "cannot append Tag* to tags " << "Object is not of type Order*";
    }
}
int Order::tagsPropertyCount(QDeclarativeListProperty<Tag> *tagsList)
{
    Order *order = qobject_cast<Order *>(tagsList->object);
    if (order) {
        return order->mTags.size();
    } else {
        qWarning() << "cannot get size tags " << "Object is not of type Order*";
    }
    return 0;
}
Tag* Order::atTagsProperty(QDeclarativeListProperty<Tag> *tagsList, int pos)
{
    Order *order = qobject_cast<Order *>(tagsList->object);
    if (order) {
        if (order->mTags.size() > pos) {
            return order->mTags.at(pos);
        }
        qWarning() << "cannot get Tag* at pos " << pos << " size is "
                << order->mTags.size();
    } else {
        qWarning() << "cannot get Tag* at pos " << pos << "Object is not of type Order*";
    }
    return 0;
}
void Order::clearTagsProperty(QDeclarativeListProperty<Tag> *tagsList)
{
    Order *order = qobject_cast<Order *>(tagsList->object);
    if (order) {
        // tags are independent - DON'T delete them
        order->mTags.clear();
    } else {
        qWarning() << "cannot clear tags " << "Object is not of type Order*";
    }
}


Order::~Order()
{
	// place cleanUp code here
}
	
