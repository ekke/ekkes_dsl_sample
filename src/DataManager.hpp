#ifndef DATAMANAGER_HPP_
#define DATAMANAGER_HPP_

#include <qobject.h>
#include <QtSql/QtSql>

#include "Order.hpp"
#include "Item.hpp"
#include "SubItem.hpp"
#include "Customer.hpp"
#include "Info.hpp"
#include "Topic.hpp"
#include "Department.hpp"
#include "Tag.hpp"
#include "Xtras.hpp"
#include  "GeoCoordinate.hpp"
#include  "GeoAddress.hpp"

class DataManager: public QObject
{
Q_OBJECT

// QDeclarativeListProperty to get easy access from QML
Q_PROPERTY(QDeclarativeListProperty<Order> orderPropertyList READ orderPropertyList CONSTANT)
Q_PROPERTY(QDeclarativeListProperty<Customer> customerPropertyList READ customerPropertyList CONSTANT)
Q_PROPERTY(QDeclarativeListProperty<Topic> topicPropertyList READ topicPropertyList CONSTANT)
Q_PROPERTY(QDeclarativeListProperty<Department> departmentPropertyList READ departmentPropertyList CONSTANT)
Q_PROPERTY(QDeclarativeListProperty<Tag> tagPropertyList READ tagPropertyList CONSTANT)
Q_PROPERTY(QDeclarativeListProperty<Xtras> xtrasPropertyList READ xtrasPropertyList CONSTANT)

public:
    DataManager(QObject *parent = 0);
    virtual ~DataManager();
    Q_INVOKABLE
    void init();

	
	Q_INVOKABLE
	void fillOrderDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInOrderDataModel(QString objectName, Order* listItem);

	Q_INVOKABLE
	void removeItemFromOrderDataModel(QString objectName, Order* listItem);

	Q_INVOKABLE
	void insertItemIntoOrderDataModel(QString objectName, Order* listItem);

	Q_INVOKABLE
	void resolveOrderReferences(Order* order);

	Q_INVOKABLE
	void resolveReferencesForAllOrder();

	Q_INVOKABLE
	QList<Order*> listOfOrderForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList orderAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allOrder();

	Q_INVOKABLE
	void deleteOrder();

	// access from QML to list of all Order
	QDeclarativeListProperty<Order> orderPropertyList();

	Q_INVOKABLE
	Order* createOrder();

	Q_INVOKABLE
	void undoCreateOrder(Order* order);

	Q_INVOKABLE
	void insertOrder(Order* order);

	Q_INVOKABLE
	void insertOrderFromMap(const QVariantMap& orderMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteOrder(Order* order);

	Q_INVOKABLE
	bool deleteOrderByUuid(const QString& uuid);

	Q_INVOKABLE
	Order* findOrderByUuid(const QString& uuid);

	Q_INVOKABLE
	bool deleteOrderByNr(const int& nr);

	Q_INVOKABLE
    Order* findOrderByNr(const int& nr);
	
	Q_INVOKABLE
	void fillCustomerDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInCustomerDataModel(QString objectName, Customer* listItem);

	Q_INVOKABLE
	void removeItemFromCustomerDataModel(QString objectName, Customer* listItem);

	Q_INVOKABLE
	void insertItemIntoCustomerDataModel(QString objectName, Customer* listItem);

	Q_INVOKABLE
	void fillCustomerDataModelByCompanyName(QString objectName, const QString& companyName);

	Q_INVOKABLE
	QList<Customer*> listOfCustomerForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList customerAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allCustomer();

	Q_INVOKABLE
	void deleteCustomer();

	// access from QML to list of all Customer
	QDeclarativeListProperty<Customer> customerPropertyList();

	Q_INVOKABLE
	Customer* createCustomer();

	Q_INVOKABLE
	void undoCreateCustomer(Customer* customer);

	Q_INVOKABLE
	void insertCustomer(Customer* customer);

	Q_INVOKABLE
	void insertCustomerFromMap(const QVariantMap& customerMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteCustomer(Customer* customer);

	Q_INVOKABLE
	bool deleteCustomerByUuid(const QString& uuid);

	Q_INVOKABLE
	Customer* findCustomerByUuid(const QString& uuid);

	Q_INVOKABLE
	bool deleteCustomerById(const int& id);

	Q_INVOKABLE
    Customer* findCustomerById(const int& id);
	
	Q_INVOKABLE
	void fillTopicDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInTopicDataModel(QString objectName, Topic* listItem);

	Q_INVOKABLE
	void removeItemFromTopicDataModel(QString objectName, Topic* listItem);

	Q_INVOKABLE
	void insertItemIntoTopicDataModel(QString objectName, Topic* listItem);

	Q_INVOKABLE
	QList<Topic*> listOfTopicForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList topicAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allTopic();

	Q_INVOKABLE
	void deleteTopic();

	// access from QML to list of all Topic
	QDeclarativeListProperty<Topic> topicPropertyList();

	Q_INVOKABLE
	Topic* createTopic();

	Q_INVOKABLE
	void undoCreateTopic(Topic* topic);

	Q_INVOKABLE
	void insertTopic(Topic* topic);

	Q_INVOKABLE
	void insertTopicFromMap(const QVariantMap& topicMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteTopic(Topic* topic);

	Q_INVOKABLE
	bool deleteTopicByUuid(const QString& uuid);

	Q_INVOKABLE
	Topic* findTopicByUuid(const QString& uuid);

	Q_INVOKABLE
	bool deleteTopicById(const int& id);

	Q_INVOKABLE
    Topic* findTopicById(const int& id);
	
	Q_INVOKABLE
	void fillDepartmentTreeDataModel(QString objectName);

	Q_INVOKABLE
	void fillDepartmentFlatDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInDepartmentDataModel(QString objectName, Department* listItem);

	Q_INVOKABLE
	void removeItemFromDepartmentDataModel(QString objectName, Department* listItem);

	Q_INVOKABLE
	void insertItemIntoDepartmentDataModel(QString objectName, Department* listItem);

	Q_INVOKABLE
	QList<Department*> listOfDepartmentForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList departmentAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allDepartment();

	Q_INVOKABLE
	void deleteDepartment();

	// access from QML to list of all Department
	QDeclarativeListProperty<Department> departmentPropertyList();

	Q_INVOKABLE
	Department* createDepartment();

	Q_INVOKABLE
	void undoCreateDepartment(Department* department);

	Q_INVOKABLE
	void insertDepartment(Department* department);

	Q_INVOKABLE
	void insertDepartmentFromMap(const QVariantMap& departmentMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteDepartment(Department* department);

	Q_INVOKABLE
	bool deleteDepartmentByUuid(const QString& uuid);

	Q_INVOKABLE
	Department* findDepartmentByUuid(const QString& uuid);

	Q_INVOKABLE
	bool deleteDepartmentById(const int& id);

	Q_INVOKABLE
    Department* findDepartmentById(const int& id);
	
	Q_INVOKABLE
	void fillTagDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInTagDataModel(QString objectName, Tag* listItem);

	Q_INVOKABLE
	void removeItemFromTagDataModel(QString objectName, Tag* listItem);

	Q_INVOKABLE
	void insertItemIntoTagDataModel(QString objectName, Tag* listItem);

	Q_INVOKABLE
	QList<Tag*> listOfTagForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList tagAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allTag();

	Q_INVOKABLE
	void deleteTag();

	// access from QML to list of all Tag
	QDeclarativeListProperty<Tag> tagPropertyList();

	Q_INVOKABLE
	Tag* createTag();

	Q_INVOKABLE
	void undoCreateTag(Tag* tag);

	Q_INVOKABLE
	void insertTag(Tag* tag);

	Q_INVOKABLE
	void insertTagFromMap(const QVariantMap& tagMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteTag(Tag* tag);

	Q_INVOKABLE
	bool deleteTagByUuid(const QString& uuid);

	Q_INVOKABLE
	Tag* findTagByUuid(const QString& uuid);
	
	Q_INVOKABLE
	void fillXtrasDataModel(QString objectName);

	Q_INVOKABLE
	void replaceItemInXtrasDataModel(QString objectName, Xtras* listItem);

	Q_INVOKABLE
	void removeItemFromXtrasDataModel(QString objectName, Xtras* listItem);

	Q_INVOKABLE
	void insertItemIntoXtrasDataModel(QString objectName, Xtras* listItem);

	Q_INVOKABLE
	QList<Xtras*> listOfXtrasForKeys(QStringList keyList);

	Q_INVOKABLE
	QVariantList xtrasAsQVariantList();

	Q_INVOKABLE
	QList<QObject*> allXtras();

	Q_INVOKABLE
	void deleteXtras();

	// access from QML to list of all Xtras
	QDeclarativeListProperty<Xtras> xtrasPropertyList();

	Q_INVOKABLE
	Xtras* createXtras();

	Q_INVOKABLE
	void undoCreateXtras(Xtras* xtras);

	Q_INVOKABLE
	void insertXtras(Xtras* xtras);

	Q_INVOKABLE
	void insertXtrasFromMap(const QVariantMap& xtrasMap, const bool& useForeignProperties);

	Q_INVOKABLE
	bool deleteXtras(Xtras* xtras);

	Q_INVOKABLE
	bool deleteXtrasByUuid(const QString& uuid);

	Q_INVOKABLE
	Xtras* findXtrasByUuid(const QString& uuid);

	Q_INVOKABLE
	bool deleteXtrasById(const int& id);

	Q_INVOKABLE
    Xtras* findXtrasById(const int& id);

	Q_INVOKABLE
	void setChunkSize(const int& newChunkSize);

    void initOrderFromCache();
    void initCustomerFromCache();
    void initTopicFromCache();
    void initDepartmentFromCache();
    void initTagFromCache();
    void initTagFromSqlCache();
    void initXtrasFromCache();

Q_SIGNALS:

	void addedToAllOrder(Order* order);
	void deletedFromAllOrderByUuid(QString uuid);
	void deletedFromAllOrderByNr(int nr);
	void deletedFromAllOrder(Order* order);
	void addedToAllCustomer(Customer* customer);
	void deletedFromAllCustomerByUuid(QString uuid);
	void deletedFromAllCustomerById(int id);
	void deletedFromAllCustomer(Customer* customer);
	void addedToAllTopic(Topic* topic);
	void deletedFromAllTopicByUuid(QString uuid);
	void deletedFromAllTopicById(int id);
	void deletedFromAllTopic(Topic* topic);
	void addedToAllDepartment(Department* department);
	void deletedFromAllDepartmentByUuid(QString uuid);
	void deletedFromAllDepartmentById(int id);
	void deletedFromAllDepartment(Department* department);
	void addedToAllTag(Tag* tag);
	void deletedFromAllTagByUuid(QString uuid);
	void deletedFromAllTag(Tag* tag);
	void addedToAllXtras(Xtras* xtras);
	void deletedFromAllXtrasByUuid(QString uuid);
	void deletedFromAllXtrasById(int id);
	void deletedFromAllXtras(Xtras* xtras);
    
public slots:
    void onManualExit();

private:

	// DataObject stored in List of QObject*
	// GroupDataModel only supports QObject*
    QList<QObject*> mAllOrder;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Order*
    static void appendToOrderProperty(
    	QDeclarativeListProperty<Order> *orderList,
    	Order* order);
    static int orderPropertyCount(
    	QDeclarativeListProperty<Order> *orderList);
    static Order* atOrderProperty(
    	QDeclarativeListProperty<Order> *orderList, int pos);
    static void clearOrderProperty(
    	QDeclarativeListProperty<Order> *orderList);
    QList<QObject*> mAllCustomer;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Customer*
    static void appendToCustomerProperty(
    	QDeclarativeListProperty<Customer> *customerList,
    	Customer* customer);
    static int customerPropertyCount(
    	QDeclarativeListProperty<Customer> *customerList);
    static Customer* atCustomerProperty(
    	QDeclarativeListProperty<Customer> *customerList, int pos);
    static void clearCustomerProperty(
    	QDeclarativeListProperty<Customer> *customerList);
    QList<QObject*> mAllTopic;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Topic*
    static void appendToTopicProperty(
    	QDeclarativeListProperty<Topic> *topicList,
    	Topic* topic);
    static int topicPropertyCount(
    	QDeclarativeListProperty<Topic> *topicList);
    static Topic* atTopicProperty(
    	QDeclarativeListProperty<Topic> *topicList, int pos);
    static void clearTopicProperty(
    	QDeclarativeListProperty<Topic> *topicList);
    QList<QObject*> mAllDepartment;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Department*
    static void appendToDepartmentProperty(
    	QDeclarativeListProperty<Department> *departmentList,
    	Department* department);
    static int departmentPropertyCount(
    	QDeclarativeListProperty<Department> *departmentList);
    static Department* atDepartmentProperty(
    	QDeclarativeListProperty<Department> *departmentList, int pos);
    static void clearDepartmentProperty(
    	QDeclarativeListProperty<Department> *departmentList);
    QList<QObject*> mAllDepartmentFlat;
    QList<QObject*> mAllTag;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Tag*
    static void appendToTagProperty(
    	QDeclarativeListProperty<Tag> *tagList,
    	Tag* tag);
    static int tagPropertyCount(
    	QDeclarativeListProperty<Tag> *tagList);
    static Tag* atTagProperty(
    	QDeclarativeListProperty<Tag> *tagList, int pos);
    static void clearTagProperty(
    	QDeclarativeListProperty<Tag> *tagList);
    QList<QObject*> mAllXtras;
    // implementation for QDeclarativeListProperty to use
    // QML functions for List of All Xtras*
    static void appendToXtrasProperty(
    	QDeclarativeListProperty<Xtras> *xtrasList,
    	Xtras* xtras);
    static int xtrasPropertyCount(
    	QDeclarativeListProperty<Xtras> *xtrasList);
    static Xtras* atXtrasProperty(
    	QDeclarativeListProperty<Xtras> *xtrasList, int pos);
    static void clearXtrasProperty(
    	QDeclarativeListProperty<Xtras> *xtrasList);

    void saveOrderToCache();
    void saveCustomerToCache();
    void saveTopicToCache();
    void saveDepartmentToCache();
    void saveTagToCache();
    	void saveTagToSqlCache();
    void saveXtrasToCache();

// S Q L
	QSqlDatabase mDatabase;
    bool mDatabaseAvailable;
    bool initDatabase();
    void bulkImport(const bool& tuneJournalAndSync);
    int mChunkSize;

	QVariantList readFromCache(QString& fileName);
	void writeToCache(QString& fileName, QVariantList& data);
	void finish();
};

#endif /* DATAMANAGER_HPP_ */
