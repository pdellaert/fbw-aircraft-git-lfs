#ifndef RTW_HEADER_AutopilotLaws_h_
#define RTW_HEADER_AutopilotLaws_h_
#include <cmath>
#include "rtwtypes.h"
#include "AutopilotLaws_types.h"

class AutopilotLawsModelClass
{
 public:
  struct rtDW_LagFilter_AutopilotLaws_T {
    real_T pY;
    real_T pU;
    boolean_T pY_not_empty;
    boolean_T pU_not_empty;
  };

  struct rtDW_RateLimiter_AutopilotLaws_T {
    real_T pY;
    boolean_T pY_not_empty;
  };

  struct rtDW_MATLABFunction_AutopilotLaws_d_T {
    real_T limit;
    real_T lastPsi;
    real_T limitDeltaPsi;
    boolean_T lastPsi_not_empty;
  };

  struct rtDW_Chart_AutopilotLaws_T {
    uint8_T is_active_c10_AutopilotLaws;
    uint8_T is_c10_AutopilotLaws;
  };

  struct rtDW_Chart_AutopilotLaws_m_T {
    uint8_T is_active_c15_AutopilotLaws;
    uint8_T is_c15_AutopilotLaws;
  };

  struct rtDW_storevalue_AutopilotLaws_T {
    real_T storage;
    boolean_T storage_not_empty;
  };

  struct rtDW_LeadLagFilter_AutopilotLaws_T {
    real_T pY;
    real_T pU;
    boolean_T pY_not_empty;
    boolean_T pU_not_empty;
  };

  struct rtDW_WashoutFilter_AutopilotLaws_T {
    real_T pY;
    real_T pU;
    boolean_T pY_not_empty;
    boolean_T pU_not_empty;
  };

  struct BlockIO_AutopilotLaws_T {
    real_T u;
  };

  struct D_Work_AutopilotLaws_T {
    real_T DelayInput1_DSTATE;
    real_T DelayInput1_DSTATE_g;
    real_T Delay_DSTATE;
    real_T Delay_DSTATE_e;
    real_T Delay_DSTATE_h;
    real_T Delay_DSTATE_hi;
    real_T Delay_DSTATE_n;
    real_T Delay_DSTATE_h2;
    real_T prevVerticalLaw;
    real_T prevTarget;
    real_T prevVerticalLaw_b;
    real_T prevTarget_k;
    real_T Tau;
    real_T H_bias;
    real_T dH_offset;
    real_T k;
    real_T maxH_dot;
    real_T nav_gs_deg;
    real_T pY_g;
    real_T limit;
    boolean_T Delay_DSTATE_l[100];
    boolean_T Delay_DSTATE_h5[100];
    uint8_T is_active_c5_AutopilotLaws;
    uint8_T is_c5_AutopilotLaws;
    boolean_T icLoad;
    boolean_T icLoad_f;
    boolean_T prevVerticalLaw_not_empty;
    boolean_T prevTarget_not_empty;
    boolean_T islevelOffActive;
    boolean_T prevVerticalLaw_not_empty_n;
    boolean_T prevTarget_not_empty_j;
    boolean_T islevelOffActive_k;
    boolean_T pY_not_empty;
    boolean_T wasActive;
    boolean_T wasActive_not_empty;
    boolean_T wasActive_c;
    boolean_T wasActive_not_empty_p;
    boolean_T nav_gs_deg_not_empty;
    boolean_T pY_not_empty_d;
    boolean_T limit_not_empty;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_j;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_p;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_c;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_fs;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_kq;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_b;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_d;
    rtDW_RateLimiter_AutopilotLaws_T sf_RateLimiter_h;
    rtDW_RateLimiter_AutopilotLaws_T sf_RateLimiter_eb;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_f;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_gn;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_p;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_i;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_n;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_g;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_nq;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_ja;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_es;
    rtDW_storevalue_AutopilotLaws_T sf_storevalue_g;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_n;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_bx;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_cu;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_j;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_g5;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_a;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_j;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_h;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_kp;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_e;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_k;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_g;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_hp;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_k;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_cs;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_b;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_a;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_g;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter_d;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_m;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_h;
    rtDW_WashoutFilter_AutopilotLaws_T sf_WashoutFilter;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter_o;
    rtDW_LeadLagFilter_AutopilotLaws_T sf_LeadLagFilter;
    rtDW_Chart_AutopilotLaws_T sf_Chart_ba;
    rtDW_MATLABFunction_AutopilotLaws_d_T sf_MATLABFunction_e;
    rtDW_RateLimiter_AutopilotLaws_T sf_RateLimiter_d;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_o;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_mp;
    rtDW_storevalue_AutopilotLaws_T sf_storevalue;
    rtDW_Chart_AutopilotLaws_m_T sf_Chart_b;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_h2;
    rtDW_Chart_AutopilotLaws_m_T sf_Chart_h;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_h;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_c;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter_m;
    rtDW_Chart_AutopilotLaws_T sf_Chart;
    rtDW_MATLABFunction_AutopilotLaws_d_T sf_MATLABFunction_m;
    rtDW_RateLimiter_AutopilotLaws_T sf_RateLimiter;
    rtDW_LagFilter_AutopilotLaws_T sf_LagFilter;
  };

  struct ExternalInputs_AutopilotLaws_T {
    ap_laws_input in;
  };

  struct ExternalOutputs_AutopilotLaws_T {
    ap_laws_output out;
  };

  struct Parameters_AutopilotLaws_T {
    ap_laws_output ap_laws_output_MATLABStruct;
    real_T ScheduledGain_BreakpointsForDimension1[3];
    real_T ScheduledGain_BreakpointsForDimension1_h[7];
    real_T ScheduledGain_BreakpointsForDimension1_o[7];
    real_T ScheduledGain2_BreakpointsForDimension1[7];
    real_T ScheduledGain_BreakpointsForDimension1_e[5];
    real_T ScheduledGain2_BreakpointsForDimension1_h[5];
    real_T ScheduledGain_BreakpointsForDimension1_ec[7];
    real_T ScheduledGain1_BreakpointsForDimension1[7];
    real_T LagFilter_C1;
    real_T LagFilter2_C1;
    real_T LagFilter_C1_n;
    real_T LagFilter_C1_a;
    real_T LagFilter1_C1;
    real_T LagFilter_C1_k;
    real_T LagFilter_C1_l;
    real_T WashoutFilter_C1;
    real_T LagFilter_C1_ai;
    real_T WashoutFilter_C1_e;
    real_T HighPassFilter_C1;
    real_T LowPassFilter_C1;
    real_T WashoutFilter_C1_e4;
    real_T HighPassFilter_C1_e;
    real_T LowPassFilter_C1_n;
    real_T WashoutFilter_C1_k;
    real_T HighPassFilter_C1_b;
    real_T LowPassFilter_C1_d;
    real_T WashoutFilter_C1_o;
    real_T HighPassFilter_C1_g;
    real_T LowPassFilter_C1_m;
    real_T WashoutFilter_C1_p;
    real_T HighPassFilter_C1_n;
    real_T LowPassFilter_C1_l;
    real_T WashoutFilter_C1_j;
    real_T HighPassFilter_C1_i;
    real_T LowPassFilter_C1_l4;
    real_T WashoutFilter_C1_c;
    real_T HighPassFilter_C1_d;
    real_T LowPassFilter_C1_e;
    real_T LagFilter2_C1_k;
    real_T WashoutFilter1_C1;
    real_T LagFilter1_C1_p;
    real_T LagFilter_C1_m;
    real_T WashoutFilter_C1_m;
    real_T LagFilterH_C1;
    real_T LeadLagFilter_C1;
    real_T LagFilter1_C1_d;
    real_T LeadLagFilter_C1_a;
    real_T LagFilterH1_C1;
    real_T WashoutFilter_C1_cn;
    real_T HighPassFilter_C1_gw;
    real_T LowPassFilter_C1_d1;
    real_T LagFilter_C1_b;
    real_T LagFilter_C1_g;
    real_T WashoutFilter1_C1_h;
    real_T LagFilter_C1_i;
    real_T HighPassFilter_C2;
    real_T LowPassFilter_C2;
    real_T HighPassFilter_C2_c;
    real_T LowPassFilter_C2_a;
    real_T HighPassFilter_C2_g;
    real_T LowPassFilter_C2_p;
    real_T HighPassFilter_C2_l;
    real_T LowPassFilter_C2_l;
    real_T HighPassFilter_C2_m;
    real_T LowPassFilter_C2_c;
    real_T HighPassFilter_C2_h;
    real_T LowPassFilter_C2_po;
    real_T HighPassFilter_C2_i;
    real_T LowPassFilter_C2_i;
    real_T LeadLagFilter_C2;
    real_T LeadLagFilter_C2_p;
    real_T HighPassFilter_C2_e;
    real_T LowPassFilter_C2_e;
    real_T HighPassFilter_C3;
    real_T LowPassFilter_C3;
    real_T HighPassFilter_C3_f;
    real_T LowPassFilter_C3_o;
    real_T HighPassFilter_C3_n;
    real_T LowPassFilter_C3_a;
    real_T HighPassFilter_C3_j;
    real_T LowPassFilter_C3_i;
    real_T HighPassFilter_C3_k;
    real_T LowPassFilter_C3_g;
    real_T HighPassFilter_C3_m;
    real_T LowPassFilter_C3_f;
    real_T HighPassFilter_C3_d;
    real_T LowPassFilter_C3_o5;
    real_T LeadLagFilter_C3;
    real_T LeadLagFilter_C3_m;
    real_T HighPassFilter_C3_di;
    real_T LowPassFilter_C3_l;
    real_T HighPassFilter_C4;
    real_T LowPassFilter_C4;
    real_T HighPassFilter_C4_c;
    real_T LowPassFilter_C4_o;
    real_T HighPassFilter_C4_b;
    real_T LowPassFilter_C4_b;
    real_T HighPassFilter_C4_i;
    real_T LowPassFilter_C4_k;
    real_T HighPassFilter_C4_h;
    real_T LowPassFilter_C4_d;
    real_T HighPassFilter_C4_n;
    real_T LowPassFilter_C4_dt;
    real_T HighPassFilter_C4_nr;
    real_T LowPassFilter_C4_f;
    real_T LeadLagFilter_C4;
    real_T LeadLagFilter_C4_k;
    real_T HighPassFilter_C4_a;
    real_T LowPassFilter_C4_a;
    real_T DiscreteTimeIntegratorVariableTs_Gain;
    real_T DiscreteDerivativeVariableTs_Gain;
    real_T VS_Gain;
    real_T VS_Gain_h;
    real_T DiscreteDerivativeVariableTs1_Gain;
    real_T DiscreteDerivativeVariableTs_Gain_o;
    real_T VS_Gain_n;
    real_T VS_Gain_j;
    real_T VS_Gain_e;
    real_T VS1_Gain;
    real_T VS_Gain_n5;
    real_T VS_Gain_nx;
    real_T VS_Gain_ne;
    real_T DiscreteTimeIntegratorVariableTs_InitialCondition;
    real_T RateLimiterVariableTs_InitialCondition;
    real_T DiscreteDerivativeVariableTs_InitialCondition;
    real_T RateLimiterVariableTs_InitialCondition_i;
    real_T RateLimiterVariableTs_InitialCondition_il;
    real_T DiscreteDerivativeVariableTs1_InitialCondition;
    real_T DiscreteDerivativeVariableTs_InitialCondition_c;
    real_T RateLimiterVariableTs1_InitialCondition;
    real_T RateLimiterVariableTs_InitialCondition_p;
    real_T DiscreteTimeIntegratorVariableTs_LowerLimit;
    real_T ScheduledGain_Table[3];
    real_T ScheduledGain_Table_o[7];
    real_T ScheduledGain_Table_e[7];
    real_T ScheduledGain2_Table[7];
    real_T ScheduledGain_Table_p[5];
    real_T ScheduledGain2_Table_f[5];
    real_T ScheduledGain_Table_l[7];
    real_T ScheduledGain1_Table[7];
    real_T DiscreteTimeIntegratorVariableTs_UpperLimit;
    real_T Subsystem_Value;
    real_T Subsystem_Value_n;
    real_T CompareToConstant2_const;
    real_T CompareToConstant_const;
    real_T CompareToConstant5_const;
    real_T CompareToConstant4_const;
    real_T CompareToConstant_const_d;
    real_T CompareToConstant5_const_e;
    real_T CompareToConstant1_const;
    real_T CompareToConstant6_const;
    real_T CompareToConstant5_const_a;
    real_T CompareToConstant2_const_d;
    real_T CompareToConstant2_const_e;
    real_T CompareToConstant_const_l;
    real_T CompareToConstant4_const_o;
    real_T CompareGSTRACK_const;
    real_T CompareGSTRACK2_const;
    real_T CompareToConstant_const_k;
    real_T CompareToConstant6_const_e;
    real_T CompareToConstant7_const;
    real_T CompareToConstant6_const_d;
    real_T CompareToConstant5_const_h;
    real_T CompareToConstant2_const_j;
    real_T CompareToConstant8_const;
    real_T CompareToConstant_const_h;
    real_T CompareToConstant4_const_e;
    real_T RateLimiterVariableTs_lo;
    real_T RateLimiterVariableTs_lo_k;
    real_T RateLimiterVariableTs_lo_b;
    real_T RateLimiterVariableTs1_lo;
    real_T RateLimiterVariableTs_lo_o;
    real_T RateLimiterVariableTs_up;
    real_T RateLimiterVariableTs_up_n;
    real_T RateLimiterVariableTs_up_b;
    real_T RateLimiterVariableTs1_up;
    real_T RateLimiterVariableTs_up_i;
    real_T DetectChange_vinit;
    real_T DetectChange1_vinit;
    boolean_T CompareToConstant_const_hx;
    boolean_T CompareToConstant_const_e;
    real_T Gain1_Gain;
    real_T Gain_Gain;
    real_T Gain1_Gain_p;
    real_T Gain_Gain_a;
    real_T Gain1_Gain_h;
    real_T Gain_Gain_e;
    real_T Gain1_Gain_k;
    real_T Gain_Gain_ae;
    real_T Gain2_Gain;
    real_T Gain_Gain_c;
    real_T Saturation_UpperSat;
    real_T Saturation_LowerSat;
    real_T k_beta_Phi_Gain;
    real_T Gain1_Gain_g;
    real_T Gain1_Gain_l;
    real_T Gain_Gain_f;
    real_T Gain_Gain_g;
    real_T Constant3_Value;
    real_T Constant3_Value_c;
    real_T Constant3_Value_d;
    real_T Gain4_Gain;
    real_T Gain1_Gain_b;
    real_T Gain2_Gain_g;
    real_T Saturation1_UpperSat;
    real_T Saturation1_LowerSat;
    real_T Constant3_Value_h;
    real_T beta_Value;
    real_T beta_Value_e;
    real_T beta_Value_b;
    real_T beta_Value_i;
    real_T beta_Value_c;
    real_T Saturation_UpperSat_e;
    real_T Saturation_LowerSat_f;
    real_T Gain7_Gain;
    real_T Gain1_Gain_j;
    real_T Gain_Gain_i;
    real_T Gain1_Gain_lt;
    real_T beta1_Value;
    real_T beta1_Value_h;
    real_T beta1_Value_l;
    real_T beta1_Value_m;
    real_T beta1_Value_d;
    real_T beta1_Value_hy;
    real_T Gain3_Gain;
    real_T Constant1_Value;
    real_T Constant_Value;
    real_T Constant_Value_g;
    real_T Gain6_Gain;
    real_T Switch1_Threshold;
    real_T Constant1_Value_g;
    real_T Gain5_Gain;
    real_T Constant_Value_p;
    real_T Gain6_Gain_j;
    real_T Switch1_Threshold_f;
    real_T Constant1_Value_a;
    real_T Gain5_Gain_l;
    real_T Y_Y0;
    real_T Gain1_Gain_bs;
    real_T Saturation1_UpperSat_a;
    real_T Saturation1_LowerSat_i;
    real_T Constant1_Value_h;
    real_T Constant_Value_p0;
    real_T Gain6_Gain_n;
    real_T Switch1_Threshold_d;
    real_T Constant1_Value_m;
    real_T Gain5_Gain_b;
    real_T Constant_Value_i;
    real_T Gain6_Gain_g;
    real_T Switch1_Threshold_h;
    real_T Constant1_Value_f;
    real_T Gain5_Gain_a;
    real_T Switch_Threshold;
    real_T Gain1_Gain_d;
    real_T Gain2_Gain_h;
    real_T Gain1_Gain_i;
    real_T Gain6_Gain_f;
    real_T Switch1_Threshold_j;
    real_T Constant_Value_f;
    real_T Constant1_Value_c;
    real_T Gain1_Gain_kg;
    real_T Constant2_Value;
    real_T Gain6_Gain_a;
    real_T Gain5_Gain_k;
    real_T Switch_Threshold_h;
    real_T Constant_Value_o;
    real_T Constant1_Value_g5;
    real_T Gain1_Gain_m;
    real_T Constant2_Value_m;
    real_T Gain6_Gain_fa;
    real_T Gain5_Gain_n;
    real_T Switch_Threshold_hz;
    real_T Switch_Threshold_k;
    real_T Gain1_Gain_m4;
    real_T Gain1_Gain_n;
    real_T Constant_Value_m;
    real_T Constant_Value_b;
    real_T Gain6_Gain_ai;
    real_T Switch1_Threshold_c;
    real_T Constant1_Value_m5;
    real_T Gain5_Gain_h;
    real_T Constant_Value_ow;
    real_T Gain6_Gain_c;
    real_T Switch1_Threshold_b;
    real_T Constant1_Value_mi;
    real_T Gain5_Gain_g;
    real_T GainTheta_Gain;
    real_T GainTheta1_Gain;
    real_T Gain_Gain_d;
    real_T Gainqk_Gain;
    real_T Gain_Gain_m;
    real_T Gain_Gain_de;
    real_T Gainpk_Gain;
    real_T Gain3_Gain_a;
    real_T Gain_Gain_n;
    real_T Constant1_Value_b;
    real_T Saturation_UpperSat_p;
    real_T Saturation_LowerSat_g;
    real_T Gain1_Gain_ll;
    real_T Saturation1_UpperSat_j;
    real_T Saturation1_LowerSat_d;
    real_T Gain2_Gain_b;
    real_T Constant3_Value_e;
    real_T Constant3_Value_b;
    real_T Constant3_Value_i;
    real_T Constant3_Value_n;
    real_T tau_Value;
    real_T zeta_Value;
    real_T Gain1_Gain_f;
    real_T Saturation_UpperSat_b;
    real_T Saturation_LowerSat_n;
    real_T Gain_Gain_h;
    real_T Constant3_Value_p;
    real_T Constant3_Value_c2;
    real_T Gain2_Gain_i;
    real_T Constant3_Value_nr;
    real_T Constant3_Value_hr;
    real_T Gain1_Gain_nr;
    real_T Saturation_UpperSat_o;
    real_T Saturation_LowerSat_o;
    real_T Gain2_Gain_gs;
    real_T Saturation1_UpperSat_g;
    real_T Saturation1_LowerSat_k;
    real_T Gain6_Gain_b;
    real_T Constant3_Value_n1;
    real_T Constant3_Value_o;
    real_T Constant3_Value_dk;
    real_T Gain1_Gain_fq;
    real_T Gain_Gain_o;
    real_T Constant1_Value_e;
    real_T Constant3_Value_m;
    real_T Constant3_Value_if;
    real_T Gain_Gain_fn;
    real_T Constant2_Value_l;
    real_T Constant3_Value_cd;
    real_T Gain_Gain_cy;
    real_T Gain1_Gain_o;
    real_T Gain_Gain_o5;
    real_T Constant3_Value_k;
    real_T Gain_Gain_p;
    real_T Gain1_Gain_i4;
    real_T Gain_Gain_l;
    real_T tau_Value_c;
    real_T zeta_Value_h;
    real_T Gain3_Gain_i;
    real_T Constant1_Value_fk;
    real_T Gain_Gain_lu;
    real_T Gain5_Gain_o;
    real_T Gain_Gain_b;
    real_T Saturation_UpperSat_k;
    real_T Saturation_LowerSat_f3;
    real_T Constant_Value_a;
    real_T Gain4_Gain_o;
    real_T Switch_Threshold_n;
    real_T Gain_Gain_m3;
    real_T Saturation_UpperSat_c;
    real_T Saturation_LowerSat_d;
    real_T Constant2_Value_h;
    real_T Gain1_Gain_kf;
    real_T Saturation_UpperSat_m;
    real_T Saturation_LowerSat_fw;
    real_T Constant_Value_ii;
    real_T Constant1_Value_i;
    real_T Constant_Value_d;
    real_T Gain_Gain_ft;
    real_T Saturation_UpperSat_n;
    real_T Saturation_LowerSat_d4;
    real_T ftmintoms_Gain;
    real_T kntoms_Gain;
    real_T Saturation_UpperSat_a;
    real_T Saturation_LowerSat_n5;
    real_T Gain_Gain_k;
    real_T ftmintoms_Gain_c;
    real_T kntoms_Gain_h;
    real_T Saturation_UpperSat_d;
    real_T Saturation_LowerSat_nr;
    real_T Gain_Gain_es;
    real_T fpmtoms_Gain;
    real_T kntoms_Gain_m;
    real_T Saturation_UpperSat_j;
    real_T Saturation_LowerSat_i;
    real_T Gain_Gain_e3;
    real_T Gain1_Gain_go;
    real_T fpmtoms_Gain_g;
    real_T kntoms_Gain_b;
    real_T Saturation_UpperSat_ei;
    real_T Saturation_LowerSat_dz;
    real_T Gain_Gain_c1;
    real_T Gain1_Gain_lx;
    real_T Constant_Value_dy;
    real_T Gain1_Gain_c;
    real_T Gain1_Gain_e;
    real_T Gain1_Gain_pf;
    real_T Gain_Gain_am;
    real_T Gain1_Gain_lp;
    real_T g_Gain;
    real_T ktstomps_Gain;
    real_T GStoGS_CAS_Gain;
    real_T _Gain;
    real_T ktstomps_Gain_b;
    real_T ug_Gain;
    real_T Gain1_Gain_bf;
    real_T Gain_Gain_b0;
    real_T Constant4_Value;
    real_T Constant3_Value_nq;
    real_T Gain1_Gain_ik;
    real_T Gain_Gain_aj;
    real_T Switch_Threshold_l;
    real_T Gain1_Gain_oz;
    real_T Gain_Gain_m0;
    real_T fpmtoms_Gain_a;
    real_T kntoms_Gain_p;
    real_T Saturation_UpperSat_h;
    real_T Saturation_LowerSat_e;
    real_T Gain_Gain_d4;
    real_T Gain1_Gain_kd;
    real_T fpmtoms_Gain_c;
    real_T kntoms_Gain_l;
    real_T Saturation_UpperSat_i;
    real_T Saturation_LowerSat_h;
    real_T Gain_Gain_bs;
    real_T Gain1_Gain_o4;
    real_T Constant_Value_c;
    real_T Gain1_Gain_j0;
    real_T Gain1_Gain_lxx;
    real_T Gain1_Gain_bk;
    real_T Gain_Gain_id;
    real_T Gain1_Gain_dv;
    real_T g_Gain_h;
    real_T ktstomps_Gain_g;
    real_T GStoGS_CAS_Gain_m;
    real_T _Gain_h;
    real_T ktstomps_Gain_i;
    real_T ug_Gain_a;
    real_T Gain1_Gain_hm;
    real_T Gain_Gain_kj;
    real_T Constant2_Value_c;
    real_T Constant1_Value_b4;
    real_T Gain1_Gain_mz;
    real_T Gain_Gain_ie;
    real_T Switch_Threshold_b;
    real_T Gain1_Gain_f1;
    real_T Gain_Gain_lr;
    real_T Constant_Value_ig;
    real_T fpmtoms_Gain_p;
    real_T kntoms_Gain_f;
    real_T Saturation_UpperSat_eik;
    real_T Saturation_LowerSat_a;
    real_T Gain_Gain_e33;
    real_T Gain1_Gain_ok;
    real_T Gain1_Gain_dh;
    real_T fpmtoms_Gain_h;
    real_T kntoms_Gain_a;
    real_T Saturation_UpperSat_f;
    real_T Saturation_LowerSat_c;
    real_T Gain_Gain_nq;
    real_T Gain1_Gain_cv;
    real_T Constant_Value_l;
    real_T Gain1_Gain_jd;
    real_T Gain1_Gain_ct;
    real_T Gain1_Gain_id;
    real_T Gain_Gain_ms;
    real_T Gain1_Gain_ca;
    real_T g_Gain_j;
    real_T ktstomps_Gain_f;
    real_T GStoGS_CAS_Gain_l;
    real_T _Gain_m;
    real_T ktstomps_Gain_j;
    real_T ug_Gain_o;
    real_T Gain1_Gain_hu;
    real_T Gain_Gain_bn;
    real_T Gain1_Gain_hz;
    real_T Gain_Gain_d4y;
    real_T Constant3_Value_ix;
    real_T ftmintoms_Gain_d;
    real_T kntoms_Gain_n;
    real_T Saturation_UpperSat_ju;
    real_T Saturation_LowerSat_gw;
    real_T Gain_Gain_nz;
    real_T ftmintoms_Gain_l;
    real_T kntoms_Gain_au;
    real_T Saturation_UpperSat_l;
    real_T Saturation_LowerSat_hm;
    real_T Gain_Gain_ey;
    real_T fpmtoms_Gain_o;
    real_T kntoms_Gain_o;
    real_T Saturation_UpperSat_fr;
    real_T Saturation_LowerSat_cd;
    real_T Gain_Gain_lx;
    real_T Gain1_Gain_jn;
    real_T fpmtoms_Gain_e;
    real_T kntoms_Gain_d;
    real_T Saturation_UpperSat_hb;
    real_T Saturation_LowerSat_k;
    real_T Gain_Gain_in;
    real_T Gain1_Gain_ps;
    real_T Constant_Value_od;
    real_T Gain1_Gain_hi;
    real_T Gain1_Gain_da;
    real_T Gain1_Gain_hg;
    real_T Gain_Gain_b5;
    real_T Gain1_Gain_kdq;
    real_T g_Gain_m;
    real_T ktstomps_Gain_m;
    real_T GStoGS_CAS_Gain_k;
    real_T _Gain_k;
    real_T ktstomps_Gain_c;
    real_T ug_Gain_aa;
    real_T Gain1_Gain_gf;
    real_T Gain_Gain_j;
    real_T Constant4_Value_f;
    real_T Constant3_Value_h1;
    real_T Gain1_Gain_ov;
    real_T Gain_Gain_jy;
    real_T Switch_Threshold_o;
    real_T Gain1_Gain_dvi;
    real_T Gain_Gain_h4;
    real_T fpmtoms_Gain_p3;
    real_T kntoms_Gain_bq;
    real_T Saturation_UpperSat_ba;
    real_T Saturation_LowerSat_p;
    real_T Gain_Gain_py;
    real_T Gain1_Gain_hk;
    real_T fpmtoms_Gain_j;
    real_T kntoms_Gain_l5;
    real_T Saturation_UpperSat_b3;
    real_T Saturation_LowerSat_es;
    real_T Gain_Gain_e5;
    real_T Gain1_Gain_ja;
    real_T Constant_Value_ia;
    real_T Gain1_Gain_er;
    real_T Gain1_Gain_fl;
    real_T Gain1_Gain_ero;
    real_T Gain_Gain_mx;
    real_T Gain1_Gain_hv;
    real_T g_Gain_g;
    real_T ktstomps_Gain_a;
    real_T GStoGS_CAS_Gain_n;
    real_T _Gain_i;
    real_T ktstomps_Gain_o;
    real_T ug_Gain_f;
    real_T Gain1_Gain_ot;
    real_T Gain_Gain_dm;
    real_T Constant2_Value_k;
    real_T Constant1_Value_d;
    real_T Gain1_Gain_ou;
    real_T Gain_Gain_jg;
    real_T Switch_Threshold_a;
    real_T Gain1_Gain_gy;
    real_T Gain_Gain_eq;
    real_T Constant_Value_ga;
    real_T fpmtoms_Gain_ps;
    real_T kntoms_Gain_c;
    real_T Saturation_UpperSat_oz;
    real_T Saturation_LowerSat_ou;
    real_T Gain_Gain_gt;
    real_T Gain_Gain_c3;
    real_T fpmtoms_Gain_d;
    real_T kntoms_Gain_cv;
    real_T Saturation_UpperSat_bb;
    real_T Saturation_LowerSat_a4;
    real_T Gain_Gain_hv;
    real_T Gain1_Gain_ej;
    real_T fpmtoms_Gain_f;
    real_T kntoms_Gain_k;
    real_T Saturation_UpperSat_pj;
    real_T Saturation_LowerSat_py;
    real_T Gain_Gain_bf;
    real_T Gain1_Gain_jv;
    real_T Constant_Value_lf;
    real_T Gain1_Gain_gfa;
    real_T Gain1_Gain_kw;
    real_T Gain1_Gain_j4;
    real_T Gain_Gain_bc;
    real_T Gain1_Gain_n4;
    real_T g_Gain_l;
    real_T ktstomps_Gain_j4;
    real_T GStoGS_CAS_Gain_o;
    real_T _Gain_kb;
    real_T ktstomps_Gain_k;
    real_T ug_Gain_n;
    real_T Gain1_Gain_b1;
    real_T Gain_Gain_d0;
    real_T Constant4_Value_o;
    real_T Constant3_Value_nk;
    real_T Gain1_Gain_on;
    real_T Gain_Gain_hy;
    real_T Switch_Threshold_d;
    real_T Gain1_Gain_m1;
    real_T Gain_Gain_fnw;
    real_T fpmtoms_Gain_o2;
    real_T kntoms_Gain_hi;
    real_T Saturation_UpperSat_cv;
    real_T Saturation_LowerSat_hd;
    real_T Gain_Gain_pp;
    real_T Gain1_Gain_iw;
    real_T fpmtoms_Gain_hz;
    real_T kntoms_Gain_i;
    real_T Saturation_UpperSat_nu;
    real_T Saturation_LowerSat_ae;
    real_T Gain_Gain_ej;
    real_T Gain1_Gain_lw;
    real_T Constant_Value_fo;
    real_T Gain1_Gain_ky;
    real_T Gain1_Gain_nrn;
    real_T Gain1_Gain_ip;
    real_T Gain_Gain_d3;
    real_T Gain1_Gain_mx;
    real_T g_Gain_hq;
    real_T ktstomps_Gain_l;
    real_T GStoGS_CAS_Gain_e;
    real_T _Gain_ip;
    real_T ktstomps_Gain_mh;
    real_T ug_Gain_e;
    real_T Gain1_Gain_be;
    real_T Gain_Gain_gx;
    real_T Constant2_Value_hd;
    real_T Constant1_Value_o;
    real_T Gain1_Gain_nj;
    real_T Gain_Gain_aq;
    real_T Switch_Threshold_g;
    real_T Gain1_Gain_fle;
    real_T Gain_Gain_ko;
    real_T Constant_Value_fov;
    real_T Gain2_Gain_m;
    real_T Gain4_Gain_g;
    real_T Gain3_Gain_c;
    real_T Saturation_UpperSat_e0;
    real_T Saturation_LowerSat_ph;
    real_T fpmtoms_Gain_g4;
    real_T kntoms_Gain_k4;
    real_T Saturation_UpperSat_eb;
    real_T Saturation_LowerSat_gk;
    real_T Gain_Gain_ow;
    real_T Gain2_Gain_l;
    real_T Bias_Bias;
    real_T Gain1_Gain_d4;
    real_T Bias1_Bias;
    real_T Gain_Gain_eyl;
    real_T Constant2_Value_f;
    real_T Gain4_Gain_oy;
    real_T Gain5_Gain_c;
    real_T kntofpm_Gain;
    real_T maxslope_Gain;
    real_T Gain7_Gain_l;
    real_T ftmintoms_Gain_k;
    real_T kntoms_Gain_av;
    real_T Saturation_UpperSat_i0;
    real_T Saturation_LowerSat_nd;
    real_T Gain_Gain_gr;
    real_T Gain1_Gain_ml;
    real_T ftmintoms_Gain_j;
    real_T Saturation_UpperSat_ew;
    real_T Saturation_LowerSat_an;
    real_T Gain_Gain_by;
    real_T Gain2_Gain_mj;
    real_T uDLookupTable_tableData[4];
    real_T uDLookupTable_bp01Data[4];
    real_T ftmintoms_Gain_dc;
    real_T kntoms1_Gain;
    real_T Saturation_UpperSat_dp;
    real_T Saturation_LowerSat_f1;
    real_T Gain_Gain_owa;
    real_T Constant1_Value_o0;
    real_T Constant2_Value_kz;
    real_T fpmtoms_Gain_po;
    real_T kntoms_Gain_bh;
    real_T Saturation_UpperSat_pd;
    real_T Saturation_LowerSat_l;
    real_T Gain_Gain_cr;
    real_T Gain1_Gain_ga;
    real_T Gain1_Gain_ol;
    real_T fpmtoms_Gain_k;
    real_T kntoms_Gain_py;
    real_T Saturation_UpperSat_ec;
    real_T Saturation_LowerSat_m;
    real_T Gain_Gain_hc;
    real_T Gain1_Gain_ln;
    real_T Constant_Value_h;
    real_T Gain1_Gain_hm2;
    real_T Gain1_Gain_a;
    real_T Gain1_Gain_it;
    real_T Gain_Gain_er;
    real_T Gain1_Gain_mxw;
    real_T g_Gain_p;
    real_T ktstomps_Gain_k5;
    real_T GStoGS_CAS_Gain_j;
    real_T _Gain_f;
    real_T ktstomps_Gain_mf;
    real_T ug_Gain_c;
    real_T Gain1_Gain_nc;
    real_T Gain_Gain_bg;
    real_T Gain1_Gain_ke;
    real_T Gain_Gain_c2;
    real_T Constant3_Value_ew;
    real_T ftmintoms_Gain_m;
    real_T kntoms_Gain_om;
    real_T Saturation_UpperSat_ed;
    real_T Saturation_LowerSat_ee;
    real_T Gain_Gain_kon;
    real_T Constant_Value_iaf;
    real_T ftmintoms_Gain_lv;
    real_T kntoms_Gain_iw;
    real_T Saturation_UpperSat_jt;
    real_T Saturation_LowerSat_ih;
    real_T Gain_Gain_o1;
    real_T Gain_Gain_pe;
    real_T Gain2_Gain_hq;
    real_T Saturation_UpperSat_f3;
    real_T Saturation_LowerSat_b;
    real_T ftmintoms_Gain_kr;
    real_T kntoms_Gain_j;
    real_T Saturation_UpperSat_nuy;
    real_T Saturation_LowerSat_dj;
    real_T Gain_Gain_fs;
    real_T Gain_Gain_k2;
    real_T Gain3_Gain_l;
    real_T Saturation_UpperSat_ig;
    real_T Saturation_LowerSat_ous;
    real_T Gain_Gain_j0;
    real_T Switch_Threshold_c;
    real_T Gain_Gain_o2;
    real_T Gain1_Gain_i0;
    real_T Saturation_UpperSat_ix;
    real_T Saturation_LowerSat_eq;
    real_T Constant_Value_i4;
    boolean_T Delay_InitialCondition;
    boolean_T Delay_InitialCondition_b;
    uint8_T ManualSwitch_CurrentSetting;
    uint8_T ManualSwitch_CurrentSetting_b;
  };

  AutopilotLawsModelClass(AutopilotLawsModelClass const&) =delete;
  AutopilotLawsModelClass& operator= (AutopilotLawsModelClass const&) & = delete;
  void setExternalInputs(const ExternalInputs_AutopilotLaws_T *pExternalInputs_AutopilotLaws_T)
  {
    AutopilotLaws_U = *pExternalInputs_AutopilotLaws_T;
  }

  const ExternalOutputs_AutopilotLaws_T &getExternalOutputs() const
  {
    return AutopilotLaws_Y;
  }

  void initialize();
  void step();
  static void terminate();
  AutopilotLawsModelClass();
  ~AutopilotLawsModelClass();
 private:
  ExternalInputs_AutopilotLaws_T AutopilotLaws_U;
  ExternalOutputs_AutopilotLaws_T AutopilotLaws_Y;
  BlockIO_AutopilotLaws_T AutopilotLaws_B;
  D_Work_AutopilotLaws_T AutopilotLaws_DWork;
  static Parameters_AutopilotLaws_T AutopilotLaws_P;
  static void AutopilotLaws_MATLABFunction(real_T rtu_tau, real_T rtu_zeta, real_T *rty_k2, real_T *rty_k1);
  static void AutopilotLaws_LagFilter(real_T rtu_U, real_T rtu_C1, real_T rtu_dt, real_T *rty_Y,
    rtDW_LagFilter_AutopilotLaws_T *localDW);
  static void AutopilotLaws_RateLimiter(real_T rtu_u, real_T rtu_up, real_T rtu_lo, real_T rtu_Ts, real_T rtu_init,
    real_T *rty_Y, rtDW_RateLimiter_AutopilotLaws_T *localDW);
  static void AutopilotLaws_MATLABFunction_f_Init(rtDW_MATLABFunction_AutopilotLaws_d_T *localDW);
  static void AutopilotLaws_MATLABFunction_m(real_T rtu_Psi_c, real_T rtu_dPsi, real_T rtu_Phi_c, real_T *rty_up, real_T
    *rty_lo, rtDW_MATLABFunction_AutopilotLaws_d_T *localDW);
  static void AutopilotLaws_Chart_Init(real_T *rty_out);
  static void AutopilotLaws_Chart(real_T rtu_right, real_T rtu_left, boolean_T rtu_use_short_path, real_T *rty_out,
    rtDW_Chart_AutopilotLaws_T *localDW);
  static void AutopilotLaws_Chart_g_Init(real_T *rty_out);
  static void AutopilotLaws_Chart_h(real_T rtu_right, real_T rtu_left, real_T rtu_use_short_path, real_T *rty_out,
    rtDW_Chart_AutopilotLaws_m_T *localDW);
  static void AutopilotLaws_storevalue(boolean_T rtu_active, real_T rtu_u, real_T *rty_y,
    rtDW_storevalue_AutopilotLaws_T *localDW);
  static void AutopilotLaws_LeadLagFilter(real_T rtu_U, real_T rtu_C1, real_T rtu_C2, real_T rtu_C3, real_T rtu_C4,
    real_T rtu_dt, real_T *rty_Y, rtDW_LeadLagFilter_AutopilotLaws_T *localDW);
  static void AutopilotLaws_WashoutFilter(real_T rtu_U, real_T rtu_C1, real_T rtu_dt, real_T *rty_Y,
    rtDW_WashoutFilter_AutopilotLaws_T *localDW);
  static void AutopilotLaws_V_LSSpeedSelection1(real_T rtu_V_c, real_T rtu_VLS, real_T *rty_y);
  static void AutopilotLaws_SpeedProtectionSignalSelection(const ap_laws_output *rtu_in, real_T rtu_VS_FD, real_T
    rtu_VS_AP, real_T rtu_VLS_FD, real_T rtu_VLS_AP, real_T rtu_VMAX_FD, real_T rtu_VMAX_AP, real_T rtu_margin, real_T
    *rty_FD, real_T *rty_AP);
  static void AutopilotLaws_VSLimiter(real_T rtu_u, const ap_laws_output *rtu_in, real_T *rty_y);
  static void AutopilotLaws_VSLimiter_f(real_T rtu_u, const ap_laws_output *rtu_in, real_T *rty_y);
  static void AutopilotLaws_SignalEnablerGSTrack(real_T rtu_u, boolean_T rtu_e, real_T *rty_y);
  static void AutopilotLaws_Voter1(real_T rtu_u1, real_T rtu_u2, real_T rtu_u3, real_T *rty_Y);
};

#endif

