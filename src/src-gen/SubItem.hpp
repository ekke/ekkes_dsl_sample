#ifndef SUBITEM_HPP_
#define SUBITEM_HPP_

#include <QObject>
#include <qvariant.h>


// forward declaration to avoid circular dependencies
class Item;


class SubItem: public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(int subPosNr READ subPosNr WRITE setSubPosNr NOTIFY subPosNrChanged FINAL)
	Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged FINAL)
	Q_PROPERTY(QString barcode READ barcode WRITE setBarcode NOTIFY barcodeChanged FINAL)
	Q_PROPERTY(Item* item READ item)


public:
	SubItem(QObject *parent = 0);


	void fillFromMap(const QVariantMap& subItemMap);
	void fillFromForeignMap(const QVariantMap& subItemMap);
	void fillFromCacheMap(const QVariantMap& subItemMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	QString uuid() const;
	void setUuid(QString uuid);
	int subPosNr() const;
	void setSubPosNr(int subPosNr);
	QString description() const;
	void setDescription(QString description);
	QString barcode() const;
	void setBarcode(QString barcode);
	Item* item() const;
	// no SETTER item() is only convenience method to get the parent



	virtual ~SubItem();

	Q_SIGNALS:

	void uuidChanged(QString uuid);
	void subPosNrChanged(int subPosNr);
	void descriptionChanged(QString description);
	void barcodeChanged(QString barcode);
	// no SIGNAL item is only convenience way to get the parent
	

private:

	QString mUuid;
	int mSubPosNr;
	QString mDescription;
	QString mBarcode;
	// no MEMBER mItem it's the parent

	Q_DISABLE_COPY (SubItem)
};
Q_DECLARE_METATYPE(SubItem*)

#endif /* SUBITEM_HPP_ */

