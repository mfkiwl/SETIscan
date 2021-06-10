#ifndef MSGIO_H
#define MSGIO_H

#include <QObject>
#include <QList>
#include <QString>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

#include "fftaggregator.h"
#include "singleton.h"

class MsgIO: public Singleton<MsgIO>, public QObject
	{
	Q_OBJECT

	public:
		/**********************************************************************\
		|* Typedefs and enums
		\**********************************************************************/
		struct SampleHeader
			{
			uint16_t order;
			uint16_t offset;
			uint32_t extent;
			uint16_t type;
			uint16_t flags;

			SampleHeader(void)
				{
				order	= 0xAA55;
				offset	= sizeof(SampleHeader);
				extent	= 0;
				type	= 0;
				flags	= 0;
				}
			};

	private:
		/**********************************************************************\
		|* Private variables
		\**********************************************************************/
		QWebSocketServer *		_server;		// Handle the connection
		QList<QWebSocket *>		_clients;		// List of connected clients


	private slots:
		/**********************************************************************\
		|* Private slots - generally for WebSocket operation
		\**********************************************************************/
		void onNewConnection(void);
		void socketDisconnected();
		void processTextMessage(const QString &message);
		void processBinaryMessage(QByteArray message);

	public:
		/**********************************************************************\
		|* Constructor / Destructor
		\**********************************************************************/
		explicit MsgIO(QObject *parent = nullptr);
		~MsgIO() override;

		/**********************************************************************\
		|* Initialise the server
		\**********************************************************************/
		void init(int port);

	public slots:
		/**********************************************************************\
		|* Receive data ready to send out, from the aggregator
		\**********************************************************************/
		void newData(FFTAggregator::DataType type, int64_t bufferId);

	};

#endif // MSGIO_H
