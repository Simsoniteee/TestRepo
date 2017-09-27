#ifndef RTW_HEADER_SimulinkGUI_h_
#define RTW_HEADER_SimulinkGUI_h_
#ifndef SimulinkGUI_COMMON_INCLUDES_
# define SimulinkGUI_COMMON_INCLUDES_
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "raccel.h"
#include "rt_logging.h"
#include "dt_info.h"
#include "ext_work.h"
#include "rt_nonfinite.h"
#endif

#include "SimulinkGUI_types.h"
#define MODEL_NAME                     SimulinkGUI
#define NSAMPLE_TIMES                  (3)
#define NINPUTS                        (0)
#define NOUTPUTS                       (0)
#define NBLOCKIO                       (2)
#define NUM_ZC_EVENTS                  (0)
#ifndef NCSTATES
# define NCSTATES                      (0)
#elif NCSTATES != 0
# error Invalid specification of NCSTATES defined in compiler command
#endif

#ifndef rtmGetDataMapInfo
# define rtmGetDataMapInfo(rtm)        (NULL)
#endif

#ifndef rtmSetDataMapInfo
# define rtmSetDataMapInfo(rtm, val)
#endif

typedef struct {
  uint8_T TmpRTBAtScopeInport1;
  uint8_T SerialReceive;
} BlockIO;

typedef struct {
  struct {
    void *LoggedData;
  } Scope_PWORK;

  uint8_T TmpRTBAtScopeInport1_Buffer0;
  uint8_T SerialReceive_CustomOutValue;
} D_Work;

extern const char *RT_MEMORY_ALLOCATION_ERROR;
extern BlockIO rtB;
extern D_Work rtDWork;
extern SimStruct *const rtS;
extern const int_T gblNumToFiles;
extern const int_T gblNumFrFiles;
extern const int_T gblNumFrWksBlocks;
extern rtInportTUtable *gblInportTUtables;
extern const char *gblInportFileName;
extern const int_T gblNumRootInportBlks;
extern const int_T gblNumModelInputs;
extern const int_T gblInportDataTypeIdx[];
extern const int_T gblInportDims[];
extern const int_T gblInportComplex[];
extern const int_T gblInportInterpoFlag[];
extern const int_T gblInportContinuous[];

#endif
