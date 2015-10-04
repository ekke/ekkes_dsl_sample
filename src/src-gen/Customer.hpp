#ifndef CUSTOMER_HPP_
#define CUSTOMER_HPP_

#include <QObject>
#include <qvariant.h>
// #include <QtLocationSubset/QGeoCoordinate>
// #include <QtLocationSubset/QGeoAddress>


#include "../GeoCoordinate.hpp"
#include "../GeoAddress.hpp"

// using namespace QtMobilitySubset;	

class Customer: public QObject
{
	Q_OBJECT

	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged FINAL)
	Q_PROPERTY(QString companyName READ companyName WRITE setCompanyName NOTIFY companyNameChanged FINAL)
	Q_PROPERTY(GeoCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged FINAL)
	Q_PROPERTY(GeoAddress* geoAddress READ geoAddress WRITE setGeoAddress NOTIFY geoAddressChanged FINAL)


public:
	Customer(QObject *parent = 0);


	void fillFromMap(const QVariantMap& customerMap);
	void fillFromForeignMap(const QVariantMap& customerMap);
	void fillFromCacheMap(const QVariantMap& customerMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	int id() const;
	void setId(int id);
	QString companyName() const;
	void setCompanyName(QString companyName);
	GeoCoordinate* coordinate() const;
	void setCoordinate(GeoCoordinate* coordinate);
	Q_INVOKABLE
	void deleteCoordinate();
	
	Q_INVOKABLE
	bool hasCoordinate();
	
	GeoAddress* geoAddress() const;
	void setGeoAddress(GeoAddress* geoAddress);
	Q_INVOKABLE
	void deleteGeoAddress();
	
	Q_INVOKABLE
	bool hasGeoAddress();
	



	virtual ~Customer();

	Q_SIGNALS:

	void idChanged(int id);
	void companyNameChanged(QString companyName);
	void coordinateChanged(GeoCoordinate* coordinate);
	void coordinateDeleted(QString uuid);
	void geoAddressChanged(GeoAddress* geoAddress);
	void geoAddressDeleted(QString uuid);
	

private:

	int mId;
	QString mCompanyName;
	GeoCoordinate* mCoordinate;
	GeoAddress* mGeoAddress;

	Q_DISABLE_COPY (Customer)
};
Q_DECLARE_METATYPE(Customer*)

#endif /* CUSTOMER_HPP_ */

