#ifndef EEPROM_ECU_SUBARU_DENSO_SH705X_CAN_H
#define EEPROM_ECU_SUBARU_DENSO_SH705X_CAN_H

#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QMainWindow>
#include <QSerialPort>
#include <QTime>
#include <QTimer>
#include <QWidget>

#include <kernelcomms.h>
#include <kernelmemorymodels.h>
#include <file_actions.h>
#include <serial_port/serial_port_actions.h>
#include <ui_ecu_operations.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class EcuOperationsWindow;
}
QT_END_NAMESPACE

class EepromEcuSubaruDensoSH705xCan : public QDialog
{
    Q_OBJECT

public:
    EepromEcuSubaruDensoSH705xCan(SerialPortActions *serial, FileActions::EcuCalDefStructure *ecuCalDef, QString cmd_type, QWidget *parent = nullptr);
    ~EepromEcuSubaruDensoSH705xCan();

    void run();

signals:
    void external_logger(QString message);
    void external_logger(int value);
    void LOG_E(QString message, bool timestamp, bool linefeed);
    void LOG_W(QString message, bool timestamp, bool linefeed);
    void LOG_I(QString message, bool timestamp, bool linefeed);
    void LOG_D(QString message, bool timestamp, bool linefeed);

private:
    FileActions::EcuCalDefStructure *ecuCalDef;
    QString cmd_type;

    #define STATUS_SUCCESS	0x00
    #define STATUS_ERROR	0x01

    bool kill_process = false;
    bool kernel_alive = false;
    bool test_write = false;
    bool request_denso_kernel_init = false;
    bool request_denso_kernel_id = false;

    int result;
    int mcu_type_index;
    int bootloader_start_countdown = 3;

    int EEPROM_MODE = 0;

    uint8_t tester_id;
    uint8_t target_id;

    uint16_t receive_timeout = 500;
    uint16_t serial_read_timeout = 2000;
    uint16_t serial_read_extra_short_timeout = 50;
    uint16_t serial_read_short_timeout = 200;
    uint16_t serial_read_medium_timeout = 400;
    uint16_t serial_read_long_timeout = 800;
    uint16_t serial_read_extra_long_timeout = 3000;

    uint32_t flashbytescount = 0;
    uint32_t flashbytesindex = 0;

    QString mcu_type_string;
    QString flash_method;
    QString kernel;

    void closeEvent(QCloseEvent *bar);

    int connect_bootloader_subaru_denso_subarucan();
    int upload_kernel_subaru_denso_subarucan(QString kernel, uint32_t kernel_start_addr);
    int read_mem_subaru_denso_subarucan(uint32_t start_addr, uint32_t length);


    uint8_t cks_add8(QByteArray chksum_data, unsigned len);
    void init_crc16_tab(void);
    uint16_t crc16(const uint8_t *data, uint32_t siz);

    QByteArray send_subaru_sid_bf_ssm_init();
    QByteArray send_subaru_denso_sid_81_start_communication();
    QByteArray send_subaru_denso_sid_83_request_timings();
    QByteArray send_subaru_denso_sid_27_request_seed();
    QByteArray send_subaru_denso_sid_27_send_seed_key(QByteArray seed_key);
    QByteArray send_subaru_denso_sid_10_start_diagnostic();
    QByteArray send_subaru_denso_sid_34_request_upload(uint32_t dataaddr, uint32_t datalen);
    QByteArray send_subaru_denso_sid_36_transferdata(uint32_t dataaddr, QByteArray buf, uint32_t len);
    QByteArray send_subaru_denso_sid_31_start_routine();

    QByteArray subaru_denso_generate_can_seed_key(QByteArray seed);
    QByteArray subaru_denso_generate_ecutek_can_seed_key(QByteArray requested_seed);
    QByteArray subaru_denso_generate_cobb_can_seed_key(QByteArray requested_seed);
    QByteArray subaru_denso_calculate_seed_key(QByteArray requested_seed, const uint16_t *keytogenerateindex, const uint8_t *indextransformation);

    QByteArray request_kernel_init();
    QByteArray request_kernel_id();

    QByteArray subaru_denso_encrypt_32bit_payload(QByteArray buf, uint32_t len);
    QByteArray subaru_denso_decrypt_32bit_payload(QByteArray buf, uint32_t len);
    QByteArray subaru_denso_calculate_32bit_payload(QByteArray buf, uint32_t len, const uint16_t *keytogenerateindex, const uint8_t *indextransformation);

    QByteArray add_ssm_header(QByteArray output, uint8_t tester_id, uint8_t target_id, bool dec_0x100);
    uint8_t calculate_checksum(QByteArray output, bool dec_0x100);

    int connect_bootloader_start_countdown(int timeout);
    QString parse_message_to_hex(QByteArray received);
    int send_log_window_message(QString message, bool timestamp, bool linefeed);
    void set_progressbar_value(int value);
    void delay(int timeout);

    SerialPortActions *serial;
    Ui::EcuOperationsWindow *ui;

};


#endif // EEPROM_ECU_SUBARU_DENSO_SH705X_CAN_H
