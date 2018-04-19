package com.yeelight.rtt;
import java.util.Scanner;

public class main {
   public static void print_test_status(rt_cmd_result_t result) {
       int i;
       rt_status_t status = null;

       System.out.printf("--------%s--------\n" , result.getType() == rt_cmd_type_t.RT_CMD_STATUS ?
           "status" : "result");
      
        
       for (i = 0; i < result.getDev_count(); i++) {
            status = rtt_interface.rtt_get_status(result, i);

            System.out.printf("\t+Device: Status[%s] MAC[%s] IP[%s]\n", 
                               status.getState(), status.getMac(), status.getIp());
            if (status.getState().equals(rtt_interfaceConstants.STATE_DEV_CONN)) {
                System.out.printf("\t\t-firmware_ver: %s\n", status.getFw_ver());
                System.out.printf("\t\t-brs: %d\n", status.getBrs());
                System.out.printf("\t\t-brf: %d\n", status.getBrf());
                System.out.printf("\t\t-avg_rssi: %d\n", status.getAvg_rssi());
                System.out.printf("\t\t-min_rssi: %d\n", status.getMin_rssi());
                System.out.printf("\t\t-max_rssi: %d\n", status.getMax_rssi());
                System.out.printf("\t\t-packet_send: %d\n", status.getPacket_send());
                System.out.printf("\t\t-packet_recv: %d\n", status.getPacket_recv());
            }
            System.out.println("");
       }       
   }

   public static void ctrl_dev(SWIGTYPE_p_void rtt_hdl, String mac) {
       rt_cmd_result_t res = new rt_cmd_result_t();
       rt_cmd_t  cmd = new rt_cmd_t();
       int rc;

       cmd.getBody().setMac(mac);
       cmd.setType(rt_cmd_type_t.RT_CMD_CTRL_DEV);
       rtt_interface.rtt_send(rtt_hdl, cmd);
       rc = rtt_interface.rtt_recv(rtt_hdl, res, rt_cmd_type_t.RT_CMD_CTRL_DEV);

       if (rc < 0 || (rc > 0 && res.getRet_code() != rt_error_t.RT_ERR_OK)) {
           System.out.println("Failed to exe start command");
           return;
       }
   }

   public static void add_controller(SWIGTYPE_p_void rtt_hdl, String dev_mac, String beacon_mac, String beacon_key) {
       rt_cmd_result_t res = new rt_cmd_result_t();
       beacon_cmd_param_t beacon_cmd = new beacon_cmd_param_t();
       rt_cmd_t  cmd = new rt_cmd_t();
       int rc;

       beacon_cmd.setMac_dev(dev_mac);
       beacon_cmd.setMac_beacon(beacon_mac);
       beacon_cmd.setKey(beacon_key);

       cmd.getBody().setBeacon(beacon_cmd);
       cmd.setType(rt_cmd_type_t.RT_CMD_ADD_BEACON);
       rtt_interface.rtt_send(rtt_hdl, cmd);
       rc = rtt_interface.rtt_recv(rtt_hdl, res, rt_cmd_type_t.RT_CMD_CTRL_DEV);

       if (rc < 0 || (rc > 0 && res.getRet_code() != rt_error_t.RT_ERR_OK)) {
           System.out.println("Failed to exe start command");
           return;
       }
   }


   public static void test_start(SWIGTYPE_p_void rtt_hdl) {
       rt_cmd_result_t res = new rt_cmd_result_t();
       rt_cmd_t  cmd = new rt_cmd_t();
       int rc;

       cmd.setType(rt_cmd_type_t.RT_CMD_START);
       rtt_interface.rtt_send(rtt_hdl, cmd);
      
       rc = rtt_interface.rtt_recv(rtt_hdl, res, rt_cmd_type_t.RT_CMD_START);
       if (rc < 0 || (rc > 0 && res.getRet_code() != rt_error_t.RT_ERR_OK)) {
           System.out.println("Failed to exe start command");
           return;
       }
       System.out.println("Command Begin");
       while (true) {
           rc = rtt_interface.rtt_recv(rtt_hdl, res, rt_cmd_type_t.RT_CMD_MAX);
           if (rc <= 0) break;
           if (res.getType() == rt_cmd_type_t.RT_CMD_STATUS) {
               System.out.println("Test Status Update");
               print_test_status(res);
           } else if (res.getType() == rt_cmd_type_t.RT_CMD_RESULT) {
               System.out.println("Test Result");
               print_test_status(res);
               break;
           }
       } 
   }
   
   public static void main(String argv[]) throws Exception {
     char c;

     Scanner scanIn = new Scanner(System.in);
     System.loadLibrary("rtt_interface");

     SWIGTYPE_p_int err = new SWIGTYPE_p_int() ;
     SWIGTYPE_p_void rtt_hdl = rtt_interface.rtt_connect(5, err, rtt_interfaceConstants.RTT_PROTO_UDP, "127.0.0.1");
     if (rtt_hdl == null) {
         System.out.println("failed to connect to server.");
         System.exit(-1);
     }

     while ((c = (char) System.in.read()) != 'q') {
        switch (c) {
        case '1':
            test_start(rtt_hdl);
            break;
        case '2':
            System.out.println("input mac:");
            String mac;
            mac = scanIn.nextLine();
            ctrl_dev(rtt_hdl, mac);
            break;
        case '3':
            String dev_mac, beacon_mac, beacon_key;
            System.out.println("input dev mac:");
            dev_mac = scanIn.nextLine();
            System.out.println("input beacon mac:");
            beacon_mac = scanIn.nextLine();
            System.out.println("input beacon key:");
            beacon_key = scanIn.nextLine();
            System.out.println(String.format("Add beacon: %s key: %s to Dev: %s", beacon_mac, beacon_key, dev_mac));
            add_controller(rtt_hdl, dev_mac, beacon_mac, beacon_key);
            break;
        default:
            System.out.println("other input");
            break;
        } 
     } 
   }
}
