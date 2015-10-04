#ifndef ORDERSTATE_HPP_
#define ORDERSTATE_HPP_

#include <QObject>

class OrderState: public QObject
{

public:

	static const int DEFAULT_VALUE = 0;
	static const int NO_VALUE = -1;
	
	// 
	enum OrderStateEnum
	{	
		NEW, 
		CONFIRMED, 
		DELIVERED, 
		CLOSED
	};
	Q_ENUMS (OrderStateEnum)

};

#endif /* ORDERSTATE_HPP_ */


