#include "Department.hpp"
#include <QDebug>
#include <quuid.h>
#include "Department.hpp"

// keys of QVariantMap used in this APP
static const QString idKey = "id";
static const QString uuidKey = "uuid";
static const QString nameKey = "name";
// no key for parentDep
static const QString childrenKey = "children";

// keys used from Server API etc
static const QString idForeignKey = "id";
static const QString uuidForeignKey = "uuid";
static const QString nameForeignKey = "name";
// no key for parentDep
static const QString childrenForeignKey = "children";

/*
 * Default Constructor if Department not initialized from QVariantMap
 */
Department::Department(QObject *parent) :
        QObject(parent), mId(-1), mUuid(""), mName("")
{
}

/*
 * initialize Department from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Department::fillFromMap(const QVariantMap& departmentMap)
{
	mId = departmentMap.value(idKey).toInt();
	mUuid = departmentMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mName = departmentMap.value(nameKey).toString();
	// mParentDep is parent (Department* containing Department)
	// mChildren is List of Department*
	QVariantList childrenList;
	childrenList = departmentMap.value(childrenKey).toList();
	mChildren.clear();
	for (int i = 0; i < childrenList.size(); ++i) {
		QVariantMap childrenMap;
		childrenMap = childrenList.at(i).toMap();
		Department* department = new Department();
		department->setParent(this);
		department->fillFromMap(childrenMap);
		mChildren.append(department);
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Department::fillFromForeignMap(const QVariantMap& departmentMap)
{
	mId = departmentMap.value(idForeignKey).toInt();
	mUuid = departmentMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mName = departmentMap.value(nameForeignKey).toString();
	// mParentDep is parent (Department* containing Department)
	// mChildren is List of Department*
	QVariantList childrenList;
	childrenList = departmentMap.value(childrenForeignKey).toList();
	mChildren.clear();
	for (int i = 0; i < childrenList.size(); ++i) {
		QVariantMap childrenMap;
		childrenMap = childrenList.at(i).toMap();
		Department* department = new Department();
		department->setParent(this);
		department->fillFromForeignMap(childrenMap);
		mChildren.append(department);
	}
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Department::fillFromCacheMap(const QVariantMap& departmentMap)
{
	mId = departmentMap.value(idKey).toInt();
	mUuid = departmentMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mName = departmentMap.value(nameKey).toString();
	// mParentDep is parent (Department* containing Department)
	// mChildren is List of Department*
	QVariantList childrenList;
	childrenList = departmentMap.value(childrenKey).toList();
	mChildren.clear();
	for (int i = 0; i < childrenList.size(); ++i) {
		QVariantMap childrenMap;
		childrenMap = childrenList.at(i).toMap();
		Department* department = new Department();
		department->setParent(this);
		department->fillFromCacheMap(childrenMap);
		mChildren.append(department);
	}
}

void Department::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Department::isValid()
{
	if (mId == -1) {
		return false;
	}
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Department as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Department::toMap()
{
	QVariantMap departmentMap;
	departmentMap.insert(idKey, mId);
	departmentMap.insert(uuidKey, mUuid);
	departmentMap.insert(nameKey, mName);
	// mParentDep points to Department* containing Department
	// mChildren points to Department*
	departmentMap.insert(childrenKey, childrenAsQVariantList());
	return departmentMap;
}

/*
 * Exports Properties from Department as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Department::toForeignMap()
{
	QVariantMap departmentMap;
	departmentMap.insert(idForeignKey, mId);
	departmentMap.insert(uuidForeignKey, mUuid);
	departmentMap.insert(nameForeignKey, mName);
	// mParentDep points to Department* containing Department
	// mChildren points to Department*
	departmentMap.insert(childrenForeignKey, childrenAsQVariantList());
	return departmentMap;
}


/*
 * Exports Properties from Department as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Department::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Mandatory: id
// Domain KEY: id
int Department::id() const
{
	return mId;
}

void Department::setId(int id)
{
	if (id != mId) {
		mId = id;
		emit idChanged(id);
	}
}
// ATT 
// Optional: uuid
QString Department::uuid() const
{
	return mUuid;
}

void Department::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Optional: name
QString Department::name() const
{
	return mName;
}

void Department::setName(QString name)
{
	if (name != mName) {
		mName = name;
		emit nameChanged(name);
	}
}
// REF
// Opposite: children
// Optional: parentDep
// No SETTER for ParentDep - it's the parent
Department* Department::parentDep() const
{
	return qobject_cast<Department*>(parent());
}
// REF
// Opposite: parentDep
// Optional: children
QVariantList Department::childrenAsQVariantList()
{
	QVariantList childrenList;
	for (int i = 0; i < mChildren.size(); ++i) {
        childrenList.append((mChildren.at(i))->toMap());
    }
	return childrenList;
}
/**
 * creates a new Department
 * parent is this Department
 * if data is successfully entered you must INVOKE addToChildren()
 * if edit was canceled you must undoCreateElementOfChildren to free up memory
 */
Department* Department::createElementOfChildren()
{
    Department* department;
    department = new Department();
    department->setParent(this);
    department->prepareNew();
    return department;
}

/**
 * if createElementOfChildren was canceled from UI
 * this method deletes the Object of Type Department
 * 
 * to delete a allready into children inserted  Department
 * you must use removeFromChildren
 */
void Department::undoCreateElementOfChildren(Department* department)
{
    if (department) {
        department->deleteLater();
        department = 0;
    }
}
void Department::addToChildren(Department* department)
{
    mChildren.append(department);
    emit addedToChildren(department);
}

bool Department::removeFromChildren(Department* department)
{
    bool ok = false;
    ok = mChildren.removeOne(department);
    if (!ok) {
    	qDebug() << "Department* not found in children";
    	return false;
    }
    emit removedFromChildrenByUuid(department->uuid());
    // children are contained - so we must delete them
    department->deleteLater();
    department = 0;
    return true;
}
void Department::clearChildren()
{
    for (int i = mChildren.size(); i > 0; --i) {
        removeFromChildren(mChildren.last());
    }
}
void Department::addToChildrenFromMap(const QVariantMap& departmentMap)
{
    Department* department = new Department();
    department->setParent(this);
    department->fillFromMap(departmentMap);
    mChildren.append(department);
    emit addedToChildren(department);
}
bool Department::removeFromChildrenByUuid(const QString& uuid)
{
    for (int i = 0; i < mChildren.size(); ++i) {
    	Department* department;
        department = mChildren.at(i);
        if (department->uuid() == uuid) {
        	mChildren.removeAt(i);
        	emit removedFromChildrenByUuid(uuid);
        	// children are contained - so we must delete them
        	department->deleteLater();
        	department = 0;
        	return true;
        }
    }
    qDebug() << "uuid not found in children: " << uuid;
    return false;
}

bool Department::removeFromChildrenById(const int& id)
{
    for (int i = 0; i < mChildren.size(); ++i) {
    	Department* department;
        department = mChildren.at(i);
        if (department->id() == id) {
        	mChildren.removeAt(i);
        	emit removedFromChildrenById(id);
        	// children are contained - so we must delete them
        	department->deleteLater();
        	department = 0;
        	return true;
        }
    }
    qDebug() << "id not found in children: " << id;
    return false;
}
int Department::childrenCount()
{
    return mChildren.size();
}
QList<Department*> Department::children()
{
	return mChildren;
}
void Department::setChildren(QList<Department*> children) 
{
	if (children != mChildren) {
		mChildren = children;
		emit childrenChanged(children);
	}
}
/**
 * to access lists from QML we're using QDeclarativeListProperty
 * and implement methods to append, count and clear
 * now from QML we can use
 * department.childrenPropertyList.length to get the size
 * department.childrenPropertyList[2] to get Department* at position 2
 * department.childrenPropertyList = [] to clear the list
 * or get easy access to properties like
 * department.childrenPropertyList[2].myPropertyName
 */
QDeclarativeListProperty<Department> Department::childrenPropertyList()
{
    return QDeclarativeListProperty<Department>(this, 0, &Department::appendToChildrenProperty,
            &Department::childrenPropertyCount, &Department::atChildrenProperty,
            &Department::clearChildrenProperty);
}
void Department::appendToChildrenProperty(QDeclarativeListProperty<Department> *childrenList,
        Department* department)
{
    Department *departmentObject = qobject_cast<Department *>(childrenList->object);
    if (departmentObject) {
		department->setParent(departmentObject);
        departmentObject->mChildren.append(department);
        emit departmentObject->addedToChildren(department);
    } else {
        qWarning() << "cannot append Department* to children " << "Object is not of type Department*";
    }
}
int Department::childrenPropertyCount(QDeclarativeListProperty<Department> *childrenList)
{
    Department *department = qobject_cast<Department *>(childrenList->object);
    if (department) {
        return department->mChildren.size();
    } else {
        qWarning() << "cannot get size children " << "Object is not of type Department*";
    }
    return 0;
}
Department* Department::atChildrenProperty(QDeclarativeListProperty<Department> *childrenList, int pos)
{
    Department *department = qobject_cast<Department *>(childrenList->object);
    if (department) {
        if (department->mChildren.size() > pos) {
            return department->mChildren.at(pos);
        }
        qWarning() << "cannot get Department* at pos " << pos << " size is "
                << department->mChildren.size();
    } else {
        qWarning() << "cannot get Department* at pos " << pos << "Object is not of type Department*";
    }
    return 0;
}
void Department::clearChildrenProperty(QDeclarativeListProperty<Department> *childrenList)
{
    Department *department = qobject_cast<Department *>(childrenList->object);
    if (department) {
        // children are contained - so we must delete them
        for (int i = 0; i < department->mChildren.size(); ++i) {
            department->mChildren.at(i)->deleteLater();
        }
        department->mChildren.clear();
    } else {
        qWarning() << "cannot clear children " << "Object is not of type Department*";
    }
}

// it's a Tree of Department*
// get a flat list of all children
QList<QObject*> Department::allDepartmentChildren(){
    QList<QObject*> allChildren;
    for (int i = 0; i < this->children().size(); ++i) {
        if (qobject_cast<Department*>(this->children().at(i))) {
            allChildren.append(this->children().at(i));
            Department* department = (Department*)this->children().at(i);
            allChildren.append(department->allDepartmentChildren());
        }
    }
    return allChildren;
}

Department::~Department()
{
	// place cleanUp code here
}
	
