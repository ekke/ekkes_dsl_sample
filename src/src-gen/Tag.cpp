#include "Tag.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString nameKey = "name";
static const QString valueKey = "value";
static const QString colorKey = "color";

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString nameForeignKey = "name";
static const QString valueForeignKey = "value";
static const QString colorForeignKey = "color";
// SQL
static bool queryPosInitialized;
static int uuidQueryPos;
static int nameQueryPos;
static int valueQueryPos;
static int colorQueryPos;

/*
 * Default Constructor if Tag not initialized from QVariantMap
 */
Tag::Tag(QObject *parent) :
        QObject(parent), mUuid(""), mName(""), mValue(0), mColor("")
{
}
// S Q L
const QString Tag::createTableCommand()
{
	QString createSQL = "CREATE TABLE tag (";
	// uuid
	createSQL.append(uuidKey).append(" TEXT");
	createSQL.append(" PRIMARY KEY");
	createSQL.append(", ");
	// name
	createSQL.append(nameKey).append(" TEXT");
	createSQL.append(", ");
	// value
	createSQL.append(valueKey).append(" INTEGER");
	createSQL.append(", ");
	// color
	createSQL.append(colorKey).append(" TEXT");
	createSQL.append(", ");
	//
    createSQL = createSQL.left(createSQL.length()-2);
    createSQL.append(");");
    return createSQL;
}
const QString Tag::createParameterizedInsertNameBinding()
{
	QString insertSQL;
    QString valueSQL;
    insertSQL = "INSERT INTO tag (";
    valueSQL = " VALUES (";
// uuid 
	insertSQL.append(uuidKey);
	insertSQL.append(", ");
	valueSQL.append(":");
	valueSQL.append(uuidKey);
	valueSQL.append(", ");
// name 
	insertSQL.append(nameKey);
	insertSQL.append(", ");
	valueSQL.append(":");
	valueSQL.append(nameKey);
	valueSQL.append(", ");
// value 
	insertSQL.append(valueKey);
	insertSQL.append(", ");
	valueSQL.append(":");
	valueSQL.append(valueKey);
	valueSQL.append(", ");
// color 
	insertSQL.append(colorKey);
	insertSQL.append(", ");
	valueSQL.append(":");
	valueSQL.append(colorKey);
	valueSQL.append(", ");
//
    insertSQL = insertSQL.left(insertSQL.length()-2);
    insertSQL.append(") ");
    valueSQL = valueSQL.left(valueSQL.length()-2);
    valueSQL.append(") ");
    insertSQL.append(valueSQL);
    return insertSQL;
}
const QString Tag::createParameterizedInsertPosBinding()
{
	QString insertSQL;
    QString valueSQL;
    insertSQL = "INSERT INTO tag (";
    valueSQL = " VALUES (";
// uuid 
	insertSQL.append(uuidKey);
	insertSQL.append(", ");
	valueSQL.append("?, ");
// name 
	insertSQL.append(nameKey);
	insertSQL.append(", ");
	valueSQL.append("?, ");
// value 
	insertSQL.append(valueKey);
	insertSQL.append(", ");
	valueSQL.append("?, ");
// color 
	insertSQL.append(colorKey);
	insertSQL.append(", ");
	valueSQL.append("?, ");
//
    insertSQL = insertSQL.left(insertSQL.length()-2);
    insertSQL.append(") ");
    valueSQL = valueSQL.left(valueSQL.length()-2);
    valueSQL.append(") ");
    insertSQL.append(valueSQL);
    return insertSQL;
}
/*
 * Exports Properties from Tag as QVariantLists
 * to insert into SQLite
 * 
 * To cache as JSON use toCacheMap()
 */
void Tag::toSqlCache(QVariantList& uuidList, QVariantList& nameList, QVariantList& valueList, QVariantList& colorList)
{
	uuidList << mUuid;
	nameList << mName;
	valueList << mValue;
	colorList << mColor;
}
void Tag::fillSqlQueryPos(const QSqlRecord& record)
{
    if(queryPosInitialized) {
        return;
    }
uuidQueryPos = record.indexOf(uuidKey);
nameQueryPos = record.indexOf(nameKey);
valueQueryPos = record.indexOf(valueKey);
colorQueryPos = record.indexOf(colorKey);
}
/*
 * initialize Tag from QSqlQuery
 * corresponding export method: toSqlMap()
 */
void Tag::fillFromSqlQuery(const QSqlQuery& sqlQuery)
{
	mUuid = sqlQuery.value(uuidQueryPos).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mName = sqlQuery.value(nameQueryPos).toString();
	mValue = sqlQuery.value(valueQueryPos).toInt();
	mColor = sqlQuery.value(colorQueryPos).toString();
}


/*
 * initialize Tag from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Tag::fillFromMap(const QVariantMap& tagMap)
{
	mUuid = tagMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mName = tagMap.value(nameKey).toString();
	mValue = tagMap.value(valueKey).toInt();
	mColor = tagMap.value(colorKey).toString();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Tag::fillFromForeignMap(const QVariantMap& tagMap)
{
	mUuid = tagMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mName = tagMap.value(nameForeignKey).toString();
	mValue = tagMap.value(valueForeignKey).toInt();
	mColor = tagMap.value(colorForeignKey).toString();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Tag::fillFromCacheMap(const QVariantMap& tagMap)
{
	mUuid = tagMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mName = tagMap.value(nameKey).toString();
	mValue = tagMap.value(valueKey).toInt();
	mColor = tagMap.value(colorKey).toString();
}

void Tag::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Tag::isValid()
{
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Tag as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Tag::toMap()
{
	QVariantMap tagMap;
	tagMap.insert(uuidKey, mUuid);
	tagMap.insert(nameKey, mName);
	tagMap.insert(valueKey, mValue);
	tagMap.insert(colorKey, mColor);
	return tagMap;
}

/*
 * Exports Properties from Tag as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Tag::toForeignMap()
{
	QVariantMap tagMap;
	tagMap.insert(uuidForeignKey, mUuid);
	tagMap.insert(nameForeignKey, mName);
	tagMap.insert(valueForeignKey, mValue);
	tagMap.insert(colorForeignKey, mColor);
	return tagMap;
}


/*
 * Exports Properties from Tag as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Tag::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Optional: uuid
QString Tag::uuid() const
{
	return mUuid;
}

void Tag::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Optional: name
QString Tag::name() const
{
	return mName;
}

void Tag::setName(QString name)
{
	if (name != mName) {
		mName = name;
		emit nameChanged(name);
	}
}
// ATT 
// Optional: value
int Tag::value() const
{
	return mValue;
}

void Tag::setValue(int value)
{
	if (value != mValue) {
		mValue = value;
		emit valueChanged(value);
	}
}
// ATT 
// Optional: color
QString Tag::color() const
{
	return mColor;
}

void Tag::setColor(QString color)
{
	if (color != mColor) {
		mColor = color;
		emit colorChanged(color);
	}
}


Tag::~Tag()
{
	// place cleanUp code here
}
	
