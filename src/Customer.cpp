#include "Customer.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString idKey = "id";
static const QString companyNameKey = "companyName";
static const QString coordinateKey = "coordinate";
static const QString geoAddressKey = "geoAddress";

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString idForeignKey = "id";
static const QString companyNameForeignKey = "companyName";
static const QString coordinateForeignKey = "coordinate";
static const QString geoAddressForeignKey = "geoAddress";

/*
 * Default Constructor if Customer not initialized from QVariantMap
 */
Customer::Customer(QObject *parent) :
        QObject(parent), mUuid(""), mId(-1), mCompanyName("")
{
	// set Types of DataObject* to NULL:
	mCoordinate = 0;
	mCoordinate = new GeoCoordinate();
	mGeoAddress = 0;
	mGeoAddress = new GeoAddress();
}

/*
 * initialize Customer from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Customer::fillFromMap(const QVariantMap& customerMap)
{
	mUuid = customerMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mId = customerMap.value(idKey).toInt();
	mCompanyName = customerMap.value(companyNameKey).toString();
	// mCoordinate points to GeoCoordinate*
	if (customerMap.contains(coordinateKey)) {
		QVariantMap coordinateMap;
		coordinateMap = customerMap.value(coordinateKey).toMap();
		if (!coordinateMap.isEmpty()) {
			mCoordinate = 0;
			mCoordinate = new GeoCoordinate();
			mCoordinate->setParent(this);
			mCoordinate->fillFromMap(coordinateMap);
		}
	}
	// mGeoAddress points to GeoAddress*
	if (customerMap.contains(geoAddressKey)) {
		QVariantMap geoAddressMap;
		geoAddressMap = customerMap.value(geoAddressKey).toMap();
		if (!geoAddressMap.isEmpty()) {
			mGeoAddress = 0;
			mGeoAddress = new GeoAddress();
			mGeoAddress->setParent(this);
			mGeoAddress->fillFromMap(geoAddressMap);
		}
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Customer::fillFromForeignMap(const QVariantMap& customerMap)
{
	mUuid = customerMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mId = customerMap.value(idForeignKey).toInt();
	mCompanyName = customerMap.value(companyNameForeignKey).toString();
	// mCoordinate points to GeoCoordinate*
	if (customerMap.contains(coordinateForeignKey)) {
		QVariantMap coordinateMap;
		coordinateMap = customerMap.value(coordinateForeignKey).toMap();
		if (!coordinateMap.isEmpty()) {
			mCoordinate = 0;
			mCoordinate = new GeoCoordinate();
			mCoordinate->setParent(this);
			mCoordinate->fillFromForeignMap(coordinateMap);
		}
	}
	// mGeoAddress points to GeoAddress*
	if (customerMap.contains(geoAddressForeignKey)) {
		QVariantMap geoAddressMap;
		geoAddressMap = customerMap.value(geoAddressForeignKey).toMap();
		if (!geoAddressMap.isEmpty()) {
			mGeoAddress = 0;
			mGeoAddress = new GeoAddress();
			mGeoAddress->setParent(this);
			mGeoAddress->fillFromForeignMap(geoAddressMap);
		}
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Customer::fillFromCacheMap(const QVariantMap& customerMap)
{
	mUuid = customerMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mId = customerMap.value(idKey).toInt();
	mCompanyName = customerMap.value(companyNameKey).toString();
	// mCoordinate points to GeoCoordinate*
	if (customerMap.contains(coordinateKey)) {
		QVariantMap coordinateMap;
		coordinateMap = customerMap.value(coordinateKey).toMap();
		if (!coordinateMap.isEmpty()) {
			mCoordinate = new GeoCoordinate();
			mCoordinate->setParent(this);
			mCoordinate->fillFromCacheMap(coordinateMap);
		}
	}
	// mGeoAddress points to GeoAddress*
	if (customerMap.contains(geoAddressKey)) {
		QVariantMap geoAddressMap;
		geoAddressMap = customerMap.value(geoAddressKey).toMap();
		if (!geoAddressMap.isEmpty()) {
			mGeoAddress = new GeoAddress();
			mGeoAddress->setParent(this);
			mGeoAddress->fillFromCacheMap(geoAddressMap);
		}
	}
}

void Customer::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Customer::isValid()
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
 * Exports Properties from Customer as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Customer::toMap()
{
	QVariantMap customerMap;
	customerMap.insert(uuidKey, mUuid);
	customerMap.insert(idKey, mId);
	customerMap.insert(companyNameKey, mCompanyName);
	// mCoordinate points to GeoCoordinate*
	if (mCoordinate) {
	if (mCoordinate->isValid()) {
		customerMap.insert(coordinateKey, mCoordinate->toMap());
	}
	}
	// mGeoAddress points to GeoAddress*
	if (mGeoAddress) {
	customerMap.insert(geoAddressKey, mGeoAddress->toMap());
	}
	return customerMap;
}

/*
 * Exports Properties from Customer as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Customer::toForeignMap()
{
	QVariantMap customerMap;
	customerMap.insert(uuidForeignKey, mUuid);
	customerMap.insert(idForeignKey, mId);
	customerMap.insert(companyNameForeignKey, mCompanyName);
	// mCoordinate points to GeoCoordinate*
	if (mCoordinate) {
		if (mCoordinate->isValid()) {
			customerMap.insert(coordinateForeignKey, mCoordinate->toMap());
		}
	}
	// mGeoAddress points to GeoAddress*
	if (mGeoAddress) {
		customerMap.insert(geoAddressForeignKey, mGeoAddress->toMap());
	}
	return customerMap;
}


/*
 * Exports Properties from Customer as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Customer::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Optional: uuid
QString Customer::uuid() const
{
	return mUuid;
}

void Customer::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Mandatory: id
// Domain KEY: id
int Customer::id() const
{
	return mId;
}

void Customer::setId(int id)
{
	if (id != mId) {
		mId = id;
		emit idChanged(id);
	}
}
// ATT 
// Optional: companyName
QString Customer::companyName() const
{
	return mCompanyName;
}

void Customer::setCompanyName(QString companyName)
{
	if (companyName != mCompanyName) {
		mCompanyName = companyName;
		emit companyNameChanged(companyName);
	}
}
// ATT 
// Optional: coordinate
GeoCoordinate* Customer::coordinate() const
{
	return mCoordinate;
}

void Customer::setCoordinate(GeoCoordinate* coordinate)
{
	if (!coordinate) {
	    return;
	}
	if (coordinate != mCoordinate) {
		if (mCoordinate) {
			mCoordinate->deleteLater();
		}
		mCoordinate = coordinate;
		mCoordinate->setParent(this);
		emit coordinateChanged(coordinate);
	}
}
void Customer::deleteCoordinate()
{
	if (mCoordinate) {
		emit coordinateDeleted(mCoordinate->uuid());
		mCoordinate->deleteLater();
		mCoordinate = 0;
	}
}
bool Customer::hasCoordinate()
{
	if (mCoordinate && mCoordinate->isValid()) {
        return true;
    } else {
        return false;
    }
}
// ATT 
// Optional: geoAddress
GeoAddress* Customer::geoAddress() const
{
	return mGeoAddress;
}

void Customer::setGeoAddress(GeoAddress* geoAddress)
{
	if (!geoAddress) {
	    return;
	}
	if (geoAddress != mGeoAddress) {
		if (mGeoAddress) {
			mGeoAddress->deleteLater();
		}
		mGeoAddress = geoAddress;
		mGeoAddress->setParent(this);
		emit geoAddressChanged(geoAddress);
	}
}
void Customer::deleteGeoAddress()
{
	if (mGeoAddress) {
		emit geoAddressDeleted(mGeoAddress->uuid());
		mGeoAddress->deleteLater();
		mGeoAddress = 0;
	}
}
bool Customer::hasGeoAddress()
{
	if (mGeoAddress && !mGeoAddress->isEmpty()) {
        return true;
    } else {
        return false;
    }
}


Customer::~Customer()
{
	// place cleanUp code here
}
	
