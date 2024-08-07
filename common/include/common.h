#ifndef _COMMON_APP_H
#define _COMMON_APP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/in.h>
#include <linux/netfilter.h>
#include <linux/netlink.h>

#define MAXRuleNameLen 11

#define REQ_GETAllIPRules 1
#define REQ_ADDIPRule 2
#define REQ_DELIPRule 3
#define REQ_SETAction 4 
#define REQ_GETAllIPLogs 5
#define REQ_GETAllConns 6
#define REQ_ADDNATRule 7
#define REQ_DELNATRule 8
#define REQ_GETNATRules 9

#define RSP_Only_Head 10
#define RSP_MSG 11
#define RSP_IPRules 12 
#define RSP_IPLogs 13 
#define RSP_NATRules 14 
#define RSP_ConnLogs 15 

struct IPRule {
    char name[MAXRuleNameLen+1];
    unsigned int saddr;
    unsigned int smask;
    unsigned int daddr;
    unsigned int dmask;
    unsigned int sport;
    unsigned int dport;
    u_int8_t protocol;
    unsigned int action;
    unsigned int log;
    struct IPRule* nx;
};

struct IPLog {
    long tm;
    unsigned int saddr;
    unsigned int daddr;
    unsigned short sport;
    unsigned short dport;
    u_int8_t protocol;
    unsigned int len;
    unsigned int action;
    struct IPLog* nx;
};

struct NATRecord { 
    unsigned int saddr;
    unsigned int smask;
    unsigned int daddr;

    unsigned short sport;
    unsigned short dport;
    unsigned short nowPort;
    struct NATRecord* nx;
};

struct ConnLog {
    unsigned int saddr;
    unsigned int daddr;
    unsigned short sport;
    unsigned short dport;
    u_int8_t protocol;
    int natType;
    struct NATRecord nat;
};

struct APPRequest {
    unsigned int tp;
    char ruleName[MAXRuleNameLen+1];
    union {
        struct IPRule ipRule;
        struct NATRecord natRule;
        unsigned int defaultAction;
        unsigned int num;
    } msg;
};

struct KernelResponseHeader {
    unsigned int bodyTp;
    unsigned int arrayLen;
};

#define NAT_TYPE_NO 0
#define NAT_TYPE_SRC 1
#define NAT_TYPE_DEST 2


#define uint8_t unsigned char
#define NETLINK_MYFW 17
#define MAX_PAYLOAD (1024 * 256)

#define ERROR_CODE_EXIT -1
#define ERROR_CODE_EXCHANGE -2 
#define ERROR_CODE_WRONG_IP -11 
#define ERROR_CODE_NO_SUCH_RULE -12


struct KernelResponse {
    int code; // <0 代表请求失败，失败码; >=0 代表body长度
    void *data; // 回应包指针，记得free
    struct KernelResponseHeader *header; // 不要free；指向data中的头部
    void *body; // 不要free；指向data中的Body
};

struct KernelResponse exchangeMsgK(void *smsg, unsigned int slen);

// ----- kernel interacion -----
struct KernelResponse addFilterRule(char *after, char *name, char *sip, char *dip, unsigned int sport, unsigned int dport,
                                    u_int8_t proto, unsigned int log, unsigned int action);
struct KernelResponse delFilterRule(char *name);
struct KernelResponse getAllFilterRules(void);
struct KernelResponse addNATRule(char *sip,char *natIP,unsigned short minport,unsigned short maxport);
struct KernelResponse delNATRule(int num);
struct KernelResponse getAllNATRules(void);
struct KernelResponse setDefaultAction(unsigned int action);
struct KernelResponse getLogs(unsigned int num);
struct KernelResponse getAllConns(void);

// ----- tools ------
int IPstr2IPint(const char *ipStr, unsigned int *ip, unsigned int *mask);
int IPint2IPstr(unsigned int ip, unsigned int mask, char *ipStr);
int IPint2IPstrNoMask(unsigned int ip, char *ipStr);
int IPint2IPstrWithPort(unsigned int ip, unsigned short port, char *ipStr);

#endif
