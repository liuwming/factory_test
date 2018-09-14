#include <stdio.h>
#include <string.h>
#include "rtt_interface.h"
#include <ctype.h>
#include <stdlib.h>

#define RSSI_THRESH_AVG -50
#define RSSI_THRESH_MIN -65
#define PACKAGE_LOSS_THRESH 75
#define MIN_PACKAGE 8/*minimum package received from device*/ 

#define res(field) result->body.status[i].field


char clnt_ver[] = "1.2";
static rt_cmd_result_t final_res;
static rt_cmd_result_t *result = &final_res;

static char *log_file = "rssi_test.log";
static void control_dev(rtt_handle_t, char*, int);

int valid_mac(char * s)
{
    int len = strlen(s);
    if (len != 17) {
        printf("error : mac length %d out of range (%s)\n", len, s);
        return 0;
    }

    int i;
    for (i = 0; i < 17; i++) {
        s[i] = tolower(s[i]);
        if ((i+1) % 3) {
            if (s[i] > 'f' ||
                s[i] < '0' ||
                (s[i] > '9' && s[i] < 'a')) {
                return 0;
            }
        } else {
            if (s[i] != ':')
                return 0;
        }
    }

    return 1;
}

int valid_key(char * s)
{
    int i;
    if(strlen(s) != 24)
        return 0;

    for(i = 0; i < 24; i++)
    {
        if (s[i] < '0' ||
            s[i] > 'f' ||
            (s[i] > '9' && s[i] < 'A') ||
            (s[i] > 'F' && s[i] < 'a')
           ) {
            return 0;
        }
    }

    return 1;
}

static char beacon_mac[32];
static char beacon_key[32];
#define beacon_info_fn ".beacon_info"

int set_beacon()
{
    char cmd[128];
    char buf[32];

    printf("请输入遥控器MAC地址:");
    scanf("%s", buf);
    if (!valid_mac(buf)) {
        printf("MAC地址格式不正确\n");
        return -1;
    }
    strcpy(beacon_mac, buf);


    sprintf(buf,"202122232425262728292a2b");
    if (!valid_key(buf)) {
        printf("KEY格式不正确%s\n", buf);
        return -1;
    }
    strcpy(beacon_key, buf);

    sprintf(cmd, "echo %s > "beacon_info_fn, beacon_mac);
    system(cmd);
    sprintf(cmd, "echo %s >> "beacon_info_fn, beacon_key);
    system(cmd);

    return 0;
}

int load_beacon()
{
    FILE *fp;
    char buf[64];

    fp = fopen(beacon_info_fn, "r");
    if (!fp) {
        printf("未发现遥控器配置文件\n");
        return -1;
    }

    fscanf(fp, "%s", buf);
    if (!valid_mac(buf)) {
        printf("遥控器配置文件包含非法MAC地址 %s\n", buf);
        goto err;
    }
    strcpy(beacon_mac, buf);

    fscanf(fp, "%s", buf);
    if (!valid_key(buf)) {
        printf("遥控器配置文件包含非法key %s\n", buf);
        goto err;
    }
    strcpy(beacon_key, buf);

    printf("发现遥控器配置信息，MAC地址-%s, key-%s\n", beacon_mac, beacon_key);

    fclose(fp);
    return 0;

err:
    fclose(fp);
    return -1;
}

static void print_test_status(rtt_handle_t hdl, rt_cmd_result_t* result)
{
    int i;
    int l;
    char buf[4096];
    beacon_cmd_param_t beacon_cmd;
    int pass;
    int add_beacon = 0;

    int passed_dev[MAX_TEST_DEV];
    bzero(passed_dev, MAX_TEST_DEV * sizeof(int));

    memset(buf, 0, sizeof buf);

    l = sprintf(buf, "\n\n--------%s--------\n" , result->type == RT_CMD_STATUS ?
                "status" : "result");
    if(result->type != RT_CMD_STATUS){
        l += sprintf(buf + l, "共测试[%d]台设备,结果如下：\n",(int)result->dev_count );
    }
    //l += sprintf(buf + l, "-model-firmware_ver-brs-brf-avg_rssi-min_rssi-max_rssi-packet_send-packet_recv\n");
    for (i = 0; i < (int)result->dev_count; i++) {
        //l += sprintf(buf + l, "+Device: Status[%s] MAC[%s] IP[%s]\n", res(state), res(mac), res(ip));
        if (result->type == RT_CMD_RESULT) {
            pass = 1;

            if (res(avg_rssi) < RSSI_THRESH_AVG) {
                printf("error : %s 平均信号强度(=%d)过低\n", res(mac), res(avg_rssi));
                pass = 0;
            }

            //if (res(min_rssi) < RSSI_THRESH_MIN) {
            //    printf("error : %s 最低信号强度(=%d)过低\n", res(mac), res(min_rssi));
            //    pass = 0;
            //}

            if (res(packet_recv) < MIN_PACKAGE) {
                printf("error : %s 收到的数据包数量(=%d)太低\n", res(mac), res(packet_recv));
                pass = 0;
            }

            else if ((100 * res(packet_recv)) / res(packet_send) < (100 - PACKAGE_LOSS_THRESH)) {
                printf("error : %s 丢包率(%d/%d)太高\n", res(mac), res(packet_send)-res(packet_recv), res(packet_send));
                pass = 0;
            }

            if (pass) {
                printf("测试通过\n");
                control_dev(hdl, result->body.status[i].mac, 0);
                l += sprintf(buf + l, "测试结果[%s],设备状态[%s] MAC[%s] IP[%s]\n", "成功", res(state), res(mac), res(ip));
                passed_dev[i] = 1;
                add_beacon = 1;
            }
            else {
                printf("测试失败\n");
                control_dev(hdl, result->body.status[i].mac, 1);
                l += sprintf(buf + l, "测试结果[%s],设备状态[%s] MAC[%s] IP[%s]\n", "失败", res(state), res(mac), res(ip));
                passed_dev[i] = 0;
            }
        }


        if (strcmp(res(state), STATE_DEV_CONN) == 0) {
            l += sprintf(buf + l, "\t%s", res(model));
            l += sprintf(buf + l, "\t%s", res(fw_ver));
            l += sprintf(buf + l, "\t%d", res(brs));
            l += sprintf(buf + l, "\t%d", res(brf));
            l += sprintf(buf + l, "\t%d", res(avg_rssi));
            l += sprintf(buf + l, "\t%d", res(min_rssi));
            l += sprintf(buf + l, "\t%d", res(max_rssi));
            l += sprintf(buf + l, "\t%d", res(packet_send));
            l += sprintf(buf + l, "\t%d", res(packet_recv));
        }
        l += sprintf(buf + l, "\n");
    }

    printf("%s", buf);


#ifdef LOG_RESULT
    if (result->type == RT_CMD_RESULT) {
        FILE *fp;
        fp = fopen(log_file, "a");
        if (!fp)
            printf("Error: log file open failure, will lose some test results\n");

        fwrite(buf, strlen(buf), 1, fp);
        fclose(fp);
    }
#endif

   if (add_beacon) {
       for (i = 0; i < (int)result->dev_count; i++) {
           if (passed_dev[i]) {
               strcpy(beacon_cmd.mac_dev, result->body.status[i].mac);
               strcpy(beacon_cmd.mac_beacon, beacon_mac);
               strcpy(beacon_cmd.key, beacon_key);
               control_dev(hdl, &beacon_cmd, 3);
               control_dev(hdl, &beacon_cmd, 3);
           }
       }
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
        print_test_status(hdl, &res);
    }

}

static void run_test(rtt_handle_t hdl)
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
            print_test_status(hdl, &res);
        }
        if (res.type == RT_CMD_RESULT) {
            printf("Test Result.\n");
            print_test_status(hdl, &res);
            memcpy(&final_res, &res, sizeof(res));
            break;
        }
    }
}

static void control_dev(rtt_handle_t hdl, char* body, int result)
{
    rt_cmd_result_t res;
    rt_cmd_t cmd;
    int rc;

    // for command types 0, 1, 2
    char * mac = body;

    if (result == 0) {
        cmd.type = RT_CMD_CTRL_DEV;
        strcpy(cmd.body.mac, mac);
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_CTRL_DEV);
    } else if (result == 1) {
        strcpy(cmd.body.mac, mac);
        cmd.type = RT_CMD_FAIL_DEV;
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_FAIL_DEV);
    } else if (result == 2){
        strcpy(cmd.body.mac, mac);
        cmd.type = RT_CMD_OTA_DEV;
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_OTA_DEV);
    } else if (result == 4) {
        strcpy(cmd.body.mac, mac);
        cmd.type = RT_CMD_START_AUDIO_TEST;
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_START_AUDIO_TEST);
    } else if (result == 5) {
        strcpy(cmd.body.mac, mac);
        cmd.type = RT_CMD_START_BLE_TEST;
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_START_BLE_TEST);
    } else if (result == 6) {   /* special handling for get audio result */
        strcpy(cmd.body.mac, mac);
        cmd.type = RT_CMD_GET_AUDIO_TEST;
        rtt_send(hdl, &cmd);
        rc = rtt_recv(hdl, &res, RT_CMD_GET_AUDIO_TEST);

        if (rc < 0 || (rc > 0 && res.ret_code != RT_ERR_OK)) {
            printf("Failed to exe command %d.\n", res.ret_code);
            return;
        } else {
            printf("command execute successfully.\n");
        }

        rc = rtt_recv(hdl, &res, RT_CMD_GET_AUDIO_TEST);

        if (rc < 0) {
            printf("Failed to get result\n");
            return;
        } else {
            printf("audio test result for [%s] is %s\n",
                   res.body.audio_res.mac,
                   res.body.audio_res.result);
        }
        return;
    } else {
        cmd.type = RT_CMD_ADD_BEACON;
        memcpy(&cmd.body, body, sizeof(beacon_cmd_param_t));
        printf("%d: add beacon %s-%s for %s\r\n", cmd.type, cmd.body.beacon.mac_beacon, cmd.body.beacon.key, cmd.body.beacon.mac_dev);
        printf("send add beacon command\r\n");
        rtt_send(hdl, &cmd);
        printf("recv response...\r\n");
        rc = rtt_recv(hdl, &res, RT_CMD_ADD_BEACON);
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
    printf("\n--------Tool client ver :v%s-----------\n\n\n", clnt_ver);
    rtt_handle_t hdl;
    int err;
    int c;

    if (load_beacon()) {
        printf("未发现遥控器信息，请手动输入\n");
        while (set_beacon()) {
        }
    }

    setbuf(stdout, NULL);

    hdl = rtt_connect(5, &err, RTT_PROTO_UDP, argv[1]);
    if (!hdl) {
        printf("连不上测试服务器 %d.\n", err);
        return -1;
    }

    do {
        printf("支持的操作：\n");
        printf("  1： 执行测试\n");
        printf("  2： 设置遥控器\n");
        printf("  q： 退出程序\n");
        printf("请输入选项（1/2/q）: ");
        c = getchar();

        switch(c) {
        case '1':
            run_test(hdl);
            break;

        case '3':
            //run_ota(hdl);
            break;

        case '2':
            set_beacon();
            break;

        default:
            break;
        }

    } while (c != 'q');
    return 0;
}
