#ifndef GEOCOORDINATE_HPP_
#define GEOCOORDINATE_HPP_

#include <QObject>
#include <qvariant.h>
#include <QtLocationSubset/QGeoCoordinate>

using namespace QtMobilitySubset;

class GeoCoordinate: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged FINAL)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged FINAL)
    Q_PROPERTY(double altitude READ altitude WRITE setAltitude NOTIFY altitudeChanged FINAL)

    Q_PROPERTY(QString asText READ asText NOTIFY asTextChanged FINAL)
    Q_PROPERTY(bool is3D READ is3D NOTIFY is3DChanged FINAL)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged FINAL)

public:
    GeoCoordinate(QObject *parent = 0);

    void fillFromMap(const QVariantMap& geoCoordinateMap);
    void fillFromForeignMap(const QVariantMap& geoCoordinateMap);
    void fillFromCacheMap(const QVariantMap& geoCoordinateMap);

    Q_INVOKABLE
    QVariantMap toMap();

    QString uuid() const;
    void setUuid(QString uuid);
    double latitude() const;
    void setLatitude(double latitude);
    double longitude() const;
    void setLongitude(double longitude);
    double altitude() const;
    void setAltitude(double altitude);

    bool is3D() const;
    bool isValid() const;
    QString asText() const;

    Q_INVOKABLE
    void clear();

    Q_INVOKABLE
    qreal distanceTo(GeoCoordinate* otherCoordinate);

    QGeoCoordinate* wrappedCoordinate();

    virtual ~GeoCoordinate();

    Q_SIGNALS:

    void uuidChanged(QString uuid);
    void latitudeChanged(double latitude);
    void longitudeChanged(double longitude);
    void altitudeChanged(double altitude);
    void is3DChanged(bool is3D);
    void isValidChanged(bool isValid);
    QString asTextChanged(QString asText);
    void coordinateChanged(GeoCoordinate* coordinate);

public slots:

    void onCoordinateReceived(QGeoCoordinate* coordinate);

private:

    // wrapped
    QGeoCoordinate* mWrappedGeoCoordinate;

    // persisted
    QString mUuid;
    double mLatitude;
    double mLongitude;
    double mAltitude;

    bool mIs3D;
    bool mIsValid;
    QString mAsText;
    void updateProperties();

    Q_DISABLE_COPY (GeoCoordinate)
};
Q_DECLARE_METATYPE(GeoCoordinate*)

#endif /* GEOCOORDINATE_HPP_ */

