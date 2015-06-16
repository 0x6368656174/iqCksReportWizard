#ifndef ARCHIVERECORD_H
#define ARCHIVERECORD_H

#include <QObject>
#include <QDateTime>
#include <IqOrmObject>

class ArchiveRecord : public QObject, public IqOrmObject
{
    Q_OBJECT
    IQORM_OBJECT
    Q_PROPERTY(QString channelName READ channelName WRITE setChannelName NOTIFY channelNameChanged)
    Q_PROPERTY(int channelNumber READ channelNumber WRITE setChannelNumber NOTIFY channelNumberChanged)
    Q_PROPERTY(QString headerInfo READ headerInfo WRITE setHeaderInfo NOTIFY headerInfoChanged)
    Q_PROPERTY(int priority READ priority WRITE setPriority NOTIFY priorityChanged)
    Q_PROPERTY(QStringList addresses READ addresses WRITE setAddresses NOTIFY addressesChanged)
    Q_PROPERTY(QStringList cc READ cc WRITE setCc NOTIFY ccChanged)
    Q_PROPERTY(QString senderTime READ senderTime WRITE setSenderTime NOTIFY senderTimeChanged)
    Q_PROPERTY(QString sender READ sender WRITE setSender NOTIFY senderChanged)
    Q_PROPERTY(QString senderInfo READ senderInfo WRITE setSenderInfo NOTIFY senderInfoChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY dateTimeChanged)
    Q_PROPERTY(int direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(int channelId READ channelId WRITE setChannelId NOTIFY channelIdChanged)
    Q_PROPERTY(QString journal READ journal WRITE setJournal NOTIFY journalChanged)
    Q_PROPERTY(int serialNumber READ serialNumber WRITE setSerialNumber NOTIFY serialNumberChanged)
    Q_PROPERTY(bool svc READ svc WRITE setSvc NOTIFY svcChanged)
    Q_PROPERTY(int routeId READ routeId WRITE setRouteId NOTIFY routeIdChanged)
    Q_PROPERTY(int msgType READ msgType WRITE setMsgType NOTIFY msgTypeChanged)
public:
    explicit ArchiveRecord(QObject *parent = 0);
    ~ArchiveRecord();

    virtual void initializeOrmMetaModel(IqOrmMetaModel *model) const Q_DECL_OVERRIDE;

protected:
    virtual void reset() Q_DECL_OVERRIDE;

public:
    QString channelName() const;
    void setChannelName(const QString &channelName);

    int channelNumber() const;
    void setChannelNumber(int channelNumber);

    QString headerInfo() const;
    void setHeaderInfo(const QString &headerInfo);

    int priority() const;
    void setPriority(int priority);

    QStringList addresses() const;
    void setAddresses(const QStringList &addresses);

    QStringList cc() const;
    void setCc(const QStringList &cc);

    QString senderTime() const;
    void setSenderTime(const QString &senderTime);

    QString sender() const;
    void setSender(const QString &sender);

    QString senderInfo() const;
    void setSenderInfo(const QString &senderInfo);

    QString text() const;
    void setText(const QString &text);

    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dateTime);

    int direction() const;
    void setDirection(int direction);

    int channelId() const;
    void setChannelId(int channelId);

    QString journal() const;
    void setJournal(const QString &journal);

    int serialNumber() const;
    void setSerialNumber(int serialNumber);

    bool svc() const;
    void setSvc(bool svc);

    int routeId() const;
    void setRouteId(int routeId);

    int msgType() const;
    void setMsgType(int msgType);

signals:
    void channelNameChanged();
    void channelNumberChanged();
    void headerInfoChanged();
    void priorityChanged();
    void addressesChanged();
    void ccChanged();
    void senderTimeChanged();
    void senderChanged();
    void senderInfoChanged();
    void textChanged();
    void dateTimeChanged();
    void directionChanged();
    void channelIdChanged();
    void journalChanged();
    void serialNumberChanged();
    void svcChanged();
    void routeIdChanged();
    void msgTypeChanged();

private:
    QString m_channelName;
    int m_channelNumber;
    QString m_headerInfo;
    int m_priority;
    QStringList m_addresses;
    QStringList m_cc;
    QString m_senderTime;
    QString m_sender;
    QString m_senderInfo;
    QString m_text;
    QDateTime m_dateTime;
    int m_direction;
    int m_channelId;
    QString m_journal;
    int m_serialNumber;
    bool m_svc;
    int m_routeId;
    int m_msgType;
};

IQORM_REGISTER_CLASS(ArchiveRecord)

#endif // ARCHIVERECORD_H
