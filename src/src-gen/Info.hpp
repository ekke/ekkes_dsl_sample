#ifndef INFO_HPP_
#define INFO_HPP_

#include <QObject>
#include <qvariant.h>


// forward declaration to avoid circular dependencies
class Order;


class Info: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(QString remarks READ remarks WRITE setRemarks NOTIFY remarksChanged FINAL)
	Q_PROPERTY(Order* order READ order)


public:
	Info(QObject *parent = 0);


	void fillFromMap(const QVariantMap& infoMap);
	void fillFromForeignMap(const QVariantMap& infoMap);
	void fillFromCacheMap(const QVariantMap& infoMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	QString uuid() const;
	void setUuid(QString uuid);
	QString remarks() const;
	void setRemarks(QString remarks);
	Order* order() const;
	// no SETTER order() is only convenience method to get the parent



	virtual ~Info();

	Q_SIGNALS:

	void uuidChanged(QString uuid);
	void remarksChanged(QString remarks);
	// no SIGNAL order is only convenience way to get the parent
	

private:

	QString mUuid;
	QString mRemarks;
	// no MEMBER mOrder it's the parent

	Q_DISABLE_COPY (Info)
};
Q_DECLARE_METATYPE(Info*)

#endif /* INFO_HPP_ */

