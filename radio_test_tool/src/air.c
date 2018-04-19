#include "air.h"
#include "string.h"
#include "iomux.h"
#include "test.h"

#define MAX_MON_IFACE 8

static io_inst_t  wif_io[MAX_MON_IFACE];
static int nr_mon_iface;
static struct wif *rtt_wi[MAX_MON_IFACE] = {NULL};
static unsigned char iface_mac[MAX_MON_IFACE][6];

const unsigned char llcnull[4] = {0, 0, 0, 0};
static int wif_recv(io_inst_t* inst);
static int setup_card(char*, int);

int air_start()
{
    int rc;
    int i;
    char ifnam[16];
    char *p = NULL;
    
    for (i = 0; i < nr_mon_iface; i++) {
        if (rtt_wi[i]) {
            strncpy(ifnam, wi_get_ifname(rtt_wi[i]), sizeof(ifnam) -1);
            wi_close(rtt_wi[i]);
            rtt_wi[i] = NULL;
            rc = setup_card(ifnam, i);
            if (rc < 0) { 
                RT_ERR("failed to init_cards.\n");
                return rc;
            }
            air_init(1, &p);
        }
        rc = iomux_add(&(wif_io[i]));
        if (rc < 0) {
            RT_ERR("Failed to monitor RAW socket.\n");
            return -1;
        }
    }
    return 0;
}

int air_stop()
{
    int rc;
    int i;
    char buf[4096];

    for (i = 0; i < nr_mon_iface; i++) {
        rc = iomux_del(&(wif_io[i]));
        if (rc < 0) {
            RT_ERR("Failed to stop monitoring RAW socket.\n");
            return -1;
        }
        /* drain the buffer */
        while ((rc = read(wif_io[i].fd, buf, sizeof(buf))) > 0) ;
    }


    return 0;
}

int air_init(int nr_mac, char** mac)
{
    int rc;
    long i;

    for (i = 0; i < nr_mac; i++) {
        if (mac[i]) {
             rc = sscanf(mac[i], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                         &iface_mac[i][0], &iface_mac[i][1], &iface_mac[i][2], 
                         &iface_mac[i][3], &iface_mac[i][4], &iface_mac[i][5]);
            if (rc != 6) {
                RT_ERR("Invalid mac address %s.\n", mac[i]);
                return -1;
            }
        }

        IO_INST_INIT(&(wif_io[i]), 
                     wi_fd(rtt_wi[i]), 
                     IO_EVENT_IN,
                     1,         /* nonblocking */
                     0,         /* oneshot */
                     wif_recv,
                     NULL, 
                     NULL,
                     (void*)i);

    }
    return 0;
}

struct wif *wi_alloc(int sz)
{
    struct wif *wi;
	void *priv;

    /* Allocate wif & private state */
    wi = xalloc(sizeof(*wi));
    priv = xalloc(sz);
    memset(priv, 0, sz);
    wi->wi_priv = priv;

	return wi;
}

struct wif *wi_open(char *iface)
{
	struct wif *wi;

	wi = wi_open_osdep(iface);
	if (!wi)
		return NULL;

	strncpy(wi->wi_interface, iface, sizeof(wi->wi_interface)-1);
	wi->wi_interface[sizeof(wi->wi_interface)-1] = 0;

	return wi;
}

static int setup_card(char *iface, int idx)
{
    struct wif *wi;

    wi = wi_open(iface);
        if (!wi)
            return -1;
    if (idx >= MAX_MON_IFACE)
        return -1;

    wi_set_channel(wi, 6);  /* only monitor on channel 6 */    

    rtt_wi[idx] = wi;

    return 0;
}

static int handle_packet(unsigned char *h80211, int caplen, struct rx_info *ri, int idx)
{
    unsigned char bssid[6];
    unsigned char stmac[6];

    /* skip packets smaller than a 802.11 header */
    if( caplen < 24 )
        goto out;

    /* skip (uninteresting) control frames */
    if( ( h80211[0] & 0x0C ) == 0x04 )
        goto out;

    /* if it's a LLC null packet, just forget it (may change in the future) */
    if ( caplen > 28)
        if ( memcmp(h80211 + 24, llcnull, 4) == 0)
            goto out;

    /* locate the access point's MAC address */
    switch( h80211[1] & 3 )
    {
    case  0: memcpy( bssid, h80211 + 16, 6 ); break;  //Adhoc
    case  1: memcpy( bssid, h80211 +  4, 6 ); break;  //ToDS
    case  2: memcpy( bssid, h80211 + 10, 6 ); break;  //FromDS
    case  3: memcpy( bssid, h80211 + 10, 6 ); break;  //WDS -> Transmitter taken as BSSID
    }

    /* check if packets are targetting to us */
    if (memcmp(bssid, iface_mac[idx], 6) != 0)
        goto out;

    switch( h80211[1] & 3 )
    {
    case  0:
        /* if management, check that SA != BSSID */
        if( memcmp( h80211 + 10, bssid, 6 ) == 0 ) {
            goto out;
        }
        memcpy( stmac, h80211 + 10, 6 );
        break;
    case  1:
        /* ToDS packet, must come from a client */
        memcpy( stmac, h80211 + 10, 6 );
        break;
    case  2:
        /* FromDS packet, reject broadcast MACs */
        if( (h80211[4]%2) != 0 ) {
            goto out;
        }
        memcpy( stmac, h80211 +  4, 6 ); 
        break;
    default: 
        goto out;
    }
    rt_update_stats(stmac, ri, h80211, bssid);

 out:
    return 0;
}

static int wif_recv(io_inst_t* inst)
{
    unsigned char      buffer[4096];
    unsigned char      *h80211;
    int caplen=0;
    struct rx_info     ri;
    char ifnam[64];
    int idx = (long)inst->priv;

    memset(buffer, 0, sizeof(buffer));
    h80211 = buffer;

    if ((caplen = wi_read(rtt_wi[idx], h80211, sizeof(buffer), &ri)) == -1) {
        RT_ERR("Read failed, fallback to monitor mode.\n");
        strncpy(ifnam, wi_get_ifname(rtt_wi[idx]), sizeof(ifnam)-1);
        ifnam[sizeof(ifnam)-1] = 0;
        wi_close(rtt_wi[idx]);
        rtt_wi[idx] = wi_open(ifnam);
        if (!rtt_wi[idx]) {
            RT_ERR("Fallback to monitor mode failed.\n");
            PANIC_IF(1);
        }
        iomux_del(&(wif_io[idx]));
        close(wif_io[idx].fd);
        wif_io[idx].fd = wi_fd(rtt_wi[idx]);
        iomux_add(&wif_io[idx]);
    } else 
        handle_packet(h80211, caplen, &ri, idx);

    return 0;
}

int init_cards(char *iface)
{
    if(setup_card(iface, nr_mon_iface++) != 0)
        return -1;
    return 0;
}
