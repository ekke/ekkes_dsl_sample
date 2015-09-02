#ifndef GEOADDRESS_HPP_
#define GEOADDRESS_HPP_

#include <QObject>
#include <qvariant.h>
#include <QtLocationSubset/QGeoAddress>

using namespace QtMobilitySubset;

class GeoAddress: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged FINAL)
	Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged FINAL)
	Q_PROPERTY(QString countryCode READ countryCode WRITE setCountryCode NOTIFY countryCodeChanged FINAL)
	Q_PROPERTY(QString county READ county WRITE setCounty NOTIFY countyChanged FINAL)
	Q_PROPERTY(QString district READ district WRITE setDistrict NOTIFY districtChanged FINAL)
	Q_PROPERTY(QString postcode READ postcode WRITE setPostcode NOTIFY postcodeChanged FINAL)
	Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged FINAL)
	Q_PROPERTY(QString street READ street WRITE setStreet NOTIFY streetChanged FINAL)

	Q_PROPERTY(QString asText READ asText NOTIFY asTextChanged FINAL)
	Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY isEmptyChanged FINAL)


public:
	GeoAddress(QObject *parent = 0);


	void fillFromMap(const QVariantMap& geoAddressMap);
	void fillFromForeignMap(const QVariantMap& geoAddressMap);
	void fillFromCacheMap(const QVariantMap& geoAddressMap);

	Q_INVOKABLE
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();

	QString uuid() const;
	void setUuid(QString uuid);
	QString city() const;
	void setCity(QString city);
	QString country() const;
	void setCountry(QString country);
	QString countryCode() const;
	void setCountryCode(QString countryCode);
	QString county() const;
	void setCounty(QString county);
	QString district() const;
	void setDistrict(QString district);
	QString postcode() const;
	void setPostcode(QString postcode);
	QString state() const;
	void setState(QString state);
	QString street() const;
	void setStreet(QString street);

	QString asText() const;
	bool isEmpty() const;

	Q_INVOKABLE
	void clear();

	QGeoAddress* wrappedAddress();

	virtual ~GeoAddress();

	Q_SIGNALS:

	void uuidChanged(QString uuid);
	void cityChanged(QString city);
	void countryChanged(QString country);
	void countryCodeChanged(QString countryCode);
	void countyChanged(QString county);
	void districtChanged(QString district);
	void postcodeChanged(QString postcode);
	void stateChanged(QString state);
	void streetChanged(QString street);
	void addressChanged(GeoAddress* address);

	void asTextChanged(QString asText);
	void isEmptyChanged(bool isEmpty);

	public slots:

    void onAddressReceived(QGeoAddress* address);

private:

	QString mUuid;
	QString mCity;
	QString mCountry;
	QString mCountryCode;
	QString mCounty;
	QString mDistrict;
	QString mPostcode;
	QString mState;
	QString mStreet;

	QString mAsText;
	void updateAsText();
	bool mIsEmpty;
	void updateIsEmpty();

	// wrapped
    QGeoAddress* mWrappedGeoAddress;

	Q_DISABLE_COPY (GeoAddress)
};
Q_DECLARE_METATYPE(GeoAddress*)

#endif /* GEOADDRESS_HPP_ */

