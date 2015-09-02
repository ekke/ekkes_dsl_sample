#include "GeoCoordinate.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString latitudeKey = "latitude";
static const QString longitudeKey = "longitude";
static const QString altitudeKey = "altitude";

/*
 * Default Constructor if GeoCoordinate not initialized from QVariantMap
 */
GeoCoordinate::GeoCoordinate(QObject *parent) :
        QObject(parent), mUuid(""), mLatitude(0.0), mLongitude(0.0), mAltitude(0.0)
{
    mWrappedGeoCoordinate = new QGeoCoordinate();
    mAsText = mWrappedGeoCoordinate->toString();
    mIsValid = mWrappedGeoCoordinate->isValid();
    mIs3D = is3D();
    mUuid = QUuid::createUuid().toString();
    mUuid = mUuid.right(mUuid.length() - 1);
    mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * initialize GeoCoordinate from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void GeoCoordinate::fillFromMap(const QVariantMap& geoCoordinateMap)
{
    if (geoCoordinateMap.contains(uuidKey)
            && !geoCoordinateMap.value(uuidKey).toString().isEmpty()) {
        mUuid = geoCoordinateMap.value(uuidKey).toString();
    }
    QString verifyValue;
    double dValue;
    bool ok;
    ok = true;
    if (geoCoordinateMap.contains(latitudeKey)) {
        verifyValue = geoCoordinateMap.value(latitudeKey).toString();
        if (!verifyValue.isEmpty() && !verifyValue.startsWith("0.000")) {
            dValue = geoCoordinateMap.value(latitudeKey).toDouble(&ok);
            if (ok) {
                mLatitude = dValue;
                mWrappedGeoCoordinate->setLatitude(dValue);
            }

        }
    }
    if (geoCoordinateMap.contains(longitudeKey)) {
        verifyValue = geoCoordinateMap.value(longitudeKey).toString();
        if (!verifyValue.isEmpty() && !verifyValue.startsWith("0.000")) {
            dValue = geoCoordinateMap.value(longitudeKey).toDouble(&ok);
            if (ok) {
                mLongitude = dValue;
                mWrappedGeoCoordinate->setLongitude(dValue);
            }

        }
    }
    if (geoCoordinateMap.contains(altitudeKey)) {
        verifyValue = geoCoordinateMap.value(altitudeKey).toString();
        if (!verifyValue.isEmpty() && !verifyValue.startsWith("0.000")) {
            dValue = geoCoordinateMap.value(altitudeKey).toDouble(&ok);
            if (ok) {
                mAltitude = dValue;
                mWrappedGeoCoordinate->setAltitude(dValue);
            }
        }
    }
    updateProperties();
}

// to be compatible to normal DTOs
void GeoCoordinate::fillFromForeignMap(const QVariantMap& geoCoordinateMap)
{
    fillFromMap(geoCoordinateMap);
}
// to be compatible to normal DTOs
void GeoCoordinate::fillFromCacheMap(const QVariantMap& geoCoordinateMap)
{
    fillFromMap(geoCoordinateMap);
}

// SLOT
void GeoCoordinate::onCoordinateReceived(QGeoCoordinate* coordinate)
{
    if (coordinate && coordinate->isValid()) {
        if (coordinate != mWrappedGeoCoordinate) {
            mWrappedGeoCoordinate = coordinate;
            mLatitude = coordinate->latitude();
            mLongitude = coordinate->longitude();
            if (coordinate->type() == QGeoCoordinate::Coordinate3D) {
                mAltitude = coordinate->altitude();
            }
            emit coordinateChanged(this);
            updateProperties();
        }
    }
}

/*
 * Exports Properties from GeoCoordinate as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap GeoCoordinate::toMap()
{
    QVariantMap geoCoordinateMap;
    if (mWrappedGeoCoordinate->isValid()) {
        geoCoordinateMap.insert(uuidKey, mUuid);
        geoCoordinateMap.insert(latitudeKey, mLatitude);
        geoCoordinateMap.insert(longitudeKey, mLongitude);
        if (mWrappedGeoCoordinate->type() == QGeoCoordinate::Coordinate3D) {
            geoCoordinateMap.insert(altitudeKey, mAltitude);
        }
    }
    return geoCoordinateMap;
}

QString GeoCoordinate::uuid() const
{
    return mUuid;
}
void GeoCoordinate::setUuid(QString uuid)
{
    if (uuid != mUuid) {
        mUuid = uuid;
        emit uuidChanged(uuid);
    }
}

double GeoCoordinate::latitude() const
{
    return mLatitude;
}
void GeoCoordinate::setLatitude(double latitude)
{
    if (latitude != mLatitude) {
        mLatitude = latitude;
        mWrappedGeoCoordinate->setLatitude(mLatitude);
        emit latitudeChanged(latitude);
        updateProperties();
    }
}

double GeoCoordinate::longitude() const
{
    return mLongitude;
}
void GeoCoordinate::setLongitude(double longitude)
{
    if (longitude != mLongitude) {
        mLongitude = longitude;
        mWrappedGeoCoordinate->setLongitude(mLongitude);
        emit longitudeChanged(longitude);
        updateProperties();
    }
}

double GeoCoordinate::altitude() const
{
    return mAltitude;
}
void GeoCoordinate::setAltitude(double altitude)
{
    if (altitude != mAltitude) {
        mAltitude = altitude;
        mWrappedGeoCoordinate->setAltitude(mAltitude);
        emit altitudeChanged(altitude);
        updateProperties();
    }
}

bool GeoCoordinate::is3D() const
{
    return mWrappedGeoCoordinate->type() == QGeoCoordinate::Coordinate3D;
}

bool GeoCoordinate::isValid() const
{
    return mWrappedGeoCoordinate->isValid();
}

QString GeoCoordinate::asText() const
{
    return mWrappedGeoCoordinate->toString();
}

void GeoCoordinate::updateProperties()
{
    if (mWrappedGeoCoordinate->toString() != mAsText) {
        mAsText = mWrappedGeoCoordinate->toString();
        emit asTextChanged(mAsText);
    }
    if (is3D() != mIs3D) {
        mIs3D = is3D();
        emit is3DChanged(mIs3D);
    }
    if (mWrappedGeoCoordinate->isValid() != mIsValid) {
        mIsValid = mWrappedGeoCoordinate->isValid();
        emit isValidChanged(mIsValid);
    }
}

/**
 * Returns the distance (in meters) from this coordinate to the coordinate specified by other.
 * Altitude is not used in the calculation.
 *
 */
qreal GeoCoordinate::distanceTo(GeoCoordinate* otherCoordinate)
{
    return mWrappedGeoCoordinate->distanceTo(*otherCoordinate->wrappedCoordinate());
}

QGeoCoordinate* GeoCoordinate::wrappedCoordinate()
{
    return mWrappedGeoCoordinate;
}

void GeoCoordinate::clear()
{
    mWrappedGeoCoordinate = 0;
    mWrappedGeoCoordinate = new QGeoCoordinate();
    updateProperties();
}

GeoCoordinate::~GeoCoordinate()
{
    // place cleanUp code here
}

