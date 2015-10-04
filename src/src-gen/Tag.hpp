#ifndef TAG_HPP_
#define TAG_HPP_

#include <QObject>
#include <qvariant.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>




class Tag: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
	Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged FINAL)
	Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged FINAL)


public:
	Tag(QObject *parent = 0);


	void fillFromMap(const QVariantMap& tagMap);
	void fillFromForeignMap(const QVariantMap& tagMap);
	void fillFromCacheMap(const QVariantMap& tagMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	QString uuid() const;
	void setUuid(QString uuid);
	QString name() const;
	void setName(QString name);
	int value() const;
	void setValue(int value);
	QString color() const;
	void setColor(QString color);


	// SQL
	static const QString createTableCommand();
	static const QString createParameterizedInsertNameBinding();
	static const QString createParameterizedInsertPosBinding();
	void toSqlCache(QVariantList& uuidList, QVariantList& nameList, QVariantList& valueList, QVariantList& colorList);
	void fillFromSqlQuery(const QSqlQuery& sqlQuery);
	static void fillSqlQueryPos(const QSqlRecord& record);

	virtual ~Tag();

	Q_SIGNALS:

	void uuidChanged(QString uuid);
	void nameChanged(QString name);
	void valueChanged(int value);
	void colorChanged(QString color);
	

private:

	QString mUuid;
	QString mName;
	int mValue;
	QString mColor;

	Q_DISABLE_COPY (Tag)
};
Q_DECLARE_METATYPE(Tag*)

#endif /* TAG_HPP_ */

