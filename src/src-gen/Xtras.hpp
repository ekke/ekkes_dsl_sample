#ifndef XTRAS_HPP_
#define XTRAS_HPP_

#include <QObject>
#include <qvariant.h>
#include <QDeclarativeListProperty>
#include <QStringList>




class Xtras: public QObject
{
	Q_OBJECT

	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged FINAL)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)

	Q_PROPERTY(QStringList textStringList READ textStringList  WRITE setTextStringList NOTIFY textStringListChanged FINAL)
	// QVariantList to get easy access from QML to int Array
	Q_PROPERTY(QVariantList valuesList READ valuesList  WRITE setValuesList NOTIFY valuesListChanged FINAL)

public:
	Xtras(QObject *parent = 0);


	void fillFromMap(const QVariantMap& xtrasMap);
	void fillFromForeignMap(const QVariantMap& xtrasMap);
	void fillFromCacheMap(const QVariantMap& xtrasMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	int id() const;
	void setId(int id);
	QString name() const;
	void setName(QString name);

	
	Q_INVOKABLE
	void addToTextStringList(const QString& stringValue);
	
	Q_INVOKABLE
	bool removeFromTextStringList(const QString& stringValue);
	
	Q_INVOKABLE
	int textCount();
	
	QStringList textStringList();
	void setTextStringList(const QStringList& text);
	
	Q_INVOKABLE
	void addToValuesList(const int& intValue);
	
	Q_INVOKABLE
	bool removeFromValuesList(const int& intValue);
	
	Q_INVOKABLE
	int valuesCount();
	
	// access from C++ to values
	QList<int> values();
	void setValues(QList<int> values);
	// access from QML to values (array of int)
	QVariantList valuesList();
	void setValuesList(const QVariantList& values);


	virtual ~Xtras();

	Q_SIGNALS:

	void idChanged(int id);
	void nameChanged(QString name);
	void textStringListChanged(QStringList text);
	void addedToTextStringList(QString stringValue);
	void removedFromTextStringList(QString stringValue);
	void valuesListChanged(QVariantList values);
	void addedToValuesList(int intValue);
	void removedFromValuesList(int intValue);
	

private:

	int mId;
	QString mName;
	QStringList mTextStringList;
	QList<int> mValues;

	Q_DISABLE_COPY (Xtras)
};
Q_DECLARE_METATYPE(Xtras*)

#endif /* XTRAS_HPP_ */

