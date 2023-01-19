#ifndef _PCC_BP_RC_H_
#define _PCC_BP_RC_H_

#include "pcc_rc.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <unistd.h>

#include "BProgram.h"
#include "SensorBThread.h"
#include "SendRateBthread.h"
#include "SendRateActuatorBThread.h"
#include "ODRL_BThread.h"
#include "InterleaveBthread.h"
#include "StatisticsFileHandler.h"
#include "YieldThroughputBThread.h"
#include "TestModelBThread.h"
#include "IdentifyRTTDeviationBThread.h"
#include "RestoreThroughputBThread.h"
#include "MonitorNetworkStateBThread.h"

class PccBPRateController: public PccRateController {
	public:

		PccBPRateController(double call_freq, PccEventLogger * log);
		~PccBPRateController();
		QuicBandwidth GetNextSendingRate(QuicBandwidth current_rate, QuicTime cur_time);
		void MonitorIntervalFinished(const MonitorInterval & mi);
		void Reset();
		void setSensorBThread(const SensorBThread& sensorBThread);

	private:
		void initializeBPProgram();
		int _id;
		int _numOfGetNextSendingRate;
		int _numOfMonitorIntervalFinished;
		StatisticsFileHandler* statisticsFileHandler_;
		SensorBThread* sensorBThread_;
		SendRateBthread* sendRateBthread_;
		SendRateActuatorBThread* sendRateActuatorBThread_;
		ODRL_BThread* odrl_BThread_;
		InterleaveBThread* interleaveBThread_;
		TestModelBThread* testModelBThread_;
		YieldThroughputBThread* yieldThroughputBThread_;
		IdentifyRTTDeviationBThread* identifyRTTDeviationBThread_;
		RestoreThroughputBThread* restoreThroughputBThread_;
		MonitorNetworkStateBThread* monitorNetworkStateBThread_;
		BProgram* bProgram_;

};

#endif