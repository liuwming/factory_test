#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "rtt_interface.h"

#define res(field) result->body.status[i].field

static rt_cmd_result_t final_res;
static rt_cmd_result_t *result = &final_res;

static char *log_file = "/home/yeelink/test_data/rssi_test99.log";

static void print_test_status(rt_cmd_result_t* result)
{
    int i;
    int l;
    char buf[1024];


FILE * fp;
char bufdate[33];
fp =popen("date +%Y%m%d%H%M%S","r");
fgets(bufdate,sizeof(bufdate),fp);



    memset(buf, 0, sizeof buf);

    l = sprintf(buf, "--------%s--------\n" , result->type == RT_CMD_STATUS ? 
           "status" : "result");
    l += sprintf(buf + l, "-firmware_ver-brs-brf-avg_rssi-min_rssi-max_rssi-packet_send-packet_recv\n");

    for (i = 0; i < result->dev_count; i++) {
        if (result->type == RT_CMD_RESULT) {
            if (res(avg_rssi) >= -50 
                    && res(packet_recv) > 10 
                    && ((res(packet_recv)*100) / res(packet_send) >= 70) )
                l += sprintf(buf + l, "%s,", "OK");
            else
                l += sprintf(buf + l, "%s,", "NG");

            l += sprintf(buf + l, "%s,%s,", res(mac), res(ip));
        } else {
            l += sprintf(buf + l, "+Device: Status[%s] MAC[%s] IP[%s]\n", res(state), res(mac), res(ip));
	}

        if (strcmp(res(state), STATE_DEV_CONN) == 0) {
            l += sprintf(buf + l, "%s", res(fw_ver));
            l += sprintf(buf + l, ",%d", res(brs));
            l += sprintf(buf + l, ",%d", res(brf));            
            l += sprintf(buf + l, ",%d", res(avg_rssi));
            l += sprintf(buf + l, ",%d", res(min_rssi));
            l += sprintf(buf + l, ",%d", res(max_rssi));
            l += sprintf(buf + l, ",%d", res(packet_send));
            l += sprintf(buf + l, ",%d", res(packet_recv));
            l += sprintf(buf + l, ",%s", bufdate);
        }
        /*l += sprintf(buf + l, "\n");
*/
    }

    printf("%s", buf);

    if (result->type == RT_CMD_RESULT) {
	    FILE *fp;
	    char filenamebuf[256];
	    struct timeval tv = {0,0};
            gettimeofday(&tv, NULL);
            snprintf(filenamebuf, sizeof filenamebuf, "%s-%ld", log_file, tv.tv_sec);
	    fp = fopen(filenamebuf, "a");
	    if (!fp)
		    printf("Error: log file open failure, will lost some test results\n");

	    fwrite(buf, strlen(buf), 1, fp);
	    fclose(fp);
    }
}

static void test_stop(rtt_handle_t hdl)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_STOP;

    rtt_send(hdl, &cmd);
    rc = rtt_recv(hdl, &res, RT_CMD_STOP);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    } else {
        rc = rtt_recv(hdl, &res, RT_CMD_STOP);
        if (rc <= 0) return;
        print_test_status(&res);
    }

}

static void open_box(rtt_handle_t hdl)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_OPEN_SBOX;

    rtt_send(hdl, &cmd);

    rc = rtt_recv(hdl, &res, RT_CMD_OPEN_SBOX);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }

    printf("sbox opened.\n");    
    
}

static void close_box(rtt_handle_t hdl)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_CLOSE_SBOX;

    rtt_send(hdl, &cmd);

    rc = rtt_recv(hdl, &res, RT_CMD_CLOSE_SBOX);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }

    printf("sbox closed.\n");

}

static void rainy_day(rtt_handle_t hdl)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_CLOSE_SBOX;
    rtt_send(hdl, &cmd);
    cmd.type = RT_CMD_OPEN_SBOX;
    rtt_send(hdl, &cmd);
    rc = rtt_recv(hdl, &res, RT_CMD_CLOSE_SBOX);
    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }
    rc = rtt_recv(hdl, &res, RT_CMD_OPEN_SBOX);
    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }
}


static void test_start(rtt_handle_t hdl)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    cmd.type = RT_CMD_CLOSE_SBOX;
    rtt_send(hdl, &cmd);
    rc = rtt_recv(hdl, &res, RT_CMD_CLOSE_SBOX);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }

    cmd.type = RT_CMD_START;
    rtt_send(hdl, &cmd);
    rc = rtt_recv(hdl, &res, RT_CMD_START);

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    }

    printf("Command Begin.\n");
    while (1) {
        rc = rtt_recv(hdl, &res, RT_CMD_MAX); /* receive all messages from daemon */
        if (rc <= 0) break;
        if (res.type == RT_CMD_STATUS) {
            printf("Test status update.\n");
            print_test_status(&res);
        }
        if (res.type == RT_CMD_RESULT) {
            printf("Test Result.\n");
            print_test_status(&res);
            memcpy(&final_res, &res, sizeof(res));
            break;
        }
    }
}

static void control_dev(rtt_handle_t hdl, char* mac, int result)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;


    strcpy(cmd.body.mac, mac);

    if (result == 0) {
        cmd.type = RT_CMD_CTRL_DEV;
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_CTRL_DEV);
    } else if (result == 1) {
        cmd.type = RT_CMD_FAIL_DEV;
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_FAIL_DEV);
    } else {
        cmd.type = RT_CMD_OTA_DEV;
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_OTA_DEV);        
    }

    if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
        printf("Failed to exe command %d.\n", res.ret_code);
        return;
    } else {
        printf("command execute successfully.\n");
    }
}


int main(int argc, char** argv)
{
    rtt_handle_t hdl;
    int err;
    int c;
    char mac[32];
    int i;

    setbuf(stdout, NULL);

    hdl = rtt_connect(5, &err, RTT_PROTO_UDP, argv[1]);
    if (!hdl) {
        printf("连不上服务器 %d.\n", err);
        return -1;
    }
    printf("客户端开始运行，请根据说明输入相应代码测试!\n");
    while ( (c = getchar()) != 'q') {
        switch(c) {
        case '1':
            test_start(hdl);
            break;
        case '2':
	    for (i = 0; i < result->dev_count; i++) {
		if (res(avg_rssi) >= -50 
		    && res(packet_recv) > 10
		    && ((res(packet_recv)*100) / res(packet_send) >= 70)) {
                        control_dev(hdl, res(mac), 0);
                }
	    }

	    break;
            printf("input mac address:\n");
            scanf("%s", mac);
            printf("control mac %s\n", mac);
            control_dev(hdl, mac, 0);
            break;
        case '3':
            printf("input mac address:\n");
            scanf("%s", mac);
            printf("control mac %s\n", mac);
            control_dev(hdl, mac, 1);
            break;
        case '4':
            /*
            printf("input mac address:\n");
            scanf("%s", mac);
            printf("control mac %s\n", mac);
            control_dev(hdl, mac, 2); 
            */
            for (i = 0; i < result->dev_count; i++) {
                if (strcmp(res(state), STATE_DEV_CONN) == 0) {
                    control_dev(hdl, res(mac), 2); 
                    sleep(1);
                }
            }
            break;
            for (i = 0; i < result->dev_count; i++) 
                control_dev(hdl, res(mac), 2);
            break;
        case '5':
            open_box(hdl);
            break;
        case '6':
            close_box(hdl);
            break;
        case '7':
            rainy_day(hdl);
            break;
        default:
            break;
        }
    }
    return 0;
}
