/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.yeelight.rtt;

public class rt_cmd_result_t {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected rt_cmd_result_t(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(rt_cmd_result_t obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        rtt_interfaceJNI.delete_rt_cmd_result_t(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setType(rt_cmd_type_t value) {
    rtt_interfaceJNI.rt_cmd_result_t_type_set(swigCPtr, this, value.swigValue());
  }

  public rt_cmd_type_t getType() {
    return rt_cmd_type_t.swigToEnum(rtt_interfaceJNI.rt_cmd_result_t_type_get(swigCPtr, this));
  }

  public void setRet_code(rt_error_t value) {
    rtt_interfaceJNI.rt_cmd_result_t_ret_code_set(swigCPtr, this, value.swigValue());
  }

  public rt_error_t getRet_code() {
    return rt_error_t.swigToEnum(rtt_interfaceJNI.rt_cmd_result_t_ret_code_get(swigCPtr, this));
  }

  public void setDev_count(int value) {
    rtt_interfaceJNI.rt_cmd_result_t_dev_count_set(swigCPtr, this, value);
  }

  public int getDev_count() {
    return rtt_interfaceJNI.rt_cmd_result_t_dev_count_get(swigCPtr, this);
  }

  public rt_cmd_result_t_body getBody() {
    long cPtr = rtt_interfaceJNI.rt_cmd_result_t_body_get(swigCPtr, this);
    return (cPtr == 0) ? null : new rt_cmd_result_t_body(cPtr, false);
  }

  public rt_cmd_result_t() {
    this(rtt_interfaceJNI.new_rt_cmd_result_t(), true);
  }

}
