#ifndef ORDER_HPP_
#define ORDER_HPP_

#include <QObject>
#include <qvariant.h>
#include <QDeclarativeListProperty>
#include <QStringList>
#include <QDate>

#include "OrderState.hpp"

#include "Item.hpp"
#include "Info.hpp"
#include "Topic.hpp"
#include "Xtras.hpp"
#include "Customer.hpp"
#include "Department.hpp"
#include "Tag.hpp"


class Order: public QObject
{
	Q_OBJECT

	Q_PROPERTY(int nr READ nr WRITE setNr NOTIFY nrChanged FINAL)
	Q_PROPERTY(bool expressOrder READ expressOrder WRITE setExpressOrder NOTIFY expressOrderChanged FINAL)
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged FINAL)
	Q_PROPERTY(QDate orderDate READ orderDate WRITE setOrderDate NOTIFY orderDateChanged FINAL)
	// int ENUM OrderState
	Q_PROPERTY(int state READ state WRITE setState NOTIFY stateChanged FINAL)
	Q_PROPERTY(int processingState READ processingState WRITE setProcessingState NOTIFY processingStateChanged FINAL)
	Q_PROPERTY(Info* info READ info WRITE setInfo NOTIFY infoChanged FINAL)
	// topicId lazy pointing to Topic* (domainKey: id)
	Q_PROPERTY(int topicId READ topicId WRITE setTopicId NOTIFY topicIdChanged FINAL)
	Q_PROPERTY(Topic* topicIdAsDataObject READ topicIdAsDataObject WRITE resolveTopicIdAsDataObject NOTIFY topicIdAsDataObjectChanged FINAL)
	// extras lazy pointing to Xtras* (domainKey: id)
	Q_PROPERTY(int extras READ extras WRITE setExtras NOTIFY extrasChanged FINAL)
	Q_PROPERTY(Xtras* extrasAsDataObject READ extrasAsDataObject WRITE resolveExtrasAsDataObject NOTIFY extrasAsDataObjectChanged FINAL)
	// customerId lazy pointing to Customer* (domainKey: id)
	Q_PROPERTY(int customerId READ customerId WRITE setCustomerId NOTIFY customerIdChanged FINAL)
	Q_PROPERTY(Customer* customerIdAsDataObject READ customerIdAsDataObject WRITE resolveCustomerIdAsDataObject NOTIFY customerIdAsDataObjectChanged FINAL)
	// depId lazy pointing to Department* (domainKey: id)
	Q_PROPERTY(int depId READ depId WRITE setDepId NOTIFY depIdChanged FINAL)
	Q_PROPERTY(Department* depIdAsDataObject READ depIdAsDataObject WRITE resolveDepIdAsDataObject NOTIFY depIdAsDataObjectChanged FINAL)

	// QDeclarativeListProperty to get easy access from QML
	Q_PROPERTY(QDeclarativeListProperty<Item> positionsPropertyList READ positionsPropertyList CONSTANT)
	// QDeclarativeListProperty to get easy access from QML
	Q_PROPERTY(QDeclarativeListProperty<Tag> tagsPropertyList READ tagsPropertyList CONSTANT)
	Q_PROPERTY(QStringList domainsStringList READ domainsStringList  WRITE setDomainsStringList NOTIFY domainsStringListChanged FINAL)

public:
	Order(QObject *parent = 0);

	Q_INVOKABLE
	bool isAllResolved();

	void fillFromMap(const QVariantMap& orderMap);
	void fillFromForeignMap(const QVariantMap& orderMap);
	void fillFromCacheMap(const QVariantMap& orderMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	int nr() const;
	void setNr(int nr);
	bool expressOrder() const;
	void setExpressOrder(bool expressOrder);
	QString title() const;
	void setTitle(QString title);
	QDate orderDate() const;

	Q_INVOKABLE
	bool hasOrderDate();
	void setOrderDate(QDate orderDate);
	int state() const;
	void setState(int state);
	void setState(QString state);
	int processingState() const;
	void setProcessingState(int processingState);
	Info* info() const;
	void setInfo(Info* info);
	Q_INVOKABLE
	Info* createInfo();

	Q_INVOKABLE
	void undoCreateInfo(Info* info);
	
	Q_INVOKABLE
	void deleteInfo();
	
	Q_INVOKABLE
	bool hasInfo();
	
	// topicId lazy pointing to Topic* (domainKey: id)
	int topicId() const;
	void setTopicId(int topicId);
	Topic* topicIdAsDataObject() const;
	
	Q_INVOKABLE
	void resolveTopicIdAsDataObject(Topic* topic);
	
	Q_INVOKABLE
	void removeTopicId();
	
	Q_INVOKABLE
	bool hasTopicId();
	
	Q_INVOKABLE
	bool isTopicIdResolvedAsDataObject();
	
	Q_INVOKABLE
	void markTopicIdAsInvalid();
	
	// extras lazy pointing to Xtras* (domainKey: id)
	int extras() const;
	void setExtras(int extras);
	Xtras* extrasAsDataObject() const;
	
	Q_INVOKABLE
	void resolveExtrasAsDataObject(Xtras* xtras);
	
	Q_INVOKABLE
	void removeExtras();
	
	Q_INVOKABLE
	bool hasExtras();
	
	Q_INVOKABLE
	bool isExtrasResolvedAsDataObject();
	
	Q_INVOKABLE
	void markExtrasAsInvalid();
	
	// customerId lazy pointing to Customer* (domainKey: id)
	int customerId() const;
	void setCustomerId(int customerId);
	Customer* customerIdAsDataObject() const;
	
	Q_INVOKABLE
	void resolveCustomerIdAsDataObject(Customer* customer);
	
	Q_INVOKABLE
	void removeCustomerId();
	
	Q_INVOKABLE
	bool hasCustomerId();
	
	Q_INVOKABLE
	bool isCustomerIdResolvedAsDataObject();
	
	Q_INVOKABLE
	void markCustomerIdAsInvalid();
	
	// depId lazy pointing to Department* (domainKey: id)
	int depId() const;
	void setDepId(int depId);
	Department* depIdAsDataObject() const;
	
	Q_INVOKABLE
	void resolveDepIdAsDataObject(Department* department);
	
	Q_INVOKABLE
	void removeDepId();
	
	Q_INVOKABLE
	bool hasDepId();
	
	Q_INVOKABLE
	bool isDepIdResolvedAsDataObject();
	
	Q_INVOKABLE
	void markDepIdAsInvalid();
	

	
	Q_INVOKABLE
	QVariantList positionsAsQVariantList();

	Q_INVOKABLE
	Item* createElementOfPositions();

	Q_INVOKABLE
	void undoCreateElementOfPositions(Item* item);
	
	Q_INVOKABLE
	void addToPositions(Item* item);
	
	Q_INVOKABLE
	bool removeFromPositions(Item* item);

	Q_INVOKABLE
	void clearPositions();

	Q_INVOKABLE
	void addToPositionsFromMap(const QVariantMap& itemMap);
	
	Q_INVOKABLE
	bool removeFromPositionsByUuid(const QString& uuid);
	
	Q_INVOKABLE
	int positionsCount();
	
	 // access from C++ to positions
	QList<Item*> positions();
	void setPositions(QList<Item*> positions);
	// access from QML to positions
	QDeclarativeListProperty<Item> positionsPropertyList();
	
	Q_INVOKABLE
	QVariantList tagsAsQVariantList();

	
	Q_INVOKABLE
	void addToTags(Tag* tag);
	
	Q_INVOKABLE
	bool removeFromTags(Tag* tag);

	Q_INVOKABLE
	void clearTags();

	// lazy Array of independent Data Objects: only keys are persisted
	Q_INVOKABLE
	bool areTagsKeysResolved();

	Q_INVOKABLE
	QStringList tagsKeys();

	Q_INVOKABLE
	void resolveTagsKeys(QList<Tag*> tags);
	
	Q_INVOKABLE
	int tagsCount();
	
	 // access from C++ to tags
	QList<Tag*> tags();
	void setTags(QList<Tag*> tags);
	// access from QML to tags
	QDeclarativeListProperty<Tag> tagsPropertyList();
	
	Q_INVOKABLE
	void addToDomainsStringList(const QString& stringValue);
	
	Q_INVOKABLE
	bool removeFromDomainsStringList(const QString& stringValue);
	
	Q_INVOKABLE
	int domainsCount();
	
	QStringList domainsStringList();
	void setDomainsStringList(const QStringList& domains);


	virtual ~Order();

	Q_SIGNALS:

	void nrChanged(int nr);
	void expressOrderChanged(bool expressOrder);
	void titleChanged(QString title);
	void orderDateChanged(QDate orderDate);
	void stateChanged(int state);
	void processingStateChanged(int processingState);
	void infoChanged(Info* info);
	void infoDeleted(QString uuid);
	// topicId lazy pointing to Topic* (domainKey: id)
	void topicIdChanged(int topicId);
	void topicIdAsDataObjectChanged(Topic* topic);
	// extras lazy pointing to Xtras* (domainKey: id)
	void extrasChanged(int extras);
	void extrasAsDataObjectChanged(Xtras* xtras);
	// customerId lazy pointing to Customer* (domainKey: id)
	void customerIdChanged(int customerId);
	void customerIdAsDataObjectChanged(Customer* customer);
	// depId lazy pointing to Department* (domainKey: id)
	void depIdChanged(int depId);
	void depIdAsDataObjectChanged(Department* department);
	void positionsChanged(QList<Item*> positions);
	void addedToPositions(Item* item);
	void removedFromPositionsByUuid(QString uuid);
	
	void tagsChanged(QList<Tag*> tags);
	void addedToTags(Tag* tag);
	
	void domainsStringListChanged(QStringList domains);
	void addedToDomainsStringList(QString stringValue);
	void removedFromDomainsStringList(QString stringValue);
	

private:

	int mNr;
	bool mExpressOrder;
	QString mTitle;
	QDate mOrderDate;
	int mState;
	int stateStringToInt(QString state);
	int mProcessingState;
	Info* mInfo;
	int mTopicId;
	bool mTopicIdInvalid;
	Topic* mTopicIdAsDataObject;
	int mExtras;
	bool mExtrasInvalid;
	Xtras* mExtrasAsDataObject;
	int mCustomerId;
	bool mCustomerIdInvalid;
	Customer* mCustomerIdAsDataObject;
	int mDepId;
	bool mDepIdInvalid;
	Department* mDepIdAsDataObject;
	QList<Item*> mPositions;
	// implementation for QDeclarativeListProperty to use
	// QML functions for List of Item*
	static void appendToPositionsProperty(QDeclarativeListProperty<Item> *positionsList,
		Item* item);
	static int positionsPropertyCount(QDeclarativeListProperty<Item> *positionsList);
	static Item* atPositionsProperty(QDeclarativeListProperty<Item> *positionsList, int pos);
	static void clearPositionsProperty(QDeclarativeListProperty<Item> *positionsList);
	// lazy Array of independent Data Objects: only keys are persisted
	QStringList mTagsKeys;
	bool mTagsKeysResolved;
	QList<Tag*> mTags;
	// implementation for QDeclarativeListProperty to use
	// QML functions for List of Tag*
	static void appendToTagsProperty(QDeclarativeListProperty<Tag> *tagsList,
		Tag* tag);
	static int tagsPropertyCount(QDeclarativeListProperty<Tag> *tagsList);
	static Tag* atTagsProperty(QDeclarativeListProperty<Tag> *tagsList, int pos);
	static void clearTagsProperty(QDeclarativeListProperty<Tag> *tagsList);
	QStringList mDomainsStringList;

	Q_DISABLE_COPY (Order)
};
Q_DECLARE_METATYPE(Order*)

#endif /* ORDER_HPP_ */

