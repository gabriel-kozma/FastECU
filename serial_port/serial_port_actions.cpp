#include "serial_port_actions.h"
#include "qtrohelper.hpp"

SerialPortActions::SerialPortActions(QString peerAddress, QWebSocket *web_socket, QObject *parent)
    : QObject{parent}
    , peerAddress(peerAddress)
    , webSocket(web_socket == nullptr ?
                    new QWebSocket("",QWebSocketProtocol::VersionLatest,this)
                    :
                    web_socket)
    , socket(new WebSocketIoDevice(webSocket, webSocket))
    , serial_direct(nullptr)
    , serial_remote(nullptr)
{
    if (isDirectConnection())
        serial_direct = new SerialPortActionsDirect(this);
    else
        startRemote();
}

SerialPortActions::~SerialPortActions()
{
    delete serial_direct;
    delete serial_remote;
}

void SerialPortActions::startRemote(void)
{
    if (peerAddress.startsWith("local:"))
    {
        startLocal();
    }
    else
    {
        startOverNetwok();
    }
    QObject::connect(serial_remote, &SerialPortActionsRemoteReplica::stateChanged,
                     this, &SerialPortActions::serialRemoteStateChanged);
}

void SerialPortActions::startOverNetwok(void)
{
    QSslConfiguration sslConfiguration;
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    webSocket->setSslConfiguration(sslConfiguration);
    //Start node when Web Socket will be up
    //If it is already connected, connection could not be established
    QObject::connect(webSocket, &QWebSocket::connected, this, &SerialPortActions::websocket_connected);
    node.setHeartbeatInterval(heartbeatInterval);
    QObject::connect(webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                     this, [=](QAbstractSocket::SocketError error)
                     { qDebug() << this->metaObject()->className() << "startOverNetwok QWebSocket error:" << error; });
    //WebSocket over SSL
    webSocket->open("wss://" + peerAddress);

    //Connect to source published with name
    //Class SerialPortActionsRemoteReplica is autogenerated from serial_port_actions.rep
    serial_remote = node.acquire<SerialPortActionsRemoteReplica>(remoteObjectName);
    //Don't wait for replication here, it should be done from outside
    //Then it is possible to run several remote objects over single Web Socket
}

void SerialPortActions::startLocal(void)
{
    node.connectToNode(QUrl(peerAddress));
    serial_remote = node.acquire<SerialPortActionsRemoteReplica>(remoteObjectName);
    /*while (!serial_remote->waitForSource(10000))
    {
        qDebug() << "Waiting for remote peer...";
    }*/
}

void SerialPortActions::sendAutoDiscoveryMessage()
{
    if (webSocket->isValid())
    {
        webSocket->sendTextMessage(autodiscoveryMessage);
    }
}

bool SerialPortActions::isDirectConnection(void)
{
    return (peerAddress == "");
}

void SerialPortActions::websocket_connected(void)
{
    //Run client node after socket is up
    node.addClientSideConnection(socket);
    sendAutoDiscoveryMessage();
}

void SerialPortActions::waitForSource(void)
{
    //Wait for replication
    while (!serial_remote->waitForSource(10000))
    {
        sendAutoDiscoveryMessage();
        qDebug() << "Waiting for remote peer...";
    }
}

void SerialPortActions::serialRemoteStateChanged(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState)
{
    emit stateChanged(state, oldState);
    if (state == QRemoteObjectReplica::Valid)
        qDebug() << "SerialPortActions remote connection established";
    else if (oldState == QRemoteObjectReplica::Valid)
        qDebug() << "SerialPortActions remote connection lost";
}

bool SerialPortActions::get_serialPortAvailable(void)
{
    if (isDirectConnection())
        return serial_direct->serialPortAvailable;
    else
        return qtrohelper::slot_sync(serial_remote->get_serialPortAvailable());
}
bool SerialPortActions::set_serialPortAvailable(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serialPortAvailable = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serialPortAvailable(value));
    return r;
}
bool SerialPortActions::get_setRequestToSend(void)
{
    if (isDirectConnection())
        return serial_direct->setRequestToSend;
    else
        return qtrohelper::slot_sync(serial_remote->get_setRequestToSend());
}
bool SerialPortActions::set_setRequestToSend(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->setRequestToSend = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_setRequestToSend(value));
    return r;
}
bool SerialPortActions::get_setDataTerminalReady(void)
{
    if (isDirectConnection())
        return serial_direct->setDataTerminalReady;
    else
        return qtrohelper::slot_sync(serial_remote->get_setDataTerminalReady());
}
bool SerialPortActions::set_setDataTerminalReady(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->setDataTerminalReady = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_setDataTerminalReady(value));
    return r;
}

bool SerialPortActions::get_add_iso14230_header(void)
{
    if (isDirectConnection())
        return serial_direct->add_iso14230_header;
    else
        return qtrohelper::slot_sync(serial_remote->get_add_iso14230_header());
}
bool SerialPortActions::set_add_iso14230_header(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->add_iso14230_header = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_add_iso14230_header(value));
    return r;
}
bool SerialPortActions::get_is_iso14230_connection(void)
{
    if (isDirectConnection())
        return serial_direct->is_iso14230_connection;
    else
        return qtrohelper::slot_sync(serial_remote->get_is_iso14230_connection());
}
bool SerialPortActions::set_is_iso14230_connection(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->is_iso14230_connection = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_is_iso14230_connection(value));
    return r;
}
bool SerialPortActions::get_is_can_connection(void)
{
    if (isDirectConnection())
        return serial_direct->is_can_connection;
    else
        return qtrohelper::slot_sync(serial_remote->get_is_can_connection());
}
bool SerialPortActions::set_is_can_connection(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->is_can_connection = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_is_can_connection(value));
    return r;
}
bool SerialPortActions::get_is_iso15765_connection(void)
{
    if (isDirectConnection())
        return serial_direct->is_iso15765_connection;
    else
        return qtrohelper::slot_sync(serial_remote->get_is_iso15765_connection());
}
bool SerialPortActions::set_is_iso15765_connection(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->is_iso15765_connection = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_is_iso15765_connection(value));
    return r;
}
bool SerialPortActions::get_is_29_bit_id(void)
{
    if (isDirectConnection())
        return serial_direct->is_29_bit_id;
    else
        return qtrohelper::slot_sync(serial_remote->get_is_29_bit_id());
}
bool SerialPortActions::set_is_29_bit_id(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->is_29_bit_id = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_is_29_bit_id(value));
    return r;
}

bool SerialPortActions::get_use_openport2_adapter(void)
{
    if (isDirectConnection())
        return serial_direct->use_openport2_adapter;
    else
        return qtrohelper::slot_sync(serial_remote->get_use_openport2_adapter());
}
bool SerialPortActions::set_use_openport2_adapter(bool value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->use_openport2_adapter = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_use_openport2_adapter(value));
    return r;
}

int SerialPortActions::get_requestToSendEnabled(void)
{
    if (isDirectConnection())
        return serial_direct->requestToSendEnabled;
    else
        return qtrohelper::slot_sync(serial_remote->get_requestToSendEnabled());
}
bool SerialPortActions::set_requestToSendEnabled(int value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->requestToSendEnabled = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_requestToSendEnabled(value));
    return r;
}
int SerialPortActions::get_requestToSendDisabled(void)
{
    if (isDirectConnection())
        return serial_direct->requestToSendDisabled;
    else
        return qtrohelper::slot_sync(serial_remote->get_requestToSendDisabled());
}
bool SerialPortActions::set_requestToSendDisabled(int value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->requestToSendDisabled = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_requestToSendDisabled(value));
    return r;
}
int SerialPortActions::get_dataTerminalEnabled(void)
{
    if (isDirectConnection())
        return serial_direct->dataTerminalEnabled;
    else
        return qtrohelper::slot_sync(serial_remote->get_dataTerminalEnabled());
}
bool SerialPortActions::set_dataTerminalEnabled(int value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->dataTerminalEnabled = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_dataTerminalEnabled(value));
    return r;
}
int SerialPortActions::get_dataTerminalDisabled(void)
{
    if (isDirectConnection())
        return serial_direct->dataTerminalDisabled;
    else
        return qtrohelper::slot_sync(serial_remote->get_dataTerminalDisabled());
}
bool SerialPortActions::set_dataTerminalDisabled(int value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->dataTerminalDisabled = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_dataTerminalDisabled(value));
    return r;
}

uint8_t SerialPortActions::get_iso14230_startbyte(void)
{
    if (isDirectConnection())
        return serial_direct->iso14230_startbyte;
    else
        return qtrohelper::slot_sync(serial_remote->get_iso14230_startbyte());
}
bool SerialPortActions::set_iso14230_startbyte(uint8_t value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->iso14230_startbyte = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_iso14230_startbyte(value));
    return r;
}
uint8_t SerialPortActions::get_iso14230_tester_id(void)
{
    if (isDirectConnection())
        return serial_direct->iso14230_tester_id;
    else
        return qtrohelper::slot_sync(serial_remote->get_iso14230_tester_id());
}
bool SerialPortActions::set_iso14230_tester_id(uint8_t value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->iso14230_tester_id = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_iso14230_tester_id(value));
    return r;
}
uint8_t SerialPortActions::get_iso14230_target_id(void)
{
    if (isDirectConnection())
        return serial_direct->iso14230_target_id;
    else
        return qtrohelper::slot_sync(serial_remote->get_iso14230_target_id());
}
bool SerialPortActions::set_iso14230_target_id(uint8_t value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->iso14230_target_id = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_iso14230_target_id(value));
    return r;
}

QByteArray SerialPortActions::get_ssm_receive_header_start(void)
{
    if (isDirectConnection())
        return serial_direct->ssm_receive_header_start;
    else
        return qtrohelper::slot_sync(serial_remote->get_ssm_receive_header_start());
}
bool SerialPortActions::set_ssm_receive_header_start(QByteArray value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->ssm_receive_header_start = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_ssm_receive_header_start(value));
    return r;
}

QStringList SerialPortActions::get_serial_port_list(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port_list;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port_list());
}
bool SerialPortActions::set_serial_port_list(QStringList value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port_list = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port_list(value));
    return r;
}
QString SerialPortActions::get_openedSerialPort(void)
{
    if (isDirectConnection())
        return serial_direct->openedSerialPort;
    else
        return qtrohelper::slot_sync(serial_remote->get_openedSerialPort());
}
bool SerialPortActions::set_openedSerialPort(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->openedSerialPort = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_openedSerialPort(value));
    return r;
}
QString SerialPortActions::get_subaru_02_16bit_bootloader_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_02_16bit_bootloader_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_02_16bit_bootloader_baudrate());
}
bool SerialPortActions::set_subaru_02_16bit_bootloader_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_02_16bit_bootloader_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_02_16bit_bootloader_baudrate(value));
    return r;
}
QString SerialPortActions::get_subaru_04_16bit_bootloader_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_04_16bit_bootloader_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_04_16bit_bootloader_baudrate());
}
bool SerialPortActions::set_subaru_04_16bit_bootloader_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_04_16bit_bootloader_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_04_16bit_bootloader_baudrate(value));
    return r;
}
QString SerialPortActions::get_subaru_02_32bit_bootloader_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_02_32bit_bootloader_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_02_32bit_bootloader_baudrate());
}
bool SerialPortActions::set_subaru_02_32bit_bootloader_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_02_32bit_bootloader_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_02_32bit_bootloader_baudrate(value));
    return r;
}
QString SerialPortActions::get_subaru_04_32bit_bootloader_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_04_32bit_bootloader_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_04_32bit_bootloader_baudrate());
}
bool SerialPortActions::set_subaru_04_32bit_bootloader_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_04_32bit_bootloader_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_04_32bit_bootloader_baudrate(value));
    return r;
}
QString SerialPortActions::get_subaru_05_32bit_bootloader_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_05_32bit_bootloader_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_05_32bit_bootloader_baudrate());
}
bool SerialPortActions::set_subaru_05_32bit_bootloader_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_05_32bit_bootloader_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_05_32bit_bootloader_baudrate(value));
    return r;
}

QString SerialPortActions::get_subaru_02_16bit_kernel_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_02_16bit_kernel_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_02_16bit_kernel_baudrate());
}
bool SerialPortActions::set_subaru_02_16bit_kernel_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_02_16bit_kernel_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_02_16bit_kernel_baudrate(value));
    return r;
}
QString SerialPortActions::get_subaru_04_16bit_kernel_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_04_16bit_kernel_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_04_16bit_kernel_baudrate());
}
bool SerialPortActions::set_subaru_04_16bit_kernel_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_04_16bit_kernel_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_04_16bit_kernel_baudrate(value));
    return r;
}
QString SerialPortActions::get_subaru_02_32bit_kernel_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_02_32bit_kernel_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_02_32bit_kernel_baudrate());
}
bool SerialPortActions::set_subaru_02_32bit_kernel_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_02_32bit_kernel_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_02_32bit_kernel_baudrate(value));
    return r;
}
QString SerialPortActions::get_subaru_04_32bit_kernel_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_04_32bit_kernel_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_04_32bit_kernel_baudrate());
}
bool SerialPortActions::set_subaru_04_32bit_kernel_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_04_32bit_kernel_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_04_32bit_kernel_baudrate(value));
    return r;
}
QString SerialPortActions::get_subaru_05_32bit_kernel_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->subaru_05_32bit_kernel_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_subaru_05_32bit_kernel_baudrate());
}
bool SerialPortActions::set_subaru_05_32bit_kernel_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->subaru_05_32bit_kernel_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_subaru_05_32bit_kernel_baudrate(value));
    return r;
}

QString SerialPortActions::get_can_speed(void)
{
    if (isDirectConnection())
        return serial_direct->can_speed;
    else
        return qtrohelper::slot_sync(serial_remote->get_can_speed());
}
bool SerialPortActions::set_can_speed(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->can_speed = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_can_speed(value));
    return r;
}
uint8_t SerialPortActions::get_serial_port_parity(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port_parity;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port_parity());
}
bool SerialPortActions::set_serial_port_parity(uint8_t parity)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port_parity = parity;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port_parity(parity));
    return r;
}
QString SerialPortActions::get_serial_port_baudrate(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port_baudrate;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port_baudrate());
}
bool SerialPortActions::set_serial_port_baudrate(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port_baudrate = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port_baudrate(value));
    return r;
}
QString SerialPortActions::get_serial_port_linux(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port_linux;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port_linux());
}
bool SerialPortActions::set_serial_port_linux(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port_linux = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port_linux(value));
    return r;
}
QString SerialPortActions::get_serial_port_windows(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port_windows;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port_windows());
}
bool SerialPortActions::set_serial_port_windows(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port_windows = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port_windows(value));
    return r;
}
QString SerialPortActions::get_serial_port(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port());
}
bool SerialPortActions::set_serial_port(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port(value));
    return r;
}
QString SerialPortActions::get_serial_port_prefix(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port_prefix;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port_prefix());
}
bool SerialPortActions::set_serial_port_prefix(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port_prefix = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port_prefix(value));
    return r;
}
QString SerialPortActions::get_serial_port_prefix_linux(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port_prefix_linux;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port_prefix_linux());
}
bool SerialPortActions::set_serial_port_prefix_linux(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port_prefix_linux = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port_prefix_linux(value));
    return r;
}
QString SerialPortActions::get_serial_port_prefix_win(void)
{
    if (isDirectConnection())
        return serial_direct->serial_port_prefix_win;
    else
        return qtrohelper::slot_sync(serial_remote->get_serial_port_prefix_win());
}
bool SerialPortActions::set_serial_port_prefix_win(QString value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->serial_port_prefix_win = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_serial_port_prefix_win(value));
    return r;
}

uint32_t SerialPortActions::get_can_source_address(void)
{
    if (isDirectConnection())
        return serial_direct->can_source_address;
    else
        return qtrohelper::slot_sync(serial_remote->get_can_source_address());
}
bool SerialPortActions::set_can_source_address(uint32_t value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->can_source_address = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_can_source_address(value));
    return r;
}
uint32_t SerialPortActions::get_can_destination_address(void)
{
    if (isDirectConnection())
        return serial_direct->can_destination_address;
    else
        return qtrohelper::slot_sync(serial_remote->get_can_destination_address());
}
bool SerialPortActions::set_can_destination_address(uint32_t value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->can_destination_address = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_can_destination_address(value));
    return r;
}
uint32_t SerialPortActions::get_iso15765_source_address(void)
{
    if (isDirectConnection())
        return serial_direct->iso15765_source_address;
    else
        return qtrohelper::slot_sync(serial_remote->get_iso15765_source_address());
}
bool SerialPortActions::set_iso15765_source_address(uint32_t value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->iso15765_source_address = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_iso15765_source_address(value));
    return r;
}
uint32_t SerialPortActions::get_iso15765_destination_address(void)
{
    if (isDirectConnection())
        return serial_direct->iso15765_destination_address;
    else
        return qtrohelper::slot_sync(serial_remote->get_iso15765_destination_address());
}
bool SerialPortActions::set_iso15765_destination_address(uint32_t value)
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->iso15765_destination_address = value;
    else
        r = qtrohelper::slot_sync(serial_remote->set_iso15765_destination_address(value));
    return r;
}

bool SerialPortActions::is_serial_port_open()
{
    if (isDirectConnection())
        return serial_direct->is_serial_port_open();
    else
        return qtrohelper::slot_sync(serial_remote->is_serial_port_open());
}

int SerialPortActions::change_port_speed(QString portSpeed)
{
    if (isDirectConnection())
        return serial_direct->change_port_speed(portSpeed);
    else
        return qtrohelper::slot_sync(serial_remote->change_port_speed(portSpeed));
}

int SerialPortActions::fast_init(QByteArray output)
{
    if (isDirectConnection())
        return serial_direct->fast_init(output);
    else
        return qtrohelper::slot_sync(serial_remote->fast_init(output));
}

int SerialPortActions::set_lec_lines(int lec1, int lec2)
{
    if (isDirectConnection())
        return serial_direct->set_lec_lines(lec1, lec2);
    else
        return qtrohelper::slot_sync(serial_remote->set_lec_lines(lec1, lec2));
}

int SerialPortActions::pulse_lec_1_line(int timeout)
{
    if (isDirectConnection())
        return serial_direct->pulse_lec_1_line(timeout);
    else
        return qtrohelper::slot_sync(serial_remote->pulse_lec_1_line(timeout));
}

int SerialPortActions::pulse_lec_2_line(int timeout)
{
    if (isDirectConnection())
        return serial_direct->pulse_lec_2_line(timeout);
    else
        return qtrohelper::slot_sync(serial_remote->pulse_lec_2_line(timeout));
}

bool SerialPortActions::reset_connection()
{
    bool r = true;
    if (isDirectConnection())
        serial_direct->reset_connection();
    else
        r = qtrohelper::slot_sync(serial_remote->reset_connection());
    return r;
}

QByteArray SerialPortActions::read_serial_data(uint32_t datalen, uint16_t timeout)
{
    if (isDirectConnection())
        return serial_direct->read_serial_data(datalen, timeout);
    else
        return qtrohelper::slot_sync(serial_remote->read_serial_data(datalen, timeout));
}

QByteArray SerialPortActions::write_serial_data(QByteArray output)
{
    if (isDirectConnection())
        return serial_direct->write_serial_data(output);
    else
        return qtrohelper::slot_sync(serial_remote->write_serial_data(output));
}

QByteArray SerialPortActions::write_serial_data_echo_check(QByteArray output)
{
    if (isDirectConnection())
        return serial_direct->write_serial_data_echo_check(output);
    else
        return qtrohelper::slot_sync(serial_remote->write_serial_data_echo_check(output));
}

int SerialPortActions::clear_rx_buffer()
{
    if (isDirectConnection())
        return serial_direct->clear_rx_buffer();
    else
        return qtrohelper::slot_sync(serial_remote->clear_rx_buffer());
}

int SerialPortActions::clear_tx_buffer()
{
    if (isDirectConnection())
        return serial_direct->clear_tx_buffer();
    else
        return qtrohelper::slot_sync(serial_remote->clear_tx_buffer());
}

int SerialPortActions::send_periodic_j2534_data(QByteArray output, int timeout)
{
    if (isDirectConnection())
        return serial_direct->send_periodic_j2534_data(output, timeout);
    else
        return qtrohelper::slot_sync(serial_remote->send_periodic_j2534_data(output, timeout));
}

int SerialPortActions::stop_periodic_j2534_data()
{
    if (isDirectConnection())
        return serial_direct->stop_periodic_j2534_data();
    else
        return qtrohelper::slot_sync(serial_remote->stop_periodic_j2534_data());
}

QStringList SerialPortActions::check_serial_ports()
{
    if (isDirectConnection())
        return serial_direct->check_serial_ports();
    else
        return qtrohelper::slot_sync(serial_remote->check_serial_ports());
}

QString SerialPortActions::open_serial_port()
{
    if (isDirectConnection())
        return serial_direct->open_serial_port();
    else
        return qtrohelper::slot_sync(serial_remote->open_serial_port());
}
