#include <iostream>
#include <cstring>
#include <cmqc.h>
#include <cmqxc.h>
#include <stdexcept>

class MQManager {
public:
    MQManager(const char* qManagerName, const char* channelName, const char* connName)
            : hConn(MQHC_UNUSABLE_HCONN), hObj(MQHO_UNUSABLE_HOBJ) {

        // 设置客户端连接
        memset(cd.ChannelName, 0 ,sizeof (cd.ChannelName));
        strcpy(cd.ChannelName, channelName);
        memset(cd.ConnectionName, 0 ,sizeof (cd.ConnectionName));
        strcpy(cd.ConnectionName, connName);
        cno.ClientConnPtr = &cd;


       cno.Options = MQCNO_STANDARD_BINDING;
        // 连接到队列管理器
        std::cout << "Connecting to queue manager: " << qManagerName << std::endl;
        MQCONNX((MQCHAR*)qManagerName, &cno, &hConn, &completionCode, &reasonCode);
        // MQCONN((MQCHAR*)qManagerName, &hConn, &completionCode, &reasonCode);


        if (completionCode != MQCC_OK) {
            std::cerr << "connect to queue manager failed, Reason code: " << reasonCode << std::endl;
            throw std::runtime_error("Failed to connect to queue manager");
        }

        std::cout << "Connected to queue manager successfully" << std::endl;
    }

    ~MQManager() {
        if (hObj != MQHO_UNUSABLE_HOBJ) {
            MQCLOSE(hConn, &hObj, MQCO_NONE, &completionCode, &reasonCode);
        }
        if (hConn != MQHC_UNUSABLE_HCONN) {
            MQDISC(&hConn, &completionCode, &reasonCode);
        }
    }

    void openQueue(const char* queueName) {
        memset(od.ObjectName, 0 ,sizeof(od.ObjectName));
        strcpy(od.ObjectName, queueName);
        MQOPEN(hConn, &od, MQOO_OUTPUT, &hObj, &completionCode, &reasonCode);

        if (completionCode != MQCC_OK) {
            std::cerr << "open queue failed, Reason code: " << reasonCode << std::endl;
            throw std::runtime_error("Failed to open queue");
        }
    }

    void putMessage(const char* message) {
        if (hObj == MQHO_UNUSABLE_HOBJ) {
            throw std::runtime_error("Queue is not open");
        }

        MQBYTE buffer[1024];
        strncpy(reinterpret_cast<char*>(buffer), message, 1024);
        buffer[1024 - 1] = '\0';

        MQLONG messageLength = strlen(message);
        MQPUT(hConn, hObj, &md, &pmo, messageLength, buffer, &completionCode, &reasonCode);

        if (completionCode != MQCC_OK) {
            std::cerr << "put message to queue failed, Reason code: " << reasonCode << std::endl;
            throw std::runtime_error("Failed to put message to queue");
        }
    }

private:
    MQHCONN hConn;
    MQHOBJ hObj;
    MQOD od = {MQOD_DEFAULT};
    MQMD md = {MQMD_DEFAULT};
    MQPMO pmo = {MQPMO_DEFAULT};
    MQCNO cno = {MQCNO_DEFAULT};
    MQCD cd = {MQCD_CLIENT_CONN_DEFAULT};
    MQCSP csp = {MQCSP_DEFAULT};
    MQLONG completionCode;
    MQLONG reasonCode;
};

int main() {
    try {
        MQManager mqManager("QM1", "", "");//MQ.QS.SVRCONN localhost(1414)
        mqManager.openQueue("QUEUE1");
        mqManager.putMessage("Hello, IBM MQ!");
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
