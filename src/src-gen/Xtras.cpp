#include "Xtras.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString idKey = "id";
static const QString nameKey = "name";
static const QString textKey = "text";
static const QString valuesKey = "values";

// keys used from Server API etc
static const QString idForeignKey = "id";
static const QString nameForeignKey = "name";
static const QString textForeignKey = "text";
static const QString valuesForeignKey = "values";

/*
 * Default Constructor if Xtras not initialized from QVariantMap
 */
Xtras::Xtras(QObject *parent) :
        QObject(parent), mId(-1), mName("")
{
}

/*
 * initialize Xtras from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Xtras::fillFromMap(const QVariantMap& xtrasMap)
{
	mId = xtrasMap.value(idKey).toInt();
	mName = xtrasMap.value(nameKey).toString();
	mTextStringList = xtrasMap.value(textKey).toStringList();
	// mValues is Array of int
	QVariantList valuesList;
	valuesList = xtrasMap.value(valuesKey).toList();
	mValues.clear();
	for (int i = 0; i < valuesList.size(); ++i) {
		mValues.append(valuesList.at(i).toInt());
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Xtras::fillFromForeignMap(const QVariantMap& xtrasMap)
{
	mId = xtrasMap.value(idForeignKey).toInt();
	mName = xtrasMap.value(nameForeignKey).toString();
	mTextStringList = xtrasMap.value(textForeignKey).toStringList();
	// mValues is Array of int
	QVariantList valuesList;
	valuesList = xtrasMap.value(valuesForeignKey).toList();
	mValues.clear();
	for (int i = 0; i < valuesList.size(); ++i) {
		mValues.append(valuesList.at(i).toInt());
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Xtras::fillFromCacheMap(const QVariantMap& xtrasMap)
{
	mId = xtrasMap.value(idKey).toInt();
	mName = xtrasMap.value(nameKey).toString();
	mTextStringList = xtrasMap.value(textKey).toStringList();
	// mValues is Array of int
	QVariantList valuesList;
	valuesList = xtrasMap.value(valuesKey).toList();
	mValues.clear();
	for (int i = 0; i < valuesList.size(); ++i) {
		mValues.append(valuesList.at(i).toInt());
	}
}

void Xtras::prepareNew()
{
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Xtras::isValid()
{
	if (mId == -1) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Xtras as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Xtras::toMap()
{
	QVariantMap xtrasMap;
	xtrasMap.insert(idKey, mId);
	xtrasMap.insert(nameKey, mName);
	// Array of QString
	xtrasMap.insert(textKey, mTextStringList);
	// Array of int
	xtrasMap.insert(valuesKey, valuesList());
	return xtrasMap;
}

/*
 * Exports Properties from Xtras as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Xtras::toForeignMap()
{
	QVariantMap xtrasMap;
	xtrasMap.insert(idForeignKey, mId);
	xtrasMap.insert(nameForeignKey, mName);
	// Array of QString
	xtrasMap.insert(textForeignKey, mTextStringList);
	// Array of int
	xtrasMap.insert(valuesForeignKey, valuesList());
	return xtrasMap;
}


/*
 * Exports Properties from Xtras as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Xtras::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Mandatory: id
// Domain KEY: id
int Xtras::id() const
{
	return mId;
}

void Xtras::setId(int id)
{
	if (id != mId) {
		mId = id;
		emit idChanged(id);
	}
}
// ATT 
// Optional: name
QString Xtras::name() const
{
	return mName;
}

void Xtras::setName(QString name)
{
	if (name != mName) {
		mName = name;
		emit nameChanged(name);
	}
}
// ATT 
// Optional: text
void Xtras::addToTextStringList(const QString& stringValue)
{
    mTextStringList.append(stringValue);
    emit addedToTextStringList(stringValue);
}

bool Xtras::removeFromTextStringList(const QString& stringValue)
{
    bool ok = false;
    ok = mTextStringList.removeOne(stringValue);
    if (!ok) {
    	qDebug() << "QString& not found in mTextStringList: " << stringValue;
    	return false;
    }
    emit removedFromTextStringList(stringValue);
    return true;
}
int Xtras::textCount()
{
    return mTextStringList.size();
}
QStringList Xtras::textStringList()
{
	return mTextStringList;
}
void Xtras::setTextStringList(const QStringList& text) 
{
	if (text != mTextStringList) {
		mTextStringList = text;
		emit textStringListChanged(text);
	}
}
// ATT 
// Optional: values
void Xtras::addToValuesList(const int& intValue)
{
    mValues.append(intValue);
    emit addedToValuesList(intValue);
}

bool Xtras::removeFromValuesList(const int& intValue)
{
    bool ok = false;
    ok = mValues.removeOne(intValue);
    if (!ok) {
    	qDebug() << "int& not found in : mValues" << intValue;
    	return false;
    }
    emit removedFromValuesList(intValue);
    return true;
}
int Xtras::valuesCount()
{
    return mValues.size();
}
QList<int> Xtras::values()
{
    return mValues;
}
void Xtras::setValues(QList<int> values)
{
    if (values != mValues) {
        mValues = values;
        QVariantList variantList;
        for (int i = 0; i < values.size(); ++i) {
            variantList.append(values.at(i));
        }
        emit valuesListChanged(variantList);
    }
}
// access from QML to values
QVariantList Xtras::valuesList()
{
	QVariantList variantList;
    for (int i = 0; i < mValues.size(); ++i) {
        variantList.append(mValues.at(i));
    }
    return variantList;
}
void Xtras::setValuesList(const QVariantList& values) 
{
	mValues.clear();
    for (int i = 0; i < values.size(); ++i) {
        mValues.append(values.at(i).toInt());
    }
}


Xtras::~Xtras()
{
	// place cleanUp code here
}
	
