/*
 * SimulinkGUI_acc.h
 *
 * Real-Time Workshop code generation for Simulink model "SimulinkGUI_acc.mdl".
 *
 * Model version              : 1.6
 * Real-Time Workshop version : 7.5  (R2010a)  25-Jan-2010
 * C source code generated on : Mon Sep 25 20:38:04 2017
 *
 * Target selection: accel.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Generic->Custom
 * Emulation hardware selection:
 *    Differs from embedded hardware (MATLAB Host)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#ifndef RTW_HEADER_SimulinkGUI_acc_h_
#define RTW_HEADER_SimulinkGUI_acc_h_
#ifndef SimulinkGUI_acc_COMMON_INCLUDES_
# define SimulinkGUI_acc_COMMON_INCLUDES_
#include <stdlib.h>
#include <stddef.h>
#define S_FUNCTION_NAME                simulink_only_sfcn
#define S_FUNCTION_LEVEL               2
#define RTW_GENERATED_S_FUNCTION
#include "rtwtypes.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "mwmathutil.h"
#endif                                 /* SimulinkGUI_acc_COMMON_INCLUDES_ */

#include "SimulinkGUI_acc_types.h"

/* Block signals (auto storage) */
typedef struct {
  real_T B_0_2_0;                      /* '<Root>/Sine Wave' */
  real_T B_0_5_0;                      /* '<Root>/Sine Wave' */
  uint8_T B_0_1_0;                     /* '<Root>/Serial Receive' */
  uint8_T B_0_4_0;                     /* '<Root>/Serial Receive' */
  char pad_B_0_4_0[6];
} BlockIO;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T TmpRTBAtScopeInport2_Buffer0; /* synthesized block */
  void *SerialConfiguration_PWORK;     /* '<Root>/Serial Configuration' */
  struct {
    void *LoggedData;
  } Scope_PWORK;                       /* '<Root>/Scope' */

  void *SerialReceive_PWORK;           /* '<Root>/Serial Receive' */
  int32_T counter;                     /* '<Root>/Sine Wave' */
  uint8_T TmpRTBAtScopeInport1_Buffer0;/* synthesized block */
  uint8_T SerialReceive_CustomOutValue;/* '<Root>/Serial Receive' */
  char pad_SerialReceive_CustomOutValue[2];
} D_Work;

/* Constant parameters (auto storage) */
typedef struct {
  /* Pooled Parameter (Expression: 0)
   * Referenced by:
   *   'synthesized block'
   *   '<Root>/Sine Wave'
   */
  real_T pooled1;

  /* Expression: 1
   * Referenced by: '<Root>/Sine Wave'
   */
  real_T SineWave_Amp;

  /* Expression: 10
   * Referenced by: '<Root>/Sine Wave'
   */
  real_T SineWave_NumSamp;

  /* Computed Parameter: TmpRTBAtScopeInport1_X0
   * Referenced by: 'synthesized block'
   */
  uint8_T TmpRTBAtScopeInport1_X0;
  char pad_TmpRTBAtScopeInport1_X0[7];
} ConstParam;

/* Constant parameters (auto storage) */
extern const ConstParam rtConstP;

#endif                                 /* RTW_HEADER_SimulinkGUI_acc_h_ */
