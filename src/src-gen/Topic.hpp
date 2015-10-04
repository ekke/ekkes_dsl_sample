#ifndef TOPIC_HPP_
#define TOPIC_HPP_

#include <QObject>
#include <qvariant.h>
#include <QDeclarativeListProperty>




class Topic: public QObject
{
	Q_OBJECT

	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged FINAL)
	Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged FINAL)
	Q_PROPERTY(QString classification READ classification WRITE setClassification NOTIFY classificationChanged FINAL)
	Q_PROPERTY(Topic* parentTopic READ parentTopic WRITE setParentTopic NOTIFY parentTopicChanged FINAL)

	// QDeclarativeListProperty to get easy access from QML
	Q_PROPERTY(QDeclarativeListProperty<Topic> subTopicPropertyList READ subTopicPropertyList CONSTANT)

public:
	Topic(QObject *parent = 0);


	void fillFromMap(const QVariantMap& topicMap);
	void fillFromForeignMap(const QVariantMap& topicMap);
	void fillFromCacheMap(const QVariantMap& topicMap);
	
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
	QString classification() const;
	void setClassification(QString classification);
	Topic* parentTopic() const;
	void setParentTopic(Topic* parentTopic);
	Q_INVOKABLE
	Topic* createParentTopic();

	Q_INVOKABLE
	void undoCreateParentTopic(Topic* topic);
	
	Q_INVOKABLE
	void deleteParentTopic();
	
	Q_INVOKABLE
	bool hasParentTopic();
	

	
	Q_INVOKABLE
	QVariantList subTopicAsQVariantList();

	Q_INVOKABLE
	Topic* createElementOfSubTopic();

	Q_INVOKABLE
	void undoCreateElementOfSubTopic(Topic* topic);
	
	Q_INVOKABLE
	void addToSubTopic(Topic* topic);
	
	Q_INVOKABLE
	bool removeFromSubTopic(Topic* topic);

	Q_INVOKABLE
	void clearSubTopic();

	Q_INVOKABLE
	void addToSubTopicFromMap(const QVariantMap& topicMap);
	
	Q_INVOKABLE
	bool removeFromSubTopicByUuid(const QString& uuid);
	
	Q_INVOKABLE
	bool removeFromSubTopicById(const int& id);
	
	Q_INVOKABLE
	int subTopicCount();
	
	 // access from C++ to subTopic
	QList<Topic*> subTopic();
	void setSubTopic(QList<Topic*> subTopic);
	// access from QML to subTopic
	QDeclarativeListProperty<Topic> subTopicPropertyList();


	virtual ~Topic();

	Q_SIGNALS:

	void idChanged(int id);
	void uuidChanged(QString uuid);
	void classificationChanged(QString classification);
	void parentTopicChanged(Topic* parentTopic);
	void parentTopicDeleted(QString uuid);
	void subTopicChanged(QList<Topic*> subTopic);
	void addedToSubTopic(Topic* topic);
	void removedFromSubTopicByUuid(QString uuid);
	void removedFromSubTopicById(int id);
	
	

private:

	int mId;
	QString mUuid;
	QString mClassification;
	Topic* mParentTopic;
	QList<Topic*> mSubTopic;
	// implementation for QDeclarativeListProperty to use
	// QML functions for List of Topic*
	static void appendToSubTopicProperty(QDeclarativeListProperty<Topic> *subTopicList,
		Topic* topic);
	static int subTopicPropertyCount(QDeclarativeListProperty<Topic> *subTopicList);
	static Topic* atSubTopicProperty(QDeclarativeListProperty<Topic> *subTopicList, int pos);
	static void clearSubTopicProperty(QDeclarativeListProperty<Topic> *subTopicList);

	Q_DISABLE_COPY (Topic)
};
Q_DECLARE_METATYPE(Topic*)

#endif /* TOPIC_HPP_ */

