#include <math.h>
#include "SimulinkGUI.h"
#include "SimulinkGUI_private.h"
#include "SimulinkGUI_dt.h"

const int_T gblNumToFiles = 0;
const int_T gblNumFrFiles = 0;
const int_T gblNumFrWksBlocks = 0;

#ifdef RSIM_WITH_SOLVER_MULTITASKING

const boolean_T gbl_raccel_isMultitasking = 1;

#else

const boolean_T gbl_raccel_isMultitasking = 0;

#endif

const boolean_T gbl_raccel_tid01eq = 1;
const int_T gbl_raccel_NumST = 3;
const char_T *gbl_raccel_Version = "7.5  (R2010a)  25-Jan-2010";
void raccel_setup_MMIStateLog(SimStruct* S)
{

#ifdef UseMMIDataLogging

  rt_FillStateSigInfoFromMMI(ssGetRTWLogInfo(S),&ssGetErrorStatus(S));

#endif

}

const char *gblSlvrJacPatternFileName =
  "slprj\\raccel\\SimulinkGUI\\SimulinkGUI_Jpattern.mat";
const int_T gblNumRootInportBlks = 0;
const int_T gblNumModelInputs = 0;
extern rtInportTUtable *gblInportTUtables;
extern const char *gblInportFileName;
const int_T gblInportDataTypeIdx[] = { -1 };

const int_T gblInportDims[] = { -1 } ;

const int_T gblInportComplex[] = { -1 };

const int_T gblInportInterpoFlag[] = { -1 };

const int_T gblInportContinuous[] = { -1 };

#include "simstruc.h"
#include "fixedpoint.h"

BlockIO rtB;
D_Work rtDWork;
static SimStruct model_S;
SimStruct *const rtS = &model_S;
void MdlInitialize(void)
{
  rtDWork.TmpRTBAtScopeInport1_Buffer0 = 0U;
}

void MdlStart(void)
{
  rtB.TmpRTBAtScopeInport1 = 0U;
  MdlInitialize();
}

void MdlOutputs(int_T tid)
{
  if (ssIsContinuousTask(rtS, tid)) {
  }

  if (ssIsSampleHit(rtS, 1, tid)) {
    if (ssIsSpecialSampleHit(rtS,2,1,tid)) {
      rtB.TmpRTBAtScopeInport1 = rtDWork.TmpRTBAtScopeInport1_Buffer0;
    }
  }

  if (ssIsSampleHit(rtS, 2, tid)) {
  }
}

void MdlUpdate(int_T tid)
{
  if (ssIsContinuousTask(rtS, tid)) {
  }

  if (ssIsSampleHit(rtS, 2, tid)) {
    rtDWork.TmpRTBAtScopeInport1_Buffer0 = rtB.SerialReceive;
  }

  if (ssIsSampleHit(rtS, 2, tid)) {
  }
}

void MdlInitSystemMatrices(void)
{
}

void MdlTerminate(void)
{
}

void MdlInitializeSizes(void)
{
  ssSetNumContStates(rtS, 0);
  ssSetNumY(rtS, 0);
  ssSetNumU(rtS, 0);
  ssSetDirectFeedThrough(rtS, 0);
  ssSetNumSampleTimes(rtS, 3);
  ssSetNumBlocks(rtS, 4);
  ssSetNumBlockIO(rtS, 2);
}

void MdlInitializeSampleTimes(void)
{
  ssSetSampleTime(rtS, 0, 0.0);
  ssSetSampleTime(rtS, 1, 0.001);
  ssSetSampleTime(rtS, 2, 0.01);
  ssSetOffsetTime(rtS, 0, 0.0);
  ssSetOffsetTime(rtS, 1, 0.0);
  ssSetOffsetTime(rtS, 2, 0.0);
}

void raccel_set_checksum(SimStruct *rtS)
{
  ssSetChecksumVal(rtS, 0, 3540272554U);
  ssSetChecksumVal(rtS, 1, 3291855859U);
  ssSetChecksumVal(rtS, 2, 64199560U);
  ssSetChecksumVal(rtS, 3, 2956934806U);
}

SimStruct * raccel_register_model(void)
{
  static struct _ssMdlInfo mdlInfo;
  (void) memset((char *)rtS, 0,
                sizeof(SimStruct));
  (void) memset((char *)&mdlInfo, 0,
                sizeof(struct _ssMdlInfo));
  ssSetMdlInfoPtr(rtS, &mdlInfo);

  {
    static time_T mdlPeriod[NSAMPLE_TIMES];
    static time_T mdlOffset[NSAMPLE_TIMES];
    static time_T mdlTaskTimes[NSAMPLE_TIMES];
    static int_T mdlTsMap[NSAMPLE_TIMES];
    static int_T mdlSampleHits[NSAMPLE_TIMES];
    static boolean_T mdlTNextWasAdjustedPtr[NSAMPLE_TIMES];
    static int_T mdlPerTaskSampleHits[NSAMPLE_TIMES * NSAMPLE_TIMES];
    static time_T mdlTimeOfNextSampleHit[NSAMPLE_TIMES];

    {
      int_T i;
      for (i = 0; i < NSAMPLE_TIMES; i++) {
        mdlPeriod[i] = 0.0;
        mdlOffset[i] = 0.0;
        mdlTaskTimes[i] = 0.0;
        mdlTsMap[i] = i;
      }
    }

    mdlSampleHits[0] = 1;
    ssSetSampleTimePtr(rtS, &mdlPeriod[0]);
    ssSetOffsetTimePtr(rtS, &mdlOffset[0]);
    ssSetSampleTimeTaskIDPtr(rtS, &mdlTsMap[0]);
    ssSetTPtr(rtS, &mdlTaskTimes[0]);
    ssSetSampleHitPtr(rtS, &mdlSampleHits[0]);
    ssSetTNextWasAdjustedPtr(rtS, &mdlTNextWasAdjustedPtr[0]);
    ssSetPerTaskSampleHitsPtr(rtS, &mdlPerTaskSampleHits[0]);
    ssSetTimeOfNextSampleHitPtr(rtS, &mdlTimeOfNextSampleHit[0]);
  }

  {
    static int_T mdlPerTaskSampleHits[NSAMPLE_TIMES * NSAMPLE_TIMES];
    (void) memset((void *)&mdlPerTaskSampleHits[0], 0,
                  3 * 3 * sizeof(int_T));
    ssSetPerTaskSampleHitsPtr(rtS, &mdlPerTaskSampleHits[0]);
  }

  ssSetSolverMode(rtS, SOLVER_MODE_MULTITASKING);

  {
    ssSetBlockIO(rtS, ((void *) &rtB));
    (void) memset(((void *) &rtB), 0,
                  sizeof(BlockIO));
  }

  {
    void *dwork = (void *) &rtDWork;
    ssSetRootDWork(rtS, dwork);
    (void) memset(dwork, 0,
                  sizeof(D_Work));
  }

  {
    static DataTypeTransInfo dtInfo;
    (void) memset((char_T *) &dtInfo, 0,
                  sizeof(dtInfo));
    ssSetModelMappingInfo(rtS, &dtInfo);
    dtInfo.numDataTypes = 14;
    dtInfo.dataTypeSizes = &rtDataTypeSizes[0];
    dtInfo.dataTypeNames = &rtDataTypeNames[0];
    dtInfo.B = &rtBTransTable;
  }

  ssSetRootSS(rtS, rtS);
  ssSetVersion(rtS, SIMSTRUCT_VERSION_LEVEL2);
  ssSetModelName(rtS, "SimulinkGUI");
  ssSetPath(rtS, "SimulinkGUI");
  ssSetTStart(rtS, 0.0);
  ssSetTFinal(rtS, rtInf);
  ssSetStepSize(rtS, 0.001);
  ssSetFixedStepSize(rtS, 0.001);

  {
    static RTWLogInfo rt_DataLoggingInfo;
    ssSetRTWLogInfo(rtS, &rt_DataLoggingInfo);
  }

  {
    rtliSetLogXSignalInfo(ssGetRTWLogInfo(rtS), (NULL));
    rtliSetLogXSignalPtrs(ssGetRTWLogInfo(rtS), (NULL));
    rtliSetLogT(ssGetRTWLogInfo(rtS), "tout");
    rtliSetLogX(ssGetRTWLogInfo(rtS), "");
    rtliSetLogXFinal(ssGetRTWLogInfo(rtS), "");
    rtliSetSigLog(ssGetRTWLogInfo(rtS), "");
    rtliSetLogVarNameModifier(ssGetRTWLogInfo(rtS), "none");
    rtliSetLogFormat(ssGetRTWLogInfo(rtS), 0);
    rtliSetLogMaxRows(ssGetRTWLogInfo(rtS), 1000);
    rtliSetLogDecimation(ssGetRTWLogInfo(rtS), 1);
    rtliSetLogY(ssGetRTWLogInfo(rtS), "");
    rtliSetLogYSignalInfo(ssGetRTWLogInfo(rtS), (NULL));
    rtliSetLogYSignalPtrs(ssGetRTWLogInfo(rtS), (NULL));
  }

  {
    static ssSolverInfo slvrInfo;
    ssSetSolverInfo(rtS, &slvrInfo);
    ssSetSolverName(rtS, "FixedStepDiscrete");
    ssSetVariableStepSolver(rtS, 0);
    ssSetSolverConsistencyChecking(rtS, 0);
    ssSetSolverAdaptiveZcDetection(rtS, 0);
    ssSetSolverRobustResetMethod(rtS, 0);
    ssSetSolverStateProjection(rtS, 0);
    ssSetSolverMassMatrixType(rtS, (ssMatrixType)0);
    ssSetSolverMassMatrixNzMax(rtS, 0);
    ssSetModelOutputs(rtS, MdlOutputs);
    ssSetModelLogData(rtS, rt_UpdateTXYLogVars);
    ssSetModelUpdate(rtS, MdlUpdate);
    ssSetTNextTid(rtS, INT_MIN);
    ssSetTNext(rtS, rtMinusInf);
    ssSetSolverNeedsReset(rtS);
    ssSetNumNonsampledZCs(rtS, 0);
  }

  ssSetChecksumVal(rtS, 0, 3540272554U);
  ssSetChecksumVal(rtS, 1, 3291855859U);
  ssSetChecksumVal(rtS, 2, 64199560U);
  ssSetChecksumVal(rtS, 3, 2956934806U);

  {
    static const sysRanDType rtAlwaysEnabled = SUBSYS_RAN_BC_ENABLE;
    static RTWExtModeInfo rt_ExtModeInfo;
    static const sysRanDType *systemRan[1];
    ssSetRTWExtModeInfo(rtS, &rt_ExtModeInfo);
    rteiSetSubSystemActiveVectorAddresses(&rt_ExtModeInfo, systemRan);
    systemRan[0] = &rtAlwaysEnabled;
    rteiSetModelMappingInfoPtr(ssGetRTWExtModeInfo(rtS), &ssGetModelMappingInfo
      (rtS));
    rteiSetChecksumsPtr(ssGetRTWExtModeInfo(rtS), ssGetChecksums(rtS));
    rteiSetTPtr(ssGetRTWExtModeInfo(rtS), ssGetTPtr(rtS));
  }

  return rtS;
}
