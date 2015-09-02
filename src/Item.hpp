#ifndef ITEM_HPP_
#define ITEM_HPP_

#include <QObject>
#include <qvariant.h>
#include <QDeclarativeListProperty>


// forward declaration to avoid circular dependencies
class Order;
#include "SubItem.hpp"


class Item: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(int posNr READ posNr WRITE setPosNr NOTIFY posNrChanged FINAL)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
	Q_PROPERTY(double quantity READ quantity WRITE setQuantity NOTIFY quantityChanged FINAL)
	Q_PROPERTY(Order* order READ order)

	// QDeclarativeListProperty to get easy access from QML
	Q_PROPERTY(QDeclarativeListProperty<SubItem> subItemsPropertyList READ subItemsPropertyList CONSTANT)

public:
	Item(QObject *parent = 0);


	void fillFromMap(const QVariantMap& itemMap);
	void fillFromForeignMap(const QVariantMap& itemMap);
	void fillFromCacheMap(const QVariantMap& itemMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	QString uuid() const;
	void setUuid(QString uuid);
	int posNr() const;
	void setPosNr(int posNr);
	QString name() const;
	void setName(QString name);
	double quantity() const;
	void setQuantity(double quantity);
	Order* order() const;
	// no SETTER order() is only convenience method to get the parent

	
	Q_INVOKABLE
	QVariantList subItemsAsQVariantList();

	Q_INVOKABLE
	SubItem* createElementOfSubItems();

	Q_INVOKABLE
	void undoCreateElementOfSubItems(SubItem* subItem);
	
	Q_INVOKABLE
	void addToSubItems(SubItem* subItem);
	
	Q_INVOKABLE
	bool removeFromSubItems(SubItem* subItem);

	Q_INVOKABLE
	void clearSubItems();

	Q_INVOKABLE
	void addToSubItemsFromMap(const QVariantMap& subItemMap);
	
	Q_INVOKABLE
	bool removeFromSubItemsByUuid(const QString& uuid);
	
	Q_INVOKABLE
	int subItemsCount();
	
	 // access from C++ to subItems
	QList<SubItem*> subItems();
	void setSubItems(QList<SubItem*> subItems);
	// access from QML to subItems
	QDeclarativeListProperty<SubItem> subItemsPropertyList();


	virtual ~Item();

	Q_SIGNALS:

	void uuidChanged(QString uuid);
	void posNrChanged(int posNr);
	void nameChanged(QString name);
	void quantityChanged(double quantity);
	// no SIGNAL order is only convenience way to get the parent
	void subItemsChanged(QList<SubItem*> subItems);
	void addedToSubItems(SubItem* subItem);
	void removedFromSubItemsByUuid(QString uuid);
	
	

private:

	QString mUuid;
	int mPosNr;
	QString mName;
	double mQuantity;
	// no MEMBER mOrder it's the parent
	QList<SubItem*> mSubItems;
	// implementation for QDeclarativeListProperty to use
	// QML functions for List of SubItem*
	static void appendToSubItemsProperty(QDeclarativeListProperty<SubItem> *subItemsList,
		SubItem* subItem);
	static int subItemsPropertyCount(QDeclarativeListProperty<SubItem> *subItemsList);
	static SubItem* atSubItemsProperty(QDeclarativeListProperty<SubItem> *subItemsList, int pos);
	static void clearSubItemsProperty(QDeclarativeListProperty<SubItem> *subItemsList);

	Q_DISABLE_COPY (Item)
};
Q_DECLARE_METATYPE(Item*)

#endif /* ITEM_HPP_ */

