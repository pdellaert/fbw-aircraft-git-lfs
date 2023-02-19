#include "A380SecComputer.h"
#include "rtwtypes.h"
#include "A380SecComputer_types.h"
#include <cmath>
#include "A380LateralDirectLaw.h"
#include "A380PitchDirectLaw.h"

void A380SecComputer::A380SecComputer_RateLimiter_Reset(rtDW_RateLimiter_A380SecComputer_T *localDW)
{
  localDW->pY_not_empty = false;
}

void A380SecComputer::A380SecComputer_RateLimiter(real_T rtu_u, real_T rtu_up, real_T rtu_lo, real_T rtu_Ts, real_T
  rtu_init, real_T *rty_Y, rtDW_RateLimiter_A380SecComputer_T *localDW)
{
  if (!localDW->pY_not_empty) {
    localDW->pY = rtu_init;
    localDW->pY_not_empty = true;
  }

  localDW->pY += std::fmax(std::fmin(rtu_u - localDW->pY, std::abs(rtu_up) * rtu_Ts), -std::abs(rtu_lo) * rtu_Ts);
  *rty_Y = localDW->pY;
}

void A380SecComputer::A380SecComputer_RateLimiter_j_Reset(rtDW_RateLimiter_A380SecComputer_o_T *localDW)
{
  localDW->pY_not_empty = false;
}

void A380SecComputer::A380SecComputer_RateLimiter_e(real_T rtu_u, real_T rtu_up, real_T rtu_lo, real_T rtu_Ts, real_T
  rtu_init, boolean_T rtu_reset, real_T *rty_Y, rtDW_RateLimiter_A380SecComputer_o_T *localDW)
{
  if ((!localDW->pY_not_empty) || rtu_reset) {
    localDW->pY = rtu_init;
    localDW->pY_not_empty = true;
  }

  if (rtu_reset) {
    *rty_Y = rtu_init;
  } else {
    *rty_Y = std::fmax(std::fmin(rtu_u - localDW->pY, std::abs(rtu_up) * rtu_Ts), -std::abs(rtu_lo) * rtu_Ts) +
      localDW->pY;
  }

  localDW->pY = *rty_Y;
}

void A380SecComputer::A380SecComputer_MATLABFunction(const base_arinc_429 *rtu_u, boolean_T *rty_y)
{
  *rty_y = (rtu_u->SSM != static_cast<uint32_T>(SignStatusMatrix::FailureWarning));
}

void A380SecComputer::A380SecComputer_MATLABFunction_c(const base_arinc_429 *rtu_u, real_T rtu_bit, uint32_T *rty_y)
{
  real32_T tmp;
  uint32_T a;
  tmp = std::round(rtu_u->Data);
  if (tmp < 4.2949673E+9F) {
    if (tmp >= 0.0F) {
      a = static_cast<uint32_T>(tmp);
    } else {
      a = 0U;
    }
  } else {
    a = MAX_uint32_T;
  }

  if (-(rtu_bit - 1.0) >= 0.0) {
    if (-(rtu_bit - 1.0) <= 31.0) {
      a <<= static_cast<uint8_T>(-(rtu_bit - 1.0));
    } else {
      a = 0U;
    }
  } else if (-(rtu_bit - 1.0) >= -31.0) {
    a >>= static_cast<uint8_T>(rtu_bit - 1.0);
  } else {
    a = 0U;
  }

  *rty_y = a & 1U;
}

void A380SecComputer::A380SecComputer_MATLABFunction_js_Reset(rtDW_MATLABFunction_A380SecComputer_k_T *localDW)
{
  localDW->output = false;
  localDW->timeSinceCondition = 0.0;
}

void A380SecComputer::A380SecComputer_MATLABFunction_cj(boolean_T rtu_u, real_T rtu_Ts, boolean_T rtu_isRisingEdge,
  real_T rtu_timeDelay, boolean_T *rty_y, rtDW_MATLABFunction_A380SecComputer_k_T *localDW)
{
  if (rtu_u == rtu_isRisingEdge) {
    localDW->timeSinceCondition += rtu_Ts;
    if (localDW->timeSinceCondition >= rtu_timeDelay) {
      localDW->output = rtu_u;
    }
  } else {
    localDW->timeSinceCondition = 0.0;
    localDW->output = rtu_u;
  }

  *rty_y = localDW->output;
}

void A380SecComputer::A380SecComputer_MATLABFunction_j2_Reset(rtDW_MATLABFunction_A380SecComputer_km_T *localDW)
{
  localDW->output = false;
}

void A380SecComputer::A380SecComputer_MATLABFunction_j(real_T rtu_u, real_T rtu_highTrigger, real_T rtu_lowTrigger,
  boolean_T *rty_y, rtDW_MATLABFunction_A380SecComputer_km_T *localDW)
{
  boolean_T output_tmp;
  output_tmp = !localDW->output;
  localDW->output = ((output_tmp && (rtu_u >= rtu_highTrigger)) || ((output_tmp || (rtu_u > rtu_lowTrigger)) &&
    localDW->output));
  *rty_y = localDW->output;
}

void A380SecComputer::A380SecComputer_MATLABFunction_f_Reset(rtDW_MATLABFunction_A380SecComputer_b_T *localDW)
{
  localDW->previousInput_not_empty = false;
}

void A380SecComputer::A380SecComputer_MATLABFunction_g(boolean_T rtu_u, boolean_T rtu_isRisingEdge, boolean_T *rty_y,
  rtDW_MATLABFunction_A380SecComputer_b_T *localDW)
{
  if (!localDW->previousInput_not_empty) {
    localDW->previousInput = rtu_isRisingEdge;
    localDW->previousInput_not_empty = true;
  }

  if (rtu_isRisingEdge) {
    *rty_y = (rtu_u && (!localDW->previousInput));
  } else {
    *rty_y = ((!rtu_u) && localDW->previousInput);
  }

  localDW->previousInput = rtu_u;
}

void A380SecComputer::A380SecComputer_MATLABFunction_cw(const boolean_T rtu_u[19], real32_T *rty_y)
{
  uint32_T out;
  out = 0U;
  for (int32_T i{0}; i < 19; i++) {
    out |= static_cast<uint32_T>(rtu_u[i]) << (i + 10);
  }

  *rty_y = static_cast<real32_T>(out);
}

void A380SecComputer::step()
{
  real_T rtb_xi_inboard_deg;
  real_T rtb_xi_midboard_deg;
  real_T rtb_xi_outboard_deg;
  real_T rtb_xi_spoiler_deg;
  real_T rtb_zeta_upper_deg;
  real_T rtb_zeta_lower_deg;
  base_arinc_429 rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi;
  real_T ca;
  real_T rtb_Switch1_k;
  real_T rtb_Switch8_o;
  real_T rtb_eta_trim_limit_up;
  real_T rtb_xi_spoiler_deg_b;
  uint32_T rtb_SSM_ov;
  uint32_T rtb_y_c0;
  uint32_T rtb_y_cf;
  uint32_T rtb_y_cq;
  uint32_T rtb_y_oq;
  boolean_T rtb_VectorConcatenate[19];
  boolean_T rtb_VectorConcatenate_a[19];
  boolean_T rtb_VectorConcatenate_aw[19];
  boolean_T rtb_VectorConcatenate_bz[19];
  boolean_T rtb_VectorConcatenate_e[19];
  boolean_T rtb_VectorConcatenate_j[19];
  boolean_T rtb_Compare_l;
  boolean_T rtb_NOT_k;
  boolean_T rtb_OR1;
  boolean_T rtb_y_ck;
  if (A380SecComputer_U.in.sim_data.computer_running) {
    real32_T rtb_Data_awh;
    real32_T rtb_Data_ch;
    real32_T rtb_Data_nu;
    real32_T rtb_V_ias;
    real32_T rtb_V_tas;
    real32_T rtb_alpha;
    real32_T rtb_leftAileron1Command;
    real32_T rtb_mach_h;
    real32_T rtb_n_x;
    real32_T rtb_n_y;
    real32_T rtb_n_z;
    real32_T rtb_phi;
    real32_T rtb_phi_dot;
    real32_T rtb_q;
    real32_T rtb_r;
    real32_T rtb_rightAileron1Command;
    real32_T rtb_theta;
    real32_T rtb_theta_dot;
    int8_T rtb_DataTypeConversion_i;
    boolean_T elevator1Avail;
    boolean_T elevator2Avail;
    boolean_T elevator3Avail;
    boolean_T leftAileron1Avail;
    boolean_T leftAileron2Avail;
    boolean_T leftSpoilerHydraulicModeAvail;
    boolean_T rightAileron1Avail;
    boolean_T rightAileron2Avail;
    boolean_T rightSpoilerHydraulicModeAvail;
    boolean_T rtb_AND;
    boolean_T rtb_AND1;
    boolean_T rtb_AND1_jx;
    boolean_T rtb_AND1_p;
    boolean_T rtb_AND2_a;
    boolean_T rtb_AND2_c;
    boolean_T rtb_AND3_dt;
    boolean_T rtb_AND4_l;
    boolean_T rtb_AND5;
    boolean_T rtb_AND5_e;
    boolean_T rtb_AND6;
    boolean_T rtb_AND6_e;
    boolean_T rtb_AND7;
    boolean_T rtb_AND7_g;
    boolean_T rtb_AND7_j;
    boolean_T rtb_AND8;
    boolean_T rtb_AND9_e;
    boolean_T rtb_AND_cw;
    boolean_T rtb_AND_h;
    boolean_T rtb_OR;
    boolean_T rtb_OR3;
    boolean_T rtb_OR6;
    boolean_T rtb_logic_is_green_hydraulic_power_avail;
    boolean_T rudder1HydraulicModeHasPriority;
    boolean_T rudder2ElectricModeHasPriority;
    boolean_T rudderTrimAvail;
    boolean_T thsAvail;
    boolean_T thsAvail_tmp;
    if (!A380SecComputer_DWork.Runtime_MODE) {
      A380SecComputer_DWork.Delay_DSTATE_c = A380SecComputer_P.Delay_InitialCondition;
      A380SecComputer_DWork.Delay1_DSTATE = A380SecComputer_P.Delay1_InitialCondition;
      A380SecComputer_DWork.icLoad = true;
      A380SecComputer_DWork.pY_not_empty = false;
      A380SecComputer_DWork.pU_not_empty = false;
      A380SecComputer_MATLABFunction_js_Reset(&A380SecComputer_DWork.sf_MATLABFunction_cj);
      A380SecComputer_MATLABFunction_js_Reset(&A380SecComputer_DWork.sf_MATLABFunction_gf);
      A380SecComputer_MATLABFunction_f_Reset(&A380SecComputer_DWork.sf_MATLABFunction_g4);
      A380SecComputer_MATLABFunction_f_Reset(&A380SecComputer_DWork.sf_MATLABFunction_nu);
      A380SecComputer_DWork.pLeftStickDisabled = false;
      A380SecComputer_DWork.pRightStickDisabled = false;
      A380SecComputer_MATLABFunction_js_Reset(&A380SecComputer_DWork.sf_MATLABFunction_j2);
      A380SecComputer_MATLABFunction_js_Reset(&A380SecComputer_DWork.sf_MATLABFunction_g2);
      LawMDLOBJ1.reset();
      A380SecComputer_RateLimiter_Reset(&A380SecComputer_DWork.sf_RateLimiter);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_e);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_o);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_a);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_p);
      A380SecComputer_RateLimiter_Reset(&A380SecComputer_DWork.sf_RateLimiter_b);
      A380SecComputer_DWork.pY_not_empty_k = false;
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_os);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_d);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_bv);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_g);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_j);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_gz);
      LawMDLOBJ2.reset();
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_c);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_p0);
      A380SecComputer_RateLimiter_j_Reset(&A380SecComputer_DWork.sf_RateLimiter_cd);
      A380SecComputer_MATLABFunction_j2_Reset(&A380SecComputer_DWork.sf_MATLABFunction_jg);
      A380SecComputer_MATLABFunction_j2_Reset(&A380SecComputer_DWork.sf_MATLABFunction_br);
      A380SecComputer_DWork.Runtime_MODE = true;
    }

    A380SecComputer_B.dt = A380SecComputer_U.in.time.dt;
    A380SecComputer_B.is_unit_1 = A380SecComputer_U.in.discrete_inputs.is_unit_1;
    A380SecComputer_B.SSM = A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_angle_deg.SSM;
    A380SecComputer_B.Data = A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_angle_deg.Data;
    A380SecComputer_B.SSM_k = A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_angle_deg.SSM;
    A380SecComputer_B.Data_f = A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_angle_deg.Data;
    A380SecComputer_B.SSM_kx = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_pitch_rate_deg_s.SSM;
    A380SecComputer_B.Data_fw = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_pitch_rate_deg_s.Data;
    A380SecComputer_B.SSM_kxx = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_roll_rate_deg_s.SSM;
    A380SecComputer_B.Data_fwx = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_roll_rate_deg_s.Data;
    A380SecComputer_B.SSM_kxxt = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.SSM;
    A380SecComputer_B.Data_fwxk = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.Data;
    A380SecComputer_B.is_unit_2 = A380SecComputer_U.in.discrete_inputs.is_unit_2;
    A380SecComputer_B.SSM_kxxta = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_long_accel_g.SSM;
    A380SecComputer_B.Data_fwxkf = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_long_accel_g.Data;
    A380SecComputer_B.SSM_kxxtac = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_lat_accel_g.SSM;
    A380SecComputer_B.Data_fwxkft = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_lat_accel_g.Data;
    A380SecComputer_B.SSM_kxxtac0 = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_normal_accel_g.SSM;
    A380SecComputer_B.Data_fwxkftc = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_normal_accel_g.Data;
    A380SecComputer_B.SSM_kxxtac0z = A380SecComputer_U.in.bus_inputs.ir_1_bus.track_angle_rate_deg_s.SSM;
    A380SecComputer_B.Data_fwxkftc3 = A380SecComputer_U.in.bus_inputs.ir_1_bus.track_angle_rate_deg_s.Data;
    A380SecComputer_B.SSM_kxxtac0zt = A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.SSM;
    A380SecComputer_B.Data_fwxkftc3e = A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.Data;
    A380SecComputer_B.is_unit_3 = A380SecComputer_U.in.discrete_inputs.is_unit_3;
    A380SecComputer_B.SSM_kxxtac0ztg = A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_att_rate_deg_s.SSM;
    A380SecComputer_B.Data_fwxkftc3ep = A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_att_rate_deg_s.Data;
    A380SecComputer_B.SSM_kxxtac0ztgf = A380SecComputer_U.in.bus_inputs.ir_1_bus.inertial_alt_ft.SSM;
    A380SecComputer_B.Data_fwxkftc3epg = A380SecComputer_U.in.bus_inputs.ir_1_bus.inertial_alt_ft.Data;
    A380SecComputer_B.SSM_kxxtac0ztgf2 = A380SecComputer_U.in.bus_inputs.ir_1_bus.along_track_horiz_acc_g.SSM;
    A380SecComputer_B.Data_fwxkftc3epgt = A380SecComputer_U.in.bus_inputs.ir_1_bus.along_track_horiz_acc_g.Data;
    A380SecComputer_B.SSM_kxxtac0ztgf2u = A380SecComputer_U.in.bus_inputs.ir_1_bus.cross_track_horiz_acc_g.SSM;
    A380SecComputer_B.Data_fwxkftc3epgtd = A380SecComputer_U.in.bus_inputs.ir_1_bus.cross_track_horiz_acc_g.Data;
    A380SecComputer_B.SSM_kxxtac0ztgf2ux = A380SecComputer_U.in.bus_inputs.ir_1_bus.vertical_accel_g.SSM;
    A380SecComputer_B.Data_fwxkftc3epgtdx = A380SecComputer_U.in.bus_inputs.ir_1_bus.vertical_accel_g.Data;
    A380SecComputer_B.capt_priority_takeover_pressed =
      A380SecComputer_U.in.discrete_inputs.capt_priority_takeover_pressed;
    A380SecComputer_B.SSM_kxxtac0ztgf2uxn = A380SecComputer_U.in.bus_inputs.ir_1_bus.inertial_vertical_speed_ft_s.SSM;
    A380SecComputer_B.Data_fwxkftc3epgtdxc = A380SecComputer_U.in.bus_inputs.ir_1_bus.inertial_vertical_speed_ft_s.Data;
    A380SecComputer_B.SSM_ky = A380SecComputer_U.in.bus_inputs.ir_1_bus.north_south_velocity_kn.SSM;
    A380SecComputer_B.Data_h = A380SecComputer_U.in.bus_inputs.ir_1_bus.north_south_velocity_kn.Data;
    A380SecComputer_B.SSM_d = A380SecComputer_U.in.bus_inputs.ir_1_bus.east_west_velocity_kn.SSM;
    A380SecComputer_B.Data_e = A380SecComputer_U.in.bus_inputs.ir_1_bus.east_west_velocity_kn.Data;
    A380SecComputer_B.SSM_h = A380SecComputer_U.in.bus_inputs.ir_2_bus.discrete_word_1.SSM;
    A380SecComputer_B.Data_j = A380SecComputer_U.in.bus_inputs.ir_2_bus.discrete_word_1.Data;
    A380SecComputer_B.SSM_kb = A380SecComputer_U.in.bus_inputs.ir_2_bus.latitude_deg.SSM;
    A380SecComputer_B.Data_d = A380SecComputer_U.in.bus_inputs.ir_2_bus.latitude_deg.Data;
    A380SecComputer_B.fo_priority_takeover_pressed = A380SecComputer_U.in.discrete_inputs.fo_priority_takeover_pressed;
    A380SecComputer_B.SSM_p = A380SecComputer_U.in.bus_inputs.ir_2_bus.longitude_deg.SSM;
    A380SecComputer_B.Data_p = A380SecComputer_U.in.bus_inputs.ir_2_bus.longitude_deg.Data;
    A380SecComputer_B.SSM_di = A380SecComputer_U.in.bus_inputs.ir_2_bus.ground_speed_kn.SSM;
    A380SecComputer_B.Data_i = A380SecComputer_U.in.bus_inputs.ir_2_bus.ground_speed_kn.Data;
    A380SecComputer_B.SSM_j = A380SecComputer_U.in.bus_inputs.ir_2_bus.track_angle_true_deg.SSM;
    A380SecComputer_B.Data_g = A380SecComputer_U.in.bus_inputs.ir_2_bus.track_angle_true_deg.Data;
    A380SecComputer_B.SSM_i = A380SecComputer_U.in.bus_inputs.ir_2_bus.heading_true_deg.SSM;
    A380SecComputer_B.Data_a = A380SecComputer_U.in.bus_inputs.ir_2_bus.heading_true_deg.Data;
    A380SecComputer_B.SSM_g = A380SecComputer_U.in.bus_inputs.ir_2_bus.wind_speed_kn.SSM;
    A380SecComputer_B.Data_eb = A380SecComputer_U.in.bus_inputs.ir_2_bus.wind_speed_kn.Data;
    A380SecComputer_B.rudder_trim_left_pressed = A380SecComputer_U.in.discrete_inputs.rudder_trim_left_pressed;
    A380SecComputer_B.SSM_db = A380SecComputer_U.in.bus_inputs.ir_2_bus.wind_direction_true_deg.SSM;
    A380SecComputer_B.Data_jo = A380SecComputer_U.in.bus_inputs.ir_2_bus.wind_direction_true_deg.Data;
    A380SecComputer_B.SSM_n = A380SecComputer_U.in.bus_inputs.ir_2_bus.track_angle_magnetic_deg.SSM;
    A380SecComputer_B.Data_ex = A380SecComputer_U.in.bus_inputs.ir_2_bus.track_angle_magnetic_deg.Data;
    A380SecComputer_B.SSM_a = A380SecComputer_U.in.bus_inputs.ir_2_bus.heading_magnetic_deg.SSM;
    A380SecComputer_B.Data_fd = A380SecComputer_U.in.bus_inputs.ir_2_bus.heading_magnetic_deg.Data;
    A380SecComputer_B.SSM_ir = A380SecComputer_U.in.bus_inputs.ir_2_bus.drift_angle_deg.SSM;
    A380SecComputer_B.Data_ja = A380SecComputer_U.in.bus_inputs.ir_2_bus.drift_angle_deg.Data;
    A380SecComputer_B.SSM_hu = A380SecComputer_U.in.bus_inputs.ir_2_bus.flight_path_angle_deg.SSM;
    A380SecComputer_B.Data_k = A380SecComputer_U.in.bus_inputs.ir_2_bus.flight_path_angle_deg.Data;
    A380SecComputer_B.rudder_trim_right_pressed = A380SecComputer_U.in.discrete_inputs.rudder_trim_right_pressed;
    A380SecComputer_B.SSM_e = A380SecComputer_U.in.bus_inputs.ir_2_bus.flight_path_accel_g.SSM;
    A380SecComputer_B.Data_joy = A380SecComputer_U.in.bus_inputs.ir_2_bus.flight_path_accel_g.Data;
    A380SecComputer_B.SSM_gr = A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_angle_deg.SSM;
    A380SecComputer_B.Data_h3 = A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_angle_deg.Data;
    A380SecComputer_B.SSM_ev = A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_angle_deg.SSM;
    A380SecComputer_B.Data_a0 = A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_angle_deg.Data;
    A380SecComputer_B.SSM_l = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_pitch_rate_deg_s.SSM;
    A380SecComputer_B.Data_b = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_pitch_rate_deg_s.Data;
    A380SecComputer_B.SSM_ei = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_roll_rate_deg_s.SSM;
    A380SecComputer_B.Data_eq = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_roll_rate_deg_s.Data;
    A380SecComputer_B.rudder_trim_reset_pressed = A380SecComputer_U.in.discrete_inputs.rudder_trim_reset_pressed;
    A380SecComputer_B.SSM_an = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.SSM;
    A380SecComputer_B.Data_iz = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.Data;
    A380SecComputer_B.SSM_c = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_long_accel_g.SSM;
    A380SecComputer_B.Data_j2 = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_long_accel_g.Data;
    A380SecComputer_B.SSM_cb = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_lat_accel_g.SSM;
    A380SecComputer_B.Data_o = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_lat_accel_g.Data;
    A380SecComputer_B.SSM_lb = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_normal_accel_g.SSM;
    A380SecComputer_B.Data_m = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_normal_accel_g.Data;
    A380SecComputer_B.SSM_ia = A380SecComputer_U.in.bus_inputs.ir_2_bus.track_angle_rate_deg_s.SSM;
    A380SecComputer_B.Data_oq = A380SecComputer_U.in.bus_inputs.ir_2_bus.track_angle_rate_deg_s.Data;
    A380SecComputer_B.pitch_trim_up_pressed = A380SecComputer_U.in.discrete_inputs.pitch_trim_up_pressed;
    A380SecComputer_B.SSM_kyz = A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.SSM;
    A380SecComputer_B.Data_fo = A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.Data;
    A380SecComputer_B.SSM_as = A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_att_rate_deg_s.SSM;
    A380SecComputer_B.Data_p1 = A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_att_rate_deg_s.Data;
    A380SecComputer_B.SSM_is = A380SecComputer_U.in.bus_inputs.ir_2_bus.inertial_alt_ft.SSM;
    A380SecComputer_B.Data_p1y = A380SecComputer_U.in.bus_inputs.ir_2_bus.inertial_alt_ft.Data;
    A380SecComputer_B.SSM_ca = A380SecComputer_U.in.bus_inputs.ir_2_bus.along_track_horiz_acc_g.SSM;
    A380SecComputer_B.Data_l = A380SecComputer_U.in.bus_inputs.ir_2_bus.along_track_horiz_acc_g.Data;
    A380SecComputer_B.SSM_o = A380SecComputer_U.in.bus_inputs.ir_2_bus.cross_track_horiz_acc_g.SSM;
    A380SecComputer_B.Data_kp = A380SecComputer_U.in.bus_inputs.ir_2_bus.cross_track_horiz_acc_g.Data;
    A380SecComputer_B.pitch_trim_down_pressed = A380SecComputer_U.in.discrete_inputs.pitch_trim_down_pressed;
    A380SecComputer_B.SSM_ak = A380SecComputer_U.in.bus_inputs.ir_2_bus.vertical_accel_g.SSM;
    A380SecComputer_B.Data_k0 = A380SecComputer_U.in.bus_inputs.ir_2_bus.vertical_accel_g.Data;
    A380SecComputer_B.SSM_cbj = A380SecComputer_U.in.bus_inputs.ir_2_bus.inertial_vertical_speed_ft_s.SSM;
    A380SecComputer_B.Data_pi = A380SecComputer_U.in.bus_inputs.ir_2_bus.inertial_vertical_speed_ft_s.Data;
    A380SecComputer_B.SSM_cu = A380SecComputer_U.in.bus_inputs.ir_2_bus.north_south_velocity_kn.SSM;
    A380SecComputer_B.Data_dm = A380SecComputer_U.in.bus_inputs.ir_2_bus.north_south_velocity_kn.Data;
    A380SecComputer_B.SSM_nn = A380SecComputer_U.in.bus_inputs.ir_2_bus.east_west_velocity_kn.SSM;
    A380SecComputer_B.Data_f5 = A380SecComputer_U.in.bus_inputs.ir_2_bus.east_west_velocity_kn.Data;
    A380SecComputer_B.SSM_b = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_flap_component_status_word.SSM;
    A380SecComputer_B.Data_js = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_flap_component_status_word.Data;
    A380SecComputer_B.simulation_time = A380SecComputer_U.in.time.simulation_time;
    A380SecComputer_B.green_low_pressure = A380SecComputer_U.in.discrete_inputs.green_low_pressure;
    A380SecComputer_B.SSM_m = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_flap_system_status_word.SSM;
    A380SecComputer_B.Data_ee = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_flap_system_status_word.Data;
    A380SecComputer_B.SSM_f = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_flap_actual_position_word.SSM;
    A380SecComputer_B.Data_ig = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_flap_actual_position_word.Data;
    A380SecComputer_B.SSM_bp = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_actual_position_deg.SSM;
    A380SecComputer_B.Data_mk = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_actual_position_deg.Data;
    A380SecComputer_B.SSM_hb = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.flap_actual_position_deg.SSM;
    A380SecComputer_B.Data_pu = A380SecComputer_U.in.bus_inputs.sfcc_1_bus.flap_actual_position_deg.Data;
    A380SecComputer_B.SSM_gz = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_flap_component_status_word.SSM;
    A380SecComputer_B.Data_ly = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_flap_component_status_word.Data;
    A380SecComputer_B.yellow_low_pressure = A380SecComputer_U.in.discrete_inputs.yellow_low_pressure;
    A380SecComputer_B.SSM_pv = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_flap_system_status_word.SSM;
    A380SecComputer_B.Data_jq = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_flap_system_status_word.Data;
    A380SecComputer_B.SSM_mf = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_flap_actual_position_word.SSM;
    A380SecComputer_B.Data_o5 = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_flap_actual_position_word.Data;
    A380SecComputer_B.SSM_m0 = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_actual_position_deg.SSM;
    A380SecComputer_B.Data_lyw = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_actual_position_deg.Data;
    A380SecComputer_B.SSM_kd = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.flap_actual_position_deg.SSM;
    A380SecComputer_B.Data_gq = A380SecComputer_U.in.bus_inputs.sfcc_2_bus.flap_actual_position_deg.Data;
    A380SecComputer_B.irdc_5_a_bus = A380SecComputer_U.in.bus_inputs.irdc_5_a_bus;
    A380SecComputer_B.irdc_5_b_bus = A380SecComputer_U.in.bus_inputs.irdc_5_b_bus;
    A380SecComputer_B.capt_pitch_stick_pos = A380SecComputer_U.in.analog_inputs.capt_pitch_stick_pos;
    A380SecComputer_B.SSM_pu = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_inboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_n = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_inboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_nv = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_inboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_bq = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_inboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_d5 = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_midboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_dmn = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_midboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_eo = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_midboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_jn = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_midboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_nd = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_outboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_c = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_outboard_aileron_command_deg.Data;
    A380SecComputer_B.fo_pitch_stick_pos = A380SecComputer_U.in.analog_inputs.fo_pitch_stick_pos;
    A380SecComputer_B.SSM_bq = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_outboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_lx = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_outboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_hi = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_1_command_deg.SSM;
    A380SecComputer_B.Data_jb = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_1_command_deg.Data;
    A380SecComputer_B.SSM_mm = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_1_command_deg.SSM;
    A380SecComputer_B.Data_fn = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_1_command_deg.Data;
    A380SecComputer_B.SSM_kz = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_2_command_deg.SSM;
    A380SecComputer_B.Data_od = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_2_command_deg.Data;
    A380SecComputer_B.SSM_il = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_2_command_deg.SSM;
    A380SecComputer_B.Data_ez = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_2_command_deg.Data;
    A380SecComputer_B.capt_roll_stick_pos = A380SecComputer_U.in.analog_inputs.capt_roll_stick_pos;
    A380SecComputer_B.SSM_i2 = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_3_command_deg.SSM;
    A380SecComputer_B.Data_pw = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_3_command_deg.Data;
    A380SecComputer_B.SSM_ah = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_3_command_deg.SSM;
    A380SecComputer_B.Data_m2 = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_3_command_deg.Data;
    A380SecComputer_B.SSM_en = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_4_command_deg.SSM;
    A380SecComputer_B.Data_ek = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_4_command_deg.Data;
    A380SecComputer_B.SSM_dq = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_4_command_deg.SSM;
    A380SecComputer_B.Data_iy = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_4_command_deg.Data;
    A380SecComputer_B.SSM_px = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_5_command_deg.SSM;
    A380SecComputer_B.Data_lk = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_5_command_deg.Data;
    A380SecComputer_B.fo_roll_stick_pos = A380SecComputer_U.in.analog_inputs.fo_roll_stick_pos;
    A380SecComputer_B.SSM_lbo = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_5_command_deg.SSM;
    A380SecComputer_B.Data_ca = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_5_command_deg.Data;
    A380SecComputer_B.SSM_p5 = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_6_command_deg.SSM;
    A380SecComputer_B.Data_pix = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_6_command_deg.Data;
    A380SecComputer_B.SSM_mk = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_6_command_deg.SSM;
    A380SecComputer_B.Data_di = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_6_command_deg.Data;
    A380SecComputer_B.SSM_mu = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_7_command_deg.SSM;
    A380SecComputer_B.Data_lz = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_7_command_deg.Data;
    A380SecComputer_B.SSM_cbl = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_7_command_deg.SSM;
    A380SecComputer_B.Data_lu = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_7_command_deg.Data;
    A380SecComputer_B.elevator_1_pos_deg = A380SecComputer_U.in.analog_inputs.elevator_1_pos_deg;
    A380SecComputer_B.SSM_gzd = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_8_command_deg.SSM;
    A380SecComputer_B.Data_dc = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_8_command_deg.Data;
    A380SecComputer_B.SSM_mo = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_8_command_deg.SSM;
    A380SecComputer_B.Data_gc = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_8_command_deg.Data;
    A380SecComputer_B.SSM_me = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_inboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_am = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_inboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_mj = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_inboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_mo = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_inboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_a5 = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_outboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_dg = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_outboard_elevator_command_deg.Data;
    A380SecComputer_B.elevator_2_pos_deg = A380SecComputer_U.in.analog_inputs.elevator_2_pos_deg;
    A380SecComputer_B.SSM_bt = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_outboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_e1 = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_outboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_om = A380SecComputer_U.in.bus_inputs.prim_1_bus.ths_command_deg.SSM;
    A380SecComputer_B.Data_fp = A380SecComputer_U.in.bus_inputs.prim_1_bus.ths_command_deg.Data;
    A380SecComputer_B.SSM_ar = A380SecComputer_U.in.bus_inputs.prim_1_bus.upper_rudder_command_deg.SSM;
    A380SecComputer_B.Data_ns = A380SecComputer_U.in.bus_inputs.prim_1_bus.upper_rudder_command_deg.Data;
    A380SecComputer_B.SSM_ce = A380SecComputer_U.in.bus_inputs.prim_1_bus.lower_rudder_command_deg.SSM;
    A380SecComputer_B.Data_m3 = A380SecComputer_U.in.bus_inputs.prim_1_bus.lower_rudder_command_deg.Data;
    A380SecComputer_B.SSM_ed = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_oj = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.elevator_3_pos_deg = A380SecComputer_U.in.analog_inputs.elevator_3_pos_deg;
    A380SecComputer_B.SSM_jh = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_jy = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.SSM_je = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_j1 = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_sidestick_roll_command_deg.Data;
    A380SecComputer_B.SSM_jt = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_fc = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_sidestick_roll_command_deg.Data;
    A380SecComputer_B.SSM_cui = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_pedal_position_deg.SSM;
    A380SecComputer_B.Data_of = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_pedal_position_deg.Data;
    A380SecComputer_B.SSM_mq = A380SecComputer_U.in.bus_inputs.prim_1_bus.aileron_status_word.SSM;
    A380SecComputer_B.Data_lg = A380SecComputer_U.in.bus_inputs.prim_1_bus.aileron_status_word.Data;
    A380SecComputer_B.ths_pos_deg = A380SecComputer_U.in.analog_inputs.ths_pos_deg;
    A380SecComputer_B.SSM_ni = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_n4 = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_aileron_1_position_deg.Data;
    A380SecComputer_B.SSM_df = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_ot = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_oe = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_gv = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_aileron_1_position_deg.Data;
    A380SecComputer_B.SSM_ha = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_ou = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_op = A380SecComputer_U.in.bus_inputs.prim_1_bus.spoiler_status_word.SSM;
    A380SecComputer_B.Data_dh = A380SecComputer_U.in.bus_inputs.prim_1_bus.spoiler_status_word.Data;
    A380SecComputer_B.monotonic_time = A380SecComputer_U.in.time.monotonic_time;
    A380SecComputer_B.left_aileron_1_pos_deg = A380SecComputer_U.in.analog_inputs.left_aileron_1_pos_deg;
    A380SecComputer_B.SSM_a50 = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_position_deg.SSM;
    A380SecComputer_B.Data_ph = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_position_deg.Data;
    A380SecComputer_B.SSM_og = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_position_deg.SSM;
    A380SecComputer_B.Data_gs = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_position_deg.Data;
    A380SecComputer_B.SSM_a4 = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.SSM;
    A380SecComputer_B.Data_fd4 = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.Data;
    A380SecComputer_B.SSM_bv = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_1_position_deg.SSM;
    A380SecComputer_B.Data_hm = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_1_position_deg.Data;
    A380SecComputer_B.SSM_bo = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_2_position_deg.SSM;
    A380SecComputer_B.Data_i2 = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_2_position_deg.Data;
    A380SecComputer_B.left_aileron_2_pos_deg = A380SecComputer_U.in.analog_inputs.left_aileron_2_pos_deg;
    A380SecComputer_B.SSM_d1 = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_3_position_deg.SSM;
    A380SecComputer_B.Data_og = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_3_position_deg.Data;
    A380SecComputer_B.SSM_hy = A380SecComputer_U.in.bus_inputs.prim_1_bus.ths_position_deg.SSM;
    A380SecComputer_B.Data_fv = A380SecComputer_U.in.bus_inputs.prim_1_bus.ths_position_deg.Data;
    A380SecComputer_B.SSM_gi = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word.SSM;
    A380SecComputer_B.Data_oc = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word.Data;
    A380SecComputer_B.SSM_pp = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_1_position_deg.SSM;
    A380SecComputer_B.Data_kq = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_1_position_deg.Data;
    A380SecComputer_B.SSM_iab = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_2_position_deg.SSM;
    A380SecComputer_B.Data_ne = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_2_position_deg.Data;
    A380SecComputer_B.right_aileron_1_pos_deg = A380SecComputer_U.in.analog_inputs.right_aileron_1_pos_deg;
    A380SecComputer_B.SSM_jtv = A380SecComputer_U.in.bus_inputs.prim_1_bus.fctl_law_status_word.SSM;
    A380SecComputer_B.Data_it = A380SecComputer_U.in.bus_inputs.prim_1_bus.fctl_law_status_word.Data;
    A380SecComputer_B.SSM_fy = A380SecComputer_U.in.bus_inputs.prim_1_bus.misc_data_status_word.SSM;
    A380SecComputer_B.Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.misc_data_status_word.Data;
    A380SecComputer_B.SSM_d4 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_inboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_bb = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_inboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_ars = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_inboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_ol = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_inboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_din = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_midboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_hw = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_midboard_aileron_command_deg.Data;
    A380SecComputer_B.right_aileron_2_pos_deg = A380SecComputer_U.in.analog_inputs.right_aileron_2_pos_deg;
    A380SecComputer_B.SSM_m3 = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_midboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_hs = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_midboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_np = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_outboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_fj = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_outboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_ax = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_outboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_ky = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_outboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_cl = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_1_command_deg.SSM;
    A380SecComputer_B.Data_h5 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_1_command_deg.Data;
    A380SecComputer_B.SSM_es = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_1_command_deg.SSM;
    A380SecComputer_B.Data_ku = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_1_command_deg.Data;
    A380SecComputer_B.left_spoiler_1_pos_deg = A380SecComputer_U.in.analog_inputs.left_spoiler_1_pos_deg;
    A380SecComputer_B.SSM_gi1 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_2_command_deg.SSM;
    A380SecComputer_B.Data_jp = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_2_command_deg.Data;
    A380SecComputer_B.SSM_jz = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_2_command_deg.SSM;
    A380SecComputer_B.Data_nu = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_2_command_deg.Data;
    A380SecComputer_B.SSM_kt = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_3_command_deg.SSM;
    A380SecComputer_B.Data_br = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_3_command_deg.Data;
    A380SecComputer_B.SSM_ds = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_3_command_deg.SSM;
    A380SecComputer_B.Data_ae = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_3_command_deg.Data;
    A380SecComputer_B.SSM_eg = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_4_command_deg.SSM;
    A380SecComputer_B.Data_pe = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_4_command_deg.Data;
    A380SecComputer_B.right_spoiler_1_pos_deg = A380SecComputer_U.in.analog_inputs.right_spoiler_1_pos_deg;
    A380SecComputer_B.SSM_a0 = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_4_command_deg.SSM;
    A380SecComputer_B.Data_fy = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_4_command_deg.Data;
    A380SecComputer_B.SSM_cv = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_5_command_deg.SSM;
    A380SecComputer_B.Data_na = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_5_command_deg.Data;
    A380SecComputer_B.SSM_ea = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_5_command_deg.SSM;
    A380SecComputer_B.Data_my = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_5_command_deg.Data;
    A380SecComputer_B.SSM_p4 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_6_command_deg.SSM;
    A380SecComputer_B.Data_i4 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_6_command_deg.Data;
    A380SecComputer_B.SSM_m2 = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_6_command_deg.SSM;
    A380SecComputer_B.Data_cx = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_6_command_deg.Data;
    A380SecComputer_B.left_spoiler_2_pos_deg = A380SecComputer_U.in.analog_inputs.left_spoiler_2_pos_deg;
    A380SecComputer_B.SSM_bt0 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_7_command_deg.SSM;
    A380SecComputer_B.Data_nz = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_7_command_deg.Data;
    A380SecComputer_B.SSM_nr = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_7_command_deg.SSM;
    A380SecComputer_B.Data_id = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_7_command_deg.Data;
    A380SecComputer_B.SSM_fr = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_8_command_deg.SSM;
    A380SecComputer_B.Data_o2 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_8_command_deg.Data;
    A380SecComputer_B.SSM_cc = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_8_command_deg.SSM;
    A380SecComputer_B.Data_gqq = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_8_command_deg.Data;
    A380SecComputer_B.SSM_lm = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_inboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_md = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_inboard_elevator_command_deg.Data;
    A380SecComputer_B.right_spoiler_2_pos_deg = A380SecComputer_U.in.analog_inputs.right_spoiler_2_pos_deg;
    A380SecComputer_B.SSM_mkm = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_inboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_cz = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_inboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_jhd = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_outboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_pm = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_outboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_av = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_outboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_bj = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_outboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_ira = A380SecComputer_U.in.bus_inputs.prim_2_bus.ths_command_deg.SSM;
    A380SecComputer_B.Data_ox = A380SecComputer_U.in.bus_inputs.prim_2_bus.ths_command_deg.Data;
    A380SecComputer_B.SSM_ge = A380SecComputer_U.in.bus_inputs.prim_2_bus.upper_rudder_command_deg.SSM;
    A380SecComputer_B.Data_pe5 = A380SecComputer_U.in.bus_inputs.prim_2_bus.upper_rudder_command_deg.Data;
    A380SecComputer_B.rudder_1_pos_deg = A380SecComputer_U.in.analog_inputs.rudder_1_pos_deg;
    A380SecComputer_B.SSM_lv = A380SecComputer_U.in.bus_inputs.prim_2_bus.lower_rudder_command_deg.SSM;
    A380SecComputer_B.Data_jj = A380SecComputer_U.in.bus_inputs.prim_2_bus.lower_rudder_command_deg.Data;
    A380SecComputer_B.SSM_cg = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_p5 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.SSM_be = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_ekl = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.SSM_axb = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_nd = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_sidestick_roll_command_deg.Data;
    A380SecComputer_B.SSM_nz = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_n2 = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_sidestick_roll_command_deg.Data;
    A380SecComputer_B.rudder_2_pos_deg = A380SecComputer_U.in.analog_inputs.rudder_2_pos_deg;
    A380SecComputer_B.SSM_cx = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_pedal_position_deg.SSM;
    A380SecComputer_B.Data_dl = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_pedal_position_deg.Data;
    A380SecComputer_B.SSM_gh = A380SecComputer_U.in.bus_inputs.prim_2_bus.aileron_status_word.SSM;
    A380SecComputer_B.Data_gs2 = A380SecComputer_U.in.bus_inputs.prim_2_bus.aileron_status_word.Data;
    A380SecComputer_B.SSM_ks = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_h4 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_aileron_1_position_deg.Data;
    A380SecComputer_B.SSM_pw = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_e3 = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_fh = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_f5h = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_aileron_1_position_deg.Data;
    A380SecComputer_B.slew_on = A380SecComputer_U.in.sim_data.slew_on;
    A380SecComputer_B.rudder_pedal_pos_deg = A380SecComputer_U.in.analog_inputs.rudder_pedal_pos_deg;
    A380SecComputer_B.SSM_gzn = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_an = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_oo = A380SecComputer_U.in.bus_inputs.prim_2_bus.spoiler_status_word.SSM;
    A380SecComputer_B.Data_i4o = A380SecComputer_U.in.bus_inputs.prim_2_bus.spoiler_status_word.Data;
    A380SecComputer_B.SSM_evh = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_position_deg.SSM;
    A380SecComputer_B.Data_af = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_position_deg.Data;
    A380SecComputer_B.SSM_cn = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_position_deg.SSM;
    A380SecComputer_B.Data_bm = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_position_deg.Data;
    A380SecComputer_B.SSM_co = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.SSM;
    A380SecComputer_B.Data_dk = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.Data;
    A380SecComputer_B.rudder_trim_pos_deg = A380SecComputer_U.in.analog_inputs.rudder_trim_pos_deg;
    A380SecComputer_B.SSM_pe = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_1_position_deg.SSM;
    A380SecComputer_B.Data_nv = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_1_position_deg.Data;
    A380SecComputer_B.SSM_cgz = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_2_position_deg.SSM;
    A380SecComputer_B.Data_jpf = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_2_position_deg.Data;
    A380SecComputer_B.SSM_fw = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_3_position_deg.SSM;
    A380SecComputer_B.Data_i5 = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_3_position_deg.Data;
    A380SecComputer_B.SSM_h4 = A380SecComputer_U.in.bus_inputs.prim_2_bus.ths_position_deg.SSM;
    A380SecComputer_B.Data_k2 = A380SecComputer_U.in.bus_inputs.prim_2_bus.ths_position_deg.Data;
    A380SecComputer_B.SSM_cb3 = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_status_word.SSM;
    A380SecComputer_B.Data_as = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_status_word.Data;
    A380SecComputer_B.SSM_pj = A380SecComputer_U.in.bus_inputs.adr_1_bus.altitude_standard_ft.SSM;
    A380SecComputer_B.SSM_dv = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_1_position_deg.SSM;
    A380SecComputer_B.Data_gk = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_1_position_deg.Data;
    A380SecComputer_B.SSM_i4 = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_2_position_deg.SSM;
    A380SecComputer_B.Data_jl = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_2_position_deg.Data;
    A380SecComputer_B.SSM_fm = A380SecComputer_U.in.bus_inputs.prim_2_bus.fctl_law_status_word.SSM;
    A380SecComputer_B.Data_e32 = A380SecComputer_U.in.bus_inputs.prim_2_bus.fctl_law_status_word.Data;
    A380SecComputer_B.SSM_e5 = A380SecComputer_U.in.bus_inputs.prim_2_bus.misc_data_status_word.SSM;
    A380SecComputer_B.Data_ih = A380SecComputer_U.in.bus_inputs.prim_2_bus.misc_data_status_word.Data;
    A380SecComputer_B.SSM_bf = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_inboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_du = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_inboard_aileron_command_deg.Data;
    A380SecComputer_B.Data_nx = A380SecComputer_U.in.bus_inputs.adr_1_bus.altitude_standard_ft.Data;
    A380SecComputer_B.SSM_fd = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_inboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_n0 = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_inboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_fv = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_midboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_eqi = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_midboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_dt = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_midboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_om = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_midboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_j5 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_outboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_nr = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_outboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_ng = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_outboard_aileron_command_deg.SSM;
    A380SecComputer_B.Data_p3 = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_outboard_aileron_command_deg.Data;
    A380SecComputer_B.SSM_cs = A380SecComputer_U.in.bus_inputs.adr_1_bus.altitude_corrected_ft.SSM;
    A380SecComputer_B.SSM_ls = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_1_command_deg.SSM;
    A380SecComputer_B.Data_nb = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_1_command_deg.Data;
    A380SecComputer_B.SSM_dg = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_1_command_deg.SSM;
    A380SecComputer_B.Data_hd = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_1_command_deg.Data;
    A380SecComputer_B.SSM_d3 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_2_command_deg.SSM;
    A380SecComputer_B.Data_al = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_2_command_deg.Data;
    A380SecComputer_B.SSM_p2 = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_2_command_deg.SSM;
    A380SecComputer_B.Data_gu = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_2_command_deg.Data;
    A380SecComputer_B.SSM_bo0 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_3_command_deg.SSM;
    A380SecComputer_B.Data_ix = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_3_command_deg.Data;
    A380SecComputer_B.Data_do = A380SecComputer_U.in.bus_inputs.adr_1_bus.altitude_corrected_ft.Data;
    A380SecComputer_B.SSM_bc = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_3_command_deg.SSM;
    A380SecComputer_B.Data_hu = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_3_command_deg.Data;
    A380SecComputer_B.SSM_h0 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_4_command_deg.SSM;
    A380SecComputer_B.Data_pm1 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_4_command_deg.Data;
    A380SecComputer_B.SSM_giz = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_4_command_deg.SSM;
    A380SecComputer_B.Data_i2y = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_4_command_deg.Data;
    A380SecComputer_B.SSM_mqp = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_5_command_deg.SSM;
    A380SecComputer_B.Data_pg = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_5_command_deg.Data;
    A380SecComputer_B.SSM_ba = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_5_command_deg.SSM;
    A380SecComputer_B.Data_ni = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_5_command_deg.Data;
    A380SecComputer_B.SSM_in = A380SecComputer_U.in.bus_inputs.adr_1_bus.mach.SSM;
    A380SecComputer_B.SSM_ff = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_6_command_deg.SSM;
    A380SecComputer_B.Data_fr = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_6_command_deg.Data;
    A380SecComputer_B.SSM_ic = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_6_command_deg.SSM;
    A380SecComputer_B.Data_cn = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_6_command_deg.Data;
    A380SecComputer_B.SSM_fs = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_7_command_deg.SSM;
    A380SecComputer_B.Data_nxl = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_7_command_deg.Data;
    A380SecComputer_B.SSM_ja = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_7_command_deg.SSM;
    A380SecComputer_B.Data_jh = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_7_command_deg.Data;
    A380SecComputer_B.SSM_js = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_8_command_deg.SSM;
    A380SecComputer_B.Data_gl = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_8_command_deg.Data;
    A380SecComputer_B.Data_gn = A380SecComputer_U.in.bus_inputs.adr_1_bus.mach.Data;
    A380SecComputer_B.SSM_is3 = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_8_command_deg.SSM;
    A380SecComputer_B.Data_myb = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_8_command_deg.Data;
    A380SecComputer_B.SSM_ag = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_inboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_l2 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_inboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_f5 = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_inboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_o5o = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_inboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_ph = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_outboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_l5 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_outboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_jw = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_outboard_elevator_command_deg.SSM;
    A380SecComputer_B.Data_dc2 = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_outboard_elevator_command_deg.Data;
    A380SecComputer_B.SSM_jy = A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_computed_kn.SSM;
    A380SecComputer_B.SSM_j1 = A380SecComputer_U.in.bus_inputs.prim_3_bus.ths_command_deg.SSM;
    A380SecComputer_B.Data_gr = A380SecComputer_U.in.bus_inputs.prim_3_bus.ths_command_deg.Data;
    A380SecComputer_B.SSM_ov = A380SecComputer_U.in.bus_inputs.prim_3_bus.upper_rudder_command_deg.SSM;
    A380SecComputer_B.Data_gp = A380SecComputer_U.in.bus_inputs.prim_3_bus.upper_rudder_command_deg.Data;
    A380SecComputer_B.SSM_mx = A380SecComputer_U.in.bus_inputs.prim_3_bus.lower_rudder_command_deg.SSM;
    A380SecComputer_B.Data_i3 = A380SecComputer_U.in.bus_inputs.prim_3_bus.lower_rudder_command_deg.Data;
    A380SecComputer_B.SSM_b4 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_et = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.SSM_gb = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_mc = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.Data_k3 = A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_computed_kn.Data;
    A380SecComputer_B.SSM_oh = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_f2 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_sidestick_roll_command_deg.Data;
    A380SecComputer_B.SSM_mm5 = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_gh = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_sidestick_roll_command_deg.Data;
    A380SecComputer_B.SSM_br = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_pedal_position_deg.SSM;
    A380SecComputer_B.Data_ed = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_pedal_position_deg.Data;
    A380SecComputer_B.SSM_c2 = A380SecComputer_U.in.bus_inputs.prim_3_bus.aileron_status_word.SSM;
    A380SecComputer_B.Data_o2j = A380SecComputer_U.in.bus_inputs.prim_3_bus.aileron_status_word.Data;
    A380SecComputer_B.SSM_hc = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_i43 = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_aileron_1_position_deg.Data;
    A380SecComputer_B.pause_on = A380SecComputer_U.in.sim_data.pause_on;
    A380SecComputer_B.SSM_ktr = A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_true_kn.SSM;
    A380SecComputer_B.SSM_gl = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_ic = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_my = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_ak = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_aileron_1_position_deg.Data;
    A380SecComputer_B.SSM_j3 = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_jg = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_go = A380SecComputer_U.in.bus_inputs.prim_3_bus.spoiler_status_word.SSM;
    A380SecComputer_B.Data_cu = A380SecComputer_U.in.bus_inputs.prim_3_bus.spoiler_status_word.Data;
    A380SecComputer_B.SSM_e5c = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_position_deg.SSM;
    A380SecComputer_B.Data_ep = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_position_deg.Data;
    A380SecComputer_B.Data_d3 = A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_true_kn.Data;
    A380SecComputer_B.SSM_dk = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_position_deg.SSM;
    A380SecComputer_B.Data_bt = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_position_deg.Data;
    A380SecComputer_B.SSM_evc = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.SSM;
    A380SecComputer_B.Data_e0 = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.Data;
    A380SecComputer_B.SSM_kk = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_1_position_deg.SSM;
    A380SecComputer_B.Data_jl3 = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_1_position_deg.Data;
    A380SecComputer_B.SSM_af = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_2_position_deg.SSM;
    A380SecComputer_B.Data_nm = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_2_position_deg.Data;
    A380SecComputer_B.SSM_npr = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_3_position_deg.SSM;
    A380SecComputer_B.Data_ia = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_3_position_deg.Data;
    A380SecComputer_B.SSM_ew = A380SecComputer_U.in.bus_inputs.adr_1_bus.vertical_speed_ft_min.SSM;
    A380SecComputer_B.SSM_lt = A380SecComputer_U.in.bus_inputs.prim_3_bus.ths_position_deg.SSM;
    A380SecComputer_B.Data_j0 = A380SecComputer_U.in.bus_inputs.prim_3_bus.ths_position_deg.Data;
    A380SecComputer_B.SSM_ger = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_status_word.SSM;
    A380SecComputer_B.Data_bs = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_status_word.Data;
    A380SecComputer_B.SSM_pxo = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_1_position_deg.SSM;
    A380SecComputer_B.Data_hp = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_1_position_deg.Data;
    A380SecComputer_B.SSM_co2 = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_2_position_deg.SSM;
    A380SecComputer_B.Data_ct = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_2_position_deg.Data;
    A380SecComputer_B.SSM_ny = A380SecComputer_U.in.bus_inputs.prim_3_bus.fctl_law_status_word.SSM;
    A380SecComputer_B.Data_pc = A380SecComputer_U.in.bus_inputs.prim_3_bus.fctl_law_status_word.Data;
    A380SecComputer_B.Data_nzt = A380SecComputer_U.in.bus_inputs.adr_1_bus.vertical_speed_ft_min.Data;
    A380SecComputer_B.SSM_l4 = A380SecComputer_U.in.bus_inputs.prim_3_bus.misc_data_status_word.SSM;
    A380SecComputer_B.Data_c0 = A380SecComputer_U.in.bus_inputs.prim_3_bus.misc_data_status_word.Data;
    A380SecComputer_B.SSM_nm = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_ojg = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.SSM_nh = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_lm = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.SSM_dl = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_fz = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_sidestick_roll_command_deg.Data;
    A380SecComputer_B.SSM_dx = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_oz = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_sidestick_roll_command_deg.Data;
    A380SecComputer_B.SSM_a5h = A380SecComputer_U.in.bus_inputs.adr_1_bus.aoa_corrected_deg.SSM;
    A380SecComputer_B.SSM_fl = A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_pedal_position_deg.SSM;
    A380SecComputer_B.Data_gf = A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_pedal_position_deg.Data;
    A380SecComputer_B.SSM_p3 = A380SecComputer_U.in.bus_inputs.sec_x_bus.aileron_status_word.SSM;
    A380SecComputer_B.Data_nn = A380SecComputer_U.in.bus_inputs.sec_x_bus.aileron_status_word.Data;
    A380SecComputer_B.SSM_ns = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_a0z = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_aileron_1_position_deg.Data;
    A380SecComputer_B.SSM_bm = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_fk = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_nl = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_bu = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_aileron_1_position_deg.Data;
    A380SecComputer_B.Data_o23 = A380SecComputer_U.in.bus_inputs.adr_1_bus.aoa_corrected_deg.Data;
    A380SecComputer_B.SSM_grm = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_g3 = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_gzm = A380SecComputer_U.in.bus_inputs.sec_x_bus.spoiler_status_word.SSM;
    A380SecComputer_B.Data_icc = A380SecComputer_U.in.bus_inputs.sec_x_bus.spoiler_status_word.Data;
    A380SecComputer_B.SSM_oi = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_spoiler_1_position_deg.SSM;
    A380SecComputer_B.Data_pwf = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_spoiler_1_position_deg.Data;
    A380SecComputer_B.SSM_aa = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_spoiler_1_position_deg.SSM;
    A380SecComputer_B.Data_gvk = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_spoiler_1_position_deg.Data;
    A380SecComputer_B.SSM_fvk = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_spoiler_2_position_deg.SSM;
    A380SecComputer_B.Data_ln = A380SecComputer_U.in.bus_inputs.sec_x_bus.left_spoiler_2_position_deg.Data;
    A380SecComputer_B.SSM_lw = A380SecComputer_U.in.bus_inputs.adr_1_bus.corrected_average_static_pressure.SSM;
    A380SecComputer_B.SSM_fa = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_spoiler_2_position_deg.SSM;
    A380SecComputer_B.Data_ka = A380SecComputer_U.in.bus_inputs.sec_x_bus.right_spoiler_2_position_deg.Data;
    A380SecComputer_B.SSM_lbx = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_status_word.SSM;
    A380SecComputer_B.Data_mp = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_status_word.Data;
    A380SecComputer_B.SSM_n3 = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_1_position_deg.SSM;
    A380SecComputer_B.Data_m4 = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_1_position_deg.Data;
    A380SecComputer_B.SSM_a1 = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_2_position_deg.SSM;
    A380SecComputer_B.Data_fki = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_2_position_deg.Data;
    A380SecComputer_B.SSM_p1 = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_3_position_deg.SSM;
    A380SecComputer_B.Data_bv = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_3_position_deg.Data;
    A380SecComputer_B.Data_m21 = A380SecComputer_U.in.bus_inputs.adr_1_bus.corrected_average_static_pressure.Data;
    A380SecComputer_B.SSM_cn2 = A380SecComputer_U.in.bus_inputs.sec_x_bus.ths_position_deg.SSM;
    A380SecComputer_B.Data_nbg = A380SecComputer_U.in.bus_inputs.sec_x_bus.ths_position_deg.Data;
    A380SecComputer_B.SSM_an3 = A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word.SSM;
    A380SecComputer_B.Data_l25 = A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word.Data;
    A380SecComputer_B.SSM_c3 = A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_1_position_deg.SSM;
    A380SecComputer_B.Data_ki = A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_1_position_deg.Data;
    A380SecComputer_B.SSM_dp = A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_2_position_deg.SSM;
    A380SecComputer_B.Data_p5p = A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_2_position_deg.Data;
    A380SecComputer_B.SSM_boy = A380SecComputer_U.in.bus_inputs.sec_x_bus.fctl_law_status_word.SSM;
    A380SecComputer_B.Data_nry = A380SecComputer_U.in.bus_inputs.sec_x_bus.fctl_law_status_word.Data;
    A380SecComputer_B.SSM_lg = A380SecComputer_U.in.bus_inputs.adr_2_bus.altitude_standard_ft.SSM;
    A380SecComputer_B.SSM_cm = A380SecComputer_U.in.bus_inputs.sec_x_bus.misc_data_status_word.SSM;
    A380SecComputer_B.Data_mh = A380SecComputer_U.in.bus_inputs.sec_x_bus.misc_data_status_word.Data;
    A380SecComputer_B.SSM_hl = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_ll = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.SSM_irh = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_sidestick_pitch_command_deg.SSM;
    A380SecComputer_B.Data_hy = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_sidestick_pitch_command_deg.Data;
    A380SecComputer_B.SSM_b42 = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_j04 = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_sidestick_roll_command_deg.Data;
    A380SecComputer_B.SSM_anz = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_sidestick_roll_command_deg.SSM;
    A380SecComputer_B.Data_pf = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_sidestick_roll_command_deg.Data;
    A380SecComputer_B.Data_pl = A380SecComputer_U.in.bus_inputs.adr_2_bus.altitude_standard_ft.Data;
    A380SecComputer_B.SSM_d2 = A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_pedal_position_deg.SSM;
    A380SecComputer_B.Data_gb = A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_pedal_position_deg.Data;
    A380SecComputer_B.SSM_gov = A380SecComputer_U.in.bus_inputs.sec_y_bus.aileron_status_word.SSM;
    A380SecComputer_B.Data_hq = A380SecComputer_U.in.bus_inputs.sec_y_bus.aileron_status_word.Data;
    A380SecComputer_B.SSM_nb = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_ai = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_aileron_1_position_deg.Data;
    A380SecComputer_B.SSM_pe3 = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_gfr = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_jj = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_aileron_1_position_deg.SSM;
    A380SecComputer_B.Data_czp = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_aileron_1_position_deg.Data;
    A380SecComputer_B.tracking_mode_on_override = A380SecComputer_U.in.sim_data.tracking_mode_on_override;
    A380SecComputer_B.SSM_jx = A380SecComputer_U.in.bus_inputs.adr_2_bus.altitude_corrected_ft.SSM;
    A380SecComputer_B.SSM_npl = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_aileron_2_position_deg.SSM;
    A380SecComputer_B.Data_fm = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_aileron_2_position_deg.Data;
    A380SecComputer_B.SSM_gf = A380SecComputer_U.in.bus_inputs.sec_y_bus.spoiler_status_word.SSM;
    A380SecComputer_B.Data_jsg = A380SecComputer_U.in.bus_inputs.sec_y_bus.spoiler_status_word.Data;
    A380SecComputer_B.SSM_gbi = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_spoiler_1_position_deg.SSM;
    A380SecComputer_B.Data_g1 = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_spoiler_1_position_deg.Data;
    A380SecComputer_B.SSM_fhm = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_spoiler_1_position_deg.SSM;
    A380SecComputer_B.Data_j4 = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_spoiler_1_position_deg.Data;
    A380SecComputer_B.SSM_ltj = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_spoiler_2_position_deg.SSM;
    A380SecComputer_B.Data_jyh = A380SecComputer_U.in.bus_inputs.sec_y_bus.left_spoiler_2_position_deg.Data;
    A380SecComputer_B.Data_e4 = A380SecComputer_U.in.bus_inputs.adr_2_bus.altitude_corrected_ft.Data;
    A380SecComputer_B.SSM_hn = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_spoiler_2_position_deg.SSM;
    A380SecComputer_B.Data_ghs = A380SecComputer_U.in.bus_inputs.sec_y_bus.right_spoiler_2_position_deg.Data;
    A380SecComputer_B.SSM_h3 = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_status_word.SSM;
    A380SecComputer_B.Data_bmk = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_status_word.Data;
    A380SecComputer_B.SSM_bfs = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_1_position_deg.SSM;
    A380SecComputer_B.Data_lzt = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_1_position_deg.Data;
    A380SecComputer_B.SSM_p0 = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_2_position_deg.SSM;
    A380SecComputer_B.Data_kn = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_2_position_deg.Data;
    A380SecComputer_B.SSM_fu = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_3_position_deg.SSM;
    A380SecComputer_B.Data_nab = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_3_position_deg.Data;
    A380SecComputer_B.SSM_hr = A380SecComputer_U.in.bus_inputs.adr_2_bus.mach.SSM;
    A380SecComputer_B.SSM_bi = A380SecComputer_U.in.bus_inputs.sec_y_bus.ths_position_deg.SSM;
    A380SecComputer_B.Data_lgf = A380SecComputer_U.in.bus_inputs.sec_y_bus.ths_position_deg.Data;
    A380SecComputer_B.SSM_bd = A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_status_word.SSM;
    A380SecComputer_B.Data_fpq = A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_status_word.Data;
    A380SecComputer_B.SSM_omt = A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_1_position_deg.SSM;
    A380SecComputer_B.Data_dt = A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_1_position_deg.Data;
    A380SecComputer_B.SSM_la = A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_2_position_deg.SSM;
    A380SecComputer_B.Data_b1 = A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_2_position_deg.Data;
    A380SecComputer_B.SSM_l1 = A380SecComputer_U.in.bus_inputs.sec_y_bus.fctl_law_status_word.SSM;
    A380SecComputer_B.Data_nmr = A380SecComputer_U.in.bus_inputs.sec_y_bus.fctl_law_status_word.Data;
    A380SecComputer_B.Data_ea = A380SecComputer_U.in.bus_inputs.adr_2_bus.mach.Data;
    A380SecComputer_B.SSM_dy = A380SecComputer_U.in.bus_inputs.sec_y_bus.misc_data_status_word.SSM;
    A380SecComputer_B.Data_nib = A380SecComputer_U.in.bus_inputs.sec_y_bus.misc_data_status_word.Data;
    A380SecComputer_B.SSM_ie = A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_computed_kn.SSM;
    A380SecComputer_B.Data_i2t = A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_computed_kn.Data;
    A380SecComputer_B.SSM_kf = A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_true_kn.SSM;
    A380SecComputer_B.Data_ng = A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_true_kn.Data;
    A380SecComputer_B.SSM_p5l = A380SecComputer_U.in.bus_inputs.adr_2_bus.vertical_speed_ft_min.SSM;
    A380SecComputer_B.Data_h31 = A380SecComputer_U.in.bus_inputs.adr_2_bus.vertical_speed_ft_min.Data;
    A380SecComputer_B.tailstrike_protection_on = A380SecComputer_U.in.sim_data.tailstrike_protection_on;
    A380SecComputer_B.SSM_g3 = A380SecComputer_U.in.bus_inputs.adr_2_bus.aoa_corrected_deg.SSM;
    A380SecComputer_B.Data_ew = A380SecComputer_U.in.bus_inputs.adr_2_bus.aoa_corrected_deg.Data;
    A380SecComputer_B.SSM_b3 = A380SecComputer_U.in.bus_inputs.adr_2_bus.corrected_average_static_pressure.SSM;
    A380SecComputer_B.Data_j1s = A380SecComputer_U.in.bus_inputs.adr_2_bus.corrected_average_static_pressure.Data;
    A380SecComputer_B.SSM_dxv = A380SecComputer_U.in.bus_inputs.ir_1_bus.discrete_word_1.SSM;
    A380SecComputer_B.Data_j5 = A380SecComputer_U.in.bus_inputs.ir_1_bus.discrete_word_1.Data;
    A380SecComputer_B.SSM_mxz = A380SecComputer_U.in.bus_inputs.ir_1_bus.latitude_deg.SSM;
    A380SecComputer_B.Data_cw = A380SecComputer_U.in.bus_inputs.ir_1_bus.latitude_deg.Data;
    A380SecComputer_B.SSM_kk4 = A380SecComputer_U.in.bus_inputs.ir_1_bus.longitude_deg.SSM;
    A380SecComputer_B.Data_gqa = A380SecComputer_U.in.bus_inputs.ir_1_bus.longitude_deg.Data;
    A380SecComputer_B.computer_running = A380SecComputer_U.in.sim_data.computer_running;
    A380SecComputer_B.SSM_cy = A380SecComputer_U.in.bus_inputs.ir_1_bus.ground_speed_kn.SSM;
    A380SecComputer_B.Data_hz = A380SecComputer_U.in.bus_inputs.ir_1_bus.ground_speed_kn.Data;
    A380SecComputer_B.SSM_ju = A380SecComputer_U.in.bus_inputs.ir_1_bus.track_angle_true_deg.SSM;
    A380SecComputer_B.Data_fri = A380SecComputer_U.in.bus_inputs.ir_1_bus.track_angle_true_deg.Data;
    A380SecComputer_B.SSM_ey = A380SecComputer_U.in.bus_inputs.ir_1_bus.heading_true_deg.SSM;
    A380SecComputer_B.Data_cm = A380SecComputer_U.in.bus_inputs.ir_1_bus.heading_true_deg.Data;
    A380SecComputer_B.SSM_jr = A380SecComputer_U.in.bus_inputs.ir_1_bus.wind_speed_kn.SSM;
    A380SecComputer_B.Data_czj = A380SecComputer_U.in.bus_inputs.ir_1_bus.wind_speed_kn.Data;
    A380SecComputer_B.SSM_hs = A380SecComputer_U.in.bus_inputs.ir_1_bus.wind_direction_true_deg.SSM;
    A380SecComputer_B.Data_mb = A380SecComputer_U.in.bus_inputs.ir_1_bus.wind_direction_true_deg.Data;
    A380SecComputer_B.sec_overhead_button_pressed = A380SecComputer_U.in.discrete_inputs.sec_overhead_button_pressed;
    A380SecComputer_B.SSM_mx3 = A380SecComputer_U.in.bus_inputs.ir_1_bus.track_angle_magnetic_deg.SSM;
    A380SecComputer_B.Data_gk4 = A380SecComputer_U.in.bus_inputs.ir_1_bus.track_angle_magnetic_deg.Data;
    A380SecComputer_B.SSM_er = A380SecComputer_U.in.bus_inputs.ir_1_bus.heading_magnetic_deg.SSM;
    A380SecComputer_B.Data_gbt = A380SecComputer_U.in.bus_inputs.ir_1_bus.heading_magnetic_deg.Data;
    A380SecComputer_B.SSM_hm = A380SecComputer_U.in.bus_inputs.ir_1_bus.drift_angle_deg.SSM;
    A380SecComputer_B.Data_p0 = A380SecComputer_U.in.bus_inputs.ir_1_bus.drift_angle_deg.Data;
    A380SecComputer_B.SSM_dm = A380SecComputer_U.in.bus_inputs.ir_1_bus.flight_path_angle_deg.SSM;
    A380SecComputer_B.Data_dn = A380SecComputer_U.in.bus_inputs.ir_1_bus.flight_path_angle_deg.Data;
    A380SecComputer_B.SSM_fk = A380SecComputer_U.in.bus_inputs.ir_1_bus.flight_path_accel_g.SSM;
    A380SecComputer_B.Data_iyw = A380SecComputer_U.in.bus_inputs.ir_1_bus.flight_path_accel_g.Data;
    rtb_OR1 = ((A380SecComputer_U.in.bus_inputs.adr_1_bus.mach.SSM == static_cast<uint32_T>(SignStatusMatrix::
      FailureWarning)) || (A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_computed_kn.SSM == static_cast<uint32_T>
                (SignStatusMatrix::FailureWarning)) || (A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_true_kn.SSM ==
                static_cast<uint32_T>(SignStatusMatrix::FailureWarning)) ||
               (A380SecComputer_U.in.bus_inputs.adr_1_bus.aoa_corrected_deg.SSM == static_cast<uint32_T>
                (SignStatusMatrix::FailureWarning)) || A380SecComputer_P.Constant1_Value_b ||
               A380SecComputer_P.Constant1_Value_b);
    rtb_OR3 = ((A380SecComputer_U.in.bus_inputs.adr_2_bus.mach.SSM == static_cast<uint32_T>(SignStatusMatrix::
      FailureWarning)) || (A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_computed_kn.SSM == static_cast<uint32_T>
                (SignStatusMatrix::FailureWarning)) || (A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_true_kn.SSM ==
                static_cast<uint32_T>(SignStatusMatrix::FailureWarning)) ||
               (A380SecComputer_U.in.bus_inputs.adr_2_bus.aoa_corrected_deg.SSM == static_cast<uint32_T>
                (SignStatusMatrix::FailureWarning)) || A380SecComputer_P.Constant1_Value_b ||
               A380SecComputer_P.Constant1_Value_b);
    rtb_AND2_c = !rtb_OR3;
    if ((!rtb_OR1) && rtb_AND2_c) {
      rtb_V_ias = (A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_computed_kn.Data +
                   A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_computed_kn.Data) / 2.0F;
      rtb_V_tas = (A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_true_kn.Data +
                   A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_true_kn.Data) / 2.0F;
      rtb_mach_h = (A380SecComputer_U.in.bus_inputs.adr_1_bus.mach.Data +
                    A380SecComputer_U.in.bus_inputs.adr_2_bus.mach.Data) / 2.0F;
      rtb_alpha = (A380SecComputer_U.in.bus_inputs.adr_1_bus.aoa_corrected_deg.Data +
                   A380SecComputer_U.in.bus_inputs.adr_2_bus.aoa_corrected_deg.Data) / 2.0F;
    } else if ((!rtb_OR1) && rtb_OR3) {
      rtb_V_ias = A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_computed_kn.Data;
      rtb_V_tas = A380SecComputer_U.in.bus_inputs.adr_1_bus.airspeed_true_kn.Data;
      rtb_mach_h = A380SecComputer_U.in.bus_inputs.adr_1_bus.mach.Data;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.adr_1_bus.aoa_corrected_deg.Data;
    } else if (rtb_OR1 && rtb_AND2_c) {
      rtb_V_ias = A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_computed_kn.Data;
      rtb_V_tas = A380SecComputer_U.in.bus_inputs.adr_2_bus.airspeed_true_kn.Data;
      rtb_mach_h = A380SecComputer_U.in.bus_inputs.adr_2_bus.mach.Data;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.adr_2_bus.aoa_corrected_deg.Data;
    } else {
      rtb_V_ias = 0.0F;
      rtb_V_tas = 0.0F;
      rtb_mach_h = 0.0F;
      rtb_alpha = 0.0F;
    }

    if ((!A380SecComputer_DWork.pY_not_empty) || (!A380SecComputer_DWork.pU_not_empty)) {
      A380SecComputer_DWork.pU = rtb_alpha;
      A380SecComputer_DWork.pU_not_empty = true;
      A380SecComputer_DWork.pY = rtb_alpha;
      A380SecComputer_DWork.pY_not_empty = true;
    }

    rtb_Switch8_o = A380SecComputer_U.in.time.dt * A380SecComputer_P.LagFilter_C1;
    ca = rtb_Switch8_o / (rtb_Switch8_o + 2.0);
    A380SecComputer_DWork.pY = (2.0 - rtb_Switch8_o) / (rtb_Switch8_o + 2.0) * A380SecComputer_DWork.pY + (rtb_alpha *
      ca + A380SecComputer_DWork.pU * ca);
    A380SecComputer_DWork.pU = rtb_alpha;
    rtb_y_ck = A380SecComputer_P.Constant1_Value_b;
    rtb_Compare_l = A380SecComputer_P.Constant1_Value_b;
    rtb_OR = ((A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_angle_deg.SSM != static_cast<uint32_T>(SignStatusMatrix::
                NormalOperation)) || (A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_angle_deg.SSM !=
               static_cast<uint32_T>(SignStatusMatrix::NormalOperation)) ||
              (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.SSM != static_cast<uint32_T>
               (SignStatusMatrix::NormalOperation)) || (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_long_accel_g.SSM
               != static_cast<uint32_T>(SignStatusMatrix::NormalOperation)) ||
              (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_lat_accel_g.SSM != static_cast<uint32_T>(SignStatusMatrix::
                NormalOperation)) || (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_normal_accel_g.SSM !=
               static_cast<uint32_T>(SignStatusMatrix::NormalOperation)) ||
              (A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.SSM != static_cast<uint32_T>
               (SignStatusMatrix::NormalOperation)) || (A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_att_rate_deg_s.SSM
               != static_cast<uint32_T>(SignStatusMatrix::NormalOperation)) || A380SecComputer_P.Constant_Value_ad);
    rtb_OR6 = ((A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_angle_deg.SSM != static_cast<uint32_T>(SignStatusMatrix::
      NormalOperation)) || (A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_angle_deg.SSM != static_cast<uint32_T>
                (SignStatusMatrix::NormalOperation)) ||
               (A380SecComputer_U.in.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.SSM != static_cast<uint32_T>
                (SignStatusMatrix::NormalOperation)) || (A380SecComputer_U.in.bus_inputs.ir_2_bus.body_long_accel_g.SSM
                != static_cast<uint32_T>(SignStatusMatrix::NormalOperation)) ||
               (A380SecComputer_U.in.bus_inputs.ir_2_bus.body_lat_accel_g.SSM != static_cast<uint32_T>(SignStatusMatrix::
      NormalOperation)) || (A380SecComputer_U.in.bus_inputs.ir_2_bus.body_normal_accel_g.SSM != static_cast<uint32_T>
                (SignStatusMatrix::NormalOperation)) ||
               (A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.SSM != static_cast<uint32_T>
                (SignStatusMatrix::NormalOperation)) ||
               (A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_att_rate_deg_s.SSM != static_cast<uint32_T>
                (SignStatusMatrix::NormalOperation)) || A380SecComputer_P.Constant_Value_ad);
    rtb_AND2_c = !rtb_OR;
    rtb_logic_is_green_hydraulic_power_avail = !rtb_OR6;
    if (rtb_AND2_c && rtb_logic_is_green_hydraulic_power_avail) {
      rtb_theta = (A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_angle_deg.Data +
                   A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_angle_deg.Data) / 2.0F;
      rtb_phi = (A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_angle_deg.Data +
                 A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_angle_deg.Data) / 2.0F;
      rtb_q = (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_pitch_rate_deg_s.Data +
               A380SecComputer_U.in.bus_inputs.ir_2_bus.body_pitch_rate_deg_s.Data) / 2.0F;
      rtb_r = (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.Data +
               A380SecComputer_U.in.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.Data) / 2.0F;
      rtb_n_x = (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_long_accel_g.Data +
                 A380SecComputer_U.in.bus_inputs.ir_2_bus.body_long_accel_g.Data) / 2.0F;
      rtb_n_y = (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_lat_accel_g.Data +
                 A380SecComputer_U.in.bus_inputs.ir_2_bus.body_lat_accel_g.Data) / 2.0F;
      rtb_n_z = (A380SecComputer_U.in.bus_inputs.ir_1_bus.body_normal_accel_g.Data +
                 A380SecComputer_U.in.bus_inputs.ir_2_bus.body_normal_accel_g.Data) / 2.0F;
      rtb_theta_dot = (A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.Data +
                       A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.Data) / 2.0F;
      rtb_phi_dot = (A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_att_rate_deg_s.Data +
                     A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_att_rate_deg_s.Data) / 2.0F;
    } else if (rtb_AND2_c && rtb_OR6) {
      rtb_theta = A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_angle_deg.Data;
      rtb_phi = A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_angle_deg.Data;
      rtb_q = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_pitch_rate_deg_s.Data;
      rtb_r = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.Data;
      rtb_n_x = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_long_accel_g.Data;
      rtb_n_y = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_lat_accel_g.Data;
      rtb_n_z = A380SecComputer_U.in.bus_inputs.ir_1_bus.body_normal_accel_g.Data;
      rtb_theta_dot = A380SecComputer_U.in.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.Data;
      rtb_phi_dot = A380SecComputer_U.in.bus_inputs.ir_1_bus.roll_att_rate_deg_s.Data;
    } else if (rtb_OR && rtb_logic_is_green_hydraulic_power_avail) {
      rtb_theta = A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_angle_deg.Data;
      rtb_phi = A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_angle_deg.Data;
      rtb_q = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_pitch_rate_deg_s.Data;
      rtb_r = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.Data;
      rtb_n_x = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_long_accel_g.Data;
      rtb_n_y = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_lat_accel_g.Data;
      rtb_n_z = A380SecComputer_U.in.bus_inputs.ir_2_bus.body_normal_accel_g.Data;
      rtb_theta_dot = A380SecComputer_U.in.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.Data;
      rtb_phi_dot = A380SecComputer_U.in.bus_inputs.ir_2_bus.roll_att_rate_deg_s.Data;
    } else {
      rtb_theta = 0.0F;
      rtb_phi = 0.0F;
      rtb_q = 0.0F;
      rtb_r = 0.0F;
      rtb_n_x = 0.0F;
      rtb_n_y = 0.0F;
      rtb_n_z = 0.0F;
      rtb_theta_dot = 0.0F;
      rtb_phi_dot = 0.0F;
    }

    rtb_NOT_k = (A380SecComputer_U.in.sim_data.slew_on || A380SecComputer_U.in.sim_data.pause_on ||
                 A380SecComputer_U.in.sim_data.tracking_mode_on_override);
    A380SecComputer_B.logic.tracking_mode_on = rtb_NOT_k;
    A380SecComputer_MATLABFunction_cj(!A380SecComputer_U.in.discrete_inputs.yellow_low_pressure,
      A380SecComputer_U.in.time.dt, A380SecComputer_P.ConfirmNode_isRisingEdge, A380SecComputer_P.ConfirmNode_timeDelay,
      &rtb_NOT_k, &A380SecComputer_DWork.sf_MATLABFunction_cj);
    A380SecComputer_MATLABFunction_cj(!A380SecComputer_U.in.discrete_inputs.green_low_pressure,
      A380SecComputer_U.in.time.dt, A380SecComputer_P.ConfirmNode2_isRisingEdge,
      A380SecComputer_P.ConfirmNode2_timeDelay, &rtb_y_ck, &A380SecComputer_DWork.sf_MATLABFunction_gf);
    A380SecComputer_B.logic.is_yellow_hydraulic_power_avail = rtb_NOT_k;
    rtb_logic_is_green_hydraulic_power_avail = rtb_y_ck;
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.prim_1_bus.fctl_law_status_word,
      A380SecComputer_P.BitfromLabel6_bit, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.prim_1_bus.fctl_law_status_word, &rtb_y_ck);
    rtb_AND = ((rtb_y_oq != 0U) && rtb_y_ck);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.prim_2_bus.fctl_law_status_word,
      A380SecComputer_P.BitfromLabel7_bit, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.prim_2_bus.fctl_law_status_word, &rtb_Compare_l);
    rtb_AND1 = ((rtb_y_oq != 0U) && rtb_Compare_l);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.prim_3_bus.fctl_law_status_word,
      A380SecComputer_P.BitfromLabel1_bit, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.prim_3_bus.fctl_law_status_word, &rtb_y_ck);
    if (rtb_AND) {
      rtb_DataTypeConversion_i = 1;
    } else if (rtb_AND1) {
      rtb_DataTypeConversion_i = 2;
    } else if ((rtb_y_oq != 0U) && rtb_y_ck) {
      rtb_DataTypeConversion_i = 3;
    } else {
      rtb_DataTypeConversion_i = 0;
    }

    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.prim_1_bus.aileron_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_1_bus.aileron_status_word.Data;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.prim_2_bus.aileron_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_2_bus.aileron_status_word.Data;
    } else {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.prim_3_bus.aileron_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_3_bus.aileron_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel1_bit_g, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      &rtb_Compare_l);
    rtb_AND = ((rtb_y_cq != 0U) && rtb_Compare_l);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel2_bit, &rtb_y_cq);
    rtb_AND1_p = (rtb_Compare_l && (rtb_y_cq != 0U));
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_2_bus.aileron_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_2_bus.aileron_status_word.Data;
    } else {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_1_bus.aileron_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.aileron_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel3_bit, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    rtb_AND1 = ((rtb_y_cq != 0U) && rtb_y_ck);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel4_bit, &rtb_y_cq);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      leftAileron1Avail = rtb_logic_is_green_hydraulic_power_avail;
      rightAileron1Avail = rtb_logic_is_green_hydraulic_power_avail;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      leftAileron1Avail = false;
      rightAileron1Avail = false;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
      leftAileron1Avail = rtb_NOT_k;
      rightAileron1Avail = rtb_NOT_k;
    } else {
      leftAileron1Avail = false;
      rightAileron1Avail = false;
    }

    A380SecComputer_B.logic.right_aileron_1_engaged = (rightAileron1Avail && ((!rtb_AND1_p) && ((!rtb_y_ck) || (rtb_y_cq
      == 0U))));
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel5_bit, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    rtb_AND1_p = ((rtb_y_cq != 0U) && rtb_y_ck);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel6_bit_d, &rtb_y_cq);
    rtb_AND5 = (rtb_y_ck && (rtb_y_cq != 0U));
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_3_bus.aileron_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_3_bus.aileron_status_word.Data;
    } else {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_1_bus.aileron_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.aileron_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel7_bit_j, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      &rtb_Compare_l);
    rtb_AND6 = ((rtb_y_cq != 0U) && rtb_Compare_l);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel8_bit, &rtb_y_cq);
    rtb_AND7 = (rtb_Compare_l && (rtb_y_cq != 0U));
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.sec_y_bus.aileron_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.sec_y_bus.aileron_status_word.Data;
    } else {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.sec_x_bus.aileron_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.sec_x_bus.aileron_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel9_bit, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    rtb_AND8 = ((rtb_y_cq != 0U) && rtb_y_ck);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel10_bit, &rtb_y_cq);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      leftAileron2Avail = true;
      rightAileron2Avail = true;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      leftAileron2Avail = true;
      rightAileron2Avail = true;
    } else {
      leftAileron2Avail = false;
      rightAileron2Avail = false;
    }

    A380SecComputer_B.logic.right_aileron_2_engaged = (rightAileron2Avail && ((!rtb_AND5) && (!rtb_AND7) && ((!rtb_y_ck)
      || (rtb_y_cq == 0U))));
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      leftSpoilerHydraulicModeAvail = rtb_NOT_k;
      rtb_AND5 = rtb_NOT_k;
      rtb_AND7 = false;
      rightSpoilerHydraulicModeAvail = false;
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.Data;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      leftSpoilerHydraulicModeAvail = rtb_logic_is_green_hydraulic_power_avail;
      rtb_AND5 = rtb_logic_is_green_hydraulic_power_avail;
      rtb_AND7 = rtb_NOT_k;
      rightSpoilerHydraulicModeAvail = rtb_NOT_k;
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.Data;
    } else {
      if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
        leftSpoilerHydraulicModeAvail = rtb_NOT_k;
        rtb_AND5 = rtb_NOT_k;
        rtb_AND7 = rtb_logic_is_green_hydraulic_power_avail;
        rightSpoilerHydraulicModeAvail = rtb_logic_is_green_hydraulic_power_avail;
      } else {
        leftSpoilerHydraulicModeAvail = false;
        rtb_AND5 = false;
        rtb_AND7 = false;
        rightSpoilerHydraulicModeAvail = false;
      }

      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel1_bit_j, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      &rtb_Compare_l);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.Data;
    } else {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel2_bit_i, &rtb_y_cf);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel3_bit_i, &rtb_y_c0);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      elevator1Avail = rtb_logic_is_green_hydraulic_power_avail;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      elevator1Avail = rtb_NOT_k;
    } else {
      elevator1Avail = (A380SecComputer_U.in.discrete_inputs.is_unit_3 && rtb_logic_is_green_hydraulic_power_avail);
    }

    if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_AND2_c = (rtb_y_cf != 0U);
    } else {
      rtb_AND2_c = (rtb_y_c0 != 0U);
    }

    A380SecComputer_B.logic.elevator_1_engaged = (elevator1Avail && (((rtb_y_cq == 0U) || (!rtb_Compare_l)) &&
      ((!rtb_AND2_c) || (!rtb_y_ck))));
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel4_bit_e, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    rtb_AND2_a = ((rtb_y_cq != 0U) && rtb_y_ck);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.Data;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.Data;
    } else {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel6_bit_l, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel7_bit_b, &rtb_y_cf);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      &rtb_Compare_l);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
      rtb_AND2_c = (rtb_y_cq != 0U);
    } else {
      rtb_AND2_c = (rtb_y_cf != 0U);
    }

    rtb_AND4_l = (rtb_AND2_c && rtb_Compare_l);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_status_word.Data;
    } else {
      rtb_y_oq = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_status_word.SSM;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel8_bit_d, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel9_bit_g, &rtb_y_cf);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_y_oq;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_alpha;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
      rtb_AND2_c = (rtb_y_cq != 0U);
    } else {
      rtb_AND2_c = (rtb_y_cf != 0U);
    }

    rtb_AND5_e = (rtb_AND2_c && rtb_y_ck);
    elevator2Avail = (A380SecComputer_U.in.discrete_inputs.is_unit_1 || (A380SecComputer_U.in.discrete_inputs.is_unit_2 ||
      (A380SecComputer_U.in.discrete_inputs.is_unit_3 && rtb_NOT_k)));
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel5_bit_e, &rtb_y_oq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    rtb_AND3_dt = ((rtb_y_oq != 0U) && rtb_y_ck);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_2_bus.elevator_status_word.Data;
    } else {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel10_bit_g, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel11_bit, &rtb_y_oq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      &rtb_Compare_l);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_AND2_c = (rtb_y_cq != 0U);
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.sec_x_bus.elevator_status_word.Data;
    } else {
      rtb_AND2_c = (rtb_y_oq != 0U);
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_status_word.Data;
    }

    rtb_AND6_e = (rtb_AND2_c && rtb_Compare_l);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel12_bit, &rtb_y_oq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel13_bit, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_AND2_c = (rtb_y_oq != 0U);
    } else {
      rtb_AND2_c = (rtb_y_cq != 0U);
    }

    rtb_AND7_j = (rtb_AND2_c && rtb_y_ck);
    elevator3Avail = (A380SecComputer_U.in.discrete_inputs.is_unit_1 || A380SecComputer_U.in.discrete_inputs.is_unit_2);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.Data;
    } else {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel1_bit_f, &rtb_y_oq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    rtb_AND_cw = ((rtb_y_oq != 0U) && rtb_y_ck);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_3_bus.elevator_status_word.Data;
    } else {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.elevator_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel2_bit_a, &rtb_y_oq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    rtb_y_ck = ((rtb_y_oq != 0U) && rtb_y_ck);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_status_word,
      A380SecComputer_P.BitfromLabel3_bit_j, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.sec_y_bus.elevator_status_word, &rtb_Compare_l);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      thsAvail = rtb_NOT_k;
      rtb_AND2_c = ((!rtb_AND_cw) && (!rtb_y_ck) && ((rtb_y_oq == 0U) || (!rtb_Compare_l)));
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word.Data;
    } else {
      thsAvail_tmp = !A380SecComputer_U.in.discrete_inputs.is_unit_2;
      thsAvail = (thsAvail_tmp && (A380SecComputer_U.in.discrete_inputs.is_unit_3 &&
        rtb_logic_is_green_hydraulic_power_avail));
      rtb_AND2_c = (thsAvail_tmp && (A380SecComputer_U.in.discrete_inputs.is_unit_3 && ((!rtb_AND_cw) && (!rtb_y_ck))));
      if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
        rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_status_word.SSM;
        rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_status_word.Data;
      } else {
        rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_status_word.SSM;
        rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_status_word.Data;
      }
    }

    A380SecComputer_B.logic.ths_engaged = (thsAvail && rtb_AND2_c);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel1_bit_o, &rtb_y_oq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi, &rtb_y_ck);
    rtb_AND_h = ((rtb_y_oq != 0U) && rtb_y_ck);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel2_bit_p, &rtb_y_cq);
    rtb_AND1_jx = (rtb_y_ck && (rtb_y_cq != 0U));
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_status_word.Data;
    } else {
      rtb_SSM_ov = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word.SSM;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word.Data;
    }

    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel10_bit_i, &rtb_y_oq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel8_bit_m, &rtb_y_cq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      &rtb_Compare_l);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
      rtb_AND2_c = (rtb_y_oq != 0U);
    } else {
      rtb_AND2_c = (rtb_y_cq != 0U);
    }

    rtb_AND7_g = (rtb_AND2_c && rtb_Compare_l);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel11_bit_l, &rtb_y_oq);
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.SSM = rtb_SSM_ov;
    rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi.Data = rtb_Data_ch;
    A380SecComputer_MATLABFunction_c(&rtb_BusConversion_InsertedFor_MATLABFunction_at_inport_0_BusCreator1_mi,
      A380SecComputer_P.BitfromLabel12_bit_b, &rtb_SSM_ov);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
      rtb_AND2_c = (rtb_y_oq != 0U);
    } else {
      rtb_AND2_c = (rtb_SSM_ov != 0U);
    }

    rtb_Compare_l = (rtb_Compare_l && rtb_AND2_c);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel9_bit_o, &rtb_SSM_ov);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel15_bit, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word, &rtb_y_ck);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
      rtb_AND2_c = (rtb_SSM_ov != 0U);
    } else {
      rtb_AND2_c = (rtb_y_oq != 0U);
    }

    rtb_AND9_e = (rtb_AND2_c && rtb_y_ck);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel13_bit_o, &rtb_SSM_ov);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel14_bit, &rtb_y_oq);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_AND_cw = rtb_NOT_k;
      thsAvail_tmp = true;
      rtb_y_ck = !rtb_AND_h;
      rtb_AND_h = !rtb_AND7_g;
      rudder1HydraulicModeHasPriority = (rtb_y_ck && rtb_AND_h);
      rtb_AND1_jx = (rtb_y_ck && (!rtb_AND1_jx) && rtb_AND_h && (!rtb_Compare_l) && (!rtb_AND9_e) && (!rtb_NOT_k));
    } else {
      if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
        rtb_AND_cw = rtb_logic_is_green_hydraulic_power_avail;
        thsAvail_tmp = true;
      } else if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
        rtb_AND_cw = rtb_NOT_k;
        thsAvail_tmp = true;
      } else {
        rtb_AND_cw = false;
        thsAvail_tmp = false;
      }

      if (A380SecComputer_U.in.discrete_inputs.is_unit_2 || A380SecComputer_U.in.discrete_inputs.is_unit_3) {
        rtb_NOT_k = !rtb_AND_h;
        rtb_AND9_e = !rtb_AND9_e;
        rudder1HydraulicModeHasPriority = (rtb_NOT_k && (!rtb_AND7_g) && rtb_AND9_e);
        if (A380SecComputer_U.in.discrete_inputs.is_unit_3) {
          rtb_AND2_c = (rtb_SSM_ov != 0U);
        } else {
          rtb_AND2_c = (rtb_y_oq != 0U);
        }

        rtb_AND1_jx = (rtb_NOT_k && (!rtb_AND1_jx) && (!rtb_AND7_g) && (!rtb_Compare_l) && rtb_AND9_e && ((!rtb_y_ck) ||
          (!rtb_AND2_c)) && (!rtb_AND_cw));
      } else {
        rudder1HydraulicModeHasPriority = false;
        rtb_AND1_jx = false;
      }
    }

    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel3_bit_o, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word, &rtb_y_ck);
    rtb_AND2_c = ((rtb_y_oq != 0U) && rtb_y_ck);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel4_bit_a, &rtb_y_oq);
    rtb_NOT_k = (rtb_y_ck && (rtb_y_oq != 0U));
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel5_bit_c, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_status_word, &rtb_Compare_l);
    rtb_AND9_e = ((rtb_y_oq != 0U) && rtb_Compare_l);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel6_bit_h, &rtb_y_oq);
    rtb_Compare_l = (rtb_Compare_l && (rtb_y_oq != 0U));
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel7_bit_i, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.sec_y_bus.rudder_status_word, &rtb_y_ck);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_AND_h = rtb_logic_is_green_hydraulic_power_avail;
      rtb_AND7_g = true;
      rtb_AND2_c = !rtb_AND2_c;
      rudder2ElectricModeHasPriority = !rtb_AND9_e;
      rtb_AND9_e = (rtb_AND2_c && rudder2ElectricModeHasPriority);
      rudder2ElectricModeHasPriority = (rtb_AND2_c && (!rtb_NOT_k) && rudder2ElectricModeHasPriority && (!rtb_Compare_l)
        && ((rtb_y_oq == 0U) || (!rtb_y_ck)) && (!rtb_logic_is_green_hydraulic_power_avail));
    } else {
      rtb_AND_h = false;
      rtb_AND7_g = false;
      rtb_AND9_e = false;
      rudder2ElectricModeHasPriority = false;
    }

    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word,
      A380SecComputer_P.BitfromLabel9_bit_m, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.sec_x_bus.rudder_status_word, &rtb_y_ck);
    rtb_NOT_k = !A380SecComputer_U.in.discrete_inputs.is_unit_2;
    rudderTrimAvail = (A380SecComputer_U.in.discrete_inputs.is_unit_1 || (rtb_NOT_k &&
      A380SecComputer_U.in.discrete_inputs.is_unit_3));
    A380SecComputer_B.logic.rudder_trim_engaged = (rudderTrimAvail && (A380SecComputer_U.in.discrete_inputs.is_unit_1 ||
      (rtb_NOT_k && (A380SecComputer_U.in.discrete_inputs.is_unit_3 && ((rtb_y_oq == 0U) || (!rtb_y_ck))))));
    if (rtb_DataTypeConversion_i == 0) {
      A380SecComputer_B.logic.active_pitch_law = a380_pitch_efcs_law::DirectLaw;
      A380SecComputer_B.logic.active_lateral_law = a380_lateral_efcs_law::DirectLaw;
    } else {
      A380SecComputer_B.logic.active_pitch_law = a380_pitch_efcs_law::None;
      A380SecComputer_B.logic.active_lateral_law = a380_lateral_efcs_law::None;
    }

    A380SecComputer_MATLABFunction_g(A380SecComputer_U.in.discrete_inputs.capt_priority_takeover_pressed,
      A380SecComputer_P.PulseNode_isRisingEdge, &rtb_NOT_k, &A380SecComputer_DWork.sf_MATLABFunction_g4);
    A380SecComputer_MATLABFunction_g(A380SecComputer_U.in.discrete_inputs.fo_priority_takeover_pressed,
      A380SecComputer_P.PulseNode1_isRisingEdge, &rtb_y_ck, &A380SecComputer_DWork.sf_MATLABFunction_nu);
    if (rtb_NOT_k) {
      A380SecComputer_DWork.pRightStickDisabled = true;
      A380SecComputer_DWork.pLeftStickDisabled = false;
    } else if (rtb_y_ck) {
      A380SecComputer_DWork.pLeftStickDisabled = true;
      A380SecComputer_DWork.pRightStickDisabled = false;
    }

    if (A380SecComputer_DWork.pRightStickDisabled &&
        ((!A380SecComputer_U.in.discrete_inputs.capt_priority_takeover_pressed) && (!A380SecComputer_DWork.Delay1_DSTATE)))
    {
      A380SecComputer_DWork.pRightStickDisabled = false;
    } else if (A380SecComputer_DWork.pLeftStickDisabled) {
      A380SecComputer_DWork.pLeftStickDisabled = (A380SecComputer_U.in.discrete_inputs.fo_priority_takeover_pressed ||
        A380SecComputer_DWork.Delay_DSTATE_c);
    }

    A380SecComputer_MATLABFunction_cj(A380SecComputer_DWork.pLeftStickDisabled &&
      (A380SecComputer_U.in.discrete_inputs.fo_priority_takeover_pressed || A380SecComputer_DWork.Delay_DSTATE_c),
      A380SecComputer_U.in.time.dt, A380SecComputer_P.ConfirmNode1_isRisingEdge,
      A380SecComputer_P.ConfirmNode1_timeDelay, &A380SecComputer_DWork.Delay_DSTATE_c,
      &A380SecComputer_DWork.sf_MATLABFunction_j2);
    A380SecComputer_MATLABFunction_cj(A380SecComputer_DWork.pRightStickDisabled &&
      (A380SecComputer_U.in.discrete_inputs.capt_priority_takeover_pressed || A380SecComputer_DWork.Delay1_DSTATE),
      A380SecComputer_U.in.time.dt, A380SecComputer_P.ConfirmNode_isRisingEdge_j,
      A380SecComputer_P.ConfirmNode_timeDelay_a, &A380SecComputer_DWork.Delay1_DSTATE,
      &A380SecComputer_DWork.sf_MATLABFunction_g2);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_flap_actual_position_word,
      A380SecComputer_P.BitfromLabel_bit, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.sfcc_1_bus.slat_flap_actual_position_word,
      &rtb_Compare_l);
    rtb_NOT_k = ((rtb_y_oq == 0U) && rtb_Compare_l);
    A380SecComputer_MATLABFunction_c(&A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_flap_actual_position_word,
      A380SecComputer_P.BitfromLabel1_bit_d, &rtb_y_oq);
    A380SecComputer_MATLABFunction(&A380SecComputer_U.in.bus_inputs.sfcc_2_bus.slat_flap_actual_position_word, &rtb_y_ck);
    A380SecComputer_B.logic.on_ground = false;
    A380SecComputer_B.logic.master_prim = rtb_DataTypeConversion_i;
    A380SecComputer_B.logic.elevator_1_avail = elevator1Avail;
    A380SecComputer_B.logic.elevator_2_avail = elevator2Avail;
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_AND2_c = ((!rtb_AND2_a) && (!rtb_AND4_l) && (!rtb_AND5_e));
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_AND2_c = ((!rtb_AND2_a) && (!rtb_AND4_l) && (!rtb_AND5_e));
    } else {
      rtb_AND2_c = (A380SecComputer_U.in.discrete_inputs.is_unit_3 && ((!rtb_AND2_a) && (!rtb_AND4_l)));
    }

    A380SecComputer_B.logic.elevator_2_engaged = (elevator2Avail && rtb_AND2_c);
    A380SecComputer_B.logic.elevator_3_avail = elevator3Avail;
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_AND2_c = ((!rtb_AND3_dt) && (!rtb_AND6_e) && (!rtb_AND7_j));
    } else {
      rtb_AND2_c = (A380SecComputer_U.in.discrete_inputs.is_unit_2 && ((!rtb_AND3_dt) && (!rtb_AND6_e) && (!rtb_AND7_j)));
    }

    A380SecComputer_B.logic.elevator_3_engaged = (elevator3Avail && rtb_AND2_c);
    A380SecComputer_B.logic.ths_avail = thsAvail;
    A380SecComputer_B.logic.left_aileron_1_avail = leftAileron1Avail;
    A380SecComputer_B.logic.left_aileron_1_engaged = (leftAileron1Avail && ((!rtb_AND) && (!rtb_AND1)));
    A380SecComputer_B.logic.left_aileron_2_avail = leftAileron2Avail;
    A380SecComputer_B.logic.left_aileron_2_engaged = (leftAileron2Avail && ((!rtb_AND1_p) && (!rtb_AND6) && (!rtb_AND8)));
    A380SecComputer_B.logic.right_aileron_1_avail = rightAileron1Avail;
    A380SecComputer_B.logic.right_aileron_2_avail = rightAileron2Avail;
    A380SecComputer_B.logic.left_spoiler_1_hydraulic_mode_avail = leftSpoilerHydraulicModeAvail;
    rtb_AND = (leftSpoilerHydraulicModeAvail && rtb_AND5);
    A380SecComputer_B.logic.left_spoiler_1_hydraulic_mode_engaged = rtb_AND;
    A380SecComputer_B.logic.right_spoiler_1_hydraulic_mode_avail = rtb_AND5;
    A380SecComputer_B.logic.right_spoiler_1_hydraulic_mode_engaged = rtb_AND;
    A380SecComputer_B.logic.left_spoiler_2_hydraulic_mode_avail = rtb_AND7;
    rtb_AND = (rtb_AND7 && rightSpoilerHydraulicModeAvail);
    A380SecComputer_B.logic.left_spoiler_2_hydraulic_mode_engaged = rtb_AND;
    A380SecComputer_B.logic.right_spoiler_2_hydraulic_mode_avail = rightSpoilerHydraulicModeAvail;
    A380SecComputer_B.logic.right_spoiler_2_hydraulic_mode_engaged = rtb_AND;
    A380SecComputer_B.logic.rudder_1_hydraulic_mode_avail = rtb_AND_cw;
    A380SecComputer_B.logic.rudder_1_electric_mode_avail = thsAvail_tmp;
    A380SecComputer_B.logic.rudder_1_hydraulic_mode_engaged = (rtb_AND_cw && rudder1HydraulicModeHasPriority);
    A380SecComputer_B.logic.rudder_1_electric_mode_engaged = (thsAvail_tmp && rtb_AND1_jx);
    A380SecComputer_B.logic.rudder_2_hydraulic_mode_avail = rtb_AND_h;
    A380SecComputer_B.logic.rudder_2_electric_mode_avail = rtb_AND7_g;
    A380SecComputer_B.logic.rudder_2_hydraulic_mode_engaged = (rtb_AND_h && rtb_AND9_e);
    A380SecComputer_B.logic.rudder_2_electric_mode_engaged = (rtb_AND7_g && rudder2ElectricModeHasPriority);
    A380SecComputer_B.logic.rudder_trim_avail = rudderTrimAvail;
    A380SecComputer_B.logic.aileron_droop_active = (rtb_NOT_k || ((rtb_y_oq == 0U) && rtb_y_ck));
    A380SecComputer_B.logic.is_green_hydraulic_power_avail = rtb_logic_is_green_hydraulic_power_avail;
    A380SecComputer_B.logic.left_sidestick_disabled = A380SecComputer_DWork.pLeftStickDisabled;
    A380SecComputer_B.logic.right_sidestick_disabled = A380SecComputer_DWork.pRightStickDisabled;
    A380SecComputer_B.logic.left_sidestick_priority_locked = A380SecComputer_DWork.Delay_DSTATE_c;
    A380SecComputer_B.logic.right_sidestick_priority_locked = A380SecComputer_DWork.Delay1_DSTATE;
    if (!A380SecComputer_DWork.pRightStickDisabled) {
      rtb_eta_trim_limit_up = A380SecComputer_U.in.analog_inputs.fo_pitch_stick_pos;
    } else {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant_Value_p;
    }

    if (A380SecComputer_DWork.pLeftStickDisabled) {
      rtb_Switch8_o = A380SecComputer_P.Constant_Value_p;
    } else {
      rtb_Switch8_o = A380SecComputer_U.in.analog_inputs.capt_pitch_stick_pos;
    }

    rtb_xi_spoiler_deg_b = rtb_eta_trim_limit_up + rtb_Switch8_o;
    if (rtb_xi_spoiler_deg_b > A380SecComputer_P.Saturation_UpperSat) {
      A380SecComputer_B.logic.total_sidestick_pitch_command = A380SecComputer_P.Saturation_UpperSat;
    } else if (rtb_xi_spoiler_deg_b < A380SecComputer_P.Saturation_LowerSat) {
      A380SecComputer_B.logic.total_sidestick_pitch_command = A380SecComputer_P.Saturation_LowerSat;
    } else {
      A380SecComputer_B.logic.total_sidestick_pitch_command = rtb_xi_spoiler_deg_b;
    }

    if (!A380SecComputer_DWork.pRightStickDisabled) {
      rtb_eta_trim_limit_up = A380SecComputer_U.in.analog_inputs.fo_roll_stick_pos;
    } else {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant1_Value_p;
    }

    if (A380SecComputer_DWork.pLeftStickDisabled) {
      rtb_Switch8_o = A380SecComputer_P.Constant1_Value_p;
    } else {
      rtb_Switch8_o = A380SecComputer_U.in.analog_inputs.capt_roll_stick_pos;
    }

    rtb_xi_spoiler_deg_b = rtb_eta_trim_limit_up + rtb_Switch8_o;
    if (rtb_xi_spoiler_deg_b > A380SecComputer_P.Saturation1_UpperSat) {
      A380SecComputer_B.logic.total_sidestick_roll_command = A380SecComputer_P.Saturation1_UpperSat;
    } else if (rtb_xi_spoiler_deg_b < A380SecComputer_P.Saturation1_LowerSat) {
      A380SecComputer_B.logic.total_sidestick_roll_command = A380SecComputer_P.Saturation1_LowerSat;
    } else {
      A380SecComputer_B.logic.total_sidestick_roll_command = rtb_xi_spoiler_deg_b;
    }

    A380SecComputer_B.logic.phased_lift_dumping_active = false;
    A380SecComputer_B.logic.double_adr_failure = (rtb_OR1 && rtb_OR3);
    A380SecComputer_B.logic.cas_or_mach_disagree = A380SecComputer_P.Constant1_Value_b;
    A380SecComputer_B.logic.alpha_disagree = A380SecComputer_P.Constant1_Value_b;
    A380SecComputer_B.logic.double_ir_failure = (rtb_OR && rtb_OR6);
    A380SecComputer_B.logic.ir_failure_not_self_detected = A380SecComputer_P.Constant_Value_ad;
    A380SecComputer_B.logic.adr_computation_data.V_ias_kn = rtb_V_ias;
    A380SecComputer_B.logic.adr_computation_data.V_tas_kn = rtb_V_tas;
    A380SecComputer_B.logic.adr_computation_data.mach = rtb_mach_h;
    A380SecComputer_B.logic.adr_computation_data.alpha_deg = A380SecComputer_DWork.pY;
    A380SecComputer_B.logic.ir_computation_data.theta_deg = rtb_theta;
    A380SecComputer_B.logic.ir_computation_data.phi_deg = rtb_phi;
    A380SecComputer_B.logic.ir_computation_data.q_deg_s = rtb_q;
    A380SecComputer_B.logic.ir_computation_data.r_deg_s = rtb_r;
    A380SecComputer_B.logic.ir_computation_data.n_x_g = rtb_n_x;
    A380SecComputer_B.logic.ir_computation_data.n_y_g = rtb_n_y;
    A380SecComputer_B.logic.ir_computation_data.n_z_g = rtb_n_z;
    A380SecComputer_B.logic.ir_computation_data.theta_dot_deg_s = rtb_theta_dot;
    A380SecComputer_B.logic.ir_computation_data.phi_dot_deg_s = rtb_phi_dot;
    LawMDLOBJ1.step(&A380SecComputer_U.in.time.dt, &A380SecComputer_B.logic.total_sidestick_roll_command,
                    &A380SecComputer_U.in.analog_inputs.rudder_pedal_pos_deg, &rtb_xi_inboard_deg, &rtb_xi_midboard_deg,
                    &rtb_xi_outboard_deg, &rtb_xi_spoiler_deg, &rtb_zeta_upper_deg, &rtb_zeta_lower_deg);
    if (static_cast<int32_T>(A380SecComputer_B.logic.active_lateral_law) == 1) {
      rtb_Switch1_k = rtb_xi_inboard_deg;
    } else {
      rtb_Switch1_k = A380SecComputer_P.Constant_Value_c;
    }

    if (A380SecComputer_B.logic.aileron_droop_active) {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant2_Value;
    } else {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant1_Value_f;
    }

    A380SecComputer_RateLimiter(rtb_eta_trim_limit_up, A380SecComputer_P.RateLimiterVariableTs2_up,
      A380SecComputer_P.RateLimiterVariableTs2_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_P.RateLimiterVariableTs2_InitialCondition, &rtb_Switch8_o, &A380SecComputer_DWork.sf_RateLimiter);
    rtb_NOT_k = (A380SecComputer_B.logic.master_prim == A380SecComputer_P.CompareToConstant_const);
    rtb_xi_spoiler_deg_b = A380SecComputer_P.Gain_Gain * rtb_Switch1_k + rtb_Switch8_o;
    if (rtb_xi_spoiler_deg_b > A380SecComputer_P.Saturation2_UpperSat) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Saturation2_UpperSat;
    } else if (rtb_xi_spoiler_deg_b < A380SecComputer_P.Saturation2_LowerSat) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Saturation2_LowerSat;
    }

    A380SecComputer_RateLimiter_e(rtb_xi_spoiler_deg_b, A380SecComputer_P.RateLimiterGenericVariableTs_up,
      A380SecComputer_P.RateLimiterGenericVariableTs_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.left_aileron_1_pos_deg, (!A380SecComputer_B.logic.left_aileron_1_engaged) ||
      (!rtb_NOT_k), &ca, &A380SecComputer_DWork.sf_RateLimiter_e);
    if (A380SecComputer_B.logic.master_prim == A380SecComputer_P.CompareToConstant_const_f) {
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_inboard_aileron_command_deg.Data;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_inboard_aileron_command_deg.Data;
      rtb_V_ias = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_midboard_aileron_command_deg.Data;
      rtb_V_tas = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_midboard_aileron_command_deg.Data;
      rtb_mach_h = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_1_command_deg.Data;
      rtb_theta = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_1_command_deg.Data;
      rtb_phi = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_2_command_deg.Data;
      rtb_q = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_2_command_deg.Data;
      rtb_r = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_3_command_deg.Data;
      rtb_n_x = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_3_command_deg.Data;
      rtb_n_y = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_7_command_deg.Data;
      rtb_n_z = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_7_command_deg.Data;
      rtb_theta_dot = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_spoiler_8_command_deg.Data;
      rtb_phi_dot = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_spoiler_8_command_deg.Data;
      rtb_Data_awh = A380SecComputer_U.in.bus_inputs.prim_1_bus.upper_rudder_command_deg.Data;
      rtb_Data_nu = A380SecComputer_U.in.bus_inputs.prim_1_bus.lower_rudder_command_deg.Data;
      A380SecComputer_Y.out.bus_outputs.aileron_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_1_bus.ths_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.spoiler_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_status_word.Data;
      A380SecComputer_Y.out.bus_outputs.elevator_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_1_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.rudder_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_1_bus.rudder_2_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.fctl_law_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_1_bus.fctl_law_status_word.Data;
    } else if (A380SecComputer_B.logic.master_prim == A380SecComputer_P.CompareToConstant1_const_p) {
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_inboard_aileron_command_deg.Data;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_inboard_aileron_command_deg.Data;
      rtb_V_ias = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_midboard_aileron_command_deg.Data;
      rtb_V_tas = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_midboard_aileron_command_deg.Data;
      rtb_mach_h = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_1_command_deg.Data;
      rtb_theta = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_1_command_deg.Data;
      rtb_phi = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_2_command_deg.Data;
      rtb_q = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_2_command_deg.Data;
      rtb_r = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_3_command_deg.Data;
      rtb_n_x = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_3_command_deg.Data;
      rtb_n_y = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_7_command_deg.Data;
      rtb_n_z = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_7_command_deg.Data;
      rtb_theta_dot = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_spoiler_8_command_deg.Data;
      rtb_phi_dot = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_spoiler_8_command_deg.Data;
      rtb_Data_awh = A380SecComputer_U.in.bus_inputs.prim_2_bus.upper_rudder_command_deg.Data;
      rtb_Data_nu = A380SecComputer_U.in.bus_inputs.prim_2_bus.lower_rudder_command_deg.Data;
      A380SecComputer_Y.out.bus_outputs.aileron_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_2_bus.ths_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.spoiler_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_status_word.Data;
      A380SecComputer_Y.out.bus_outputs.elevator_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_1_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.rudder_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_2_bus.rudder_2_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.fctl_law_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_2_bus.fctl_law_status_word.Data;
    } else {
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_inboard_aileron_command_deg.Data;
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_inboard_aileron_command_deg.Data;
      rtb_V_ias = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_midboard_aileron_command_deg.Data;
      rtb_V_tas = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_midboard_aileron_command_deg.Data;
      rtb_mach_h = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_1_command_deg.Data;
      rtb_theta = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_1_command_deg.Data;
      rtb_phi = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_2_command_deg.Data;
      rtb_q = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_2_command_deg.Data;
      rtb_r = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_3_command_deg.Data;
      rtb_n_x = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_3_command_deg.Data;
      rtb_n_y = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_7_command_deg.Data;
      rtb_n_z = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_7_command_deg.Data;
      rtb_theta_dot = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_spoiler_8_command_deg.Data;
      rtb_phi_dot = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_spoiler_8_command_deg.Data;
      rtb_Data_awh = A380SecComputer_U.in.bus_inputs.prim_3_bus.upper_rudder_command_deg.Data;
      rtb_Data_nu = A380SecComputer_U.in.bus_inputs.prim_3_bus.lower_rudder_command_deg.Data;
      A380SecComputer_Y.out.bus_outputs.aileron_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_3_bus.ths_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.spoiler_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_status_word.Data;
      A380SecComputer_Y.out.bus_outputs.elevator_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_1_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.rudder_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_3_bus.rudder_2_position_deg.Data;
      A380SecComputer_Y.out.bus_outputs.fctl_law_status_word.Data =
        A380SecComputer_U.in.bus_inputs.prim_3_bus.fctl_law_status_word.Data;
    }

    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_leftAileron1Command = rtb_Data_ch;
      rtb_rightAileron1Command = rtb_alpha;
      rtb_Data_ch = rtb_V_ias;
      rtb_alpha = rtb_V_tas;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_leftAileron1Command = 0.0F;
      rtb_rightAileron1Command = 0.0F;
    } else {
      rtb_leftAileron1Command = rtb_V_ias;
      rtb_rightAileron1Command = rtb_V_tas;
      rtb_Data_ch = 0.0F;
      rtb_alpha = 0.0F;
    }

    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.left_aileron_1_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.left_aileron_1_command_deg = rtb_leftAileron1Command;
    }

    rtb_xi_spoiler_deg_b = rtb_Switch8_o + rtb_Switch1_k;
    if (rtb_xi_spoiler_deg_b > A380SecComputer_P.Saturation1_UpperSat_o) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Saturation1_UpperSat_o;
    } else if (rtb_xi_spoiler_deg_b < A380SecComputer_P.Saturation1_LowerSat_n) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Saturation1_LowerSat_n;
    }

    A380SecComputer_RateLimiter_e(rtb_xi_spoiler_deg_b, A380SecComputer_P.RateLimiterGenericVariableTs1_up,
      A380SecComputer_P.RateLimiterGenericVariableTs1_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.right_aileron_1_pos_deg, (!A380SecComputer_B.logic.right_aileron_1_engaged) ||
      (!rtb_NOT_k), &ca, &A380SecComputer_DWork.sf_RateLimiter_o);
    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.right_aileron_1_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.right_aileron_1_command_deg = rtb_rightAileron1Command;
    }

    if (static_cast<int32_T>(A380SecComputer_B.logic.active_lateral_law) == 1) {
      rtb_eta_trim_limit_up = rtb_xi_midboard_deg;
    } else {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant_Value_c;
    }

    rtb_xi_spoiler_deg_b = A380SecComputer_P.Gain3_Gain * rtb_eta_trim_limit_up + rtb_Switch8_o;
    if (rtb_xi_spoiler_deg_b > A380SecComputer_P.Saturation3_UpperSat) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Saturation3_UpperSat;
    } else if (rtb_xi_spoiler_deg_b < A380SecComputer_P.Saturation3_LowerSat) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Saturation3_LowerSat;
    }

    A380SecComputer_RateLimiter_e(rtb_xi_spoiler_deg_b, A380SecComputer_P.RateLimiterGenericVariableTs2_up,
      A380SecComputer_P.RateLimiterGenericVariableTs2_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.left_aileron_2_pos_deg, (!A380SecComputer_B.logic.left_aileron_2_engaged) ||
      (!rtb_NOT_k), &ca, &A380SecComputer_DWork.sf_RateLimiter_a);
    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.left_aileron_2_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.left_aileron_2_command_deg = rtb_Data_ch;
    }

    rtb_xi_spoiler_deg_b = rtb_Switch8_o + rtb_eta_trim_limit_up;
    if (rtb_xi_spoiler_deg_b > A380SecComputer_P.Saturation4_UpperSat) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Saturation4_UpperSat;
    } else if (rtb_xi_spoiler_deg_b < A380SecComputer_P.Saturation4_LowerSat) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Saturation4_LowerSat;
    }

    A380SecComputer_RateLimiter_e(rtb_xi_spoiler_deg_b, A380SecComputer_P.RateLimiterGenericVariableTs3_up,
      A380SecComputer_P.RateLimiterGenericVariableTs3_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.right_aileron_2_pos_deg, (!A380SecComputer_B.logic.right_aileron_2_engaged) ||
      (!rtb_NOT_k), &ca, &A380SecComputer_DWork.sf_RateLimiter_p);
    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.right_aileron_2_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.right_aileron_2_command_deg = rtb_alpha;
    }

    if (A380SecComputer_B.logic.phased_lift_dumping_active) {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant5_Value;
    } else {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant6_Value;
    }

    A380SecComputer_RateLimiter(rtb_eta_trim_limit_up, A380SecComputer_P.RateLimiterVariableTs4_up,
      A380SecComputer_P.RateLimiterVariableTs4_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_P.RateLimiterVariableTs4_InitialCondition, &rtb_Switch8_o, &A380SecComputer_DWork.sf_RateLimiter_b);
    if (static_cast<int32_T>(A380SecComputer_B.logic.active_lateral_law) == 1) {
      rtb_xi_spoiler_deg_b = rtb_xi_spoiler_deg;
    } else {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Constant_Value_c;
    }

    if ((!A380SecComputer_DWork.pY_not_empty_k) || A380SecComputer_P.reset_Value) {
      A380SecComputer_DWork.pY_e = A380SecComputer_P.RateLimiterGenericVariableTs25_InitialCondition;
      A380SecComputer_DWork.pY_not_empty_k = true;
    }

    if (A380SecComputer_P.reset_Value) {
      A380SecComputer_DWork.pY_e = A380SecComputer_P.RateLimiterGenericVariableTs25_InitialCondition;
    } else {
      if (A380SecComputer_P.Constant7_Value_h) {
        rtb_eta_trim_limit_up = A380SecComputer_P.Constant9_Value;
      } else {
        rtb_eta_trim_limit_up = A380SecComputer_P.Constant8_Value;
      }

      A380SecComputer_DWork.pY_e += std::fmax(std::fmin(rtb_eta_trim_limit_up - A380SecComputer_DWork.pY_e, std::abs
        (A380SecComputer_P.RateLimiterGenericVariableTs25_up) * A380SecComputer_U.in.time.dt), -std::abs
        (A380SecComputer_P.RateLimiterGenericVariableTs25_lo) * A380SecComputer_U.in.time.dt);
    }

    if (rtb_xi_spoiler_deg_b >= 0.0) {
      ca = A380SecComputer_DWork.pY_e - rtb_xi_spoiler_deg_b;
      rtb_eta_trim_limit_up = A380SecComputer_DWork.pY_e;
    } else {
      ca = A380SecComputer_DWork.pY_e;
      rtb_eta_trim_limit_up = A380SecComputer_DWork.pY_e + rtb_xi_spoiler_deg_b;
    }

    rtb_xi_spoiler_deg_b = std::fmax(ca - (rtb_eta_trim_limit_up - std::fmax(rtb_eta_trim_limit_up, -45.0)), -45.0);
    rtb_Switch1_k = std::fmax(rtb_eta_trim_limit_up - (ca - std::fmax(ca, -45.0)), -45.0);
    if (A380SecComputer_B.logic.phased_lift_dumping_active) {
      rtb_eta_trim_limit_up = rtb_Switch8_o;
    } else {
      rtb_eta_trim_limit_up = rtb_xi_spoiler_deg_b;
    }

    A380SecComputer_RateLimiter_e(rtb_eta_trim_limit_up, A380SecComputer_P.RateLimiterGenericVariableTs8_up,
      A380SecComputer_P.RateLimiterGenericVariableTs8_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.left_spoiler_1_pos_deg,
      (!A380SecComputer_B.logic.left_spoiler_1_hydraulic_mode_engaged) || (!rtb_NOT_k), &ca,
      &A380SecComputer_DWork.sf_RateLimiter_os);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_mach_h = rtb_r;
      rtb_theta = rtb_n_x;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_mach_h = rtb_phi;
      rtb_theta = rtb_q;
    }

    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.left_spoiler_1_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.left_spoiler_1_command_deg = rtb_mach_h;
    }

    if (A380SecComputer_B.logic.phased_lift_dumping_active) {
      rtb_eta_trim_limit_up = rtb_Switch8_o;
    } else {
      rtb_eta_trim_limit_up = rtb_Switch1_k;
    }

    A380SecComputer_RateLimiter_e(rtb_eta_trim_limit_up, A380SecComputer_P.RateLimiterGenericVariableTs9_up,
      A380SecComputer_P.RateLimiterGenericVariableTs9_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.right_spoiler_1_pos_deg,
      (!A380SecComputer_B.logic.right_spoiler_1_hydraulic_mode_engaged) || (!rtb_NOT_k), &ca,
      &A380SecComputer_DWork.sf_RateLimiter_d);
    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.right_spoiler_1_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.right_spoiler_1_command_deg = rtb_theta;
    }

    if (A380SecComputer_B.logic.phased_lift_dumping_active) {
      rtb_xi_spoiler_deg_b = rtb_Switch8_o;
    }

    A380SecComputer_RateLimiter_e(rtb_xi_spoiler_deg_b, A380SecComputer_P.RateLimiterGenericVariableTs10_up,
      A380SecComputer_P.RateLimiterGenericVariableTs10_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.left_spoiler_2_pos_deg,
      (!A380SecComputer_B.logic.left_spoiler_2_hydraulic_mode_engaged) || (!rtb_NOT_k), &ca,
      &A380SecComputer_DWork.sf_RateLimiter_bv);
    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_theta_dot = 0.0F;
      rtb_phi_dot = 0.0F;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_theta_dot = rtb_n_y;
      rtb_phi_dot = rtb_n_z;
    }

    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.left_spoiler_2_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.left_spoiler_2_command_deg = rtb_theta_dot;
    }

    if (A380SecComputer_B.logic.phased_lift_dumping_active) {
      rtb_Switch1_k = rtb_Switch8_o;
    }

    A380SecComputer_RateLimiter_e(rtb_Switch1_k, A380SecComputer_P.RateLimiterGenericVariableTs11_up,
      A380SecComputer_P.RateLimiterGenericVariableTs11_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.right_spoiler_2_pos_deg,
      (!A380SecComputer_B.logic.right_spoiler_2_hydraulic_mode_engaged) || (!rtb_NOT_k), &ca,
      &A380SecComputer_DWork.sf_RateLimiter_g);
    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.right_spoiler_2_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.right_spoiler_2_command_deg = rtb_phi_dot;
    }

    if (static_cast<int32_T>(A380SecComputer_B.logic.active_lateral_law) == 1) {
      rtb_eta_trim_limit_up = rtb_zeta_upper_deg;
    } else {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant_Value_c;
    }

    A380SecComputer_RateLimiter_e(rtb_eta_trim_limit_up, A380SecComputer_P.RateLimiterGenericVariableTs6_up,
      A380SecComputer_P.RateLimiterGenericVariableTs6_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.rudder_1_pos_deg, ((!A380SecComputer_B.logic.rudder_1_electric_mode_engaged) &&
      (!A380SecComputer_B.logic.rudder_1_hydraulic_mode_engaged)) || (!rtb_NOT_k), &ca,
      &A380SecComputer_DWork.sf_RateLimiter_j);
    if (!A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
        rtb_Data_nu = 0.0F;
      } else {
        rtb_Data_awh = rtb_Data_nu;
        rtb_Data_nu = 0.0F;
      }
    }

    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.rudder_1_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.rudder_1_command_deg = rtb_Data_awh;
    }

    if (static_cast<int32_T>(A380SecComputer_B.logic.active_lateral_law) == 1) {
      rtb_eta_trim_limit_up = rtb_zeta_lower_deg;
    } else {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant_Value_c;
    }

    A380SecComputer_RateLimiter_e(rtb_eta_trim_limit_up, A380SecComputer_P.RateLimiterGenericVariableTs7_up,
      A380SecComputer_P.RateLimiterGenericVariableTs7_lo, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.rudder_2_pos_deg, ((!A380SecComputer_B.logic.rudder_2_electric_mode_engaged) &&
      (!A380SecComputer_B.logic.rudder_2_hydraulic_mode_engaged)) || (!rtb_NOT_k), &ca,
      &A380SecComputer_DWork.sf_RateLimiter_gz);
    if (rtb_NOT_k) {
      A380SecComputer_B.laws.lateral_law_outputs.rudder_2_command_deg = ca;
    } else {
      A380SecComputer_B.laws.lateral_law_outputs.rudder_2_command_deg = rtb_Data_nu;
    }

    rtb_NOT_k = (A380SecComputer_B.logic.master_prim == A380SecComputer_P.CompareToConstant_const_fl);
    LawMDLOBJ2.step(&A380SecComputer_U.in.time.dt, &A380SecComputer_B.logic.total_sidestick_pitch_command,
                    &rtb_xi_spoiler_deg_b, &rtb_Switch1_k, &rtb_Switch8_o, &rtb_eta_trim_limit_up);
    if (static_cast<int32_T>(A380SecComputer_B.logic.active_pitch_law) != 5) {
      rtb_xi_spoiler_deg_b = A380SecComputer_P.Constant_Value_a;
    }

    A380SecComputer_RateLimiter_e(rtb_xi_spoiler_deg_b, A380SecComputer_P.RateLimiterGenericVariableTs_up_a,
      A380SecComputer_P.RateLimiterGenericVariableTs_lo_f, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.elevator_1_pos_deg, (!A380SecComputer_B.logic.elevator_1_engaged) ||
      (!rtb_NOT_k), &ca, &A380SecComputer_DWork.sf_RateLimiter_c);
    if (A380SecComputer_B.logic.master_prim == A380SecComputer_P.CompareToConstant_const_fs) {
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_inboard_elevator_command_deg.Data;
      rtb_theta = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_inboard_elevator_command_deg.Data;
      rtb_mach_h = A380SecComputer_U.in.bus_inputs.prim_1_bus.left_outboard_elevator_command_deg.Data;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_1_bus.right_outboard_elevator_command_deg.Data;
      rtb_V_ias = A380SecComputer_U.in.bus_inputs.prim_1_bus.ths_command_deg.Data;
    } else if (A380SecComputer_B.logic.master_prim == A380SecComputer_P.CompareToConstant1_const_c) {
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_inboard_elevator_command_deg.Data;
      rtb_theta = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_inboard_elevator_command_deg.Data;
      rtb_mach_h = A380SecComputer_U.in.bus_inputs.prim_2_bus.left_outboard_elevator_command_deg.Data;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_2_bus.right_outboard_elevator_command_deg.Data;
      rtb_V_ias = A380SecComputer_U.in.bus_inputs.prim_2_bus.ths_command_deg.Data;
    } else {
      rtb_alpha = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_inboard_elevator_command_deg.Data;
      rtb_theta = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_inboard_elevator_command_deg.Data;
      rtb_mach_h = A380SecComputer_U.in.bus_inputs.prim_3_bus.left_outboard_elevator_command_deg.Data;
      rtb_Data_ch = A380SecComputer_U.in.bus_inputs.prim_3_bus.right_outboard_elevator_command_deg.Data;
      rtb_V_ias = A380SecComputer_U.in.bus_inputs.prim_3_bus.ths_command_deg.Data;
    }

    if (A380SecComputer_U.in.discrete_inputs.is_unit_1) {
      rtb_V_tas = rtb_mach_h;
      rtb_mach_h = rtb_alpha;
      rtb_theta = rtb_Data_ch;
    } else if (A380SecComputer_U.in.discrete_inputs.is_unit_2) {
      rtb_V_tas = rtb_Data_ch;
    } else {
      rtb_V_tas = rtb_alpha;
      rtb_mach_h = rtb_theta;
      rtb_theta = 0.0F;
    }

    if (rtb_NOT_k) {
      A380SecComputer_B.laws.pitch_law_outputs.elevator_1_command_deg = ca;
    } else {
      A380SecComputer_B.laws.pitch_law_outputs.elevator_1_command_deg = rtb_V_tas;
    }

    A380SecComputer_RateLimiter_e(rtb_xi_spoiler_deg_b, A380SecComputer_P.RateLimiterGenericVariableTs1_up_a,
      A380SecComputer_P.RateLimiterGenericVariableTs1_lo_c, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.elevator_2_pos_deg, (!A380SecComputer_B.logic.elevator_2_engaged) ||
      (!rtb_NOT_k), &ca, &A380SecComputer_DWork.sf_RateLimiter_p0);
    if (rtb_NOT_k) {
      A380SecComputer_B.laws.pitch_law_outputs.elevator_2_command_deg = ca;
    } else {
      A380SecComputer_B.laws.pitch_law_outputs.elevator_2_command_deg = rtb_mach_h;
    }

    A380SecComputer_RateLimiter_e(rtb_xi_spoiler_deg_b, A380SecComputer_P.RateLimiterGenericVariableTs2_up_l,
      A380SecComputer_P.RateLimiterGenericVariableTs2_lo_k, A380SecComputer_U.in.time.dt,
      A380SecComputer_U.in.analog_inputs.elevator_3_pos_deg, (!A380SecComputer_B.logic.elevator_3_engaged) ||
      (!rtb_NOT_k), &ca, &A380SecComputer_DWork.sf_RateLimiter_cd);
    if (rtb_NOT_k) {
      A380SecComputer_B.laws.pitch_law_outputs.elevator_3_command_deg = ca;
    } else {
      A380SecComputer_B.laws.pitch_law_outputs.elevator_3_command_deg = rtb_theta;
    }

    if (static_cast<int32_T>(A380SecComputer_B.logic.active_pitch_law) != 5) {
      rtb_eta_trim_limit_up = A380SecComputer_P.Constant2_Value_l;
      rtb_Switch1_k = A380SecComputer_P.Constant_Value_a;
    }

    ca = A380SecComputer_P.DiscreteTimeIntegratorVariableTsLimit_Gain * rtb_Switch1_k * A380SecComputer_U.in.time.dt;
    A380SecComputer_DWork.icLoad = ((!A380SecComputer_B.logic.ths_engaged) || (!rtb_NOT_k) ||
      A380SecComputer_DWork.icLoad);
    if (A380SecComputer_DWork.icLoad) {
      A380SecComputer_DWork.Delay_DSTATE = A380SecComputer_U.in.analog_inputs.ths_pos_deg - ca;
    }

    A380SecComputer_DWork.Delay_DSTATE += ca;
    if (A380SecComputer_DWork.Delay_DSTATE > rtb_eta_trim_limit_up) {
      A380SecComputer_DWork.Delay_DSTATE = rtb_eta_trim_limit_up;
    } else {
      if (static_cast<int32_T>(A380SecComputer_B.logic.active_pitch_law) != 5) {
        rtb_Switch8_o = A380SecComputer_P.Constant3_Value;
      }

      if (A380SecComputer_DWork.Delay_DSTATE < rtb_Switch8_o) {
        A380SecComputer_DWork.Delay_DSTATE = rtb_Switch8_o;
      }
    }

    if (rtb_NOT_k) {
      A380SecComputer_B.laws.pitch_law_outputs.ths_command_deg = A380SecComputer_DWork.Delay_DSTATE;
    } else {
      A380SecComputer_B.laws.pitch_law_outputs.ths_command_deg = rtb_V_ias;
    }

    if (A380SecComputer_B.logic.elevator_1_engaged) {
      A380SecComputer_Y.out.analog_outputs.elevator_1_pos_order_deg =
        A380SecComputer_B.laws.pitch_law_outputs.elevator_1_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.elevator_1_pos_order_deg = A380SecComputer_P.Constant_Value;
    }

    if (A380SecComputer_B.logic.elevator_2_engaged) {
      A380SecComputer_Y.out.analog_outputs.elevator_2_pos_order_deg =
        A380SecComputer_B.laws.pitch_law_outputs.elevator_2_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.elevator_2_pos_order_deg = A380SecComputer_P.Constant1_Value_n;
    }

    if (A380SecComputer_B.logic.elevator_3_engaged) {
      A380SecComputer_Y.out.analog_outputs.elevator_3_pos_order_deg =
        A380SecComputer_B.laws.pitch_law_outputs.elevator_3_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.elevator_3_pos_order_deg = A380SecComputer_P.Constant2_Value_k;
    }

    if (A380SecComputer_B.logic.ths_engaged) {
      A380SecComputer_Y.out.analog_outputs.ths_pos_order_deg = A380SecComputer_B.laws.pitch_law_outputs.ths_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.ths_pos_order_deg = A380SecComputer_P.Constant3_Value_g;
    }

    if (A380SecComputer_B.logic.left_aileron_1_engaged) {
      A380SecComputer_Y.out.analog_outputs.left_aileron_1_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.left_aileron_1_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.left_aileron_1_pos_order_deg = A380SecComputer_P.Constant4_Value_i;
    }

    if (A380SecComputer_B.logic.left_aileron_2_engaged) {
      A380SecComputer_Y.out.analog_outputs.left_aileron_2_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.left_aileron_2_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.left_aileron_2_pos_order_deg = A380SecComputer_P.Constant5_Value_n;
    }

    if (A380SecComputer_B.logic.right_aileron_1_engaged) {
      A380SecComputer_Y.out.analog_outputs.right_aileron_1_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.right_aileron_1_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.right_aileron_1_pos_order_deg = A380SecComputer_P.Constant6_Value_f;
    }

    if (A380SecComputer_B.logic.right_aileron_2_engaged) {
      A380SecComputer_Y.out.analog_outputs.right_aileron_2_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.right_aileron_2_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.right_aileron_2_pos_order_deg = A380SecComputer_P.Constant7_Value;
    }

    if (A380SecComputer_B.logic.left_spoiler_1_hydraulic_mode_engaged) {
      A380SecComputer_Y.out.analog_outputs.left_spoiler_1_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.left_spoiler_1_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.left_spoiler_1_pos_order_deg = A380SecComputer_P.Constant8_Value_p;
    }

    if (A380SecComputer_B.logic.right_spoiler_1_hydraulic_mode_engaged) {
      A380SecComputer_Y.out.analog_outputs.right_spoiler_1_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.right_spoiler_1_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.right_spoiler_1_pos_order_deg = A380SecComputer_P.Constant9_Value_n;
    }

    if (A380SecComputer_B.logic.left_spoiler_2_hydraulic_mode_engaged) {
      A380SecComputer_Y.out.analog_outputs.left_spoiler_2_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.left_spoiler_2_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.left_spoiler_2_pos_order_deg = A380SecComputer_P.Constant12_Value;
    }

    if (A380SecComputer_B.logic.right_spoiler_2_hydraulic_mode_engaged) {
      A380SecComputer_Y.out.analog_outputs.right_spoiler_2_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.right_spoiler_2_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.right_spoiler_2_pos_order_deg = A380SecComputer_P.Constant13_Value;
    }

    if (A380SecComputer_B.logic.rudder_1_electric_mode_engaged ||
        A380SecComputer_B.logic.rudder_1_hydraulic_mode_engaged) {
      A380SecComputer_Y.out.analog_outputs.rudder_1_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.rudder_1_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.rudder_1_pos_order_deg = A380SecComputer_P.Constant10_Value;
    }

    if (A380SecComputer_B.logic.rudder_2_electric_mode_engaged ||
        A380SecComputer_B.logic.rudder_2_hydraulic_mode_engaged) {
      A380SecComputer_Y.out.analog_outputs.rudder_2_pos_order_deg =
        A380SecComputer_B.laws.lateral_law_outputs.rudder_2_command_deg;
    } else {
      A380SecComputer_Y.out.analog_outputs.rudder_2_pos_order_deg = A380SecComputer_P.Constant11_Value;
    }

    A380SecComputer_Y.out.analog_outputs.rudder_trim_pos_order_deg = A380SecComputer_P.Constant14_Value;
    rtb_VectorConcatenate[0] = A380SecComputer_B.logic.left_aileron_1_avail;
    rtb_VectorConcatenate[9] = A380SecComputer_B.logic.right_aileron_2_avail;
    rtb_VectorConcatenate[10] = A380SecComputer_B.logic.right_aileron_2_engaged;
    rtb_VectorConcatenate[1] = A380SecComputer_B.logic.left_aileron_1_engaged;
    rtb_VectorConcatenate[3] = A380SecComputer_B.logic.right_aileron_1_avail;
    rtb_VectorConcatenate[4] = A380SecComputer_B.logic.right_aileron_1_engaged;
    rtb_VectorConcatenate[6] = A380SecComputer_B.logic.left_aileron_2_avail;
    rtb_VectorConcatenate[7] = A380SecComputer_B.logic.left_aileron_2_engaged;
    rtb_VectorConcatenate_a[0] = A380SecComputer_B.logic.left_spoiler_1_hydraulic_mode_avail;
    rtb_VectorConcatenate_a[9] = A380SecComputer_B.logic.right_spoiler_2_hydraulic_mode_avail;
    rtb_VectorConcatenate_a[10] = A380SecComputer_B.logic.right_spoiler_2_hydraulic_mode_engaged;
    rtb_VectorConcatenate_a[1] = A380SecComputer_B.logic.left_spoiler_1_hydraulic_mode_engaged;
    rtb_VectorConcatenate_a[3] = A380SecComputer_B.logic.right_spoiler_1_hydraulic_mode_avail;
    rtb_VectorConcatenate_a[4] = A380SecComputer_B.logic.right_spoiler_1_hydraulic_mode_engaged;
    rtb_VectorConcatenate_a[6] = A380SecComputer_B.logic.left_spoiler_2_hydraulic_mode_avail;
    rtb_VectorConcatenate_a[7] = A380SecComputer_B.logic.left_spoiler_2_hydraulic_mode_engaged;
    rtb_VectorConcatenate_aw[0] = A380SecComputer_B.logic.elevator_1_avail;
    rtb_VectorConcatenate_aw[9] = A380SecComputer_B.logic.ths_avail;
    rtb_VectorConcatenate_aw[10] = A380SecComputer_B.logic.ths_engaged;
    rtb_VectorConcatenate_aw[1] = A380SecComputer_B.logic.elevator_1_engaged;
    rtb_VectorConcatenate_aw[3] = A380SecComputer_B.logic.elevator_2_avail;
    rtb_VectorConcatenate_aw[4] = A380SecComputer_B.logic.elevator_2_engaged;
    rtb_VectorConcatenate_aw[6] = A380SecComputer_B.logic.elevator_3_avail;
    rtb_VectorConcatenate_aw[7] = A380SecComputer_B.logic.elevator_3_engaged;
    rtb_VectorConcatenate_e[0] = A380SecComputer_B.logic.rudder_1_hydraulic_mode_avail;
    rtb_VectorConcatenate_e[9] = A380SecComputer_B.logic.rudder_2_electric_mode_engaged;
    rtb_VectorConcatenate_e[16] = A380SecComputer_B.logic.rudder_trim_avail;
    rtb_VectorConcatenate_e[17] = A380SecComputer_B.logic.rudder_trim_engaged;
    rtb_VectorConcatenate_e[1] = A380SecComputer_B.logic.rudder_1_electric_mode_avail;
    rtb_VectorConcatenate_e[2] = A380SecComputer_B.logic.rudder_1_hydraulic_mode_engaged;
    rtb_VectorConcatenate_e[3] = A380SecComputer_B.logic.rudder_1_electric_mode_engaged;
    rtb_VectorConcatenate_e[6] = A380SecComputer_B.logic.rudder_2_hydraulic_mode_avail;
    rtb_VectorConcatenate_e[7] = A380SecComputer_B.logic.rudder_2_electric_mode_avail;
    rtb_VectorConcatenate_e[8] = A380SecComputer_B.logic.rudder_2_hydraulic_mode_engaged;
    switch (A380SecComputer_B.logic.active_pitch_law) {
     case a380_pitch_efcs_law::None:
      rtb_VectorConcatenate_j[5] = false;
      rtb_VectorConcatenate_j[6] = false;
      rtb_VectorConcatenate_j[7] = false;
      break;

     case a380_pitch_efcs_law::NormalLaw:
      rtb_VectorConcatenate_j[5] = false;
      rtb_VectorConcatenate_j[6] = false;
      rtb_VectorConcatenate_j[7] = true;
      break;

     case a380_pitch_efcs_law::AlternateLaw1A:
      rtb_VectorConcatenate_j[5] = false;
      rtb_VectorConcatenate_j[6] = true;
      rtb_VectorConcatenate_j[7] = false;
      break;

     case a380_pitch_efcs_law::AlternateLaw1B:
      rtb_VectorConcatenate_j[5] = false;
      rtb_VectorConcatenate_j[6] = true;
      rtb_VectorConcatenate_j[7] = true;
      break;

     case a380_pitch_efcs_law::AlternateLaw1C:
      rtb_VectorConcatenate_j[5] = true;
      rtb_VectorConcatenate_j[6] = false;
      rtb_VectorConcatenate_j[7] = false;
      break;

     case a380_pitch_efcs_law::AlternateLaw2:
      rtb_VectorConcatenate_j[5] = true;
      rtb_VectorConcatenate_j[6] = false;
      rtb_VectorConcatenate_j[7] = true;
      break;

     default:
      rtb_VectorConcatenate_j[5] = true;
      rtb_VectorConcatenate_j[6] = true;
      rtb_VectorConcatenate_j[7] = false;
      break;
    }

    switch (A380SecComputer_B.logic.active_lateral_law) {
     case a380_lateral_efcs_law::None:
      rtb_VectorConcatenate_j[8] = false;
      rtb_VectorConcatenate_j[9] = false;
      break;

     case a380_lateral_efcs_law::NormalLaw:
      rtb_VectorConcatenate_j[8] = true;
      rtb_VectorConcatenate_j[9] = false;
      break;

     default:
      rtb_VectorConcatenate_j[8] = false;
      rtb_VectorConcatenate_j[9] = true;
      break;
    }

    A380SecComputer_Y.out.discrete_outputs.elevator_1_active_mode = A380SecComputer_B.logic.elevator_1_engaged;
    A380SecComputer_Y.out.discrete_outputs.elevator_2_active_mode = A380SecComputer_B.logic.elevator_2_engaged;
    A380SecComputer_Y.out.discrete_outputs.elevator_3_active_mode = A380SecComputer_B.logic.elevator_3_engaged;
    A380SecComputer_Y.out.discrete_outputs.ths_active_mode = A380SecComputer_B.logic.ths_engaged;
    A380SecComputer_Y.out.discrete_outputs.left_aileron_1_active_mode = A380SecComputer_B.logic.left_aileron_1_engaged;
    A380SecComputer_Y.out.discrete_outputs.left_aileron_2_active_mode = A380SecComputer_B.logic.left_aileron_2_engaged;
    A380SecComputer_Y.out.discrete_outputs.right_aileron_1_active_mode = A380SecComputer_B.logic.right_aileron_1_engaged;
    A380SecComputer_Y.out.discrete_outputs.right_aileron_2_active_mode = A380SecComputer_B.logic.right_aileron_2_engaged;
    A380SecComputer_Y.out.discrete_outputs.rudder_1_hydraulic_active_mode =
      A380SecComputer_B.logic.rudder_1_hydraulic_mode_engaged;
    A380SecComputer_Y.out.discrete_outputs.rudder_1_electric_active_mode =
      A380SecComputer_B.logic.rudder_1_electric_mode_engaged;
    A380SecComputer_Y.out.discrete_outputs.rudder_2_hydraulic_active_mode =
      A380SecComputer_B.logic.rudder_2_hydraulic_mode_engaged;
    A380SecComputer_Y.out.discrete_outputs.rudder_2_electric_active_mode =
      A380SecComputer_B.logic.rudder_2_electric_mode_engaged;
    A380SecComputer_Y.out.discrete_outputs.rudder_trim_active_mode = A380SecComputer_B.logic.rudder_trim_engaged;
    A380SecComputer_Y.out.discrete_outputs.sec_healthy = A380SecComputer_P.Constant1_Value_f3;
    rtb_VectorConcatenate[11] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[12] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[13] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[14] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[15] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[16] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[17] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[18] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[2] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[5] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate[8] = A380SecComputer_P.Constant16_Value;
    rtb_VectorConcatenate_a[11] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[12] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[13] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[14] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[15] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[16] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[17] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[18] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[2] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[5] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_a[8] = A380SecComputer_P.Constant17_Value;
    rtb_VectorConcatenate_aw[11] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[12] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[13] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[14] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[15] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[16] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[17] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[18] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[2] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[5] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_aw[8] = A380SecComputer_P.Constant18_Value;
    rtb_VectorConcatenate_e[10] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_e[11] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_e[12] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_e[13] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_e[14] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_e[15] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_e[18] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_e[4] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_e[5] = A380SecComputer_P.Constant19_Value;
    rtb_VectorConcatenate_j[0] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[10] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[11] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[12] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[13] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[14] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[15] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[16] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[17] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[18] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[1] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[2] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[3] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_j[4] = A380SecComputer_P.Constant21_Value;
    rtb_VectorConcatenate_bz[0] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[9] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[10] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[11] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[12] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[13] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[14] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[15] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[16] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[17] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[18] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[1] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[2] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[3] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[4] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[5] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[6] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[7] = A380SecComputer_P.Constant22_Value;
    rtb_VectorConcatenate_bz[8] = A380SecComputer_P.Constant22_Value;
    A380SecComputer_MATLABFunction_cw(rtb_VectorConcatenate, &A380SecComputer_Y.out.bus_outputs.aileron_status_word.Data);
    A380SecComputer_MATLABFunction_cw(rtb_VectorConcatenate_a,
      &A380SecComputer_Y.out.bus_outputs.spoiler_status_word.Data);
    A380SecComputer_MATLABFunction_cw(rtb_VectorConcatenate_aw,
      &A380SecComputer_Y.out.bus_outputs.elevator_status_word.Data);
    A380SecComputer_MATLABFunction_cw(rtb_VectorConcatenate_e,
      &A380SecComputer_Y.out.bus_outputs.rudder_status_word.Data);
    A380SecComputer_Y.out.bus_outputs.misc_data_status_word.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.rudder_2_pos_deg);
    A380SecComputer_MATLABFunction_cw(rtb_VectorConcatenate_j,
      &A380SecComputer_Y.out.bus_outputs.fctl_law_status_word.Data);
    A380SecComputer_MATLABFunction_cw(rtb_VectorConcatenate_bz,
      &A380SecComputer_Y.out.bus_outputs.misc_data_status_word.Data);
    A380SecComputer_Y.out.bus_outputs.left_sidestick_pitch_command_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.left_sidestick_pitch_command_deg.Data = A380SecComputer_P.Gain_Gain_b *
      static_cast<real32_T>(A380SecComputer_U.in.analog_inputs.capt_pitch_stick_pos);
    A380SecComputer_Y.out.bus_outputs.right_sidestick_pitch_command_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.right_sidestick_pitch_command_deg.Data = A380SecComputer_P.Gain1_Gain *
      static_cast<real32_T>(A380SecComputer_U.in.analog_inputs.fo_pitch_stick_pos);
    A380SecComputer_Y.out.bus_outputs.left_sidestick_roll_command_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.left_sidestick_roll_command_deg.Data = A380SecComputer_P.Gain2_Gain * static_cast<
      real32_T>(A380SecComputer_U.in.analog_inputs.capt_roll_stick_pos);
    A380SecComputer_Y.out.bus_outputs.right_sidestick_roll_command_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.right_sidestick_roll_command_deg.Data = A380SecComputer_P.Gain3_Gain_g *
      static_cast<real32_T>(A380SecComputer_U.in.analog_inputs.fo_roll_stick_pos);
    A380SecComputer_Y.out.bus_outputs.rudder_pedal_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.rudder_pedal_position_deg.Data = A380SecComputer_P.Gain4_Gain *
      static_cast<real32_T>(A380SecComputer_U.in.analog_inputs.rudder_pedal_pos_deg);
    A380SecComputer_Y.out.bus_outputs.aileron_status_word.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.left_aileron_1_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.left_aileron_1_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.left_aileron_1_pos_deg);
    A380SecComputer_Y.out.bus_outputs.left_aileron_2_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.left_aileron_2_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.left_aileron_2_pos_deg);
    A380SecComputer_Y.out.bus_outputs.right_aileron_1_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.right_aileron_1_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.right_aileron_1_pos_deg);
    A380SecComputer_Y.out.bus_outputs.right_aileron_2_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.right_aileron_2_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.right_aileron_2_pos_deg);
    A380SecComputer_Y.out.bus_outputs.spoiler_status_word.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.left_spoiler_1_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.left_spoiler_1_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.left_spoiler_1_pos_deg);
    A380SecComputer_Y.out.bus_outputs.right_spoiler_1_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.right_spoiler_1_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.right_spoiler_1_pos_deg);
    A380SecComputer_Y.out.bus_outputs.left_spoiler_2_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.left_spoiler_2_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.left_spoiler_2_pos_deg);
    A380SecComputer_Y.out.bus_outputs.right_spoiler_2_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.right_spoiler_2_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.right_spoiler_2_pos_deg);
    A380SecComputer_Y.out.bus_outputs.elevator_status_word.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.elevator_1_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.elevator_1_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.elevator_1_pos_deg);
    A380SecComputer_Y.out.bus_outputs.elevator_2_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.elevator_2_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.elevator_2_pos_deg);
    A380SecComputer_Y.out.bus_outputs.elevator_3_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.elevator_3_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.elevator_3_pos_deg);
    A380SecComputer_Y.out.bus_outputs.ths_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.ths_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.ths_pos_deg);
    A380SecComputer_Y.out.bus_outputs.rudder_status_word.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.rudder_1_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.rudder_1_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.rudder_1_pos_deg);
    A380SecComputer_Y.out.bus_outputs.rudder_2_position_deg.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.rudder_2_position_deg.Data = static_cast<real32_T>
      (A380SecComputer_U.in.analog_inputs.rudder_2_pos_deg);
    A380SecComputer_Y.out.bus_outputs.fctl_law_status_word.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_Y.out.bus_outputs.misc_data_status_word.SSM = static_cast<uint32_T>
      (A380SecComputer_P.EnumeratedConstant1_Value);
    A380SecComputer_MATLABFunction_j(0.0, A380SecComputer_P.HysteresisNode2_highTrigger,
      A380SecComputer_P.HysteresisNode2_lowTrigger, &rtb_OR1, &A380SecComputer_DWork.sf_MATLABFunction_jg);
    A380SecComputer_MATLABFunction_j(0.0, A380SecComputer_P.HysteresisNode3_highTrigger,
      A380SecComputer_P.HysteresisNode3_lowTrigger, &rtb_OR1, &A380SecComputer_DWork.sf_MATLABFunction_br);
    A380SecComputer_DWork.icLoad = false;
  } else {
    A380SecComputer_DWork.Runtime_MODE = false;
  }

  A380SecComputer_Y.out.data.time.dt = A380SecComputer_B.dt;
  A380SecComputer_Y.out.data.time.simulation_time = A380SecComputer_B.simulation_time;
  A380SecComputer_Y.out.data.time.monotonic_time = A380SecComputer_B.monotonic_time;
  A380SecComputer_Y.out.data.sim_data.slew_on = A380SecComputer_B.slew_on;
  A380SecComputer_Y.out.data.sim_data.pause_on = A380SecComputer_B.pause_on;
  A380SecComputer_Y.out.data.sim_data.tracking_mode_on_override = A380SecComputer_B.tracking_mode_on_override;
  A380SecComputer_Y.out.data.sim_data.tailstrike_protection_on = A380SecComputer_B.tailstrike_protection_on;
  A380SecComputer_Y.out.data.sim_data.computer_running = A380SecComputer_B.computer_running;
  A380SecComputer_Y.out.data.discrete_inputs.sec_overhead_button_pressed = A380SecComputer_B.sec_overhead_button_pressed;
  A380SecComputer_Y.out.data.discrete_inputs.is_unit_1 = A380SecComputer_B.is_unit_1;
  A380SecComputer_Y.out.data.discrete_inputs.is_unit_2 = A380SecComputer_B.is_unit_2;
  A380SecComputer_Y.out.data.discrete_inputs.is_unit_3 = A380SecComputer_B.is_unit_3;
  A380SecComputer_Y.out.data.discrete_inputs.capt_priority_takeover_pressed =
    A380SecComputer_B.capt_priority_takeover_pressed;
  A380SecComputer_Y.out.data.discrete_inputs.fo_priority_takeover_pressed =
    A380SecComputer_B.fo_priority_takeover_pressed;
  A380SecComputer_Y.out.data.discrete_inputs.rudder_trim_left_pressed = A380SecComputer_B.rudder_trim_left_pressed;
  A380SecComputer_Y.out.data.discrete_inputs.rudder_trim_right_pressed = A380SecComputer_B.rudder_trim_right_pressed;
  A380SecComputer_Y.out.data.discrete_inputs.rudder_trim_reset_pressed = A380SecComputer_B.rudder_trim_reset_pressed;
  A380SecComputer_Y.out.data.discrete_inputs.pitch_trim_up_pressed = A380SecComputer_B.pitch_trim_up_pressed;
  A380SecComputer_Y.out.data.discrete_inputs.pitch_trim_down_pressed = A380SecComputer_B.pitch_trim_down_pressed;
  A380SecComputer_Y.out.data.discrete_inputs.green_low_pressure = A380SecComputer_B.green_low_pressure;
  A380SecComputer_Y.out.data.discrete_inputs.yellow_low_pressure = A380SecComputer_B.yellow_low_pressure;
  A380SecComputer_Y.out.data.analog_inputs.capt_pitch_stick_pos = A380SecComputer_B.capt_pitch_stick_pos;
  A380SecComputer_Y.out.data.analog_inputs.fo_pitch_stick_pos = A380SecComputer_B.fo_pitch_stick_pos;
  A380SecComputer_Y.out.data.analog_inputs.capt_roll_stick_pos = A380SecComputer_B.capt_roll_stick_pos;
  A380SecComputer_Y.out.data.analog_inputs.fo_roll_stick_pos = A380SecComputer_B.fo_roll_stick_pos;
  A380SecComputer_Y.out.data.analog_inputs.elevator_1_pos_deg = A380SecComputer_B.elevator_1_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.elevator_2_pos_deg = A380SecComputer_B.elevator_2_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.elevator_3_pos_deg = A380SecComputer_B.elevator_3_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.ths_pos_deg = A380SecComputer_B.ths_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.left_aileron_1_pos_deg = A380SecComputer_B.left_aileron_1_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.left_aileron_2_pos_deg = A380SecComputer_B.left_aileron_2_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.right_aileron_1_pos_deg = A380SecComputer_B.right_aileron_1_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.right_aileron_2_pos_deg = A380SecComputer_B.right_aileron_2_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.left_spoiler_1_pos_deg = A380SecComputer_B.left_spoiler_1_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.right_spoiler_1_pos_deg = A380SecComputer_B.right_spoiler_1_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.left_spoiler_2_pos_deg = A380SecComputer_B.left_spoiler_2_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.right_spoiler_2_pos_deg = A380SecComputer_B.right_spoiler_2_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.rudder_1_pos_deg = A380SecComputer_B.rudder_1_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.rudder_2_pos_deg = A380SecComputer_B.rudder_2_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.rudder_pedal_pos_deg = A380SecComputer_B.rudder_pedal_pos_deg;
  A380SecComputer_Y.out.data.analog_inputs.rudder_trim_pos_deg = A380SecComputer_B.rudder_trim_pos_deg;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.altitude_standard_ft.SSM = A380SecComputer_B.SSM_pj;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.altitude_standard_ft.Data = A380SecComputer_B.Data_nx;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.altitude_corrected_ft.SSM = A380SecComputer_B.SSM_cs;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.altitude_corrected_ft.Data = A380SecComputer_B.Data_do;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.mach.SSM = A380SecComputer_B.SSM_in;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.mach.Data = A380SecComputer_B.Data_gn;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.airspeed_computed_kn.SSM = A380SecComputer_B.SSM_jy;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.airspeed_computed_kn.Data = A380SecComputer_B.Data_k3;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.airspeed_true_kn.SSM = A380SecComputer_B.SSM_ktr;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.airspeed_true_kn.Data = A380SecComputer_B.Data_d3;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.vertical_speed_ft_min.SSM = A380SecComputer_B.SSM_ew;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.vertical_speed_ft_min.Data = A380SecComputer_B.Data_nzt;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.aoa_corrected_deg.SSM = A380SecComputer_B.SSM_a5h;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.aoa_corrected_deg.Data = A380SecComputer_B.Data_o23;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.corrected_average_static_pressure.SSM = A380SecComputer_B.SSM_lw;
  A380SecComputer_Y.out.data.bus_inputs.adr_1_bus.corrected_average_static_pressure.Data = A380SecComputer_B.Data_m21;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.altitude_standard_ft.SSM = A380SecComputer_B.SSM_lg;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.altitude_standard_ft.Data = A380SecComputer_B.Data_pl;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.altitude_corrected_ft.SSM = A380SecComputer_B.SSM_jx;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.altitude_corrected_ft.Data = A380SecComputer_B.Data_e4;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.mach.SSM = A380SecComputer_B.SSM_hr;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.mach.Data = A380SecComputer_B.Data_ea;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.airspeed_computed_kn.SSM = A380SecComputer_B.SSM_ie;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.airspeed_computed_kn.Data = A380SecComputer_B.Data_i2t;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.airspeed_true_kn.SSM = A380SecComputer_B.SSM_kf;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.airspeed_true_kn.Data = A380SecComputer_B.Data_ng;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.vertical_speed_ft_min.SSM = A380SecComputer_B.SSM_p5l;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.vertical_speed_ft_min.Data = A380SecComputer_B.Data_h31;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.aoa_corrected_deg.SSM = A380SecComputer_B.SSM_g3;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.aoa_corrected_deg.Data = A380SecComputer_B.Data_ew;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.corrected_average_static_pressure.SSM = A380SecComputer_B.SSM_b3;
  A380SecComputer_Y.out.data.bus_inputs.adr_2_bus.corrected_average_static_pressure.Data = A380SecComputer_B.Data_j1s;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.discrete_word_1.SSM = A380SecComputer_B.SSM_dxv;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.discrete_word_1.Data = A380SecComputer_B.Data_j5;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.latitude_deg.SSM = A380SecComputer_B.SSM_mxz;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.latitude_deg.Data = A380SecComputer_B.Data_cw;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.longitude_deg.SSM = A380SecComputer_B.SSM_kk4;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.longitude_deg.Data = A380SecComputer_B.Data_gqa;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.ground_speed_kn.SSM = A380SecComputer_B.SSM_cy;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.ground_speed_kn.Data = A380SecComputer_B.Data_hz;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.track_angle_true_deg.SSM = A380SecComputer_B.SSM_ju;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.track_angle_true_deg.Data = A380SecComputer_B.Data_fri;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.heading_true_deg.SSM = A380SecComputer_B.SSM_ey;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.heading_true_deg.Data = A380SecComputer_B.Data_cm;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.wind_speed_kn.SSM = A380SecComputer_B.SSM_jr;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.wind_speed_kn.Data = A380SecComputer_B.Data_czj;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.wind_direction_true_deg.SSM = A380SecComputer_B.SSM_hs;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.wind_direction_true_deg.Data = A380SecComputer_B.Data_mb;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.track_angle_magnetic_deg.SSM = A380SecComputer_B.SSM_mx3;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.track_angle_magnetic_deg.Data = A380SecComputer_B.Data_gk4;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.heading_magnetic_deg.SSM = A380SecComputer_B.SSM_er;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.heading_magnetic_deg.Data = A380SecComputer_B.Data_gbt;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.drift_angle_deg.SSM = A380SecComputer_B.SSM_hm;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.drift_angle_deg.Data = A380SecComputer_B.Data_p0;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.flight_path_angle_deg.SSM = A380SecComputer_B.SSM_dm;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.flight_path_angle_deg.Data = A380SecComputer_B.Data_dn;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.flight_path_accel_g.SSM = A380SecComputer_B.SSM_fk;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.flight_path_accel_g.Data = A380SecComputer_B.Data_iyw;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.pitch_angle_deg.SSM = A380SecComputer_B.SSM;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.pitch_angle_deg.Data = A380SecComputer_B.Data;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.roll_angle_deg.SSM = A380SecComputer_B.SSM_k;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.roll_angle_deg.Data = A380SecComputer_B.Data_f;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_pitch_rate_deg_s.SSM = A380SecComputer_B.SSM_kx;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_pitch_rate_deg_s.Data = A380SecComputer_B.Data_fw;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_roll_rate_deg_s.SSM = A380SecComputer_B.SSM_kxx;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_roll_rate_deg_s.Data = A380SecComputer_B.Data_fwx;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.SSM = A380SecComputer_B.SSM_kxxt;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.Data = A380SecComputer_B.Data_fwxk;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_long_accel_g.SSM = A380SecComputer_B.SSM_kxxta;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_long_accel_g.Data = A380SecComputer_B.Data_fwxkf;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_lat_accel_g.SSM = A380SecComputer_B.SSM_kxxtac;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_lat_accel_g.Data = A380SecComputer_B.Data_fwxkft;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_normal_accel_g.SSM = A380SecComputer_B.SSM_kxxtac0;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.body_normal_accel_g.Data = A380SecComputer_B.Data_fwxkftc;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.track_angle_rate_deg_s.SSM = A380SecComputer_B.SSM_kxxtac0z;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.track_angle_rate_deg_s.Data = A380SecComputer_B.Data_fwxkftc3;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.SSM = A380SecComputer_B.SSM_kxxtac0zt;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.Data = A380SecComputer_B.Data_fwxkftc3e;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.roll_att_rate_deg_s.SSM = A380SecComputer_B.SSM_kxxtac0ztg;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.roll_att_rate_deg_s.Data = A380SecComputer_B.Data_fwxkftc3ep;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.inertial_alt_ft.SSM = A380SecComputer_B.SSM_kxxtac0ztgf;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.inertial_alt_ft.Data = A380SecComputer_B.Data_fwxkftc3epg;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.along_track_horiz_acc_g.SSM = A380SecComputer_B.SSM_kxxtac0ztgf2;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.along_track_horiz_acc_g.Data = A380SecComputer_B.Data_fwxkftc3epgt;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.cross_track_horiz_acc_g.SSM = A380SecComputer_B.SSM_kxxtac0ztgf2u;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.cross_track_horiz_acc_g.Data = A380SecComputer_B.Data_fwxkftc3epgtd;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.vertical_accel_g.SSM = A380SecComputer_B.SSM_kxxtac0ztgf2ux;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.vertical_accel_g.Data = A380SecComputer_B.Data_fwxkftc3epgtdx;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.inertial_vertical_speed_ft_s.SSM =
    A380SecComputer_B.SSM_kxxtac0ztgf2uxn;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.inertial_vertical_speed_ft_s.Data =
    A380SecComputer_B.Data_fwxkftc3epgtdxc;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.north_south_velocity_kn.SSM = A380SecComputer_B.SSM_ky;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.north_south_velocity_kn.Data = A380SecComputer_B.Data_h;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.east_west_velocity_kn.SSM = A380SecComputer_B.SSM_d;
  A380SecComputer_Y.out.data.bus_inputs.ir_1_bus.east_west_velocity_kn.Data = A380SecComputer_B.Data_e;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.discrete_word_1.SSM = A380SecComputer_B.SSM_h;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.discrete_word_1.Data = A380SecComputer_B.Data_j;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.latitude_deg.SSM = A380SecComputer_B.SSM_kb;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.latitude_deg.Data = A380SecComputer_B.Data_d;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.longitude_deg.SSM = A380SecComputer_B.SSM_p;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.longitude_deg.Data = A380SecComputer_B.Data_p;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.ground_speed_kn.SSM = A380SecComputer_B.SSM_di;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.ground_speed_kn.Data = A380SecComputer_B.Data_i;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.track_angle_true_deg.SSM = A380SecComputer_B.SSM_j;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.track_angle_true_deg.Data = A380SecComputer_B.Data_g;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.heading_true_deg.SSM = A380SecComputer_B.SSM_i;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.heading_true_deg.Data = A380SecComputer_B.Data_a;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.wind_speed_kn.SSM = A380SecComputer_B.SSM_g;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.wind_speed_kn.Data = A380SecComputer_B.Data_eb;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.wind_direction_true_deg.SSM = A380SecComputer_B.SSM_db;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.wind_direction_true_deg.Data = A380SecComputer_B.Data_jo;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.track_angle_magnetic_deg.SSM = A380SecComputer_B.SSM_n;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.track_angle_magnetic_deg.Data = A380SecComputer_B.Data_ex;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.heading_magnetic_deg.SSM = A380SecComputer_B.SSM_a;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.heading_magnetic_deg.Data = A380SecComputer_B.Data_fd;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.drift_angle_deg.SSM = A380SecComputer_B.SSM_ir;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.drift_angle_deg.Data = A380SecComputer_B.Data_ja;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.flight_path_angle_deg.SSM = A380SecComputer_B.SSM_hu;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.flight_path_angle_deg.Data = A380SecComputer_B.Data_k;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.flight_path_accel_g.SSM = A380SecComputer_B.SSM_e;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.flight_path_accel_g.Data = A380SecComputer_B.Data_joy;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.pitch_angle_deg.SSM = A380SecComputer_B.SSM_gr;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.pitch_angle_deg.Data = A380SecComputer_B.Data_h3;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.roll_angle_deg.SSM = A380SecComputer_B.SSM_ev;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.roll_angle_deg.Data = A380SecComputer_B.Data_a0;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_pitch_rate_deg_s.SSM = A380SecComputer_B.SSM_l;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_pitch_rate_deg_s.Data = A380SecComputer_B.Data_b;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_roll_rate_deg_s.SSM = A380SecComputer_B.SSM_ei;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_roll_rate_deg_s.Data = A380SecComputer_B.Data_eq;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.SSM = A380SecComputer_B.SSM_an;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.Data = A380SecComputer_B.Data_iz;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_long_accel_g.SSM = A380SecComputer_B.SSM_c;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_long_accel_g.Data = A380SecComputer_B.Data_j2;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_lat_accel_g.SSM = A380SecComputer_B.SSM_cb;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_lat_accel_g.Data = A380SecComputer_B.Data_o;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_normal_accel_g.SSM = A380SecComputer_B.SSM_lb;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.body_normal_accel_g.Data = A380SecComputer_B.Data_m;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.track_angle_rate_deg_s.SSM = A380SecComputer_B.SSM_ia;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.track_angle_rate_deg_s.Data = A380SecComputer_B.Data_oq;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.SSM = A380SecComputer_B.SSM_kyz;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.Data = A380SecComputer_B.Data_fo;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.roll_att_rate_deg_s.SSM = A380SecComputer_B.SSM_as;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.roll_att_rate_deg_s.Data = A380SecComputer_B.Data_p1;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.inertial_alt_ft.SSM = A380SecComputer_B.SSM_is;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.inertial_alt_ft.Data = A380SecComputer_B.Data_p1y;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.along_track_horiz_acc_g.SSM = A380SecComputer_B.SSM_ca;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.along_track_horiz_acc_g.Data = A380SecComputer_B.Data_l;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.cross_track_horiz_acc_g.SSM = A380SecComputer_B.SSM_o;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.cross_track_horiz_acc_g.Data = A380SecComputer_B.Data_kp;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.vertical_accel_g.SSM = A380SecComputer_B.SSM_ak;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.vertical_accel_g.Data = A380SecComputer_B.Data_k0;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.inertial_vertical_speed_ft_s.SSM = A380SecComputer_B.SSM_cbj;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.inertial_vertical_speed_ft_s.Data = A380SecComputer_B.Data_pi;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.north_south_velocity_kn.SSM = A380SecComputer_B.SSM_cu;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.north_south_velocity_kn.Data = A380SecComputer_B.Data_dm;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.east_west_velocity_kn.SSM = A380SecComputer_B.SSM_nn;
  A380SecComputer_Y.out.data.bus_inputs.ir_2_bus.east_west_velocity_kn.Data = A380SecComputer_B.Data_f5;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.slat_flap_component_status_word.SSM = A380SecComputer_B.SSM_b;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.slat_flap_component_status_word.Data = A380SecComputer_B.Data_js;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.slat_flap_system_status_word.SSM = A380SecComputer_B.SSM_m;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.slat_flap_system_status_word.Data = A380SecComputer_B.Data_ee;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.slat_flap_actual_position_word.SSM = A380SecComputer_B.SSM_f;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.slat_flap_actual_position_word.Data = A380SecComputer_B.Data_ig;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.slat_actual_position_deg.SSM = A380SecComputer_B.SSM_bp;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.slat_actual_position_deg.Data = A380SecComputer_B.Data_mk;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.flap_actual_position_deg.SSM = A380SecComputer_B.SSM_hb;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_1_bus.flap_actual_position_deg.Data = A380SecComputer_B.Data_pu;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.slat_flap_component_status_word.SSM = A380SecComputer_B.SSM_gz;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.slat_flap_component_status_word.Data = A380SecComputer_B.Data_ly;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.slat_flap_system_status_word.SSM = A380SecComputer_B.SSM_pv;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.slat_flap_system_status_word.Data = A380SecComputer_B.Data_jq;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.slat_flap_actual_position_word.SSM = A380SecComputer_B.SSM_mf;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.slat_flap_actual_position_word.Data = A380SecComputer_B.Data_o5;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.slat_actual_position_deg.SSM = A380SecComputer_B.SSM_m0;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.slat_actual_position_deg.Data = A380SecComputer_B.Data_lyw;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.flap_actual_position_deg.SSM = A380SecComputer_B.SSM_kd;
  A380SecComputer_Y.out.data.bus_inputs.sfcc_2_bus.flap_actual_position_deg.Data = A380SecComputer_B.Data_gq;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_inboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_pu;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_inboard_aileron_command_deg.Data = A380SecComputer_B.Data_n;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_inboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_nv;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_inboard_aileron_command_deg.Data = A380SecComputer_B.Data_bq;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_midboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_d5;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_midboard_aileron_command_deg.Data = A380SecComputer_B.Data_dmn;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_midboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_eo;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_midboard_aileron_command_deg.Data = A380SecComputer_B.Data_jn;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_outboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_nd;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_outboard_aileron_command_deg.Data = A380SecComputer_B.Data_c;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_outboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_bq;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_outboard_aileron_command_deg.Data = A380SecComputer_B.Data_lx;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_1_command_deg.SSM = A380SecComputer_B.SSM_hi;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_1_command_deg.Data = A380SecComputer_B.Data_jb;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_1_command_deg.SSM = A380SecComputer_B.SSM_mm;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_1_command_deg.Data = A380SecComputer_B.Data_fn;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_2_command_deg.SSM = A380SecComputer_B.SSM_kz;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_2_command_deg.Data = A380SecComputer_B.Data_od;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_2_command_deg.SSM = A380SecComputer_B.SSM_il;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_2_command_deg.Data = A380SecComputer_B.Data_ez;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_3_command_deg.SSM = A380SecComputer_B.SSM_i2;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_3_command_deg.Data = A380SecComputer_B.Data_pw;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_3_command_deg.SSM = A380SecComputer_B.SSM_ah;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_3_command_deg.Data = A380SecComputer_B.Data_m2;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_4_command_deg.SSM = A380SecComputer_B.SSM_en;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_4_command_deg.Data = A380SecComputer_B.Data_ek;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_4_command_deg.SSM = A380SecComputer_B.SSM_dq;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_4_command_deg.Data = A380SecComputer_B.Data_iy;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_5_command_deg.SSM = A380SecComputer_B.SSM_px;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_5_command_deg.Data = A380SecComputer_B.Data_lk;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_5_command_deg.SSM = A380SecComputer_B.SSM_lbo;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_5_command_deg.Data = A380SecComputer_B.Data_ca;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_6_command_deg.SSM = A380SecComputer_B.SSM_p5;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_6_command_deg.Data = A380SecComputer_B.Data_pix;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_6_command_deg.SSM = A380SecComputer_B.SSM_mk;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_6_command_deg.Data = A380SecComputer_B.Data_di;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_7_command_deg.SSM = A380SecComputer_B.SSM_mu;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_7_command_deg.Data = A380SecComputer_B.Data_lz;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_7_command_deg.SSM = A380SecComputer_B.SSM_cbl;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_7_command_deg.Data = A380SecComputer_B.Data_lu;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_8_command_deg.SSM = A380SecComputer_B.SSM_gzd;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_8_command_deg.Data = A380SecComputer_B.Data_dc;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_8_command_deg.SSM = A380SecComputer_B.SSM_mo;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_8_command_deg.Data = A380SecComputer_B.Data_gc;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_inboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_me;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_inboard_elevator_command_deg.Data = A380SecComputer_B.Data_am;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_inboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_mj;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_inboard_elevator_command_deg.Data = A380SecComputer_B.Data_mo;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_outboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_a5;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_outboard_elevator_command_deg.Data = A380SecComputer_B.Data_dg;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_outboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_bt;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_outboard_elevator_command_deg.Data = A380SecComputer_B.Data_e1;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.ths_command_deg.SSM = A380SecComputer_B.SSM_om;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.ths_command_deg.Data = A380SecComputer_B.Data_fp;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.upper_rudder_command_deg.SSM = A380SecComputer_B.SSM_ar;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.upper_rudder_command_deg.Data = A380SecComputer_B.Data_ns;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.lower_rudder_command_deg.SSM = A380SecComputer_B.SSM_ce;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.lower_rudder_command_deg.Data = A380SecComputer_B.Data_m3;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_ed;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_oj;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_jh;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_jy;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_je;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_j1;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_jt;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_fc;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.rudder_pedal_position_deg.SSM = A380SecComputer_B.SSM_cui;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.rudder_pedal_position_deg.Data = A380SecComputer_B.Data_of;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.aileron_status_word.SSM = A380SecComputer_B.SSM_mq;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.aileron_status_word.Data = A380SecComputer_B.Data_lg;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_ni;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_aileron_1_position_deg.Data = A380SecComputer_B.Data_n4;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_df;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_aileron_2_position_deg.Data = A380SecComputer_B.Data_ot;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_oe;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_aileron_1_position_deg.Data = A380SecComputer_B.Data_gv;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_ha;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_aileron_2_position_deg.Data = A380SecComputer_B.Data_ou;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.spoiler_status_word.SSM = A380SecComputer_B.SSM_op;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.spoiler_status_word.Data = A380SecComputer_B.Data_dh;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_position_deg.SSM = A380SecComputer_B.SSM_a50;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.left_spoiler_position_deg.Data = A380SecComputer_B.Data_ph;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_position_deg.SSM = A380SecComputer_B.SSM_og;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.right_spoiler_position_deg.Data = A380SecComputer_B.Data_gs;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.elevator_status_word.SSM = A380SecComputer_B.SSM_a4;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.elevator_status_word.Data = A380SecComputer_B.Data_fd4;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.elevator_1_position_deg.SSM = A380SecComputer_B.SSM_bv;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.elevator_1_position_deg.Data = A380SecComputer_B.Data_hm;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.elevator_2_position_deg.SSM = A380SecComputer_B.SSM_bo;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.elevator_2_position_deg.Data = A380SecComputer_B.Data_i2;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.elevator_3_position_deg.SSM = A380SecComputer_B.SSM_d1;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.elevator_3_position_deg.Data = A380SecComputer_B.Data_og;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.ths_position_deg.SSM = A380SecComputer_B.SSM_hy;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.ths_position_deg.Data = A380SecComputer_B.Data_fv;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.rudder_status_word.SSM = A380SecComputer_B.SSM_gi;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.rudder_status_word.Data = A380SecComputer_B.Data_oc;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.rudder_1_position_deg.SSM = A380SecComputer_B.SSM_pp;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.rudder_1_position_deg.Data = A380SecComputer_B.Data_kq;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.rudder_2_position_deg.SSM = A380SecComputer_B.SSM_iab;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.rudder_2_position_deg.Data = A380SecComputer_B.Data_ne;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.fctl_law_status_word.SSM = A380SecComputer_B.SSM_jtv;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.fctl_law_status_word.Data = A380SecComputer_B.Data_it;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.misc_data_status_word.SSM = A380SecComputer_B.SSM_fy;
  A380SecComputer_Y.out.data.bus_inputs.prim_1_bus.misc_data_status_word.Data = A380SecComputer_B.Data_ch;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_inboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_d4;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_inboard_aileron_command_deg.Data = A380SecComputer_B.Data_bb;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_inboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_ars;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_inboard_aileron_command_deg.Data = A380SecComputer_B.Data_ol;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_midboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_din;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_midboard_aileron_command_deg.Data = A380SecComputer_B.Data_hw;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_midboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_m3;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_midboard_aileron_command_deg.Data = A380SecComputer_B.Data_hs;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_outboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_np;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_outboard_aileron_command_deg.Data = A380SecComputer_B.Data_fj;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_outboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_ax;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_outboard_aileron_command_deg.Data = A380SecComputer_B.Data_ky;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_1_command_deg.SSM = A380SecComputer_B.SSM_cl;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_1_command_deg.Data = A380SecComputer_B.Data_h5;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_1_command_deg.SSM = A380SecComputer_B.SSM_es;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_1_command_deg.Data = A380SecComputer_B.Data_ku;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_2_command_deg.SSM = A380SecComputer_B.SSM_gi1;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_2_command_deg.Data = A380SecComputer_B.Data_jp;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_2_command_deg.SSM = A380SecComputer_B.SSM_jz;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_2_command_deg.Data = A380SecComputer_B.Data_nu;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_3_command_deg.SSM = A380SecComputer_B.SSM_kt;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_3_command_deg.Data = A380SecComputer_B.Data_br;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_3_command_deg.SSM = A380SecComputer_B.SSM_ds;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_3_command_deg.Data = A380SecComputer_B.Data_ae;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_4_command_deg.SSM = A380SecComputer_B.SSM_eg;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_4_command_deg.Data = A380SecComputer_B.Data_pe;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_4_command_deg.SSM = A380SecComputer_B.SSM_a0;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_4_command_deg.Data = A380SecComputer_B.Data_fy;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_5_command_deg.SSM = A380SecComputer_B.SSM_cv;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_5_command_deg.Data = A380SecComputer_B.Data_na;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_5_command_deg.SSM = A380SecComputer_B.SSM_ea;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_5_command_deg.Data = A380SecComputer_B.Data_my;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_6_command_deg.SSM = A380SecComputer_B.SSM_p4;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_6_command_deg.Data = A380SecComputer_B.Data_i4;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_6_command_deg.SSM = A380SecComputer_B.SSM_m2;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_6_command_deg.Data = A380SecComputer_B.Data_cx;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_7_command_deg.SSM = A380SecComputer_B.SSM_bt0;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_7_command_deg.Data = A380SecComputer_B.Data_nz;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_7_command_deg.SSM = A380SecComputer_B.SSM_nr;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_7_command_deg.Data = A380SecComputer_B.Data_id;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_8_command_deg.SSM = A380SecComputer_B.SSM_fr;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_8_command_deg.Data = A380SecComputer_B.Data_o2;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_8_command_deg.SSM = A380SecComputer_B.SSM_cc;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_8_command_deg.Data = A380SecComputer_B.Data_gqq;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_inboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_lm;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_inboard_elevator_command_deg.Data = A380SecComputer_B.Data_md;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_inboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_mkm;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_inboard_elevator_command_deg.Data = A380SecComputer_B.Data_cz;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_outboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_jhd;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_outboard_elevator_command_deg.Data = A380SecComputer_B.Data_pm;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_outboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_av;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_outboard_elevator_command_deg.Data = A380SecComputer_B.Data_bj;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.ths_command_deg.SSM = A380SecComputer_B.SSM_ira;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.ths_command_deg.Data = A380SecComputer_B.Data_ox;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.upper_rudder_command_deg.SSM = A380SecComputer_B.SSM_ge;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.upper_rudder_command_deg.Data = A380SecComputer_B.Data_pe5;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.lower_rudder_command_deg.SSM = A380SecComputer_B.SSM_lv;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.lower_rudder_command_deg.Data = A380SecComputer_B.Data_jj;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_cg;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_p5;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_be;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_ekl;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_axb;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_nd;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_nz;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_n2;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.rudder_pedal_position_deg.SSM = A380SecComputer_B.SSM_cx;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.rudder_pedal_position_deg.Data = A380SecComputer_B.Data_dl;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.aileron_status_word.SSM = A380SecComputer_B.SSM_gh;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.aileron_status_word.Data = A380SecComputer_B.Data_gs2;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_ks;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_aileron_1_position_deg.Data = A380SecComputer_B.Data_h4;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_pw;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_aileron_2_position_deg.Data = A380SecComputer_B.Data_e3;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_fh;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_aileron_1_position_deg.Data = A380SecComputer_B.Data_f5h;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_gzn;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_aileron_2_position_deg.Data = A380SecComputer_B.Data_an;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.spoiler_status_word.SSM = A380SecComputer_B.SSM_oo;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.spoiler_status_word.Data = A380SecComputer_B.Data_i4o;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_position_deg.SSM = A380SecComputer_B.SSM_evh;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.left_spoiler_position_deg.Data = A380SecComputer_B.Data_af;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_position_deg.SSM = A380SecComputer_B.SSM_cn;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.right_spoiler_position_deg.Data = A380SecComputer_B.Data_bm;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.elevator_status_word.SSM = A380SecComputer_B.SSM_co;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.elevator_status_word.Data = A380SecComputer_B.Data_dk;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.elevator_1_position_deg.SSM = A380SecComputer_B.SSM_pe;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.elevator_1_position_deg.Data = A380SecComputer_B.Data_nv;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.elevator_2_position_deg.SSM = A380SecComputer_B.SSM_cgz;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.elevator_2_position_deg.Data = A380SecComputer_B.Data_jpf;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.elevator_3_position_deg.SSM = A380SecComputer_B.SSM_fw;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.elevator_3_position_deg.Data = A380SecComputer_B.Data_i5;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.ths_position_deg.SSM = A380SecComputer_B.SSM_h4;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.ths_position_deg.Data = A380SecComputer_B.Data_k2;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.rudder_status_word.SSM = A380SecComputer_B.SSM_cb3;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.rudder_status_word.Data = A380SecComputer_B.Data_as;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.rudder_1_position_deg.SSM = A380SecComputer_B.SSM_dv;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.rudder_1_position_deg.Data = A380SecComputer_B.Data_gk;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.rudder_2_position_deg.SSM = A380SecComputer_B.SSM_i4;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.rudder_2_position_deg.Data = A380SecComputer_B.Data_jl;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.fctl_law_status_word.SSM = A380SecComputer_B.SSM_fm;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.fctl_law_status_word.Data = A380SecComputer_B.Data_e32;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.misc_data_status_word.SSM = A380SecComputer_B.SSM_e5;
  A380SecComputer_Y.out.data.bus_inputs.prim_2_bus.misc_data_status_word.Data = A380SecComputer_B.Data_ih;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_inboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_bf;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_inboard_aileron_command_deg.Data = A380SecComputer_B.Data_du;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_inboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_fd;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_inboard_aileron_command_deg.Data = A380SecComputer_B.Data_n0;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_midboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_fv;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_midboard_aileron_command_deg.Data = A380SecComputer_B.Data_eqi;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_midboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_dt;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_midboard_aileron_command_deg.Data = A380SecComputer_B.Data_om;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_outboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_j5;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_outboard_aileron_command_deg.Data = A380SecComputer_B.Data_nr;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_outboard_aileron_command_deg.SSM = A380SecComputer_B.SSM_ng;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_outboard_aileron_command_deg.Data = A380SecComputer_B.Data_p3;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_1_command_deg.SSM = A380SecComputer_B.SSM_ls;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_1_command_deg.Data = A380SecComputer_B.Data_nb;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_1_command_deg.SSM = A380SecComputer_B.SSM_dg;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_1_command_deg.Data = A380SecComputer_B.Data_hd;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_2_command_deg.SSM = A380SecComputer_B.SSM_d3;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_2_command_deg.Data = A380SecComputer_B.Data_al;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_2_command_deg.SSM = A380SecComputer_B.SSM_p2;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_2_command_deg.Data = A380SecComputer_B.Data_gu;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_3_command_deg.SSM = A380SecComputer_B.SSM_bo0;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_3_command_deg.Data = A380SecComputer_B.Data_ix;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_3_command_deg.SSM = A380SecComputer_B.SSM_bc;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_3_command_deg.Data = A380SecComputer_B.Data_hu;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_4_command_deg.SSM = A380SecComputer_B.SSM_h0;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_4_command_deg.Data = A380SecComputer_B.Data_pm1;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_4_command_deg.SSM = A380SecComputer_B.SSM_giz;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_4_command_deg.Data = A380SecComputer_B.Data_i2y;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_5_command_deg.SSM = A380SecComputer_B.SSM_mqp;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_5_command_deg.Data = A380SecComputer_B.Data_pg;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_5_command_deg.SSM = A380SecComputer_B.SSM_ba;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_5_command_deg.Data = A380SecComputer_B.Data_ni;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_6_command_deg.SSM = A380SecComputer_B.SSM_ff;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_6_command_deg.Data = A380SecComputer_B.Data_fr;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_6_command_deg.SSM = A380SecComputer_B.SSM_ic;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_6_command_deg.Data = A380SecComputer_B.Data_cn;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_7_command_deg.SSM = A380SecComputer_B.SSM_fs;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_7_command_deg.Data = A380SecComputer_B.Data_nxl;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_7_command_deg.SSM = A380SecComputer_B.SSM_ja;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_7_command_deg.Data = A380SecComputer_B.Data_jh;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_8_command_deg.SSM = A380SecComputer_B.SSM_js;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_8_command_deg.Data = A380SecComputer_B.Data_gl;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_8_command_deg.SSM = A380SecComputer_B.SSM_is3;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_8_command_deg.Data = A380SecComputer_B.Data_myb;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_inboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_ag;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_inboard_elevator_command_deg.Data = A380SecComputer_B.Data_l2;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_inboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_f5;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_inboard_elevator_command_deg.Data = A380SecComputer_B.Data_o5o;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_outboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_ph;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_outboard_elevator_command_deg.Data = A380SecComputer_B.Data_l5;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_outboard_elevator_command_deg.SSM = A380SecComputer_B.SSM_jw;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_outboard_elevator_command_deg.Data = A380SecComputer_B.Data_dc2;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.ths_command_deg.SSM = A380SecComputer_B.SSM_j1;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.ths_command_deg.Data = A380SecComputer_B.Data_gr;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.upper_rudder_command_deg.SSM = A380SecComputer_B.SSM_ov;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.upper_rudder_command_deg.Data = A380SecComputer_B.Data_gp;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.lower_rudder_command_deg.SSM = A380SecComputer_B.SSM_mx;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.lower_rudder_command_deg.Data = A380SecComputer_B.Data_i3;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_b4;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_et;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_gb;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_mc;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_oh;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_f2;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_mm5;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_gh;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.rudder_pedal_position_deg.SSM = A380SecComputer_B.SSM_br;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.rudder_pedal_position_deg.Data = A380SecComputer_B.Data_ed;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.aileron_status_word.SSM = A380SecComputer_B.SSM_c2;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.aileron_status_word.Data = A380SecComputer_B.Data_o2j;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_hc;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_aileron_1_position_deg.Data = A380SecComputer_B.Data_i43;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_gl;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_aileron_2_position_deg.Data = A380SecComputer_B.Data_ic;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_my;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_aileron_1_position_deg.Data = A380SecComputer_B.Data_ak;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_j3;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_aileron_2_position_deg.Data = A380SecComputer_B.Data_jg;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.spoiler_status_word.SSM = A380SecComputer_B.SSM_go;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.spoiler_status_word.Data = A380SecComputer_B.Data_cu;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_position_deg.SSM = A380SecComputer_B.SSM_e5c;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.left_spoiler_position_deg.Data = A380SecComputer_B.Data_ep;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_position_deg.SSM = A380SecComputer_B.SSM_dk;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.right_spoiler_position_deg.Data = A380SecComputer_B.Data_bt;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.elevator_status_word.SSM = A380SecComputer_B.SSM_evc;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.elevator_status_word.Data = A380SecComputer_B.Data_e0;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.elevator_1_position_deg.SSM = A380SecComputer_B.SSM_kk;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.elevator_1_position_deg.Data = A380SecComputer_B.Data_jl3;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.elevator_2_position_deg.SSM = A380SecComputer_B.SSM_af;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.elevator_2_position_deg.Data = A380SecComputer_B.Data_nm;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.elevator_3_position_deg.SSM = A380SecComputer_B.SSM_npr;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.elevator_3_position_deg.Data = A380SecComputer_B.Data_ia;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.ths_position_deg.SSM = A380SecComputer_B.SSM_lt;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.ths_position_deg.Data = A380SecComputer_B.Data_j0;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.rudder_status_word.SSM = A380SecComputer_B.SSM_ger;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.rudder_status_word.Data = A380SecComputer_B.Data_bs;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.rudder_1_position_deg.SSM = A380SecComputer_B.SSM_pxo;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.rudder_1_position_deg.Data = A380SecComputer_B.Data_hp;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.rudder_2_position_deg.SSM = A380SecComputer_B.SSM_co2;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.rudder_2_position_deg.Data = A380SecComputer_B.Data_ct;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.fctl_law_status_word.SSM = A380SecComputer_B.SSM_ny;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.fctl_law_status_word.Data = A380SecComputer_B.Data_pc;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.misc_data_status_word.SSM = A380SecComputer_B.SSM_l4;
  A380SecComputer_Y.out.data.bus_inputs.prim_3_bus.misc_data_status_word.Data = A380SecComputer_B.Data_c0;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_nm;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_ojg;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_nh;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_lm;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_dl;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_fz;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_dx;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_oz;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.rudder_pedal_position_deg.SSM = A380SecComputer_B.SSM_fl;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.rudder_pedal_position_deg.Data = A380SecComputer_B.Data_gf;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.aileron_status_word.SSM = A380SecComputer_B.SSM_p3;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.aileron_status_word.Data = A380SecComputer_B.Data_nn;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_ns;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_aileron_1_position_deg.Data = A380SecComputer_B.Data_a0z;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_bm;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_aileron_2_position_deg.Data = A380SecComputer_B.Data_fk;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_nl;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_aileron_1_position_deg.Data = A380SecComputer_B.Data_bu;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_grm;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_aileron_2_position_deg.Data = A380SecComputer_B.Data_g3;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.spoiler_status_word.SSM = A380SecComputer_B.SSM_gzm;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.spoiler_status_word.Data = A380SecComputer_B.Data_icc;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_spoiler_1_position_deg.SSM = A380SecComputer_B.SSM_oi;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_spoiler_1_position_deg.Data = A380SecComputer_B.Data_pwf;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_spoiler_1_position_deg.SSM = A380SecComputer_B.SSM_aa;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_spoiler_1_position_deg.Data = A380SecComputer_B.Data_gvk;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_spoiler_2_position_deg.SSM = A380SecComputer_B.SSM_fvk;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.left_spoiler_2_position_deg.Data = A380SecComputer_B.Data_ln;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_spoiler_2_position_deg.SSM = A380SecComputer_B.SSM_fa;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.right_spoiler_2_position_deg.Data = A380SecComputer_B.Data_ka;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.elevator_status_word.SSM = A380SecComputer_B.SSM_lbx;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.elevator_status_word.Data = A380SecComputer_B.Data_mp;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.elevator_1_position_deg.SSM = A380SecComputer_B.SSM_n3;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.elevator_1_position_deg.Data = A380SecComputer_B.Data_m4;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.elevator_2_position_deg.SSM = A380SecComputer_B.SSM_a1;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.elevator_2_position_deg.Data = A380SecComputer_B.Data_fki;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.elevator_3_position_deg.SSM = A380SecComputer_B.SSM_p1;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.elevator_3_position_deg.Data = A380SecComputer_B.Data_bv;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.ths_position_deg.SSM = A380SecComputer_B.SSM_cn2;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.ths_position_deg.Data = A380SecComputer_B.Data_nbg;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.rudder_status_word.SSM = A380SecComputer_B.SSM_an3;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.rudder_status_word.Data = A380SecComputer_B.Data_l25;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.rudder_1_position_deg.SSM = A380SecComputer_B.SSM_c3;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.rudder_1_position_deg.Data = A380SecComputer_B.Data_ki;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.rudder_2_position_deg.SSM = A380SecComputer_B.SSM_dp;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.rudder_2_position_deg.Data = A380SecComputer_B.Data_p5p;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.fctl_law_status_word.SSM = A380SecComputer_B.SSM_boy;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.fctl_law_status_word.Data = A380SecComputer_B.Data_nry;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.misc_data_status_word.SSM = A380SecComputer_B.SSM_cm;
  A380SecComputer_Y.out.data.bus_inputs.sec_x_bus.misc_data_status_word.Data = A380SecComputer_B.Data_mh;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_hl;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_ll;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_sidestick_pitch_command_deg.SSM = A380SecComputer_B.SSM_irh;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_sidestick_pitch_command_deg.Data = A380SecComputer_B.Data_hy;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_b42;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_j04;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_sidestick_roll_command_deg.SSM = A380SecComputer_B.SSM_anz;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_sidestick_roll_command_deg.Data = A380SecComputer_B.Data_pf;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.rudder_pedal_position_deg.SSM = A380SecComputer_B.SSM_d2;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.rudder_pedal_position_deg.Data = A380SecComputer_B.Data_gb;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.aileron_status_word.SSM = A380SecComputer_B.SSM_gov;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.aileron_status_word.Data = A380SecComputer_B.Data_hq;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_nb;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_aileron_1_position_deg.Data = A380SecComputer_B.Data_ai;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_pe3;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_aileron_2_position_deg.Data = A380SecComputer_B.Data_gfr;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_aileron_1_position_deg.SSM = A380SecComputer_B.SSM_jj;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_aileron_1_position_deg.Data = A380SecComputer_B.Data_czp;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_aileron_2_position_deg.SSM = A380SecComputer_B.SSM_npl;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_aileron_2_position_deg.Data = A380SecComputer_B.Data_fm;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.spoiler_status_word.SSM = A380SecComputer_B.SSM_gf;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.spoiler_status_word.Data = A380SecComputer_B.Data_jsg;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_spoiler_1_position_deg.SSM = A380SecComputer_B.SSM_gbi;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_spoiler_1_position_deg.Data = A380SecComputer_B.Data_g1;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_spoiler_1_position_deg.SSM = A380SecComputer_B.SSM_fhm;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_spoiler_1_position_deg.Data = A380SecComputer_B.Data_j4;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_spoiler_2_position_deg.SSM = A380SecComputer_B.SSM_ltj;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.left_spoiler_2_position_deg.Data = A380SecComputer_B.Data_jyh;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_spoiler_2_position_deg.SSM = A380SecComputer_B.SSM_hn;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.right_spoiler_2_position_deg.Data = A380SecComputer_B.Data_ghs;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.elevator_status_word.SSM = A380SecComputer_B.SSM_h3;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.elevator_status_word.Data = A380SecComputer_B.Data_bmk;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.elevator_1_position_deg.SSM = A380SecComputer_B.SSM_bfs;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.elevator_1_position_deg.Data = A380SecComputer_B.Data_lzt;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.elevator_2_position_deg.SSM = A380SecComputer_B.SSM_p0;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.elevator_2_position_deg.Data = A380SecComputer_B.Data_kn;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.elevator_3_position_deg.SSM = A380SecComputer_B.SSM_fu;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.elevator_3_position_deg.Data = A380SecComputer_B.Data_nab;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.ths_position_deg.SSM = A380SecComputer_B.SSM_bi;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.ths_position_deg.Data = A380SecComputer_B.Data_lgf;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.rudder_status_word.SSM = A380SecComputer_B.SSM_bd;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.rudder_status_word.Data = A380SecComputer_B.Data_fpq;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.rudder_1_position_deg.SSM = A380SecComputer_B.SSM_omt;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.rudder_1_position_deg.Data = A380SecComputer_B.Data_dt;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.rudder_2_position_deg.SSM = A380SecComputer_B.SSM_la;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.rudder_2_position_deg.Data = A380SecComputer_B.Data_b1;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.fctl_law_status_word.SSM = A380SecComputer_B.SSM_l1;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.fctl_law_status_word.Data = A380SecComputer_B.Data_nmr;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.misc_data_status_word.SSM = A380SecComputer_B.SSM_dy;
  A380SecComputer_Y.out.data.bus_inputs.sec_y_bus.misc_data_status_word.Data = A380SecComputer_B.Data_nib;
  A380SecComputer_Y.out.data.bus_inputs.irdc_5_a_bus = A380SecComputer_B.irdc_5_a_bus;
  A380SecComputer_Y.out.data.bus_inputs.irdc_5_b_bus = A380SecComputer_B.irdc_5_b_bus;
  A380SecComputer_Y.out.laws = A380SecComputer_B.laws;
  A380SecComputer_Y.out.logic = A380SecComputer_B.logic;
}

void A380SecComputer::initialize()
{
  A380SecComputer_DWork.Delay_DSTATE_c = A380SecComputer_P.Delay_InitialCondition;
  A380SecComputer_DWork.Delay1_DSTATE = A380SecComputer_P.Delay1_InitialCondition;
  A380SecComputer_DWork.icLoad = true;
  A380SecComputer_B.dt = A380SecComputer_P.out_Y0.data.time.dt;
  A380SecComputer_B.simulation_time = A380SecComputer_P.out_Y0.data.time.simulation_time;
  A380SecComputer_B.monotonic_time = A380SecComputer_P.out_Y0.data.time.monotonic_time;
  A380SecComputer_B.slew_on = A380SecComputer_P.out_Y0.data.sim_data.slew_on;
  A380SecComputer_B.pause_on = A380SecComputer_P.out_Y0.data.sim_data.pause_on;
  A380SecComputer_B.tracking_mode_on_override = A380SecComputer_P.out_Y0.data.sim_data.tracking_mode_on_override;
  A380SecComputer_B.tailstrike_protection_on = A380SecComputer_P.out_Y0.data.sim_data.tailstrike_protection_on;
  A380SecComputer_B.computer_running = A380SecComputer_P.out_Y0.data.sim_data.computer_running;
  A380SecComputer_B.sec_overhead_button_pressed =
    A380SecComputer_P.out_Y0.data.discrete_inputs.sec_overhead_button_pressed;
  A380SecComputer_B.is_unit_1 = A380SecComputer_P.out_Y0.data.discrete_inputs.is_unit_1;
  A380SecComputer_B.is_unit_2 = A380SecComputer_P.out_Y0.data.discrete_inputs.is_unit_2;
  A380SecComputer_B.is_unit_3 = A380SecComputer_P.out_Y0.data.discrete_inputs.is_unit_3;
  A380SecComputer_B.capt_priority_takeover_pressed =
    A380SecComputer_P.out_Y0.data.discrete_inputs.capt_priority_takeover_pressed;
  A380SecComputer_B.fo_priority_takeover_pressed =
    A380SecComputer_P.out_Y0.data.discrete_inputs.fo_priority_takeover_pressed;
  A380SecComputer_B.rudder_trim_left_pressed = A380SecComputer_P.out_Y0.data.discrete_inputs.rudder_trim_left_pressed;
  A380SecComputer_B.rudder_trim_right_pressed = A380SecComputer_P.out_Y0.data.discrete_inputs.rudder_trim_right_pressed;
  A380SecComputer_B.rudder_trim_reset_pressed = A380SecComputer_P.out_Y0.data.discrete_inputs.rudder_trim_reset_pressed;
  A380SecComputer_B.pitch_trim_up_pressed = A380SecComputer_P.out_Y0.data.discrete_inputs.pitch_trim_up_pressed;
  A380SecComputer_B.pitch_trim_down_pressed = A380SecComputer_P.out_Y0.data.discrete_inputs.pitch_trim_down_pressed;
  A380SecComputer_B.green_low_pressure = A380SecComputer_P.out_Y0.data.discrete_inputs.green_low_pressure;
  A380SecComputer_B.yellow_low_pressure = A380SecComputer_P.out_Y0.data.discrete_inputs.yellow_low_pressure;
  A380SecComputer_B.capt_pitch_stick_pos = A380SecComputer_P.out_Y0.data.analog_inputs.capt_pitch_stick_pos;
  A380SecComputer_B.fo_pitch_stick_pos = A380SecComputer_P.out_Y0.data.analog_inputs.fo_pitch_stick_pos;
  A380SecComputer_B.capt_roll_stick_pos = A380SecComputer_P.out_Y0.data.analog_inputs.capt_roll_stick_pos;
  A380SecComputer_B.fo_roll_stick_pos = A380SecComputer_P.out_Y0.data.analog_inputs.fo_roll_stick_pos;
  A380SecComputer_B.elevator_1_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.elevator_1_pos_deg;
  A380SecComputer_B.elevator_2_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.elevator_2_pos_deg;
  A380SecComputer_B.elevator_3_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.elevator_3_pos_deg;
  A380SecComputer_B.ths_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.ths_pos_deg;
  A380SecComputer_B.left_aileron_1_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.left_aileron_1_pos_deg;
  A380SecComputer_B.left_aileron_2_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.left_aileron_2_pos_deg;
  A380SecComputer_B.right_aileron_1_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.right_aileron_1_pos_deg;
  A380SecComputer_B.right_aileron_2_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.right_aileron_2_pos_deg;
  A380SecComputer_B.left_spoiler_1_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.left_spoiler_1_pos_deg;
  A380SecComputer_B.right_spoiler_1_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.right_spoiler_1_pos_deg;
  A380SecComputer_B.left_spoiler_2_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.left_spoiler_2_pos_deg;
  A380SecComputer_B.right_spoiler_2_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.right_spoiler_2_pos_deg;
  A380SecComputer_B.rudder_1_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.rudder_1_pos_deg;
  A380SecComputer_B.rudder_2_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.rudder_2_pos_deg;
  A380SecComputer_B.rudder_pedal_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.rudder_pedal_pos_deg;
  A380SecComputer_B.rudder_trim_pos_deg = A380SecComputer_P.out_Y0.data.analog_inputs.rudder_trim_pos_deg;
  A380SecComputer_B.SSM_pj = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.altitude_standard_ft.SSM;
  A380SecComputer_B.Data_nx = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.altitude_standard_ft.Data;
  A380SecComputer_B.SSM_cs = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.altitude_corrected_ft.SSM;
  A380SecComputer_B.Data_do = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.altitude_corrected_ft.Data;
  A380SecComputer_B.SSM_in = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.mach.SSM;
  A380SecComputer_B.Data_gn = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.mach.Data;
  A380SecComputer_B.SSM_jy = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.airspeed_computed_kn.SSM;
  A380SecComputer_B.Data_k3 = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.airspeed_computed_kn.Data;
  A380SecComputer_B.SSM_ktr = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.airspeed_true_kn.SSM;
  A380SecComputer_B.Data_d3 = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.airspeed_true_kn.Data;
  A380SecComputer_B.SSM_ew = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.vertical_speed_ft_min.SSM;
  A380SecComputer_B.Data_nzt = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.vertical_speed_ft_min.Data;
  A380SecComputer_B.SSM_a5h = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.aoa_corrected_deg.SSM;
  A380SecComputer_B.Data_o23 = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.aoa_corrected_deg.Data;
  A380SecComputer_B.SSM_lw = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.corrected_average_static_pressure.SSM;
  A380SecComputer_B.Data_m21 = A380SecComputer_P.out_Y0.data.bus_inputs.adr_1_bus.corrected_average_static_pressure.Data;
  A380SecComputer_B.SSM_lg = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.altitude_standard_ft.SSM;
  A380SecComputer_B.Data_pl = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.altitude_standard_ft.Data;
  A380SecComputer_B.SSM_jx = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.altitude_corrected_ft.SSM;
  A380SecComputer_B.Data_e4 = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.altitude_corrected_ft.Data;
  A380SecComputer_B.SSM_hr = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.mach.SSM;
  A380SecComputer_B.Data_ea = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.mach.Data;
  A380SecComputer_B.SSM_ie = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.airspeed_computed_kn.SSM;
  A380SecComputer_B.Data_i2t = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.airspeed_computed_kn.Data;
  A380SecComputer_B.SSM_kf = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.airspeed_true_kn.SSM;
  A380SecComputer_B.Data_ng = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.airspeed_true_kn.Data;
  A380SecComputer_B.SSM_p5l = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.vertical_speed_ft_min.SSM;
  A380SecComputer_B.Data_h31 = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.vertical_speed_ft_min.Data;
  A380SecComputer_B.SSM_g3 = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.aoa_corrected_deg.SSM;
  A380SecComputer_B.Data_ew = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.aoa_corrected_deg.Data;
  A380SecComputer_B.SSM_b3 = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.corrected_average_static_pressure.SSM;
  A380SecComputer_B.Data_j1s = A380SecComputer_P.out_Y0.data.bus_inputs.adr_2_bus.corrected_average_static_pressure.Data;
  A380SecComputer_B.SSM_dxv = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.discrete_word_1.SSM;
  A380SecComputer_B.Data_j5 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.discrete_word_1.Data;
  A380SecComputer_B.SSM_mxz = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.latitude_deg.SSM;
  A380SecComputer_B.Data_cw = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.latitude_deg.Data;
  A380SecComputer_B.SSM_kk4 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.longitude_deg.SSM;
  A380SecComputer_B.Data_gqa = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.longitude_deg.Data;
  A380SecComputer_B.SSM_cy = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.ground_speed_kn.SSM;
  A380SecComputer_B.Data_hz = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.ground_speed_kn.Data;
  A380SecComputer_B.SSM_ju = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.track_angle_true_deg.SSM;
  A380SecComputer_B.Data_fri = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.track_angle_true_deg.Data;
  A380SecComputer_B.SSM_ey = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.heading_true_deg.SSM;
  A380SecComputer_B.Data_cm = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.heading_true_deg.Data;
  A380SecComputer_B.SSM_jr = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.wind_speed_kn.SSM;
  A380SecComputer_B.Data_czj = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.wind_speed_kn.Data;
  A380SecComputer_B.SSM_hs = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.wind_direction_true_deg.SSM;
  A380SecComputer_B.Data_mb = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.wind_direction_true_deg.Data;
  A380SecComputer_B.SSM_mx3 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.track_angle_magnetic_deg.SSM;
  A380SecComputer_B.Data_gk4 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.track_angle_magnetic_deg.Data;
  A380SecComputer_B.SSM_er = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.heading_magnetic_deg.SSM;
  A380SecComputer_B.Data_gbt = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.heading_magnetic_deg.Data;
  A380SecComputer_B.SSM_hm = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.drift_angle_deg.SSM;
  A380SecComputer_B.Data_p0 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.drift_angle_deg.Data;
  A380SecComputer_B.SSM_dm = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.flight_path_angle_deg.SSM;
  A380SecComputer_B.Data_dn = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.flight_path_angle_deg.Data;
  A380SecComputer_B.SSM_fk = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.flight_path_accel_g.SSM;
  A380SecComputer_B.Data_iyw = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.flight_path_accel_g.Data;
  A380SecComputer_B.SSM = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.pitch_angle_deg.SSM;
  A380SecComputer_B.Data = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.pitch_angle_deg.Data;
  A380SecComputer_B.SSM_k = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.roll_angle_deg.SSM;
  A380SecComputer_B.Data_f = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.roll_angle_deg.Data;
  A380SecComputer_B.SSM_kx = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_pitch_rate_deg_s.SSM;
  A380SecComputer_B.Data_fw = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_pitch_rate_deg_s.Data;
  A380SecComputer_B.SSM_kxx = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_roll_rate_deg_s.SSM;
  A380SecComputer_B.Data_fwx = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_roll_rate_deg_s.Data;
  A380SecComputer_B.SSM_kxxt = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.SSM;
  A380SecComputer_B.Data_fwxk = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_yaw_rate_deg_s.Data;
  A380SecComputer_B.SSM_kxxta = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_long_accel_g.SSM;
  A380SecComputer_B.Data_fwxkf = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_long_accel_g.Data;
  A380SecComputer_B.SSM_kxxtac = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_lat_accel_g.SSM;
  A380SecComputer_B.Data_fwxkft = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_lat_accel_g.Data;
  A380SecComputer_B.SSM_kxxtac0 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_normal_accel_g.SSM;
  A380SecComputer_B.Data_fwxkftc = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.body_normal_accel_g.Data;
  A380SecComputer_B.SSM_kxxtac0z = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.track_angle_rate_deg_s.SSM;
  A380SecComputer_B.Data_fwxkftc3 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.track_angle_rate_deg_s.Data;
  A380SecComputer_B.SSM_kxxtac0zt = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.SSM;
  A380SecComputer_B.Data_fwxkftc3e = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.pitch_att_rate_deg_s.Data;
  A380SecComputer_B.SSM_kxxtac0ztg = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.roll_att_rate_deg_s.SSM;
  A380SecComputer_B.Data_fwxkftc3ep = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.roll_att_rate_deg_s.Data;
  A380SecComputer_B.SSM_kxxtac0ztgf = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.inertial_alt_ft.SSM;
  A380SecComputer_B.Data_fwxkftc3epg = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.inertial_alt_ft.Data;
  A380SecComputer_B.SSM_kxxtac0ztgf2 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.along_track_horiz_acc_g.SSM;
  A380SecComputer_B.Data_fwxkftc3epgt = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.along_track_horiz_acc_g.Data;
  A380SecComputer_B.SSM_kxxtac0ztgf2u = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.cross_track_horiz_acc_g.SSM;
  A380SecComputer_B.Data_fwxkftc3epgtd = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.cross_track_horiz_acc_g.Data;
  A380SecComputer_B.SSM_kxxtac0ztgf2ux = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.vertical_accel_g.SSM;
  A380SecComputer_B.Data_fwxkftc3epgtdx = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.vertical_accel_g.Data;
  A380SecComputer_B.SSM_kxxtac0ztgf2uxn =
    A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.inertial_vertical_speed_ft_s.SSM;
  A380SecComputer_B.Data_fwxkftc3epgtdxc =
    A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.inertial_vertical_speed_ft_s.Data;
  A380SecComputer_B.SSM_ky = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.north_south_velocity_kn.SSM;
  A380SecComputer_B.Data_h = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.north_south_velocity_kn.Data;
  A380SecComputer_B.SSM_d = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.east_west_velocity_kn.SSM;
  A380SecComputer_B.Data_e = A380SecComputer_P.out_Y0.data.bus_inputs.ir_1_bus.east_west_velocity_kn.Data;
  A380SecComputer_B.SSM_h = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.discrete_word_1.SSM;
  A380SecComputer_B.Data_j = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.discrete_word_1.Data;
  A380SecComputer_B.SSM_kb = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.latitude_deg.SSM;
  A380SecComputer_B.Data_d = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.latitude_deg.Data;
  A380SecComputer_B.SSM_p = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.longitude_deg.SSM;
  A380SecComputer_B.Data_p = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.longitude_deg.Data;
  A380SecComputer_B.SSM_di = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.ground_speed_kn.SSM;
  A380SecComputer_B.Data_i = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.ground_speed_kn.Data;
  A380SecComputer_B.SSM_j = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.track_angle_true_deg.SSM;
  A380SecComputer_B.Data_g = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.track_angle_true_deg.Data;
  A380SecComputer_B.SSM_i = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.heading_true_deg.SSM;
  A380SecComputer_B.Data_a = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.heading_true_deg.Data;
  A380SecComputer_B.SSM_g = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.wind_speed_kn.SSM;
  A380SecComputer_B.Data_eb = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.wind_speed_kn.Data;
  A380SecComputer_B.SSM_db = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.wind_direction_true_deg.SSM;
  A380SecComputer_B.Data_jo = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.wind_direction_true_deg.Data;
  A380SecComputer_B.SSM_n = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.track_angle_magnetic_deg.SSM;
  A380SecComputer_B.Data_ex = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.track_angle_magnetic_deg.Data;
  A380SecComputer_B.SSM_a = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.heading_magnetic_deg.SSM;
  A380SecComputer_B.Data_fd = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.heading_magnetic_deg.Data;
  A380SecComputer_B.SSM_ir = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.drift_angle_deg.SSM;
  A380SecComputer_B.Data_ja = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.drift_angle_deg.Data;
  A380SecComputer_B.SSM_hu = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.flight_path_angle_deg.SSM;
  A380SecComputer_B.Data_k = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.flight_path_angle_deg.Data;
  A380SecComputer_B.SSM_e = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.flight_path_accel_g.SSM;
  A380SecComputer_B.Data_joy = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.flight_path_accel_g.Data;
  A380SecComputer_B.SSM_gr = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.pitch_angle_deg.SSM;
  A380SecComputer_B.Data_h3 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.pitch_angle_deg.Data;
  A380SecComputer_B.SSM_ev = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.roll_angle_deg.SSM;
  A380SecComputer_B.Data_a0 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.roll_angle_deg.Data;
  A380SecComputer_B.SSM_l = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_pitch_rate_deg_s.SSM;
  A380SecComputer_B.Data_b = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_pitch_rate_deg_s.Data;
  A380SecComputer_B.SSM_ei = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_roll_rate_deg_s.SSM;
  A380SecComputer_B.Data_eq = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_roll_rate_deg_s.Data;
  A380SecComputer_B.SSM_an = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.SSM;
  A380SecComputer_B.Data_iz = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_yaw_rate_deg_s.Data;
  A380SecComputer_B.SSM_c = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_long_accel_g.SSM;
  A380SecComputer_B.Data_j2 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_long_accel_g.Data;
  A380SecComputer_B.SSM_cb = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_lat_accel_g.SSM;
  A380SecComputer_B.Data_o = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_lat_accel_g.Data;
  A380SecComputer_B.SSM_lb = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_normal_accel_g.SSM;
  A380SecComputer_B.Data_m = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.body_normal_accel_g.Data;
  A380SecComputer_B.SSM_ia = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.track_angle_rate_deg_s.SSM;
  A380SecComputer_B.Data_oq = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.track_angle_rate_deg_s.Data;
  A380SecComputer_B.SSM_kyz = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.SSM;
  A380SecComputer_B.Data_fo = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.pitch_att_rate_deg_s.Data;
  A380SecComputer_B.SSM_as = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.roll_att_rate_deg_s.SSM;
  A380SecComputer_B.Data_p1 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.roll_att_rate_deg_s.Data;
  A380SecComputer_B.SSM_is = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.inertial_alt_ft.SSM;
  A380SecComputer_B.Data_p1y = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.inertial_alt_ft.Data;
  A380SecComputer_B.SSM_ca = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.along_track_horiz_acc_g.SSM;
  A380SecComputer_B.Data_l = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.along_track_horiz_acc_g.Data;
  A380SecComputer_B.SSM_o = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.cross_track_horiz_acc_g.SSM;
  A380SecComputer_B.Data_kp = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.cross_track_horiz_acc_g.Data;
  A380SecComputer_B.SSM_ak = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.vertical_accel_g.SSM;
  A380SecComputer_B.Data_k0 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.vertical_accel_g.Data;
  A380SecComputer_B.SSM_cbj = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.inertial_vertical_speed_ft_s.SSM;
  A380SecComputer_B.Data_pi = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.inertial_vertical_speed_ft_s.Data;
  A380SecComputer_B.SSM_cu = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.north_south_velocity_kn.SSM;
  A380SecComputer_B.Data_dm = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.north_south_velocity_kn.Data;
  A380SecComputer_B.SSM_nn = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.east_west_velocity_kn.SSM;
  A380SecComputer_B.Data_f5 = A380SecComputer_P.out_Y0.data.bus_inputs.ir_2_bus.east_west_velocity_kn.Data;
  A380SecComputer_B.SSM_b = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.slat_flap_component_status_word.SSM;
  A380SecComputer_B.Data_js = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.slat_flap_component_status_word.Data;
  A380SecComputer_B.SSM_m = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.slat_flap_system_status_word.SSM;
  A380SecComputer_B.Data_ee = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.slat_flap_system_status_word.Data;
  A380SecComputer_B.SSM_f = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.slat_flap_actual_position_word.SSM;
  A380SecComputer_B.Data_ig = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.slat_flap_actual_position_word.Data;
  A380SecComputer_B.SSM_bp = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.slat_actual_position_deg.SSM;
  A380SecComputer_B.Data_mk = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.slat_actual_position_deg.Data;
  A380SecComputer_B.SSM_hb = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.flap_actual_position_deg.SSM;
  A380SecComputer_B.Data_pu = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_1_bus.flap_actual_position_deg.Data;
  A380SecComputer_B.SSM_gz = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.slat_flap_component_status_word.SSM;
  A380SecComputer_B.Data_ly = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.slat_flap_component_status_word.Data;
  A380SecComputer_B.SSM_pv = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.slat_flap_system_status_word.SSM;
  A380SecComputer_B.Data_jq = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.slat_flap_system_status_word.Data;
  A380SecComputer_B.SSM_mf = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.slat_flap_actual_position_word.SSM;
  A380SecComputer_B.Data_o5 = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.slat_flap_actual_position_word.Data;
  A380SecComputer_B.SSM_m0 = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.slat_actual_position_deg.SSM;
  A380SecComputer_B.Data_lyw = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.slat_actual_position_deg.Data;
  A380SecComputer_B.SSM_kd = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.flap_actual_position_deg.SSM;
  A380SecComputer_B.Data_gq = A380SecComputer_P.out_Y0.data.bus_inputs.sfcc_2_bus.flap_actual_position_deg.Data;
  A380SecComputer_B.irdc_5_a_bus = A380SecComputer_P.out_Y0.data.bus_inputs.irdc_5_a_bus;
  A380SecComputer_B.irdc_5_b_bus = A380SecComputer_P.out_Y0.data.bus_inputs.irdc_5_b_bus;
  A380SecComputer_B.SSM_pu = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_inboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_n = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_inboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_nv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_inboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_bq = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_inboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_d5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_midboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_dmn =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_midboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_eo = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_midboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_jn =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_midboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_nd = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_outboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_c = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_outboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_bq = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_outboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_lx =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_outboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_hi = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_1_command_deg.SSM;
  A380SecComputer_B.Data_jb = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_1_command_deg.Data;
  A380SecComputer_B.SSM_mm = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_1_command_deg.SSM;
  A380SecComputer_B.Data_fn = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_1_command_deg.Data;
  A380SecComputer_B.SSM_kz = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_2_command_deg.SSM;
  A380SecComputer_B.Data_od = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_2_command_deg.Data;
  A380SecComputer_B.SSM_il = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_2_command_deg.SSM;
  A380SecComputer_B.Data_ez = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_2_command_deg.Data;
  A380SecComputer_B.SSM_i2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_3_command_deg.SSM;
  A380SecComputer_B.Data_pw = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_3_command_deg.Data;
  A380SecComputer_B.SSM_ah = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_3_command_deg.SSM;
  A380SecComputer_B.Data_m2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_3_command_deg.Data;
  A380SecComputer_B.SSM_en = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_4_command_deg.SSM;
  A380SecComputer_B.Data_ek = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_4_command_deg.Data;
  A380SecComputer_B.SSM_dq = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_4_command_deg.SSM;
  A380SecComputer_B.Data_iy = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_4_command_deg.Data;
  A380SecComputer_B.SSM_px = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_5_command_deg.SSM;
  A380SecComputer_B.Data_lk = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_5_command_deg.Data;
  A380SecComputer_B.SSM_lbo = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_5_command_deg.SSM;
  A380SecComputer_B.Data_ca = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_5_command_deg.Data;
  A380SecComputer_B.SSM_p5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_6_command_deg.SSM;
  A380SecComputer_B.Data_pix = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_6_command_deg.Data;
  A380SecComputer_B.SSM_mk = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_6_command_deg.SSM;
  A380SecComputer_B.Data_di = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_6_command_deg.Data;
  A380SecComputer_B.SSM_mu = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_7_command_deg.SSM;
  A380SecComputer_B.Data_lz = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_7_command_deg.Data;
  A380SecComputer_B.SSM_cbl = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_7_command_deg.SSM;
  A380SecComputer_B.Data_lu = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_7_command_deg.Data;
  A380SecComputer_B.SSM_gzd = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_8_command_deg.SSM;
  A380SecComputer_B.Data_dc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_8_command_deg.Data;
  A380SecComputer_B.SSM_mo = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_8_command_deg.SSM;
  A380SecComputer_B.Data_gc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_8_command_deg.Data;
  A380SecComputer_B.SSM_me = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_inboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_am = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_inboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_mj = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_inboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_mo =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_inboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_a5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_outboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_dg =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_outboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_bt = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_outboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_e1 =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_outboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_om = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.ths_command_deg.SSM;
  A380SecComputer_B.Data_fp = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.ths_command_deg.Data;
  A380SecComputer_B.SSM_ar = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.upper_rudder_command_deg.SSM;
  A380SecComputer_B.Data_ns = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.upper_rudder_command_deg.Data;
  A380SecComputer_B.SSM_ce = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.lower_rudder_command_deg.SSM;
  A380SecComputer_B.Data_m3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.lower_rudder_command_deg.Data;
  A380SecComputer_B.SSM_ed = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_oj = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_jh = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_jy = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_je = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_j1 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_jt = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_fc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_cui = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.rudder_pedal_position_deg.SSM;
  A380SecComputer_B.Data_of = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.rudder_pedal_position_deg.Data;
  A380SecComputer_B.SSM_mq = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.aileron_status_word.SSM;
  A380SecComputer_B.Data_lg = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.aileron_status_word.Data;
  A380SecComputer_B.SSM_ni = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_n4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_df = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_ot = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_oe = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_gv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_ha = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_ou = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_op = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.spoiler_status_word.SSM;
  A380SecComputer_B.Data_dh = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.spoiler_status_word.Data;
  A380SecComputer_B.SSM_a50 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_position_deg.SSM;
  A380SecComputer_B.Data_ph = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.left_spoiler_position_deg.Data;
  A380SecComputer_B.SSM_og = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_position_deg.SSM;
  A380SecComputer_B.Data_gs = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.right_spoiler_position_deg.Data;
  A380SecComputer_B.SSM_a4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.elevator_status_word.SSM;
  A380SecComputer_B.Data_fd4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.elevator_status_word.Data;
  A380SecComputer_B.SSM_bv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.elevator_1_position_deg.SSM;
  A380SecComputer_B.Data_hm = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.elevator_1_position_deg.Data;
  A380SecComputer_B.SSM_bo = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.elevator_2_position_deg.SSM;
  A380SecComputer_B.Data_i2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.elevator_2_position_deg.Data;
  A380SecComputer_B.SSM_d1 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.elevator_3_position_deg.SSM;
  A380SecComputer_B.Data_og = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.elevator_3_position_deg.Data;
  A380SecComputer_B.SSM_hy = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.ths_position_deg.SSM;
  A380SecComputer_B.Data_fv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.ths_position_deg.Data;
  A380SecComputer_B.SSM_gi = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.rudder_status_word.SSM;
  A380SecComputer_B.Data_oc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.rudder_status_word.Data;
  A380SecComputer_B.SSM_pp = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.rudder_1_position_deg.SSM;
  A380SecComputer_B.Data_kq = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.rudder_1_position_deg.Data;
  A380SecComputer_B.SSM_iab = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.rudder_2_position_deg.SSM;
  A380SecComputer_B.Data_ne = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.rudder_2_position_deg.Data;
  A380SecComputer_B.SSM_jtv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.fctl_law_status_word.SSM;
  A380SecComputer_B.Data_it = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.fctl_law_status_word.Data;
  A380SecComputer_B.SSM_fy = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.misc_data_status_word.SSM;
  A380SecComputer_B.Data_ch = A380SecComputer_P.out_Y0.data.bus_inputs.prim_1_bus.misc_data_status_word.Data;
  A380SecComputer_B.SSM_d4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_inboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_bb = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_inboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_ars = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_inboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_ol = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_inboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_din = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_midboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_hw = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_midboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_m3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_midboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_hs =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_midboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_np = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_outboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_fj = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_outboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_ax = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_outboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_ky =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_outboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_cl = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_1_command_deg.SSM;
  A380SecComputer_B.Data_h5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_1_command_deg.Data;
  A380SecComputer_B.SSM_es = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_1_command_deg.SSM;
  A380SecComputer_B.Data_ku = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_1_command_deg.Data;
  A380SecComputer_B.SSM_gi1 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_2_command_deg.SSM;
  A380SecComputer_B.Data_jp = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_2_command_deg.Data;
  A380SecComputer_B.SSM_jz = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_2_command_deg.SSM;
  A380SecComputer_B.Data_nu = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_2_command_deg.Data;
  A380SecComputer_B.SSM_kt = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_3_command_deg.SSM;
  A380SecComputer_B.Data_br = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_3_command_deg.Data;
  A380SecComputer_B.SSM_ds = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_3_command_deg.SSM;
  A380SecComputer_B.Data_ae = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_3_command_deg.Data;
  A380SecComputer_B.SSM_eg = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_4_command_deg.SSM;
  A380SecComputer_B.Data_pe = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_4_command_deg.Data;
  A380SecComputer_B.SSM_a0 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_4_command_deg.SSM;
  A380SecComputer_B.Data_fy = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_4_command_deg.Data;
  A380SecComputer_B.SSM_cv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_5_command_deg.SSM;
  A380SecComputer_B.Data_na = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_5_command_deg.Data;
  A380SecComputer_B.SSM_ea = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_5_command_deg.SSM;
  A380SecComputer_B.Data_my = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_5_command_deg.Data;
  A380SecComputer_B.SSM_p4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_6_command_deg.SSM;
  A380SecComputer_B.Data_i4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_6_command_deg.Data;
  A380SecComputer_B.SSM_m2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_6_command_deg.SSM;
  A380SecComputer_B.Data_cx = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_6_command_deg.Data;
  A380SecComputer_B.SSM_bt0 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_7_command_deg.SSM;
  A380SecComputer_B.Data_nz = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_7_command_deg.Data;
  A380SecComputer_B.SSM_nr = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_7_command_deg.SSM;
  A380SecComputer_B.Data_id = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_7_command_deg.Data;
  A380SecComputer_B.SSM_fr = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_8_command_deg.SSM;
  A380SecComputer_B.Data_o2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_8_command_deg.Data;
  A380SecComputer_B.SSM_cc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_8_command_deg.SSM;
  A380SecComputer_B.Data_gqq = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_8_command_deg.Data;
  A380SecComputer_B.SSM_lm = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_inboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_md = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_inboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_mkm = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_inboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_cz =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_inboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_jhd = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_outboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_pm =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_outboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_av = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_outboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_bj =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_outboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_ira = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.ths_command_deg.SSM;
  A380SecComputer_B.Data_ox = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.ths_command_deg.Data;
  A380SecComputer_B.SSM_ge = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.upper_rudder_command_deg.SSM;
  A380SecComputer_B.Data_pe5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.upper_rudder_command_deg.Data;
  A380SecComputer_B.SSM_lv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.lower_rudder_command_deg.SSM;
  A380SecComputer_B.Data_jj = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.lower_rudder_command_deg.Data;
  A380SecComputer_B.SSM_cg = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_p5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_be = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_ekl =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_axb = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_nd = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_nz = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_n2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_cx = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.rudder_pedal_position_deg.SSM;
  A380SecComputer_B.Data_dl = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.rudder_pedal_position_deg.Data;
  A380SecComputer_B.SSM_gh = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.aileron_status_word.SSM;
  A380SecComputer_B.Data_gs2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.aileron_status_word.Data;
  A380SecComputer_B.SSM_ks = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_h4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_pw = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_e3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_fh = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_f5h = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_gzn = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_an = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_oo = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.spoiler_status_word.SSM;
  A380SecComputer_B.Data_i4o = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.spoiler_status_word.Data;
  A380SecComputer_B.SSM_evh = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_position_deg.SSM;
  A380SecComputer_B.Data_af = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.left_spoiler_position_deg.Data;
  A380SecComputer_B.SSM_cn = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_position_deg.SSM;
  A380SecComputer_B.Data_bm = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.right_spoiler_position_deg.Data;
  A380SecComputer_B.SSM_co = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.elevator_status_word.SSM;
  A380SecComputer_B.Data_dk = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.elevator_status_word.Data;
  A380SecComputer_B.SSM_pe = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.elevator_1_position_deg.SSM;
  A380SecComputer_B.Data_nv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.elevator_1_position_deg.Data;
  A380SecComputer_B.SSM_cgz = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.elevator_2_position_deg.SSM;
  A380SecComputer_B.Data_jpf = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.elevator_2_position_deg.Data;
  A380SecComputer_B.SSM_fw = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.elevator_3_position_deg.SSM;
  A380SecComputer_B.Data_i5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.elevator_3_position_deg.Data;
  A380SecComputer_B.SSM_h4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.ths_position_deg.SSM;
  A380SecComputer_B.Data_k2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.ths_position_deg.Data;
  A380SecComputer_B.SSM_cb3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.rudder_status_word.SSM;
  A380SecComputer_B.Data_as = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.rudder_status_word.Data;
  A380SecComputer_B.SSM_dv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.rudder_1_position_deg.SSM;
  A380SecComputer_B.Data_gk = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.rudder_1_position_deg.Data;
  A380SecComputer_B.SSM_i4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.rudder_2_position_deg.SSM;
  A380SecComputer_B.Data_jl = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.rudder_2_position_deg.Data;
  A380SecComputer_B.SSM_fm = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.fctl_law_status_word.SSM;
  A380SecComputer_B.Data_e32 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.fctl_law_status_word.Data;
  A380SecComputer_B.SSM_e5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.misc_data_status_word.SSM;
  A380SecComputer_B.Data_ih = A380SecComputer_P.out_Y0.data.bus_inputs.prim_2_bus.misc_data_status_word.Data;
  A380SecComputer_B.SSM_bf = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_inboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_du = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_inboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_fd = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_inboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_n0 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_inboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_fv = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_midboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_eqi =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_midboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_dt = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_midboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_om =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_midboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_j5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_outboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_nr = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_outboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_ng = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_outboard_aileron_command_deg.SSM;
  A380SecComputer_B.Data_p3 =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_outboard_aileron_command_deg.Data;
  A380SecComputer_B.SSM_ls = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_1_command_deg.SSM;
  A380SecComputer_B.Data_nb = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_1_command_deg.Data;
  A380SecComputer_B.SSM_dg = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_1_command_deg.SSM;
  A380SecComputer_B.Data_hd = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_1_command_deg.Data;
  A380SecComputer_B.SSM_d3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_2_command_deg.SSM;
  A380SecComputer_B.Data_al = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_2_command_deg.Data;
  A380SecComputer_B.SSM_p2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_2_command_deg.SSM;
  A380SecComputer_B.Data_gu = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_2_command_deg.Data;
  A380SecComputer_B.SSM_bo0 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_3_command_deg.SSM;
  A380SecComputer_B.Data_ix = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_3_command_deg.Data;
  A380SecComputer_B.SSM_bc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_3_command_deg.SSM;
  A380SecComputer_B.Data_hu = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_3_command_deg.Data;
  A380SecComputer_B.SSM_h0 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_4_command_deg.SSM;
  A380SecComputer_B.Data_pm1 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_4_command_deg.Data;
  A380SecComputer_B.SSM_giz = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_4_command_deg.SSM;
  A380SecComputer_B.Data_i2y = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_4_command_deg.Data;
  A380SecComputer_B.SSM_mqp = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_5_command_deg.SSM;
  A380SecComputer_B.Data_pg = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_5_command_deg.Data;
  A380SecComputer_B.SSM_ba = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_5_command_deg.SSM;
  A380SecComputer_B.Data_ni = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_5_command_deg.Data;
  A380SecComputer_B.SSM_ff = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_6_command_deg.SSM;
  A380SecComputer_B.Data_fr = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_6_command_deg.Data;
  A380SecComputer_B.SSM_ic = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_6_command_deg.SSM;
  A380SecComputer_B.Data_cn = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_6_command_deg.Data;
  A380SecComputer_B.SSM_fs = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_7_command_deg.SSM;
  A380SecComputer_B.Data_nxl = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_7_command_deg.Data;
  A380SecComputer_B.SSM_ja = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_7_command_deg.SSM;
  A380SecComputer_B.Data_jh = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_7_command_deg.Data;
  A380SecComputer_B.SSM_js = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_8_command_deg.SSM;
  A380SecComputer_B.Data_gl = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_8_command_deg.Data;
  A380SecComputer_B.SSM_is3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_8_command_deg.SSM;
  A380SecComputer_B.Data_myb = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_8_command_deg.Data;
  A380SecComputer_B.SSM_ag = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_inboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_l2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_inboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_f5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_inboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_o5o =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_inboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_ph = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_outboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_l5 =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_outboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_jw = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_outboard_elevator_command_deg.SSM;
  A380SecComputer_B.Data_dc2 =
    A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_outboard_elevator_command_deg.Data;
  A380SecComputer_B.SSM_j1 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.ths_command_deg.SSM;
  A380SecComputer_B.Data_gr = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.ths_command_deg.Data;
  A380SecComputer_B.SSM_ov = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.upper_rudder_command_deg.SSM;
  A380SecComputer_B.Data_gp = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.upper_rudder_command_deg.Data;
  A380SecComputer_B.SSM_mx = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.lower_rudder_command_deg.SSM;
  A380SecComputer_B.Data_i3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.lower_rudder_command_deg.Data;
  A380SecComputer_B.SSM_b4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_et = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_gb = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_mc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_oh = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_f2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_mm5 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_gh = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_br = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.rudder_pedal_position_deg.SSM;
  A380SecComputer_B.Data_ed = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.rudder_pedal_position_deg.Data;
  A380SecComputer_B.SSM_c2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.aileron_status_word.SSM;
  A380SecComputer_B.Data_o2j = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.aileron_status_word.Data;
  A380SecComputer_B.SSM_hc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_i43 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_gl = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_ic = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_my = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_ak = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_j3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_jg = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_go = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.spoiler_status_word.SSM;
  A380SecComputer_B.Data_cu = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.spoiler_status_word.Data;
  A380SecComputer_B.SSM_e5c = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_position_deg.SSM;
  A380SecComputer_B.Data_ep = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.left_spoiler_position_deg.Data;
  A380SecComputer_B.SSM_dk = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_position_deg.SSM;
  A380SecComputer_B.Data_bt = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.right_spoiler_position_deg.Data;
  A380SecComputer_B.SSM_evc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.elevator_status_word.SSM;
  A380SecComputer_B.Data_e0 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.elevator_status_word.Data;
  A380SecComputer_B.SSM_kk = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.elevator_1_position_deg.SSM;
  A380SecComputer_B.Data_jl3 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.elevator_1_position_deg.Data;
  A380SecComputer_B.SSM_af = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.elevator_2_position_deg.SSM;
  A380SecComputer_B.Data_nm = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.elevator_2_position_deg.Data;
  A380SecComputer_B.SSM_npr = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.elevator_3_position_deg.SSM;
  A380SecComputer_B.Data_ia = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.elevator_3_position_deg.Data;
  A380SecComputer_B.SSM_lt = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.ths_position_deg.SSM;
  A380SecComputer_B.Data_j0 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.ths_position_deg.Data;
  A380SecComputer_B.SSM_ger = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.rudder_status_word.SSM;
  A380SecComputer_B.Data_bs = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.rudder_status_word.Data;
  A380SecComputer_B.SSM_pxo = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.rudder_1_position_deg.SSM;
  A380SecComputer_B.Data_hp = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.rudder_1_position_deg.Data;
  A380SecComputer_B.SSM_co2 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.rudder_2_position_deg.SSM;
  A380SecComputer_B.Data_ct = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.rudder_2_position_deg.Data;
  A380SecComputer_B.SSM_ny = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.fctl_law_status_word.SSM;
  A380SecComputer_B.Data_pc = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.fctl_law_status_word.Data;
  A380SecComputer_B.SSM_l4 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.misc_data_status_word.SSM;
  A380SecComputer_B.Data_c0 = A380SecComputer_P.out_Y0.data.bus_inputs.prim_3_bus.misc_data_status_word.Data;
  A380SecComputer_B.SSM_nm = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_ojg = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_nh = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_lm = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_dl = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_fz = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_dx = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_oz = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_fl = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.rudder_pedal_position_deg.SSM;
  A380SecComputer_B.Data_gf = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.rudder_pedal_position_deg.Data;
  A380SecComputer_B.SSM_p3 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.aileron_status_word.SSM;
  A380SecComputer_B.Data_nn = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.aileron_status_word.Data;
  A380SecComputer_B.SSM_ns = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_a0z = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_bm = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_fk = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_nl = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_bu = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_grm = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_g3 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_gzm = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.spoiler_status_word.SSM;
  A380SecComputer_B.Data_icc = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.spoiler_status_word.Data;
  A380SecComputer_B.SSM_oi = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_spoiler_1_position_deg.SSM;
  A380SecComputer_B.Data_pwf = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_spoiler_1_position_deg.Data;
  A380SecComputer_B.SSM_aa = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_spoiler_1_position_deg.SSM;
  A380SecComputer_B.Data_gvk = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_spoiler_1_position_deg.Data;
  A380SecComputer_B.SSM_fvk = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_spoiler_2_position_deg.SSM;
  A380SecComputer_B.Data_ln = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.left_spoiler_2_position_deg.Data;
  A380SecComputer_B.SSM_fa = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_spoiler_2_position_deg.SSM;
  A380SecComputer_B.Data_ka = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.right_spoiler_2_position_deg.Data;
  A380SecComputer_B.SSM_lbx = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.elevator_status_word.SSM;
  A380SecComputer_B.Data_mp = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.elevator_status_word.Data;
  A380SecComputer_B.SSM_n3 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.elevator_1_position_deg.SSM;
  A380SecComputer_B.Data_m4 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.elevator_1_position_deg.Data;
  A380SecComputer_B.SSM_a1 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.elevator_2_position_deg.SSM;
  A380SecComputer_B.Data_fki = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.elevator_2_position_deg.Data;
  A380SecComputer_B.SSM_p1 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.elevator_3_position_deg.SSM;
  A380SecComputer_B.Data_bv = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.elevator_3_position_deg.Data;
  A380SecComputer_B.SSM_cn2 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.ths_position_deg.SSM;
  A380SecComputer_B.Data_nbg = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.ths_position_deg.Data;
  A380SecComputer_B.SSM_an3 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.rudder_status_word.SSM;
  A380SecComputer_B.Data_l25 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.rudder_status_word.Data;
  A380SecComputer_B.SSM_c3 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.rudder_1_position_deg.SSM;
  A380SecComputer_B.Data_ki = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.rudder_1_position_deg.Data;
  A380SecComputer_B.SSM_dp = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.rudder_2_position_deg.SSM;
  A380SecComputer_B.Data_p5p = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.rudder_2_position_deg.Data;
  A380SecComputer_B.SSM_boy = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.fctl_law_status_word.SSM;
  A380SecComputer_B.Data_nry = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.fctl_law_status_word.Data;
  A380SecComputer_B.SSM_cm = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.misc_data_status_word.SSM;
  A380SecComputer_B.Data_mh = A380SecComputer_P.out_Y0.data.bus_inputs.sec_x_bus.misc_data_status_word.Data;
  A380SecComputer_B.SSM_hl = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_ll = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_irh = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_sidestick_pitch_command_deg.SSM;
  A380SecComputer_B.Data_hy = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_sidestick_pitch_command_deg.Data;
  A380SecComputer_B.SSM_b42 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_j04 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_anz = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_sidestick_roll_command_deg.SSM;
  A380SecComputer_B.Data_pf = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_sidestick_roll_command_deg.Data;
  A380SecComputer_B.SSM_d2 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.rudder_pedal_position_deg.SSM;
  A380SecComputer_B.Data_gb = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.rudder_pedal_position_deg.Data;
  A380SecComputer_B.SSM_gov = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.aileron_status_word.SSM;
  A380SecComputer_B.Data_hq = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.aileron_status_word.Data;
  A380SecComputer_B.SSM_nb = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_ai = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_pe3 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_gfr = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_jj = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_aileron_1_position_deg.SSM;
  A380SecComputer_B.Data_czp = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_aileron_1_position_deg.Data;
  A380SecComputer_B.SSM_npl = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_aileron_2_position_deg.SSM;
  A380SecComputer_B.Data_fm = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_aileron_2_position_deg.Data;
  A380SecComputer_B.SSM_gf = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.spoiler_status_word.SSM;
  A380SecComputer_B.Data_jsg = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.spoiler_status_word.Data;
  A380SecComputer_B.SSM_gbi = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_spoiler_1_position_deg.SSM;
  A380SecComputer_B.Data_g1 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_spoiler_1_position_deg.Data;
  A380SecComputer_B.SSM_fhm = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_spoiler_1_position_deg.SSM;
  A380SecComputer_B.Data_j4 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_spoiler_1_position_deg.Data;
  A380SecComputer_B.SSM_ltj = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_spoiler_2_position_deg.SSM;
  A380SecComputer_B.Data_jyh = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.left_spoiler_2_position_deg.Data;
  A380SecComputer_B.SSM_hn = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_spoiler_2_position_deg.SSM;
  A380SecComputer_B.Data_ghs = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.right_spoiler_2_position_deg.Data;
  A380SecComputer_B.SSM_h3 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.elevator_status_word.SSM;
  A380SecComputer_B.Data_bmk = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.elevator_status_word.Data;
  A380SecComputer_B.SSM_bfs = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.elevator_1_position_deg.SSM;
  A380SecComputer_B.Data_lzt = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.elevator_1_position_deg.Data;
  A380SecComputer_B.SSM_p0 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.elevator_2_position_deg.SSM;
  A380SecComputer_B.Data_kn = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.elevator_2_position_deg.Data;
  A380SecComputer_B.SSM_fu = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.elevator_3_position_deg.SSM;
  A380SecComputer_B.Data_nab = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.elevator_3_position_deg.Data;
  A380SecComputer_B.SSM_bi = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.ths_position_deg.SSM;
  A380SecComputer_B.Data_lgf = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.ths_position_deg.Data;
  A380SecComputer_B.SSM_bd = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.rudder_status_word.SSM;
  A380SecComputer_B.Data_fpq = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.rudder_status_word.Data;
  A380SecComputer_B.SSM_omt = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.rudder_1_position_deg.SSM;
  A380SecComputer_B.Data_dt = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.rudder_1_position_deg.Data;
  A380SecComputer_B.SSM_la = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.rudder_2_position_deg.SSM;
  A380SecComputer_B.Data_b1 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.rudder_2_position_deg.Data;
  A380SecComputer_B.SSM_l1 = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.fctl_law_status_word.SSM;
  A380SecComputer_B.Data_nmr = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.fctl_law_status_word.Data;
  A380SecComputer_B.SSM_dy = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.misc_data_status_word.SSM;
  A380SecComputer_B.Data_nib = A380SecComputer_P.out_Y0.data.bus_inputs.sec_y_bus.misc_data_status_word.Data;
  A380SecComputer_B.laws = A380SecComputer_P.out_Y0.laws;
  A380SecComputer_B.logic = A380SecComputer_P.out_Y0.logic;
  A380SecComputer_Y.out.discrete_outputs = A380SecComputer_P.out_Y0.discrete_outputs;
  A380SecComputer_Y.out.analog_outputs = A380SecComputer_P.out_Y0.analog_outputs;
  A380SecComputer_Y.out.bus_outputs = A380SecComputer_P.out_Y0.bus_outputs;
}

void A380SecComputer::terminate()
{
}

A380SecComputer::A380SecComputer():
  A380SecComputer_U(),
  A380SecComputer_Y(),
  A380SecComputer_B(),
  A380SecComputer_DWork()
{
}

A380SecComputer::~A380SecComputer()
{
}
