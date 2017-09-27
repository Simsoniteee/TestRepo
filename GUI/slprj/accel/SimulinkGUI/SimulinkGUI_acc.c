/*
 * This file is not available for use in any application other than as a
 * MATLAB(R) MEX file for use with the Simulink(R) product.
 */

/*
 * SimulinkGUI_acc.c
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
#include <math.h>
#include "SimulinkGUI_acc.h"
#include "SimulinkGUI_acc_private.h"
#include <stdio.h>
#include "simstruc.h"
#include "fixedpoint.h"
#define CodeFormat                     S-Function
#define AccDefine1                     Accelerator_S-Function

/* Outputs for root system: '<Root>' */
static void mdlOutputs(SimStruct *S, int_T tid)
{
  if (ssIsContinuousTask(S, tid)) {
  }

  if (ssIsSampleHit(S, 1, tid)) {
    if (ssIsSpecialSampleHit(S,2,1,tid)) {
      ((BlockIO *) _ssGetBlockIO(S))->B_0_1_0 = ((D_Work *) ssGetRootDWork(S))
        ->TmpRTBAtScopeInport1_Buffer0;
    }

    if (ssIsSpecialSampleHit(S,3,1,tid)) {
      ((BlockIO *) _ssGetBlockIO(S))->B_0_2_0 = ((D_Work *) ssGetRootDWork(S))
        ->TmpRTBAtScopeInport2_Buffer0;
    }

    /* Scope: '<Root>/Scope' */

    /* Call into Simulink for Scope */
    ssCallAccelRunBlock(S, 0, 3, SS_CALL_MDL_OUTPUTS);
  }

  if (ssIsSampleHit(S, 2, tid)) {
    /* M-S-Function: '<Root>/Serial Receive' */
    /* Call into Simulink */
    ssCallAccelRunBlock(S, 0, 4, SS_CALL_MDL_OUTPUTS);
  }

  if (ssIsSampleHit(S, 3, tid)) {
    ((BlockIO *) _ssGetBlockIO(S))->B_0_5_0 = muDoubleScalarSin((real_T)((D_Work
      *) ssGetRootDWork(S))->counter * 2.0 * 3.1415926535897931E+000 / 10.0);
  }
}

/* Update for root system: '<Root>' */
#define MDL_UPDATE

static void mdlUpdate(SimStruct *S, int_T tid)
{
  if (ssIsContinuousTask(S, tid)) {
  }

  if (ssIsSampleHit(S, 2, tid)) {
    ((D_Work *) ssGetRootDWork(S))->TmpRTBAtScopeInport1_Buffer0 = ((BlockIO *)
      _ssGetBlockIO(S))->B_0_4_0;
  }

  if (ssIsSampleHit(S, 3, tid)) {
    ((D_Work *) ssGetRootDWork(S))->TmpRTBAtScopeInport2_Buffer0 = ((BlockIO *)
      _ssGetBlockIO(S))->B_0_5_0;
  }

  if (ssIsSampleHit(S, 2, tid)) {
  }

  if (ssIsSampleHit(S, 3, tid)) {
    ((D_Work *) ssGetRootDWork(S))->counter = ((D_Work *) ssGetRootDWork(S))
      ->counter + 1;
    if (((D_Work *) ssGetRootDWork(S))->counter == 10) {
      ((D_Work *) ssGetRootDWork(S))->counter = 0;
    }
  }
}

/* Function to initialize sizes */
static void mdlInitializeSizes(SimStruct *S)
{
  /* checksum */
  ssSetChecksumVal(S, 0, 1795402664U);
  ssSetChecksumVal(S, 1, 498796245U);
  ssSetChecksumVal(S, 2, 2097030773U);
  ssSetChecksumVal(S, 3, 2671163382U);

  /* options */
  ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);

  /* Accelerator check memory map size match for DWork */
  if (ssGetSizeofDWork(S) != sizeof(D_Work)) {
    ssSetErrorStatus(S,"Unexpected error: Internal DWork sizes do "
                     "not match for accelerator mex file.");
  }

  /* Accelerator check memory map size match for BlockIO */
  if (ssGetSizeofGlobalBlockIO(S) != sizeof(BlockIO)) {
    ssSetErrorStatus(S,"Unexpected error: Internal BlockIO sizes do "
                     "not match for accelerator mex file.");
  }
}

/* mdlInitializeSampleTimes function (used to set up function-call connections) */
static void mdlInitializeSampleTimes(SimStruct *S)
{
}

/* Empty mdlTerminate function (never called) */
static void mdlTerminate(SimStruct *S)
{
}

/* MATLAB MEX Glue */
#include "simulink.c"
