/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.8
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.yeelight.rtt;

public final class rt_cmd_type_t {
  public final static rt_cmd_type_t RT_CMD_START = new rt_cmd_type_t("RT_CMD_START");
  public final static rt_cmd_type_t RT_CMD_STOP = new rt_cmd_type_t("RT_CMD_STOP");
  public final static rt_cmd_type_t RT_CMD_STATUS = new rt_cmd_type_t("RT_CMD_STATUS");
  public final static rt_cmd_type_t RT_CMD_RESULT = new rt_cmd_type_t("RT_CMD_RESULT");
  public final static rt_cmd_type_t RT_CMD_SET_PARAM = new rt_cmd_type_t("RT_CMD_SET_PARAM");
  public final static rt_cmd_type_t RT_CMD_GET_PARAM = new rt_cmd_type_t("RT_CMD_GET_PARAM");
  public final static rt_cmd_type_t RT_CMD_CTRL_DEV = new rt_cmd_type_t("RT_CMD_CTRL_DEV");
  public final static rt_cmd_type_t RT_CMD_FAIL_DEV = new rt_cmd_type_t("RT_CMD_FAIL_DEV");
  public final static rt_cmd_type_t RT_CMD_OTA_DEV = new rt_cmd_type_t("RT_CMD_OTA_DEV");
  public final static rt_cmd_type_t RT_CMD_QUIT = new rt_cmd_type_t("RT_CMD_QUIT");
  public final static rt_cmd_type_t RT_CMD_OPEN_SBOX = new rt_cmd_type_t("RT_CMD_OPEN_SBOX");
  public final static rt_cmd_type_t RT_CMD_CLOSE_SBOX = new rt_cmd_type_t("RT_CMD_CLOSE_SBOX");
  public final static rt_cmd_type_t RT_CMD_ADD_BEACON = new rt_cmd_type_t("RT_CMD_ADD_BEACON");
  public final static rt_cmd_type_t RT_CMD_START_AUDIO_TEST = new rt_cmd_type_t("RT_CMD_START_AUDIO_TEST");
  public final static rt_cmd_type_t RT_CMD_START_BLE_TEST = new rt_cmd_type_t("RT_CMD_START_BLE_TEST");
  public final static rt_cmd_type_t RT_CMD_GET_AUDIO_TEST = new rt_cmd_type_t("RT_CMD_GET_AUDIO_TEST");
  public final static rt_cmd_type_t RT_CMD_PRINT_LABEL = new rt_cmd_type_t("RT_CMD_PRINT_LABEL");
  public final static rt_cmd_type_t RT_CMD_MAX = new rt_cmd_type_t("RT_CMD_MAX");
  public final static rt_cmd_type_t RT_CMD_INF = new rt_cmd_type_t("RT_CMD_INF", rtt_interfaceJNI.RT_CMD_INF_get());

  public final int swigValue() {
    return swigValue;
  }

  public String toString() {
    return swigName;
  }

  public static rt_cmd_type_t swigToEnum(int swigValue) {
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (int i = 0; i < swigValues.length; i++)
      if (swigValues[i].swigValue == swigValue)
        return swigValues[i];
    throw new IllegalArgumentException("No enum " + rt_cmd_type_t.class + " with value " + swigValue);
  }

  private rt_cmd_type_t(String swigName) {
    this.swigName = swigName;
    this.swigValue = swigNext++;
  }

  private rt_cmd_type_t(String swigName, int swigValue) {
    this.swigName = swigName;
    this.swigValue = swigValue;
    swigNext = swigValue+1;
  }

  private rt_cmd_type_t(String swigName, rt_cmd_type_t swigEnum) {
    this.swigName = swigName;
    this.swigValue = swigEnum.swigValue;
    swigNext = this.swigValue+1;
  }

  private static rt_cmd_type_t[] swigValues = { RT_CMD_START, RT_CMD_STOP, RT_CMD_STATUS, RT_CMD_RESULT, RT_CMD_SET_PARAM, RT_CMD_GET_PARAM, RT_CMD_CTRL_DEV, RT_CMD_FAIL_DEV, RT_CMD_OTA_DEV, RT_CMD_QUIT, RT_CMD_OPEN_SBOX, RT_CMD_CLOSE_SBOX, RT_CMD_ADD_BEACON, RT_CMD_START_AUDIO_TEST, RT_CMD_START_BLE_TEST, RT_CMD_GET_AUDIO_TEST, RT_CMD_PRINT_LABEL, RT_CMD_MAX, RT_CMD_INF };
  private static int swigNext = 0;
  private final int swigValue;
  private final String swigName;
}

