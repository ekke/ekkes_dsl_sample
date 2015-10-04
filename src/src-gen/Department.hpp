#ifndef DEPARTMENT_HPP_
#define DEPARTMENT_HPP_

#include <QObject>
#include <qvariant.h>
#include <QDeclarativeListProperty>


// forward declaration to avoid circular dependencies
class Department;


class Department: public QObject
{
	Q_OBJECT

	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged FINAL)
	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
	Q_PROPERTY(Department* parentDep READ parentDep)

	// QDeclarativeListProperty to get easy access from QML
	Q_PROPERTY(QDeclarativeListProperty<Department> childrenPropertyList READ childrenPropertyList CONSTANT)

public:
	Department(QObject *parent = 0);


	void fillFromMap(const QVariantMap& departmentMap);
	void fillFromForeignMap(const QVariantMap& departmentMap);
	void fillFromCacheMap(const QVariantMap& departmentMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	int id() const;
	void setId(int id);
	QString uuid() const;
	void setUuid(QString uuid);
	QString name() const;
	void setName(QString name);
	Department* parentDep() const;
	// no SETTER parentDep() is only convenience method to get the parent

	
	Q_INVOKABLE
	QVariantList childrenAsQVariantList();

	Q_INVOKABLE
	Department* createElementOfChildren();

	Q_INVOKABLE
	void undoCreateElementOfChildren(Department* department);
	
	Q_INVOKABLE
	void addToChildren(Department* department);
	
	Q_INVOKABLE
	bool removeFromChildren(Department* department);

	Q_INVOKABLE
	void clearChildren();

	Q_INVOKABLE
	void addToChildrenFromMap(const QVariantMap& departmentMap);
	
	Q_INVOKABLE
	bool removeFromChildrenByUuid(const QString& uuid);
	
	Q_INVOKABLE
	bool removeFromChildrenById(const int& id);
	
	Q_INVOKABLE
	int childrenCount();
	
	 // access from C++ to children
	QList<Department*> children();
	void setChildren(QList<Department*> children);
	// access from QML to children
	QDeclarativeListProperty<Department> childrenPropertyList();

	// tree with children of same type - get all as flat list
	QList<QObject*> allDepartmentChildren();

	virtual ~Department();

	Q_SIGNALS:

	void idChanged(int id);
	void uuidChanged(QString uuid);
	void nameChanged(QString name);
	// no SIGNAL parentDep is only convenience way to get the parent
	void childrenChanged(QList<Department*> children);
	void addedToChildren(Department* department);
	void removedFromChildrenByUuid(QString uuid);
	void removedFromChildrenById(int id);
	
	

private:

	int mId;
	QString mUuid;
	QString mName;
	// no MEMBER mParentDep it's the parent
	QList<Department*> mChildren;
	// implementation for QDeclarativeListProperty to use
	// QML functions for List of Department*
	static void appendToChildrenProperty(QDeclarativeListProperty<Department> *childrenList,
		Department* department);
	static int childrenPropertyCount(QDeclarativeListProperty<Department> *childrenList);
	static Department* atChildrenProperty(QDeclarativeListProperty<Department> *childrenList, int pos);
	static void clearChildrenProperty(QDeclarativeListProperty<Department> *childrenList);

	Q_DISABLE_COPY (Department)
};
Q_DECLARE_METATYPE(Department*)

#endif /* DEPARTMENT_HPP_ */

