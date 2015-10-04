#include <QObject>

#include "DataManager.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/AbstractPane>
#include <bb/data/JsonDataAccess>
#include <bb/cascades/GroupDataModel>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

static QString dbName = "sqlcache.db";

static QString dataAssetsPath(const QString& fileName)
{
    return QDir::currentPath() + "/app/native/assets/datamodel/" + fileName;
}
static QString dataPath(const QString& fileName)
{
    return QDir::currentPath() + "/data/" + fileName;
}
static QString cacheOrder = "cacheOrder.json";
static QString cacheCustomer = "cacheCustomer.json";
static QString cacheTopic = "cacheTopic.json";
// cacheDepartment is tree of  Department
// there's also a flat list (in memory only) useful for easy filtering
static QString cacheDepartment = "cacheDepartment.json";
static QString cacheTag = "cacheTag.json";
static QString cacheXtras = "cacheXtras.json";

using namespace bb::cascades;
using namespace bb::data;

DataManager::DataManager(QObject *parent) :
        QObject(parent)
{
    // ApplicationUI is parent of DataManager
    // DataManager is parent of all root DataObjects
    // ROOT DataObjects are parent of contained DataObjects
    // ROOT:
    // Order
    // Customer
    // Topic
    // Department
    // Tag
    // Xtras

    // register all DataObjects to get access to properties from QML:
	qmlRegisterType<Order>("org.ekkescorner.data", 1, 0, "Order");
	qmlRegisterType<Item>("org.ekkescorner.data", 1, 0, "Item");
	qmlRegisterType<SubItem>("org.ekkescorner.data", 1, 0, "SubItem");
	qmlRegisterType<Customer>("org.ekkescorner.data", 1, 0, "Customer");
	qmlRegisterType<Info>("org.ekkescorner.data", 1, 0, "Info");
	qmlRegisterType<Topic>("org.ekkescorner.data", 1, 0, "Topic");
	qmlRegisterType<Department>("org.ekkescorner.data", 1, 0, "Department");
	qmlRegisterType<Tag>("org.ekkescorner.data", 1, 0, "Tag");
	qmlRegisterType<Xtras>("org.ekkescorner.data", 1, 0, "Xtras");
	// QGeo... classes wrapped as QObject* to be able to access via Q_PROPERTY
	qmlRegisterType<GeoCoordinate>("org.ekkescorner.data", 1, 0, "GeoCoordinate");
	qmlRegisterType<GeoAddress>("org.ekkescorner.data", 1, 0, "GeoAddress");
	// register all ENUMs to get access from QML
	qmlRegisterType<OrderState>("org.ekkescorner.enums", 1, 0, "OrderState");
	// useful Types for all APPs dealing with data
	// QTimer
	qmlRegisterType<QTimer>("org.ekkescorner.common", 1, 0, "QTimer");

	// no auto exit: we must persist the cache before
    bb::Application::instance()->setAutoExit(false);
    bool res = QObject::connect(bb::Application::instance(), SIGNAL(manualExit()), this, SLOT(onManualExit()));
    Q_ASSERT(res);


    Q_UNUSED(res);
}

/*
 * loads all data from cache.
 * called from main.qml with delay using QTimer
 * Data with 2PhaseInit Caching Policy will only
 * load priority records needed to resolve from others
 */
void DataManager::init()
{
	// SQL init the sqlite database
	mDatabaseAvailable = initDatabase();
	qDebug() << "SQLite created or opened ? " << mDatabaseAvailable;

    initOrderFromCache();
    initCustomerFromCache();
    initTopicFromCache();
    initDepartmentFromCache();
    initTagFromSqlCache();
    initXtrasFromCache();
}


//  S Q L
/**
 * OPENs the DATABASE FILE
 *
 * Also initialized the DATABASE CONNECTION (SqlDataAccess),
 * we're reusing for all SQL commands on this database
 */
bool DataManager::initDatabase()
{
    mChunkSize = 10000;
    QString pathname;
    pathname = dataPath(dbName);
    QFile dataFile(pathname);
    if (!dataFile.exists()) {
        // of there's a default at assets we copy it - otherwise a new db will be created later
        QFile assetDataFile(dataAssetsPath(dbName));
        if (assetDataFile.exists()) {
            // copy file from assets to data
            bool copyOk = assetDataFile.copy(pathname);
            if (!copyOk) {
                qDebug() << "cannot copy dataAssetsPath(fileName) to dataPath(fileName)";
            }
        }
    }
    //
    mDatabase = QSqlDatabase::addDatabase("QSQLITE");
    mDatabase.setDatabaseName(dataPath(dbName));
    if (mDatabase.open() == false) {
        const QSqlError error = mDatabase.lastError();
        // you should notify the user !
        qWarning() << "Cannot open " << dbName << ":" << error.text();
        return false;
    }
    qDebug() << "Database opened: " << dbName;
    return true;
}

void DataManager::setChunkSize(const int& newChunkSize)
{
    mChunkSize = newChunkSize;
}

/**
 * tune PRAGMA synchronous and journal_mode for better speed with bulk import
 * see https://www.sqlite.org/pragma.html
 * and http://stackoverflow.com/questions/1711631/improve-insert-per-second-performance-of-sqlite
 *
 * PRAGMA database.synchronous = 0 | OFF | 1 | NORMAL | 2 | FULL;
 * default: FULL
 *
 * PRAGMA database.journal_mode = DELETE | TRUNCATE | PERSIST | MEMORY | WAL | OFF
 * default: DELETE
 */
void DataManager::bulkImport(const bool& tuneJournalAndSync)
{
    QSqlQuery query (mDatabase);
    bool success;
    QString journalMode;
    QString syncMode;
    query.prepare("PRAGMA journal_mode");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA journal_mode";
        return;
    }
    success = query.first();
    if(!success) {
        qWarning() << "NO RESULT PRAGMA journal_mode";
        return;
    }
    journalMode = query.value(0).toString();
    //
    query.clear();
    query.prepare("PRAGMA synchronous");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA synchronous";
        return;
    }
        success = query.first();
    if(!success) {
        qWarning() << "NO RESULT PRAGMA synchronous";
        return;
    }
    switch (query.value(0).toInt()) {
        case 0:
            syncMode = "OFF";
            break;
        case 1:
            syncMode = "NORMAL";
            break;
        case 2:
            syncMode = "FULL";
            break;
        default:
            syncMode = query.value(0).toString();
            break;
    }
    qDebug() << "PRAGMA current values - " << "journal: " << journalMode << " synchronous: " << syncMode;
    //
    query.clear();
    if (tuneJournalAndSync) {
        query.prepare("PRAGMA journal_mode = MEMORY");
    } else {
        query.prepare("PRAGMA journal_mode = DELETE");
    }
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA set journal_mode";
        return;
    }
    //
    query.prepare("PRAGMA journal_mode");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA journal_mode";
        return;
    }
    success = query.first();
    if(!success) {
        qWarning() << "NO RESULT PRAGMA journal_mode";
        return;
    }
    qDebug() << "PRAGMA NEW VALUE journal_mode: " << query.value(0).toString();
    //
    query.clear();
    if (tuneJournalAndSync) {
        query.prepare("PRAGMA synchronous = OFF");
    } else {
        query.prepare("PRAGMA synchronous = FULL");
    }
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA set synchronous";
        return;
    }
    //
    query.prepare("PRAGMA synchronous");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS PRAGMA synchronous";
        return;
    }
    success = query.first();
    if(!success) {
        qWarning() << "NO RESULT PRAGMA synchronous";
        return;
    }
    switch (query.value(0).toInt()) {
        case 0:
            syncMode = "OFF";
            break;
        case 1:
            syncMode = "NORMAL";
            break;
        case 2:
            syncMode = "FULL";
            break;
        default:
            syncMode = query.value(0).toString();
            break;
    }
    qDebug() << "PRAGMA synchronous NEW VALUE: " << syncMode;
}

void DataManager::finish()
{
    saveOrderToCache();
    saveCustomerToCache();
    saveTopicToCache();
    // Department is read-only - not saved to cache
    // Tag is read-only - not saved to cache
    saveXtrasToCache();
}

/*
 * reads Maps of Order in from JSON cache
 * creates List of Order*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initOrderFromCache()
{
	qDebug() << "start initOrderFromCache";
    mAllOrder.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheOrder);
    qDebug() << "read Order from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Order* order = new Order();
        // Important: DataManager must be parent of all root DTOs
        order->setParent(this);
        order->fillFromCacheMap(cacheMap);
        mAllOrder.append(order);
    }
    qDebug() << "created Order* #" << mAllOrder.size();
}


/*
 * save List of Order* to JSON cache
 * convert list of Order* to QVariantList
 * toCacheMap stores all properties without transient values
 */
void DataManager::saveOrderToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Order* #" << mAllOrder.size();
    for (int i = 0; i < mAllOrder.size(); ++i) {
        Order* order;
        order = (Order*)mAllOrder.at(i);
        QVariantMap cacheMap;
        cacheMap = order->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Order* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheOrder, cacheList);
}


void DataManager::resolveOrderReferences(Order* order)
{
	if (!order) {
        qDebug() << "cannot resolveOrderReferences with order NULL";
        return;
    }
    if(order->isAllResolved()) {
	    qDebug() << "nothing to do: all is resolved";
	    return;
	}
    if (order->hasTopicId() && !order->isTopicIdResolvedAsDataObject()) {
    	Topic* topicId;
   		topicId = findTopicById(order->topicId());
    	if (topicId) {
    		order->resolveTopicIdAsDataObject(topicId);
    	} else {
    		qDebug() << "markTopicIdAsInvalid: " << order->topicId();
    		order->markTopicIdAsInvalid();
    	}
    }
    if (order->hasExtras() && !order->isExtrasResolvedAsDataObject()) {
    	Xtras* extras;
   		extras = findXtrasById(order->extras());
    	if (extras) {
    		order->resolveExtrasAsDataObject(extras);
    	} else {
    		qDebug() << "markExtrasAsInvalid: " << order->extras();
    		order->markExtrasAsInvalid();
    	}
    }
    if (order->hasCustomerId() && !order->isCustomerIdResolvedAsDataObject()) {
    	Customer* customerId;
   		customerId = findCustomerById(order->customerId());
    	if (customerId) {
    		order->resolveCustomerIdAsDataObject(customerId);
    	} else {
    		qDebug() << "markCustomerIdAsInvalid: " << order->customerId();
    		order->markCustomerIdAsInvalid();
    	}
    }
    if (order->hasDepId() && !order->isDepIdResolvedAsDataObject()) {
    	Department* depId;
   		depId = findDepartmentById(order->depId());
    	if (depId) {
    		order->resolveDepIdAsDataObject(depId);
    	} else {
    		qDebug() << "markDepIdAsInvalid: " << order->depId();
    		order->markDepIdAsInvalid();
    	}
    }
    if (!order->areTagsKeysResolved()) {
        order->resolveTagsKeys(
                listOfTagForKeys(order->tagsKeys()));
    }
}
void DataManager::resolveReferencesForAllOrder()
{
    for (int i = 0; i < mAllOrder.size(); ++i) {
        Order* order;
        order = (Order*)mAllOrder.at(i);
    	resolveOrderReferences(order);
    }
}
/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Order*> DataManager::listOfOrderForKeys(
        QStringList keyList)
{
    QList<Order*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllOrder.size(); ++i) {
        Order* order;
        order = (Order*) mAllOrder.at(i);
        if (keyList.contains(QString::number(order->nr()))) {
            listOfData.append(order);
            keyList.removeOne(QString::number(order->nr()));
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Order: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::orderAsQVariantList()
{
    QVariantList orderList;
    for (int i = 0; i < mAllOrder.size(); ++i) {
        orderList.append(((Order*) (mAllOrder.at(i)))->toMap());
    }
    return orderList;
}

QList<QObject*> DataManager::allOrder()
{
    return mAllOrder;
}

QDeclarativeListProperty<Order> DataManager::orderPropertyList()
{
    return QDeclarativeListProperty<Order>(this, 0,
            &DataManager::appendToOrderProperty, &DataManager::orderPropertyCount,
            &DataManager::atOrderProperty, &DataManager::clearOrderProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Order*
void DataManager::appendToOrderProperty(
        QDeclarativeListProperty<Order> *orderList,
        Order* order)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(orderList->object);
    if (dataManagerObject) {
        order->setParent(dataManagerObject);
        dataManagerObject->mAllOrder.append(order);
        emit dataManagerObject->addedToAllOrder(order);
    } else {
        qWarning() << "cannot append Order* to mAllOrder "
                << "Object is not of type DataManager*";
    }
}
int DataManager::orderPropertyCount(
        QDeclarativeListProperty<Order> *orderList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(orderList->object);
    if (dataManager) {
        return dataManager->mAllOrder.size();
    } else {
        qWarning() << "cannot get size mAllOrder " << "Object is not of type DataManager*";
    }
    return 0;
}
Order* DataManager::atOrderProperty(
        QDeclarativeListProperty<Order> *orderList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(orderList->object);
    if (dataManager) {
        if (dataManager->mAllOrder.size() > pos) {
            return (Order*) dataManager->mAllOrder.at(pos);
        }
        qWarning() << "cannot get Order* at pos " << pos << " size is "
                << dataManager->mAllOrder.size();
    } else {
        qWarning() << "cannot get Order* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearOrderProperty(
        QDeclarativeListProperty<Order> *orderList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(orderList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllOrder.size(); ++i) {
            Order* order;
            order = (Order*) dataManager->mAllOrder.at(i);
			emit dataManager->deletedFromAllOrderByNr(order->nr());
			emit dataManager->deletedFromAllOrder(order);
            order->deleteLater();
            order = 0;
        }
        dataManager->mAllOrder.clear();
    } else {
        qWarning() << "cannot clear mAllOrder " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Order
 * and clears the list
 */
void DataManager::deleteOrder()
{
    for (int i = 0; i < mAllOrder.size(); ++i) {
        Order* order;
        order = (Order*) mAllOrder.at(i);
        emit deletedFromAllOrderByNr(order->nr());
		emit deletedFromAllOrder(order);
        order->deleteLater();
        order = 0;
     }
     mAllOrder.clear();
}

/**
 * creates a new Order
 * parent is DataManager
 * if data is successfully entered you must insertOrder
 * if edit was canceled you must undoCreateOrder to free up memory
 */
Order* DataManager::createOrder()
{
    Order* order;
    order = new Order();
    order->setParent(this);
    order->prepareNew();
    return order;
}

/**
 * deletes Order
 * if createOrder was canceled from UI
 * to delete a previous successfully inserted Order
 * use deleteOrder
 */
void DataManager::undoCreateOrder(Order* order)
{
    if (order) {
        qDebug() << "undoCreateOrder " << order->nr();
        order->deleteLater();
        order = 0;
    }
}

void DataManager::insertOrder(Order* order)
{
    // Important: DataManager must be parent of all root DTOs
    order->setParent(this);
    mAllOrder.append(order);
    emit addedToAllOrder(order);
}

void DataManager::insertOrderFromMap(const QVariantMap& orderMap,
        const bool& useForeignProperties)
{
    Order* order = new Order();
    order->setParent(this);
    if (useForeignProperties) {
        order->fillFromForeignMap(orderMap);
    } else {
        order->fillFromMap(orderMap);
    }
    mAllOrder.append(order);
    emit addedToAllOrder(order);
}

bool DataManager::deleteOrder(Order* order)
{
    bool ok = false;
    ok = mAllOrder.removeOne(order);
    if (!ok) {
        return ok;
    }
    emit deletedFromAllOrderByNr(order->nr());
    emit deletedFromAllOrder(order);
    order->deleteLater();
    order = 0;
    return ok;
}


bool DataManager::deleteOrderByNr(const int& nr)
{
    for (int i = 0; i < mAllOrder.size(); ++i) {
        Order* order;
        order = (Order*) mAllOrder.at(i);
        if (order->nr() == nr) {
            mAllOrder.removeAt(i);
            emit deletedFromAllOrderByNr(nr);
            emit deletedFromAllOrder(order);
            order->deleteLater();
            order = 0;
            return true;
        }
    }
    return false;
}

void DataManager::fillOrderDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllOrder.size(); ++i) {
            	theList.append(mAllOrder.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Order for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInOrderDataModel(QString objectName,
        Order* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Order Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromOrderDataModel(QString objectName, Order* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Order Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoOrderDataModel(QString objectName, Order* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

// nr is DomainKey
Order* DataManager::findOrderByNr(const int& nr){
    for (int i = 0; i < mAllOrder.size(); ++i) {
        Order* order;
        order = (Order*)mAllOrder.at(i);
        if(order->nr() == nr){
            return order;
        }
    }
    qDebug() << "no Order found for nr " << nr;
    return 0;
}
/*
 * reads Maps of Customer in from JSON cache
 * creates List of Customer*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initCustomerFromCache()
{
	qDebug() << "start initCustomerFromCache";
    mAllCustomer.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheCustomer);
    qDebug() << "read Customer from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Customer* customer = new Customer();
        // Important: DataManager must be parent of all root DTOs
        customer->setParent(this);
        customer->fillFromCacheMap(cacheMap);
        mAllCustomer.append(customer);
    }
    qDebug() << "created Customer* #" << mAllCustomer.size();
}


/*
 * save List of Customer* to JSON cache
 * convert list of Customer* to QVariantList
 * toCacheMap stores all properties without transient values
 */
void DataManager::saveCustomerToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Customer* #" << mAllCustomer.size();
    for (int i = 0; i < mAllCustomer.size(); ++i) {
        Customer* customer;
        customer = (Customer*)mAllCustomer.at(i);
        QVariantMap cacheMap;
        cacheMap = customer->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Customer* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheCustomer, cacheList);
}

/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Customer*> DataManager::listOfCustomerForKeys(
        QStringList keyList)
{
    QList<Customer*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllCustomer.size(); ++i) {
        Customer* customer;
        customer = (Customer*) mAllCustomer.at(i);
        if (keyList.contains(QString::number(customer->id()))) {
            listOfData.append(customer);
            keyList.removeOne(QString::number(customer->id()));
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Customer: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::customerAsQVariantList()
{
    QVariantList customerList;
    for (int i = 0; i < mAllCustomer.size(); ++i) {
        customerList.append(((Customer*) (mAllCustomer.at(i)))->toMap());
    }
    return customerList;
}

QList<QObject*> DataManager::allCustomer()
{
    return mAllCustomer;
}

QDeclarativeListProperty<Customer> DataManager::customerPropertyList()
{
    return QDeclarativeListProperty<Customer>(this, 0,
            &DataManager::appendToCustomerProperty, &DataManager::customerPropertyCount,
            &DataManager::atCustomerProperty, &DataManager::clearCustomerProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Customer*
void DataManager::appendToCustomerProperty(
        QDeclarativeListProperty<Customer> *customerList,
        Customer* customer)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(customerList->object);
    if (dataManagerObject) {
        customer->setParent(dataManagerObject);
        dataManagerObject->mAllCustomer.append(customer);
        emit dataManagerObject->addedToAllCustomer(customer);
    } else {
        qWarning() << "cannot append Customer* to mAllCustomer "
                << "Object is not of type DataManager*";
    }
}
int DataManager::customerPropertyCount(
        QDeclarativeListProperty<Customer> *customerList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(customerList->object);
    if (dataManager) {
        return dataManager->mAllCustomer.size();
    } else {
        qWarning() << "cannot get size mAllCustomer " << "Object is not of type DataManager*";
    }
    return 0;
}
Customer* DataManager::atCustomerProperty(
        QDeclarativeListProperty<Customer> *customerList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(customerList->object);
    if (dataManager) {
        if (dataManager->mAllCustomer.size() > pos) {
            return (Customer*) dataManager->mAllCustomer.at(pos);
        }
        qWarning() << "cannot get Customer* at pos " << pos << " size is "
                << dataManager->mAllCustomer.size();
    } else {
        qWarning() << "cannot get Customer* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearCustomerProperty(
        QDeclarativeListProperty<Customer> *customerList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(customerList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllCustomer.size(); ++i) {
            Customer* customer;
            customer = (Customer*) dataManager->mAllCustomer.at(i);
			emit dataManager->deletedFromAllCustomerById(customer->id());
			emit dataManager->deletedFromAllCustomer(customer);
            customer->deleteLater();
            customer = 0;
        }
        dataManager->mAllCustomer.clear();
    } else {
        qWarning() << "cannot clear mAllCustomer " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Customer
 * and clears the list
 */
void DataManager::deleteCustomer()
{
    for (int i = 0; i < mAllCustomer.size(); ++i) {
        Customer* customer;
        customer = (Customer*) mAllCustomer.at(i);
        emit deletedFromAllCustomerById(customer->id());
		emit deletedFromAllCustomer(customer);
        customer->deleteLater();
        customer = 0;
     }
     mAllCustomer.clear();
}

/**
 * creates a new Customer
 * parent is DataManager
 * if data is successfully entered you must insertCustomer
 * if edit was canceled you must undoCreateCustomer to free up memory
 */
Customer* DataManager::createCustomer()
{
    Customer* customer;
    customer = new Customer();
    customer->setParent(this);
    customer->prepareNew();
    return customer;
}

/**
 * deletes Customer
 * if createCustomer was canceled from UI
 * to delete a previous successfully inserted Customer
 * use deleteCustomer
 */
void DataManager::undoCreateCustomer(Customer* customer)
{
    if (customer) {
        qDebug() << "undoCreateCustomer " << customer->id();
        customer->deleteLater();
        customer = 0;
    }
}

void DataManager::insertCustomer(Customer* customer)
{
    // Important: DataManager must be parent of all root DTOs
    customer->setParent(this);
    mAllCustomer.append(customer);
    emit addedToAllCustomer(customer);
}

void DataManager::insertCustomerFromMap(const QVariantMap& customerMap,
        const bool& useForeignProperties)
{
    Customer* customer = new Customer();
    customer->setParent(this);
    if (useForeignProperties) {
        customer->fillFromForeignMap(customerMap);
    } else {
        customer->fillFromMap(customerMap);
    }
    mAllCustomer.append(customer);
    emit addedToAllCustomer(customer);
}

bool DataManager::deleteCustomer(Customer* customer)
{
    bool ok = false;
    ok = mAllCustomer.removeOne(customer);
    if (!ok) {
        return ok;
    }
    emit deletedFromAllCustomerById(customer->id());
    emit deletedFromAllCustomer(customer);
    customer->deleteLater();
    customer = 0;
    return ok;
}


bool DataManager::deleteCustomerById(const int& id)
{
    for (int i = 0; i < mAllCustomer.size(); ++i) {
        Customer* customer;
        customer = (Customer*) mAllCustomer.at(i);
        if (customer->id() == id) {
            mAllCustomer.removeAt(i);
            emit deletedFromAllCustomerById(id);
            emit deletedFromAllCustomer(customer);
            customer->deleteLater();
            customer = 0;
            return true;
        }
    }
    return false;
}

void DataManager::fillCustomerDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllCustomer.size(); ++i) {
            	theList.append(mAllCustomer.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Customer for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInCustomerDataModel(QString objectName,
        Customer* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Customer Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromCustomerDataModel(QString objectName, Customer* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Customer Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoCustomerDataModel(QString objectName, Customer* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::fillCustomerDataModelByCompanyName(QString objectName, const QString& companyName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            QList<QObject*> theList;
            for (int i = 0; i < mAllCustomer.size(); ++i) {
                Customer* customer;
                customer = (Customer*) mAllCustomer.at(i);
                if (customer->companyName() == companyName) {
                    theList.append(mAllCustomer.at(i));
                }
            }
            dataModel->clear();
            dataModel->insertList(theList);
            qDebug() << "fillCustomerDataModelByCompanyName " << companyName << " (" << objectName << ") #"
                    << theList.size();
            return;
        }
    }
    qDebug() << "NO GRP DATA FOUND Customer for " << objectName;
}

// nr is DomainKey
Customer* DataManager::findCustomerById(const int& id){
    for (int i = 0; i < mAllCustomer.size(); ++i) {
        Customer* customer;
        customer = (Customer*)mAllCustomer.at(i);
        if(customer->id() == id){
            return customer;
        }
    }
    qDebug() << "no Customer found for id " << id;
    return 0;
}
/*
 * reads Maps of Topic in from JSON cache
 * creates List of Topic*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initTopicFromCache()
{
	qDebug() << "start initTopicFromCache";
    mAllTopic.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheTopic);
    qDebug() << "read Topic from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Topic* topic = new Topic();
        // Important: DataManager must be parent of all root DTOs
        topic->setParent(this);
        topic->fillFromCacheMap(cacheMap);
        mAllTopic.append(topic);
    }
    qDebug() << "created Topic* #" << mAllTopic.size();
}


/*
 * save List of Topic* to JSON cache
 * convert list of Topic* to QVariantList
 * toCacheMap stores all properties without transient values
 */
void DataManager::saveTopicToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Topic* #" << mAllTopic.size();
    for (int i = 0; i < mAllTopic.size(); ++i) {
        Topic* topic;
        topic = (Topic*)mAllTopic.at(i);
        QVariantMap cacheMap;
        cacheMap = topic->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Topic* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheTopic, cacheList);
}

/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Topic*> DataManager::listOfTopicForKeys(
        QStringList keyList)
{
    QList<Topic*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllTopic.size(); ++i) {
        Topic* topic;
        topic = (Topic*) mAllTopic.at(i);
        if (keyList.contains(QString::number(topic->id()))) {
            listOfData.append(topic);
            keyList.removeOne(QString::number(topic->id()));
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Topic: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::topicAsQVariantList()
{
    QVariantList topicList;
    for (int i = 0; i < mAllTopic.size(); ++i) {
        topicList.append(((Topic*) (mAllTopic.at(i)))->toMap());
    }
    return topicList;
}

QList<QObject*> DataManager::allTopic()
{
    return mAllTopic;
}

QDeclarativeListProperty<Topic> DataManager::topicPropertyList()
{
    return QDeclarativeListProperty<Topic>(this, 0,
            &DataManager::appendToTopicProperty, &DataManager::topicPropertyCount,
            &DataManager::atTopicProperty, &DataManager::clearTopicProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Topic*
void DataManager::appendToTopicProperty(
        QDeclarativeListProperty<Topic> *topicList,
        Topic* topic)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(topicList->object);
    if (dataManagerObject) {
        topic->setParent(dataManagerObject);
        dataManagerObject->mAllTopic.append(topic);
        emit dataManagerObject->addedToAllTopic(topic);
    } else {
        qWarning() << "cannot append Topic* to mAllTopic "
                << "Object is not of type DataManager*";
    }
}
int DataManager::topicPropertyCount(
        QDeclarativeListProperty<Topic> *topicList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(topicList->object);
    if (dataManager) {
        return dataManager->mAllTopic.size();
    } else {
        qWarning() << "cannot get size mAllTopic " << "Object is not of type DataManager*";
    }
    return 0;
}
Topic* DataManager::atTopicProperty(
        QDeclarativeListProperty<Topic> *topicList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(topicList->object);
    if (dataManager) {
        if (dataManager->mAllTopic.size() > pos) {
            return (Topic*) dataManager->mAllTopic.at(pos);
        }
        qWarning() << "cannot get Topic* at pos " << pos << " size is "
                << dataManager->mAllTopic.size();
    } else {
        qWarning() << "cannot get Topic* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearTopicProperty(
        QDeclarativeListProperty<Topic> *topicList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(topicList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllTopic.size(); ++i) {
            Topic* topic;
            topic = (Topic*) dataManager->mAllTopic.at(i);
			emit dataManager->deletedFromAllTopicByUuid(topic->uuid());
			emit dataManager->deletedFromAllTopic(topic);
            topic->deleteLater();
            topic = 0;
        }
        dataManager->mAllTopic.clear();
    } else {
        qWarning() << "cannot clear mAllTopic " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Topic
 * and clears the list
 */
void DataManager::deleteTopic()
{
    for (int i = 0; i < mAllTopic.size(); ++i) {
        Topic* topic;
        topic = (Topic*) mAllTopic.at(i);
        emit deletedFromAllTopicByUuid(topic->uuid());
		emit deletedFromAllTopic(topic);
        topic->deleteLater();
        topic = 0;
     }
     mAllTopic.clear();
}

/**
 * creates a new Topic
 * parent is DataManager
 * if data is successfully entered you must insertTopic
 * if edit was canceled you must undoCreateTopic to free up memory
 */
Topic* DataManager::createTopic()
{
    Topic* topic;
    topic = new Topic();
    topic->setParent(this);
    topic->prepareNew();
    return topic;
}

/**
 * deletes Topic
 * if createTopic was canceled from UI
 * to delete a previous successfully inserted Topic
 * use deleteTopic
 */
void DataManager::undoCreateTopic(Topic* topic)
{
    if (topic) {
        qDebug() << "undoCreateTopic " << topic->id();
        topic->deleteLater();
        topic = 0;
    }
}

void DataManager::insertTopic(Topic* topic)
{
    // Important: DataManager must be parent of all root DTOs
    topic->setParent(this);
    mAllTopic.append(topic);
    emit addedToAllTopic(topic);
}

void DataManager::insertTopicFromMap(const QVariantMap& topicMap,
        const bool& useForeignProperties)
{
    Topic* topic = new Topic();
    topic->setParent(this);
    if (useForeignProperties) {
        topic->fillFromForeignMap(topicMap);
    } else {
        topic->fillFromMap(topicMap);
    }
    mAllTopic.append(topic);
    emit addedToAllTopic(topic);
}

bool DataManager::deleteTopic(Topic* topic)
{
    bool ok = false;
    ok = mAllTopic.removeOne(topic);
    if (!ok) {
        return ok;
    }
    emit deletedFromAllTopicByUuid(topic->uuid());
    emit deletedFromAllTopic(topic);
    topic->deleteLater();
    topic = 0;
    return ok;
}

bool DataManager::deleteTopicByUuid(const QString& uuid)
{
    if (uuid.isNull() || uuid.isEmpty()) {
        qDebug() << "cannot delete Topic from empty uuid";
        return false;
    }
    for (int i = 0; i < mAllTopic.size(); ++i) {
        Topic* topic;
        topic = (Topic*) mAllTopic.at(i);
        if (topic->uuid() == uuid) {
            mAllTopic.removeAt(i);
            emit deletedFromAllTopicByUuid(uuid);
            emit deletedFromAllTopic(topic);
            topic->deleteLater();
            topic = 0;
            return true;
        }
    }
    return false;
}

bool DataManager::deleteTopicById(const int& id)
{
    for (int i = 0; i < mAllTopic.size(); ++i) {
        Topic* topic;
        topic = (Topic*) mAllTopic.at(i);
        if (topic->id() == id) {
            mAllTopic.removeAt(i);
            emit deletedFromAllTopicById(id);
            emit deletedFromAllTopic(topic);
            topic->deleteLater();
            topic = 0;
            return true;
        }
    }
    return false;
}

void DataManager::fillTopicDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllTopic.size(); ++i) {
            	theList.append(mAllTopic.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Topic for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInTopicDataModel(QString objectName,
        Topic* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Topic Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromTopicDataModel(QString objectName, Topic* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Topic Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoTopicDataModel(QString objectName, Topic* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}
Topic* DataManager::findTopicByUuid(const QString& uuid){
    if (uuid.isNull() || uuid.isEmpty()) {
        qDebug() << "cannot find Topic from empty uuid";
        return 0;
    }
    for (int i = 0; i < mAllTopic.size(); ++i) {
        Topic* topic;
        topic = (Topic*)mAllTopic.at(i);
        if(topic->uuid() == uuid){
            return topic;
        }
    }
    qDebug() << "no Topic found for uuid " << uuid;
    return 0;
}

// nr is DomainKey
Topic* DataManager::findTopicById(const int& id){
    for (int i = 0; i < mAllTopic.size(); ++i) {
        Topic* topic;
        topic = (Topic*)mAllTopic.at(i);
        if(topic->id() == id){
            return topic;
        }
    }
    qDebug() << "no Topic found for id " << id;
    return 0;
}
/*
 * reads Maps of Department in from JSON cache
 * creates List of Department*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initDepartmentFromCache()
{
	qDebug() << "start initDepartmentFromCache";
    mAllDepartment.clear();
    mAllDepartmentFlat.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheDepartment);
    qDebug() << "read Department from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Department* department = new Department();
        // Important: DataManager must be parent of all root DTOs
        department->setParent(this);
        department->fillFromCacheMap(cacheMap);
        mAllDepartment.append(department);
        mAllDepartmentFlat.append(department);
        mAllDepartmentFlat.append(department->allDepartmentChildren());
    }
    qDebug() << "created Tree of Department* #" << mAllDepartment.size();
    qDebug() << "created Flat list of Department* #" << mAllDepartmentFlat.size();
}


/*
 * save List of Department* to JSON cache
 * convert list of Department* to QVariantList
 * toCacheMap stores all properties without transient values
 * Department is read-only Cache - so it's not saved automatically at exit
 */
void DataManager::saveDepartmentToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Department* #" << mAllDepartment.size();
    for (int i = 0; i < mAllDepartment.size(); ++i) {
        Department* department;
        department = (Department*)mAllDepartment.at(i);
        QVariantMap cacheMap;
        cacheMap = department->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Department* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheDepartment, cacheList);
}

/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Department*> DataManager::listOfDepartmentForKeys(
        QStringList keyList)
{
    QList<Department*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllDepartment.size(); ++i) {
        Department* department;
        department = (Department*) mAllDepartment.at(i);
        if (keyList.contains(QString::number(department->id()))) {
            listOfData.append(department);
            keyList.removeOne(QString::number(department->id()));
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Department: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::departmentAsQVariantList()
{
    QVariantList departmentList;
    for (int i = 0; i < mAllDepartment.size(); ++i) {
        departmentList.append(((Department*) (mAllDepartment.at(i)))->toMap());
    }
    return departmentList;
}

QList<QObject*> DataManager::allDepartment()
{
    return mAllDepartment;
}

QDeclarativeListProperty<Department> DataManager::departmentPropertyList()
{
    return QDeclarativeListProperty<Department>(this, 0,
            &DataManager::appendToDepartmentProperty, &DataManager::departmentPropertyCount,
            &DataManager::atDepartmentProperty, &DataManager::clearDepartmentProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Department*
void DataManager::appendToDepartmentProperty(
        QDeclarativeListProperty<Department> *departmentList,
        Department* department)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(departmentList->object);
    if (dataManagerObject) {
        department->setParent(dataManagerObject);
        dataManagerObject->mAllDepartment.append(department);
        emit dataManagerObject->addedToAllDepartment(department);
    } else {
        qWarning() << "cannot append Department* to mAllDepartment "
                << "Object is not of type DataManager*";
    }
}
int DataManager::departmentPropertyCount(
        QDeclarativeListProperty<Department> *departmentList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(departmentList->object);
    if (dataManager) {
        return dataManager->mAllDepartment.size();
    } else {
        qWarning() << "cannot get size mAllDepartment " << "Object is not of type DataManager*";
    }
    return 0;
}
Department* DataManager::atDepartmentProperty(
        QDeclarativeListProperty<Department> *departmentList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(departmentList->object);
    if (dataManager) {
        if (dataManager->mAllDepartment.size() > pos) {
            return (Department*) dataManager->mAllDepartment.at(pos);
        }
        qWarning() << "cannot get Department* at pos " << pos << " size is "
                << dataManager->mAllDepartment.size();
    } else {
        qWarning() << "cannot get Department* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearDepartmentProperty(
        QDeclarativeListProperty<Department> *departmentList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(departmentList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllDepartment.size(); ++i) {
            Department* department;
            department = (Department*) dataManager->mAllDepartment.at(i);
			emit dataManager->deletedFromAllDepartmentByUuid(department->uuid());
			emit dataManager->deletedFromAllDepartment(department);
            department->deleteLater();
            department = 0;
        }
        dataManager->mAllDepartment.clear();
    } else {
        qWarning() << "cannot clear mAllDepartment " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Department
 * and clears the list
 */
void DataManager::deleteDepartment()
{
    for (int i = 0; i < mAllDepartment.size(); ++i) {
        Department* department;
        department = (Department*) mAllDepartment.at(i);
        emit deletedFromAllDepartmentByUuid(department->uuid());
		emit deletedFromAllDepartment(department);
        department->deleteLater();
        department = 0;
     }
     mAllDepartment.clear();
}

/**
 * creates a new Department
 * parent is DataManager
 * if data is successfully entered you must insertDepartment
 * if edit was canceled you must undoCreateDepartment to free up memory
 */
Department* DataManager::createDepartment()
{
    Department* department;
    department = new Department();
    department->setParent(this);
    department->prepareNew();
    return department;
}

/**
 * deletes Department
 * if createDepartment was canceled from UI
 * to delete a previous successfully inserted Department
 * use deleteDepartment
 */
void DataManager::undoCreateDepartment(Department* department)
{
    if (department) {
        qDebug() << "undoCreateDepartment " << department->id();
        department->deleteLater();
        department = 0;
    }
}

void DataManager::insertDepartment(Department* department)
{
    // Important: DataManager must be parent of all root DTOs
    department->setParent(this);
    mAllDepartment.append(department);
    mAllDepartmentFlat.append(department);
    emit addedToAllDepartment(department);
}

void DataManager::insertDepartmentFromMap(const QVariantMap& departmentMap,
        const bool& useForeignProperties)
{
    Department* department = new Department();
    department->setParent(this);
    if (useForeignProperties) {
        department->fillFromForeignMap(departmentMap);
    } else {
        department->fillFromMap(departmentMap);
    }
    mAllDepartment.append(department);
    mAllDepartmentFlat.append(department);
    emit addedToAllDepartment(department);
}

bool DataManager::deleteDepartment(Department* department)
{
    bool ok = false;
    ok = mAllDepartment.removeOne(department);
    if (!ok) {
        return ok;
    }
    mAllDepartmentFlat.removeOne(department);
    emit deletedFromAllDepartmentByUuid(department->uuid());
    emit deletedFromAllDepartment(department);
    department->deleteLater();
    department = 0;
    return ok;
}

bool DataManager::deleteDepartmentByUuid(const QString& uuid)
{
    if (uuid.isNull() || uuid.isEmpty()) {
        qDebug() << "cannot delete Department from empty uuid";
        return false;
    }
    for (int i = 0; i < mAllDepartment.size(); ++i) {
        Department* department;
        department = (Department*) mAllDepartment.at(i);
        if (department->uuid() == uuid) {
            mAllDepartment.removeAt(i);
            mAllDepartmentFlat.removeOne(department);
            emit deletedFromAllDepartmentByUuid(uuid);
            emit deletedFromAllDepartment(department);
            department->deleteLater();
            department = 0;
            return true;
        }
    }
    return false;
}

bool DataManager::deleteDepartmentById(const int& id)
{
    for (int i = 0; i < mAllDepartment.size(); ++i) {
        Department* department;
        department = (Department*) mAllDepartment.at(i);
        if (department->id() == id) {
            mAllDepartment.removeAt(i);
            mAllDepartmentFlat.removeOne(department);
            emit deletedFromAllDepartmentById(id);
            emit deletedFromAllDepartment(department);
            department->deleteLater();
            department = 0;
            return true;
        }
    }
    return false;
}

void DataManager::fillDepartmentTreeDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllDepartment.size(); ++i) {
            	theList.append(mAllDepartment.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Department for " << objectName;
}
void DataManager::fillDepartmentFlatDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllDepartmentFlat.size(); ++i) {
            	theList.append(mAllDepartmentFlat.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Department for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInDepartmentDataModel(QString objectName,
        Department* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Department Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromDepartmentDataModel(QString objectName, Department* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Department Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoDepartmentDataModel(QString objectName, Department* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}
Department* DataManager::findDepartmentByUuid(const QString& uuid){
    if (uuid.isNull() || uuid.isEmpty()) {
        qDebug() << "cannot find Department from empty uuid";
        return 0;
    }
    for (int i = 0; i < mAllDepartmentFlat.size(); ++i) {
        Department* department;
        department = (Department*)mAllDepartmentFlat.at(i);
        if(department->uuid() == uuid){
            return department;
        }
    }
    qDebug() << "no Department found for uuid " << uuid;
    return 0;
}

// nr is DomainKey
Department* DataManager::findDepartmentById(const int& id){
    for (int i = 0; i < mAllDepartmentFlat.size(); ++i) {
        Department* department;
        department = (Department*)mAllDepartmentFlat.at(i);
        if(department->id() == id){
            return department;
        }
    }
    qDebug() << "no Department found for id " << id;
    return 0;
}
/*
 * reads Maps of Tag in from JSON cache
 * creates List of Tag*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initTagFromCache()
{
	qDebug() << "start initTagFromCache";
    mAllTag.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheTag);
    qDebug() << "read Tag from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Tag* tag = new Tag();
        // Important: DataManager must be parent of all root DTOs
        tag->setParent(this);
        tag->fillFromCacheMap(cacheMap);
        mAllTag.append(tag);
    }
    qDebug() << "created Tag* #" << mAllTag.size();
}

/*
 * queries SELECT * FROM Tag (SQLite cache)
* creates List of Tag*  from QSqlQuery
 */
void DataManager::initTagFromSqlCache()
{
	qDebug() << "start initTag From S Q L Cache";
	mAllTag.clear();
    QString sqlQuery = "SELECT * FROM tag";
    QSqlQuery query (mDatabase);
    query.setForwardOnly(true);
    query.prepare(sqlQuery);
    bool success = query.exec();
    if(!success) {
    	qDebug() << "NO SUCCESS query tag";
    	return;
    }
    QSqlRecord record = query.record();
    Tag::fillSqlQueryPos(record);
    while (query.next())
    	{
    		Tag* tag = new Tag();
    		// Important: DataManager must be parent of all root DTOs
    		tag->setParent(this);
    		tag->fillFromSqlQuery(query);
    		mAllTag.append(tag);
    	}
    qDebug() << "read from SQLite and created Tag* #" << mAllTag.size();
}

/*
 * save List of Tag* to JSON cache
 * convert list of Tag* to QVariantList
 * toCacheMap stores all properties without transient values
 * Tag is read-only Cache - so it's not saved automatically at exit
 */
void DataManager::saveTagToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Tag* #" << mAllTag.size();
    for (int i = 0; i < mAllTag.size(); ++i) {
        Tag* tag;
        tag = (Tag*)mAllTag.at(i);
        QVariantMap cacheMap;
        cacheMap = tag->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Tag* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheTag, cacheList);
}

/*
 * save List of Tag* to SQLite cache
 * convert list of Tag* to QVariantLists for each COLUMN
 * tune PRAGMA journal_mode and synchronous for bulk import into SQLite
 * INSERT chunks of data into SQLite (default: 10k rows at once)
 * 
 * Tag is read-only Cache - so it's not saved automatically at exit
 */
void DataManager::saveTagToSqlCache()
{
    qDebug() << "now caching Tag* #" << mAllTag.size();
    bulkImport(true);
    bool success = false;
    QSqlQuery query (mDatabase);
    query.prepare("DROP TABLE IF EXISTS tag");
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS DROP tag";
        bulkImport(false);
        return;
    }
    qDebug() << "table DROPPED tag";
    // create table
    query.clear();
    query.prepare(Tag::createTableCommand());
    success = query.exec();
    if(!success) {
        qWarning() << "NO SUCCESS CREATE tag";
        bulkImport(false);
        return;
    }
    qDebug() << "table CREATED tag";

	qDebug() << "BEGIN INSERT chunks of tag";
    //
    QVariantList uuidList, nameList, valueList, colorList;
    QString insertSQL = Tag::createParameterizedInsertPosBinding();
    int laps = mAllTag.size()/mChunkSize;
    if(mAllTag.size()%mChunkSize) {
        laps ++;
    }
    int count = 0;
    qDebug() << "chunks of " << mChunkSize << " laps: " << laps;
    int fromPos = 0;
    int toPos = mChunkSize;
    if(toPos > mAllTag.size()) {
        toPos = mAllTag.size();
    }
    while (count < laps) {
    	//
    	query.clear();
    	query.prepare("BEGIN TRANSACTION");
    	success = query.exec();
    	if(!success) {
        	qWarning() << "NO SUCCESS BEGIN TRANSACTION";
        	bulkImport(false);
        	return;
    	}
    	// do it
		uuidList.clear();
		nameList.clear();
		valueList.clear();
		colorList.clear();
    	for (int i = fromPos; i < toPos; ++i) {
        	Tag* tag;
        	tag = (Tag*)mAllTag.at(i);
        	tag->toSqlCache(uuidList, nameList, valueList, colorList);
    	}
        //
    	query.clear();
    	query.prepare(insertSQL);
    	query.addBindValue(uuidList);
    	query.addBindValue(nameList);
    	query.addBindValue(valueList);
    	query.addBindValue(colorList);
    	success = query.execBatch();
    	if(!success) {
        	qWarning() << "NO SUCCESS INSERT batch tag";
        	bulkImport(false);
        	return;
    	}
    	query.clear();
    	query.prepare("END TRANSACTION");
    	success = query.exec();
    	if(!success) {
        	qWarning() << "NO SUCCESS END TRANSACTION";
        	bulkImport(false);
        	return;
    	}
        //
        count ++;
        fromPos += mChunkSize;
        toPos += mChunkSize;
        if(toPos > mAllTag.size()) {
            toPos = mAllTag.size();
        }
    }
    qDebug() << "END INSERT chunks of tag";
    bulkImport(false);
}
/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Tag*> DataManager::listOfTagForKeys(
        QStringList keyList)
{
    QList<Tag*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllTag.size(); ++i) {
        Tag* tag;
        tag = (Tag*) mAllTag.at(i);
        if (keyList.contains(tag->uuid())) {
            listOfData.append(tag);
            keyList.removeOne(tag->uuid());
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Tag: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::tagAsQVariantList()
{
    QVariantList tagList;
    for (int i = 0; i < mAllTag.size(); ++i) {
        tagList.append(((Tag*) (mAllTag.at(i)))->toMap());
    }
    return tagList;
}

QList<QObject*> DataManager::allTag()
{
    return mAllTag;
}

QDeclarativeListProperty<Tag> DataManager::tagPropertyList()
{
    return QDeclarativeListProperty<Tag>(this, 0,
            &DataManager::appendToTagProperty, &DataManager::tagPropertyCount,
            &DataManager::atTagProperty, &DataManager::clearTagProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Tag*
void DataManager::appendToTagProperty(
        QDeclarativeListProperty<Tag> *tagList,
        Tag* tag)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(tagList->object);
    if (dataManagerObject) {
        tag->setParent(dataManagerObject);
        dataManagerObject->mAllTag.append(tag);
        emit dataManagerObject->addedToAllTag(tag);
    } else {
        qWarning() << "cannot append Tag* to mAllTag "
                << "Object is not of type DataManager*";
    }
}
int DataManager::tagPropertyCount(
        QDeclarativeListProperty<Tag> *tagList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(tagList->object);
    if (dataManager) {
        return dataManager->mAllTag.size();
    } else {
        qWarning() << "cannot get size mAllTag " << "Object is not of type DataManager*";
    }
    return 0;
}
Tag* DataManager::atTagProperty(
        QDeclarativeListProperty<Tag> *tagList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(tagList->object);
    if (dataManager) {
        if (dataManager->mAllTag.size() > pos) {
            return (Tag*) dataManager->mAllTag.at(pos);
        }
        qWarning() << "cannot get Tag* at pos " << pos << " size is "
                << dataManager->mAllTag.size();
    } else {
        qWarning() << "cannot get Tag* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearTagProperty(
        QDeclarativeListProperty<Tag> *tagList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(tagList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllTag.size(); ++i) {
            Tag* tag;
            tag = (Tag*) dataManager->mAllTag.at(i);
			emit dataManager->deletedFromAllTagByUuid(tag->uuid());
			emit dataManager->deletedFromAllTag(tag);
            tag->deleteLater();
            tag = 0;
        }
        dataManager->mAllTag.clear();
    } else {
        qWarning() << "cannot clear mAllTag " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Tag
 * and clears the list
 */
void DataManager::deleteTag()
{
    for (int i = 0; i < mAllTag.size(); ++i) {
        Tag* tag;
        tag = (Tag*) mAllTag.at(i);
        emit deletedFromAllTagByUuid(tag->uuid());
		emit deletedFromAllTag(tag);
        tag->deleteLater();
        tag = 0;
     }
     mAllTag.clear();
}

/**
 * creates a new Tag
 * parent is DataManager
 * if data is successfully entered you must insertTag
 * if edit was canceled you must undoCreateTag to free up memory
 */
Tag* DataManager::createTag()
{
    Tag* tag;
    tag = new Tag();
    tag->setParent(this);
    tag->prepareNew();
    return tag;
}

/**
 * deletes Tag
 * if createTag was canceled from UI
 * to delete a previous successfully inserted Tag
 * use deleteTag
 */
void DataManager::undoCreateTag(Tag* tag)
{
    if (tag) {
        qDebug() << "undoCreateTag " << tag->uuid();
        tag->deleteLater();
        tag = 0;
    }
}

void DataManager::insertTag(Tag* tag)
{
    // Important: DataManager must be parent of all root DTOs
    tag->setParent(this);
    mAllTag.append(tag);
    emit addedToAllTag(tag);
}

void DataManager::insertTagFromMap(const QVariantMap& tagMap,
        const bool& useForeignProperties)
{
    Tag* tag = new Tag();
    tag->setParent(this);
    if (useForeignProperties) {
        tag->fillFromForeignMap(tagMap);
    } else {
        tag->fillFromMap(tagMap);
    }
    mAllTag.append(tag);
    emit addedToAllTag(tag);
}

bool DataManager::deleteTag(Tag* tag)
{
    bool ok = false;
    ok = mAllTag.removeOne(tag);
    if (!ok) {
        return ok;
    }
    emit deletedFromAllTagByUuid(tag->uuid());
    emit deletedFromAllTag(tag);
    tag->deleteLater();
    tag = 0;
    return ok;
}

bool DataManager::deleteTagByUuid(const QString& uuid)
{
    if (uuid.isNull() || uuid.isEmpty()) {
        qDebug() << "cannot delete Tag from empty uuid";
        return false;
    }
    for (int i = 0; i < mAllTag.size(); ++i) {
        Tag* tag;
        tag = (Tag*) mAllTag.at(i);
        if (tag->uuid() == uuid) {
            mAllTag.removeAt(i);
            emit deletedFromAllTagByUuid(uuid);
            emit deletedFromAllTag(tag);
            tag->deleteLater();
            tag = 0;
            return true;
        }
    }
    return false;
}


void DataManager::fillTagDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllTag.size(); ++i) {
            	theList.append(mAllTag.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Tag for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInTagDataModel(QString objectName,
        Tag* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Tag Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromTagDataModel(QString objectName, Tag* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Tag Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoTagDataModel(QString objectName, Tag* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}
Tag* DataManager::findTagByUuid(const QString& uuid){
    if (uuid.isNull() || uuid.isEmpty()) {
        qDebug() << "cannot find Tag from empty uuid";
        return 0;
    }
    for (int i = 0; i < mAllTag.size(); ++i) {
        Tag* tag;
        tag = (Tag*)mAllTag.at(i);
        if(tag->uuid() == uuid){
            return tag;
        }
    }
    qDebug() << "no Tag found for uuid " << uuid;
    return 0;
}

/*
 * reads Maps of Xtras in from JSON cache
 * creates List of Xtras*  from QVariantList
 * List declared as list of QObject* - only way to use in GroupDataModel
 */
void DataManager::initXtrasFromCache()
{
	qDebug() << "start initXtrasFromCache";
    mAllXtras.clear();
    QVariantList cacheList;
    cacheList = readFromCache(cacheXtras);
    qDebug() << "read Xtras from cache #" << cacheList.size();
    for (int i = 0; i < cacheList.size(); ++i) {
        QVariantMap cacheMap;
        cacheMap = cacheList.at(i).toMap();
        Xtras* xtras = new Xtras();
        // Important: DataManager must be parent of all root DTOs
        xtras->setParent(this);
        xtras->fillFromCacheMap(cacheMap);
        mAllXtras.append(xtras);
    }
    qDebug() << "created Xtras* #" << mAllXtras.size();
}


/*
 * save List of Xtras* to JSON cache
 * convert list of Xtras* to QVariantList
 * toCacheMap stores all properties without transient values
 */
void DataManager::saveXtrasToCache()
{
    QVariantList cacheList;
    qDebug() << "now caching Xtras* #" << mAllXtras.size();
    for (int i = 0; i < mAllXtras.size(); ++i) {
        Xtras* xtras;
        xtras = (Xtras*)mAllXtras.at(i);
        QVariantMap cacheMap;
        cacheMap = xtras->toCacheMap();
        cacheList.append(cacheMap);
    }
    qDebug() << "Xtras* converted to JSON cache #" << cacheList.size();
    writeToCache(cacheXtras, cacheList);
}

/**
* converts a list of keys in to a list of DataObjects
* per ex. used to resolve lazy arrays
*/
QList<Xtras*> DataManager::listOfXtrasForKeys(
        QStringList keyList)
{
    QList<Xtras*> listOfData;
    keyList.removeDuplicates();
    if (keyList.isEmpty()) {
        return listOfData;
    }
    for (int i = 0; i < mAllXtras.size(); ++i) {
        Xtras* xtras;
        xtras = (Xtras*) mAllXtras.at(i);
        if (keyList.contains(QString::number(xtras->id()))) {
            listOfData.append(xtras);
            keyList.removeOne(QString::number(xtras->id()));
            if(keyList.isEmpty()){
                break;
            }
        }
    }
    if (keyList.isEmpty()) {
        return listOfData;
    }
    qWarning() << "not all keys found for Xtras: " << keyList.join(", ");
    return listOfData;
}

QVariantList DataManager::xtrasAsQVariantList()
{
    QVariantList xtrasList;
    for (int i = 0; i < mAllXtras.size(); ++i) {
        xtrasList.append(((Xtras*) (mAllXtras.at(i)))->toMap());
    }
    return xtrasList;
}

QList<QObject*> DataManager::allXtras()
{
    return mAllXtras;
}

QDeclarativeListProperty<Xtras> DataManager::xtrasPropertyList()
{
    return QDeclarativeListProperty<Xtras>(this, 0,
            &DataManager::appendToXtrasProperty, &DataManager::xtrasPropertyCount,
            &DataManager::atXtrasProperty, &DataManager::clearXtrasProperty);
}

// implementation for QDeclarativeListProperty to use
// QML functions for List of Xtras*
void DataManager::appendToXtrasProperty(
        QDeclarativeListProperty<Xtras> *xtrasList,
        Xtras* xtras)
{
    DataManager *dataManagerObject = qobject_cast<DataManager *>(xtrasList->object);
    if (dataManagerObject) {
        xtras->setParent(dataManagerObject);
        dataManagerObject->mAllXtras.append(xtras);
        emit dataManagerObject->addedToAllXtras(xtras);
    } else {
        qWarning() << "cannot append Xtras* to mAllXtras "
                << "Object is not of type DataManager*";
    }
}
int DataManager::xtrasPropertyCount(
        QDeclarativeListProperty<Xtras> *xtrasList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(xtrasList->object);
    if (dataManager) {
        return dataManager->mAllXtras.size();
    } else {
        qWarning() << "cannot get size mAllXtras " << "Object is not of type DataManager*";
    }
    return 0;
}
Xtras* DataManager::atXtrasProperty(
        QDeclarativeListProperty<Xtras> *xtrasList, int pos)
{
    DataManager *dataManager = qobject_cast<DataManager *>(xtrasList->object);
    if (dataManager) {
        if (dataManager->mAllXtras.size() > pos) {
            return (Xtras*) dataManager->mAllXtras.at(pos);
        }
        qWarning() << "cannot get Xtras* at pos " << pos << " size is "
                << dataManager->mAllXtras.size();
    } else {
        qWarning() << "cannot get Xtras* at pos " << pos
                << "Object is not of type DataManager*";
    }
    return 0;
}
void DataManager::clearXtrasProperty(
        QDeclarativeListProperty<Xtras> *xtrasList)
{
    DataManager *dataManager = qobject_cast<DataManager *>(xtrasList->object);
    if (dataManager) {
        for (int i = 0; i < dataManager->mAllXtras.size(); ++i) {
            Xtras* xtras;
            xtras = (Xtras*) dataManager->mAllXtras.at(i);
			emit dataManager->deletedFromAllXtrasById(xtras->id());
			emit dataManager->deletedFromAllXtras(xtras);
            xtras->deleteLater();
            xtras = 0;
        }
        dataManager->mAllXtras.clear();
    } else {
        qWarning() << "cannot clear mAllXtras " << "Object is not of type DataManager*";
    }
}

/**
 * deletes all Xtras
 * and clears the list
 */
void DataManager::deleteXtras()
{
    for (int i = 0; i < mAllXtras.size(); ++i) {
        Xtras* xtras;
        xtras = (Xtras*) mAllXtras.at(i);
        emit deletedFromAllXtrasById(xtras->id());
		emit deletedFromAllXtras(xtras);
        xtras->deleteLater();
        xtras = 0;
     }
     mAllXtras.clear();
}

/**
 * creates a new Xtras
 * parent is DataManager
 * if data is successfully entered you must insertXtras
 * if edit was canceled you must undoCreateXtras to free up memory
 */
Xtras* DataManager::createXtras()
{
    Xtras* xtras;
    xtras = new Xtras();
    xtras->setParent(this);
    xtras->prepareNew();
    return xtras;
}

/**
 * deletes Xtras
 * if createXtras was canceled from UI
 * to delete a previous successfully inserted Xtras
 * use deleteXtras
 */
void DataManager::undoCreateXtras(Xtras* xtras)
{
    if (xtras) {
        qDebug() << "undoCreateXtras " << xtras->id();
        xtras->deleteLater();
        xtras = 0;
    }
}

void DataManager::insertXtras(Xtras* xtras)
{
    // Important: DataManager must be parent of all root DTOs
    xtras->setParent(this);
    mAllXtras.append(xtras);
    emit addedToAllXtras(xtras);
}

void DataManager::insertXtrasFromMap(const QVariantMap& xtrasMap,
        const bool& useForeignProperties)
{
    Xtras* xtras = new Xtras();
    xtras->setParent(this);
    if (useForeignProperties) {
        xtras->fillFromForeignMap(xtrasMap);
    } else {
        xtras->fillFromMap(xtrasMap);
    }
    mAllXtras.append(xtras);
    emit addedToAllXtras(xtras);
}

bool DataManager::deleteXtras(Xtras* xtras)
{
    bool ok = false;
    ok = mAllXtras.removeOne(xtras);
    if (!ok) {
        return ok;
    }
    emit deletedFromAllXtrasById(xtras->id());
    emit deletedFromAllXtras(xtras);
    xtras->deleteLater();
    xtras = 0;
    return ok;
}


bool DataManager::deleteXtrasById(const int& id)
{
    for (int i = 0; i < mAllXtras.size(); ++i) {
        Xtras* xtras;
        xtras = (Xtras*) mAllXtras.at(i);
        if (xtras->id() == id) {
            mAllXtras.removeAt(i);
            emit deletedFromAllXtrasById(id);
            emit deletedFromAllXtras(xtras);
            xtras->deleteLater();
            xtras = 0;
            return true;
        }
    }
    return false;
}

void DataManager::fillXtrasDataModel(QString objectName)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
    	GroupDataModel* dataModel = dataModelList.last();
    	if (dataModel) {
        	QList<QObject*> theList;
        	for (int i = 0; i < mAllXtras.size(); ++i) {
            	theList.append(mAllXtras.at(i));
        	}
        	dataModel->clear();
        	dataModel->insertList(theList);
        	return;
    	}
    }
    qDebug() << "NO GRP DATA FOUND Xtras for " << objectName;
}
/**
 * removing and re-inserting a single item of a DataModel
 * this will cause the ListView to redraw or recalculate all values for this ListItem
 * we do this, because only changing properties won't call List functions
 */
void DataManager::replaceItemInXtrasDataModel(QString objectName,
        Xtras* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                dataModel->insert(listItem);
                return;
            }
            qDebug() << "Xtras Object not found and not replaced in " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::removeItemFromXtrasDataModel(QString objectName, Xtras* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            bool exists = dataModel->remove(listItem);
            if (exists) {
                return;
            }
            qDebug() << "Xtras Object not found and not removed from " << objectName;
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

void DataManager::insertItemIntoXtrasDataModel(QString objectName, Xtras* listItem)
{
    // using dynamic created Pages / Lists it's a good idea to use findChildren ... last()
    // probably there are GroupDataModels not deleted yet from previous destroyed Pages
    QList<GroupDataModel*> dataModelList = Application::instance()->scene()->findChildren<
            GroupDataModel*>(objectName);
    if (dataModelList.size() > 0) {
        GroupDataModel* dataModel = dataModelList.last();
        if (dataModel) {
            dataModel->insert(listItem);
        }
        return;
    }
    qDebug() << "no DataModel found for " << objectName;
}

// nr is DomainKey
Xtras* DataManager::findXtrasById(const int& id){
    for (int i = 0; i < mAllXtras.size(); ++i) {
        Xtras* xtras;
        xtras = (Xtras*)mAllXtras.at(i);
        if(xtras->id() == id){
            return xtras;
        }
    }
    qDebug() << "no Xtras found for id " << id;
    return 0;
}

/*
 * reads data in from stored cache
 * if no cache found tries to get data from assets/datamodel
 */
QVariantList DataManager::readFromCache(QString& fileName)
{
    JsonDataAccess jda;
    QVariantList cacheList;
    QFile dataFile(dataPath(fileName));
    if (!dataFile.exists()) {
        QFile assetDataFile(dataAssetsPath(fileName));
        if (assetDataFile.exists()) {
            // copy file from assets to data
            bool copyOk = assetDataFile.copy(dataPath(fileName));
            if (!copyOk) {
                qDebug() << "cannot copy dataAssetsPath(fileName) to dataPath(fileName)";
                // no cache, no assets - empty list
                return cacheList;
            }
        } else {
            // no cache, no assets - empty list
            return cacheList;
        }
    }
    cacheList = jda.load(dataPath(fileName)).toList();
    return cacheList;
}

void DataManager::writeToCache(QString& fileName, QVariantList& data)
{
    QString filePath;
    filePath = dataPath(fileName);
    JsonDataAccess jda;
    jda.save(data, filePath);
}

void DataManager::onManualExit()
{
    qDebug() << "## DataManager ## MANUAL EXIT";
    finish();
    bb::Application::instance()->exit(0);
}

DataManager::~DataManager()
{
    // clean up
}
