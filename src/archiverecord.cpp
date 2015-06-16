#include "archiverecord.h"

ArchiveRecord::ArchiveRecord(QObject *parent) :
    QObject(parent),
    IqOrmObject()
{
}

ArchiveRecord::~ArchiveRecord()
{
}

void ArchiveRecord::initializeOrmMetaModel(IqOrmMetaModel *model) const
{
    model->setTableName("archive");
    model->setPropertyColumn("channelName", "channel_name");
    model->setPropertyColumn("channelNumber", "channel_num");
    model->setPropertyColumn("headerInfo", "header_info");
    model->setPropertyColumn("priority");
    model->setPropertyColumn("addresses");
    model->setPropertyColumn("cc");
    model->setPropertyColumn("senderTime", "sender_time");
    model->setPropertyColumn("sender");
    model->setPropertyColumn("senderInfo", "sender_info");
    model->setPropertyColumn("text");
    model->setPropertyColumn("dateTime", "date_time");
    model->setPropertyColumn("direction");
    model->setPropertyColumn("channelId", "channel_id");
    model->setPropertyColumn("journal");
    model->setPropertyColumn("serialNumber", "serial_num");
    model->setPropertyColumn("svc");
    model->setPropertyColumn("routeId", "route_id");
    model->setPropertyColumn("msgType", "msg_type");
}

void ArchiveRecord::reset()
{
    setChannelName("");
    setChannelNumber(0);
    setHeaderInfo("");
    setPriority(0);
    setAddresses(QStringList());
    setCc(QStringList());
    setSenderTime("");
    setSender("");
    setSenderInfo("");
    setText("");
    setDateTime(QDateTime());
    setDirection(0);
    setChannelId(0);
    setJournal("");
    setSerialNumber(0);
    setSvc(false);
    setRouteId(0);
    setMsgType(0);
}

QString ArchiveRecord::channelName() const
{
    return m_channelName;
}

void ArchiveRecord::setChannelName(const QString &channelName)
{
    if (m_channelName != channelName) {
        m_channelName = channelName;
        emit channelNameChanged();
    }
}

int ArchiveRecord::channelNumber() const
{
    return m_channelNumber;
}

void ArchiveRecord::setChannelNumber(int channelNumber)
{
    if (m_channelNumber != channelNumber) {
        m_channelNumber = channelNumber;
        emit channelNumberChanged();
    }
}

QString ArchiveRecord::headerInfo() const
{
    return m_headerInfo;
}

void ArchiveRecord::setHeaderInfo(const QString &headerInfo)
{
    if (m_headerInfo != headerInfo) {
        m_headerInfo = headerInfo;
        emit headerInfoChanged();
    }
}

int ArchiveRecord::priority() const
{
    return m_priority;
}

void ArchiveRecord::setPriority(int priority)
{
    if (m_priority != priority) {
        m_priority = priority;
        emit priorityChanged();
    }
}

QStringList ArchiveRecord::addresses() const
{
    return m_addresses;
}

void ArchiveRecord::setAddresses(const QStringList &address)
{
    if (m_addresses != address) {
        m_addresses = address;
        emit addressesChanged();
    }
}

QStringList ArchiveRecord::cc() const
{
    return m_cc;
}

void ArchiveRecord::setCc(const QStringList &cc)
{
    if (m_cc != cc) {
        m_cc = cc;
        emit ccChanged();
    }
}

QString ArchiveRecord::senderTime() const
{
    return m_senderTime;
}

void ArchiveRecord::setSenderTime(const QString &senderTime)
{
    if (m_senderTime != senderTime) {
        m_senderTime = senderTime;
        emit senderTimeChanged();
    }
}

QString ArchiveRecord::sender() const
{
    return m_sender;
}

void ArchiveRecord::setSender(const QString &sender)
{
    if (m_sender != sender) {
        m_sender = sender;
        emit senderChanged();
    }
}
QString ArchiveRecord::senderInfo() const
{
    return m_senderInfo;
}

void ArchiveRecord::setSenderInfo(const QString &senderInfo)
{
    if (m_senderInfo != senderInfo) {
        m_senderInfo = senderInfo;
        emit senderInfoChanged();
    }
}

QString ArchiveRecord::text() const
{
    return m_text;
}

void ArchiveRecord::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}

QDateTime ArchiveRecord::dateTime() const
{
    return m_dateTime;
}

void ArchiveRecord::setDateTime(const QDateTime &dateTime)
{
    if (m_dateTime != dateTime) {
        m_dateTime = dateTime;
        emit dateTimeChanged();
    }
}

int ArchiveRecord::direction() const
{
    return m_direction;
}

void ArchiveRecord::setDirection(int direction)
{
    if (m_direction != direction) {
        m_direction = direction;
        emit directionChanged();
    }
}

int ArchiveRecord::channelId() const
{
    return m_channelId;
}

void ArchiveRecord::setChannelId(int channelId)
{
    if (m_channelId != channelId) {
        m_channelId = channelId;
        emit channelIdChanged();
    }
}

QString ArchiveRecord::journal() const
{
    return m_journal;
}

void ArchiveRecord::setJournal(const QString &journal)
{
    if (m_journal != journal) {
        m_journal = journal;
        emit journalChanged();
    }
}

int ArchiveRecord::serialNumber() const
{
    return m_serialNumber;
}

void ArchiveRecord::setSerialNumber(int serialNumber)
{
    if (m_serialNumber != serialNumber) {
        m_serialNumber = serialNumber;
        emit serialNumberChanged();
    }
}

bool ArchiveRecord::svc() const
{
    return m_svc;
}

void ArchiveRecord::setSvc(bool svc)
{
    if (m_svc != svc) {
        m_svc = svc;
        emit svcChanged();
    }
}

int ArchiveRecord::routeId() const
{
    return m_routeId;
}

void ArchiveRecord::setRouteId(int routeId)
{
    if (m_routeId != routeId) {
        m_routeId = routeId;
        emit routeIdChanged();
    }
}

int ArchiveRecord::msgType() const
{
    return m_msgType;
}

void ArchiveRecord::setMsgType(int msgType)
{
    if (m_msgType != msgType) {
        m_msgType = msgType;
        emit msgTypeChanged();
    }
}
