/************************************
 * Radio Test Tool (rt) main file **
 ************************************/

#include "util.h"
#include "iomux.h"
#include "timer.h"
#include "cmd.h"
#include "air.h"
#include "rtt_interface.h"
#include "sbox.h"
#include <limits.h>
#include <stdlib.h>

char ver[] = "1.9";
static int rt_prepare(int);

static int rt_prepare(int nr_itfc)
{
    int rc, i, cnt, item;
    char itfc[8];
    char sbox[16];
    char cmd[512];
    FILE* fp;

    rc = rtt_exec("./setup.sh", 1);
    if (rc) {
        RT_ERR("Failed to run setup.sh.\n");
        return rc;
    }

    /* setup cards */
    for (i = 0; i < nr_itfc; i++) {
        sprintf(itfc, "mon%d", i);
        printf("setup wireless cards %s\n", itfc);
        rc = init_cards(itfc);
        if (rc < 0) {
            RT_ERR("Failed to setup cards.\n");
            return rc;
        }
    }

    /* setup sheild box communication channel */
    snprintf(cmd, sizeof(cmd), "ls /dev/ttyUSB* 2>&- | wc -w");
    fp = popen(cmd, "r");
    if (fp == NULL)
        return 0;

    item = fscanf(fp, "%d", &cnt);
    if (item != 1) {
        fclose(fp);
        return 0;
    }
    fclose(fp);

    for (i = 0; i < cnt; i++) {
        sprintf(sbox, "/dev/ttyUSB%d", i);
        printf("setup shield box control channel %s\n", sbox);
        rc = sbox_init(sbox);
        if (rc < 0) {
            RT_ERR("WARNING: Failed to setup shield box %s\n", sbox);
        }
    }

    return 0;
}

char bin_name[128];
char bin_dir[128];

int main(int argc, char** argv)
{
    int rc, dummy;
    int proto = RT_CMD_PROTO_UNIX;
    char *ip;

    printf("\n--------Tool server ver :v%s-----------\n\n\n", ver);
    if (argc < 2) {
        printf("usage: %s <mac addresses of wlan> [ip address of eth0]\n", argv[0]);
        return -1;
    }

    /* findout whereabouts */
    realpath(argv[0], bin_name);
    strcpy(bin_dir, bin_name);
    dirname(bin_dir);

    if (argc > 2 && sscanf(argv[argc-1], "%d.%d.%d.%d", &dummy, &dummy, &dummy, &dummy) == 4) {
        printf("rtt daemon listen on ip [%s]\n", argv[argc-1]);
        proto = RT_CMD_PROTO_UDP;
        ip = argv[argc-1];
        argc--;
    }
    /* calculate how many mac addresses are given */
    argc--;

    rc = rt_prepare(argc);
    if (rc) {
        RT_ERR("Failed to setup radio test evironment, errcode %d\n", WEXITSTATUS(rc));
        return -1;
    }

    rc = timer_init();
    if (rc < 0) {
        RT_ERR("Failed to init timer.\n");
        return -1;
    }

    rc = iomux_init();
    if (rc < 0) {
        RT_ERR("Failed to init io-multiplexing.\n");
        return -1;
    }

    if (proto == RT_CMD_PROTO_UDP)
        rc = cmd_init(ip);
    else
        rc = cmd_init(NULL);
    if (rc < 0) {
        RT_ERR("Failed to init command interface.\n");
        return -1;
    }

    rc = air_init(argc, argv+1);
    if (rc < 0) {
        RT_ERR("Failed to init air monitor.\n");
        return -1;
    }

    rc = sbox_connect();
    if (rc < 0) {
        RT_ERR("Failed to connect shield box.\n");
        return -1;
    }
    printf("Server started, ready to take commands\n");

    /* main loop */
    while (alive) {
        iomux_loop();
    }
    return 0;
}
