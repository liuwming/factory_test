/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.8
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.yeelight.rtt;

public class rt_status_t {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected rt_status_t(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(rt_status_t obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        rtt_interfaceJNI.delete_rt_status_t(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setRssi_calc_cnt(int value) {
    rtt_interfaceJNI.rt_status_t_rssi_calc_cnt_set(swigCPtr, this, value);
  }

  public int getRssi_calc_cnt() {
    return rtt_interfaceJNI.rt_status_t_rssi_calc_cnt_get(swigCPtr, this);
  }

  public void setAvg_rssi(int value) {
    rtt_interfaceJNI.rt_status_t_avg_rssi_set(swigCPtr, this, value);
  }

  public int getAvg_rssi() {
    return rtt_interfaceJNI.rt_status_t_avg_rssi_get(swigCPtr, this);
  }

  public void setMin_rssi(int value) {
    rtt_interfaceJNI.rt_status_t_min_rssi_set(swigCPtr, this, value);
  }

  public int getMin_rssi() {
    return rtt_interfaceJNI.rt_status_t_min_rssi_get(swigCPtr, this);
  }

  public void setMax_rssi(int value) {
    rtt_interfaceJNI.rt_status_t_max_rssi_set(swigCPtr, this, value);
  }

  public int getMax_rssi() {
    return rtt_interfaceJNI.rt_status_t_max_rssi_get(swigCPtr, this);
  }

  public void setDid(SWIGTYPE_p_uint64_t value) {
    rtt_interfaceJNI.rt_status_t_did_set(swigCPtr, this, SWIGTYPE_p_uint64_t.getCPtr(value));
  }

  public SWIGTYPE_p_uint64_t getDid() {
    return new SWIGTYPE_p_uint64_t(rtt_interfaceJNI.rt_status_t_did_get(swigCPtr, this), true);
  }

  public void setPacket_send(long value) {
    rtt_interfaceJNI.rt_status_t_packet_send_set(swigCPtr, this, value);
  }

  public long getPacket_send() {
    return rtt_interfaceJNI.rt_status_t_packet_send_get(swigCPtr, this);
  }

  public void setPacket_recv(long value) {
    rtt_interfaceJNI.rt_status_t_packet_recv_set(swigCPtr, this, value);
  }

  public long getPacket_recv() {
    return rtt_interfaceJNI.rt_status_t_packet_recv_get(swigCPtr, this);
  }

  public void setFw_ver(String value) {
    rtt_interfaceJNI.rt_status_t_fw_ver_set(swigCPtr, this, value);
  }

  public String getFw_ver() {
    return rtt_interfaceJNI.rt_status_t_fw_ver_get(swigCPtr, this);
  }

  public void setMac(String value) {
    rtt_interfaceJNI.rt_status_t_mac_set(swigCPtr, this, value);
  }

  public String getMac() {
    return rtt_interfaceJNI.rt_status_t_mac_get(swigCPtr, this);
  }

  public void setIp(String value) {
    rtt_interfaceJNI.rt_status_t_ip_set(swigCPtr, this, value);
  }

  public String getIp() {
    return rtt_interfaceJNI.rt_status_t_ip_get(swigCPtr, this);
  }

  public void setState(String value) {
    rtt_interfaceJNI.rt_status_t_state_set(swigCPtr, this, value);
  }

  public String getState() {
    return rtt_interfaceJNI.rt_status_t_state_get(swigCPtr, this);
  }

  public void setModel(String value) {
    rtt_interfaceJNI.rt_status_t_model_set(swigCPtr, this, value);
  }

  public String getModel() {
    return rtt_interfaceJNI.rt_status_t_model_get(swigCPtr, this);
  }

  public void setBrs(int value) {
    rtt_interfaceJNI.rt_status_t_brs_set(swigCPtr, this, value);
  }

  public int getBrs() {
    return rtt_interfaceJNI.rt_status_t_brs_get(swigCPtr, this);
  }

  public void setBrf(int value) {
    rtt_interfaceJNI.rt_status_t_brf_set(swigCPtr, this, value);
  }

  public int getBrf() {
    return rtt_interfaceJNI.rt_status_t_brf_get(swigCPtr, this);
  }

  public void setAudio_test_result(int value) {
    rtt_interfaceJNI.rt_status_t_audio_test_result_set(swigCPtr, this, value);
  }

  public int getAudio_test_result() {
    return rtt_interfaceJNI.rt_status_t_audio_test_result_get(swigCPtr, this);
  }

  public void setHealth_status(int value) {
    rtt_interfaceJNI.rt_status_t_health_status_set(swigCPtr, this, value);
  }

  public int getHealth_status() {
    return rtt_interfaceJNI.rt_status_t_health_status_get(swigCPtr, this);
  }

  public void setFcc(int value) {
    rtt_interfaceJNI.rt_status_t_fcc_set(swigCPtr, this, value);
  }

  public int getFcc() {
    return rtt_interfaceJNI.rt_status_t_fcc_get(swigCPtr, this);
  }

  public rt_status_t() {
    this(rtt_interfaceJNI.new_rt_status_t(), true);
  }

}
