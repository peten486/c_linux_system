#ifndef PACKET_HEADER
#define PACKET_HEADER

#define MTYPE_CREATE_SUBS_REQ 1
#define MTYPE_CREATE_SUBS_RES 2
#define MTYPE_DELETE_SUBS_REQ 3
#define MTYPE_DELETE_SUBS_RES 4
#define MTYPE_CHANGE_SUBS_REQ 5
#define MTYPE_CHANGE_SUBS_RES 6
#define MTYPE_DISPLAY_SUBS_REQ 7
#define MTYPE_DISPLAY_SUBS_RES 8

typedef struct {
        long mtype;
        char mtext[1];
} msgbuf;

typedef struct {
        unsigned int len;
        char data[2048];
} mtext_t;

typedef struct _sub_info {
        char mdn[12];
        int cnap;
        int oip;
        int cnhp;
        int cfd;
        int icb;
        int crbt;
        int vmsu;
        int cw;
        char cf_number[12];
} mobile_station_info;


/*--------- error -----------*/
typedef struct {
        int code;
        char reason[40];
} err_s;

/*-------- interface --------*/

typedef struct {
        mobile_station_info *data;
} create_subs_req_t;

typedef struct {
        char mdn[12];
        int result;
} create_subs_res_t;

typedef struct {
        mobile_station_info *data;
} delete_subs_req_t;

typedef struct {
        char mdn[12];
        int result;
} delete_subs_res_t;

typedef struct {
        mobile_station_info *data;
} change_subs_req_t;

typedef struct {
        char mdn[12];
        int result;
} change_subs_res_t;

typedef struct {
        mobile_station_info *data;
} query_subs_req_t;

typedef struct {
        mobile_station_info data;
} query_subs_res_t;


/*-------- tcp client socket info --------*/
typedef struct {
        int sock;
        struct sockaddr_in adr;
} clin_sock_info;



void print_d(mobile_station_info *cmd){
        printf("mobile Number : %s\n", cmd->mdn);
        printf("cnap : %d\n", cmd->cnap);
        printf("oip : %d\n", cmd->oip);
        printf("cnhp : %d\n", cmd->cnhp);
        printf("cfd : %d\n", cmd->cfd);
        printf("icb : %d\n", cmd->icb);
        printf("crbt : %d\n", cmd->crbt);
        printf("vmsu : %d\n", cmd->vmsu);
        printf("cw : %d\n", cmd->cw);
        printf("cf_number : %s\n", cmd->cf_number);
}

#endif