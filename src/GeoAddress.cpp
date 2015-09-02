#include "GeoAddress.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString cityKey = "city";
static const QString countryKey = "country";
static const QString countryCodeKey = "countryCode";
static const QString countyKey = "county";
static const QString districtKey = "district";
static const QString postcodeKey = "postcode";
static const QString stateKey = "state";
static const QString streetKey = "street";

/*
 * Default Constructor if GeoAddress not initialized from QVariantMap
 */
GeoAddress::GeoAddress(QObject *parent) :
        QObject(parent), mUuid(""), mCity(""), mCountry(""), mCountryCode(""), mCounty(""), mDistrict(
                ""), mPostcode(""), mState(""), mStreet("")
{
    mWrappedGeoAddress = new QGeoAddress;
    mAsText = mWrappedGeoAddress->text();
    mIsEmpty = mWrappedGeoAddress->isEmpty();
    mUuid = QUuid::createUuid().toString();
    mUuid = mUuid.right(mUuid.length() - 1);
    mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * initialize GeoAddress from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void GeoAddress::fillFromMap(const QVariantMap& geoAddressMap)
{
    if (geoAddressMap.contains(uuidKey) && !geoAddressMap.value(uuidKey).toString().isEmpty()) {
        mUuid = geoAddressMap.value(uuidKey).toString();
    }
    mWrappedGeoAddress->clear();
    if (geoAddressMap.contains(cityKey)) {
        mCity = geoAddressMap.value(cityKey).toString();
        mWrappedGeoAddress->setCity(mCity);
    }
    if (geoAddressMap.contains(countryKey)) {
        mCountry = geoAddressMap.value(countryKey).toString();
        mWrappedGeoAddress->setCountry(mCountry);
    }
    if (geoAddressMap.contains(countryCodeKey)) {
        mCountryCode = geoAddressMap.value(countryCodeKey).toString();
        mWrappedGeoAddress->setCountryCode(mCountryCode);
    }
    if (geoAddressMap.contains(countyKey)) {
        mCounty = geoAddressMap.value(countyKey).toString();
        mWrappedGeoAddress->setCounty(mCounty);
    }
    if (geoAddressMap.contains(districtKey)) {
        mDistrict = geoAddressMap.value(districtKey).toString();
        mWrappedGeoAddress->setDistrict(mDistrict);
    }
    if (geoAddressMap.contains(postcodeKey)) {
        mPostcode = geoAddressMap.value(postcodeKey).toString();
        mWrappedGeoAddress->setPostcode(mPostcode);
    }
    if (geoAddressMap.contains(stateKey)) {
        mState = geoAddressMap.value(stateKey).toString();
        mWrappedGeoAddress->setState(mState);
    }
    if (geoAddressMap.contains(streetKey)) {
        mStreet = geoAddressMap.value(streetKey).toString();
        mWrappedGeoAddress->setStreet(mStreet);
    }
    mAsText = mWrappedGeoAddress->text();
}
// to be compatible to normal DTOs
void GeoAddress::fillFromForeignMap(const QVariantMap& geoAddressMap)
{
    fillFromMap(geoAddressMap);
}
// to be compatible to normal DTOs
void GeoAddress::fillFromCacheMap(const QVariantMap& geoAddressMap)
{
    fillFromMap(geoAddressMap);
}

// SLOT
void GeoAddress::onAddressReceived(QGeoAddress* address)
{
    if (address && !address->isEmpty()) {
        if (address != mWrappedGeoAddress) {
            mWrappedGeoAddress = address;
            mCity.clear();
            if (!address->city().isNull() && !address->city().isEmpty()) {
                mCity = address->city();
            }
            mCountry.clear();
            if (!address->country().isNull() && !address->country().isEmpty()) {
                mCountry = address->country();
            }
            mCountryCode.clear();
            if (!address->countryCode().isNull() && !address->countryCode().isEmpty()) {
                mCountryCode = address->countryCode();
            }
            mCounty.clear();
            if (!address->county().isNull() && !address->county().isEmpty()) {
                mCounty = address->county();
            }
            mDistrict.clear();
            if (!address->district().isNull() && !address->district().isEmpty()) {
                mDistrict = address->district();
            }
            mPostcode.clear();
            if (!address->postcode().isNull() && !address->postcode().isEmpty()) {
                mPostcode = address->postcode();
            }
            mState.clear();
            if (!address->state().isNull() && !address->state().isEmpty()) {
                mState = address->state();
            }
            mStreet.clear();
            if (!address->street().isNull() && !address->street().isEmpty()) {
                mStreet = address->street();
            }
            emit addressChanged(this);
            updateAsText();
            updateIsEmpty();
        }
    }
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool GeoAddress::isValid()
{
    if (mUuid.isNull() || mUuid.isEmpty()) {
        return false;
    }
    if (mWrappedGeoAddress->isEmpty()) {
        return false;
    }
    if (mStreet.isEmpty() && mCity.isEmpty()) {
        return false;
    }
    return true;
}

/*
 * Exports Properties from GeoAddress as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap GeoAddress::toMap()
{
    QVariantMap geoAddressMap;
    if (!mWrappedGeoAddress->isEmpty()) {
        geoAddressMap.insert(uuidKey, mUuid);
        if (!mCity.isEmpty()) {
            geoAddressMap.insert(cityKey, mCity);
        }
        if (!mCountry.isEmpty()) {
            geoAddressMap.insert(countryKey, mCountry);
        }
        if (!mCountryCode.isEmpty()) {
            geoAddressMap.insert(countryCodeKey, mCountryCode);
        }
        if (!mCounty.isEmpty()) {
            geoAddressMap.insert(countyKey, mCounty);
        }
        if (!mDistrict.isEmpty()) {
            geoAddressMap.insert(districtKey, mDistrict);
        }
        if (!mPostcode.isEmpty()) {
            geoAddressMap.insert(postcodeKey, mPostcode);
        }
        if (!mState.isEmpty()) {
            geoAddressMap.insert(stateKey, mState);
        }
        if (!mStreet.isEmpty()) {
            geoAddressMap.insert(streetKey, mStreet);
        }
    }
    return geoAddressMap;
}

QString GeoAddress::uuid() const
{
    return mUuid;
}
void GeoAddress::setUuid(QString uuid)
{
    if (uuid != mUuid) {
        mUuid = uuid;
        emit uuidChanged(uuid);
    }
}

QString GeoAddress::city() const
{
    return mCity;
}
void GeoAddress::setCity(QString city)
{
    if (city != mCity) {
        mCity = city;
        mWrappedGeoAddress->setCity(mCity);
        emit cityChanged(city);
        updateAsText();
        updateIsEmpty();
    }
}

QString GeoAddress::country() const
{
    return mCountry;
}
void GeoAddress::setCountry(QString country)
{
    if (country != mCountry) {
        mCountry = country;
        mWrappedGeoAddress->setCountry(mCountry);
        emit countryChanged(country);
        updateAsText();
        updateIsEmpty();
    }
}

QString GeoAddress::countryCode() const
{
    return mCountryCode;
}
void GeoAddress::setCountryCode(QString countryCode)
{
    if (countryCode != mCountryCode) {
        mCountryCode = countryCode;
        mWrappedGeoAddress->setCountryCode(mCountryCode);
        emit countryCodeChanged(countryCode);
        updateAsText();
        updateIsEmpty();
    }
}

QString GeoAddress::county() const
{
    return mCounty;
}
void GeoAddress::setCounty(QString county)
{
    if (county != mCounty) {
        mCounty = county;
        mWrappedGeoAddress->setCounty(mCounty);
        emit countyChanged(county);
        updateAsText();
        updateIsEmpty();
    }
}

QString GeoAddress::district() const
{
    return mDistrict;
}
void GeoAddress::setDistrict(QString district)
{
    if (district != mDistrict) {
        mDistrict = district;
        mWrappedGeoAddress->setDistrict(mDistrict);
        emit districtChanged(district);
        updateAsText();
        updateIsEmpty();
    }
}

QString GeoAddress::postcode() const
{
    return mPostcode;
}
void GeoAddress::setPostcode(QString postcode)
{
    if (postcode != mPostcode) {
        mPostcode = postcode;
        mWrappedGeoAddress->setPostcode(mPostcode);
        emit postcodeChanged(postcode);
        updateAsText();
        updateIsEmpty();
    }
}

QString GeoAddress::state() const
{
    return mState;
}
void GeoAddress::setState(QString state)
{
    if (state != mState) {
        mState = state;
        mWrappedGeoAddress->setState(mState);
        emit stateChanged(state);
        updateAsText();
        updateIsEmpty();
    }
}

QString GeoAddress::street() const
{
    return mStreet;
}
void GeoAddress::setStreet(QString street)
{
    if (street != mStreet) {
        mStreet = street;
        mWrappedGeoAddress->setStreet(mStreet);
        emit streetChanged(street);
        updateAsText();
        updateIsEmpty();
    }
}

QString GeoAddress::asText() const
{
    return mWrappedGeoAddress->text();
}

void GeoAddress::updateAsText()
{
    if (mWrappedGeoAddress->text() != mAsText) {
        mAsText = mWrappedGeoAddress->text();
        emit asTextChanged(mAsText);
    }
}

bool GeoAddress::isEmpty() const
{
    return mWrappedGeoAddress->isEmpty();
}

void GeoAddress::updateIsEmpty()
{
    if (mWrappedGeoAddress->isEmpty() != mIsEmpty) {
        mIsEmpty = mWrappedGeoAddress->isEmpty();
        emit isEmptyChanged(mIsEmpty);
    }
}

void GeoAddress::clear()
{
    mWrappedGeoAddress->clear();
    mCity.clear();
    mCountry.clear();
    mCountryCode.clear();
    mCounty.clear();
    mDistrict.clear();
    mPostcode.clear();
    mState.clear();
    mStreet.clear();
    updateAsText();
    updateIsEmpty();
}

QGeoAddress* GeoAddress::wrappedAddress()
{
    return mWrappedGeoAddress;
}

GeoAddress::~GeoAddress()
{
    // place cleanUp code here
}

