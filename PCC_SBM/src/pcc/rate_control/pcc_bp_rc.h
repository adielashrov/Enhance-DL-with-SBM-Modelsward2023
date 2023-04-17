#ifndef _PCC_BP_RC_H_
#define _PCC_BP_RC_H_

#include "pcc_rc.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <unistd.h>

#include "BProgram.h"
#include "MonitorIntervalSensorBThread.h"
#include "SendRateSensorBthread.h"
#include "SendRateActuatorBThread.h"
#include "ODNN_BThread.h"
#include "InterleaveBthread.h"
#include "StatisticsFileHandler.h"
#include "YieldThroughputBThread.h"
#include "TestModelBThread.h"
#include "ReduceThroughputBThread.h"
#include "RestoreThroughputBThread.h"
#include "MonitorNetworkStateBThread.h"

class PccBPRateController: public PccRateController {
	public:

		PccBPRateController(double call_freq, PccEventLogger * log);
		~PccBPRateController();
		QuicBandwidth GetNextSendingRate(QuicBandwidth current_rate, QuicTime cur_time);
		void MonitorIntervalFinished(const MonitorInterval & mi);
		void Reset();
		void setSensorBThread(const MonitorIntervalSensorBThread& monitorIntervalSensorBThread);

	private:
		void initializeBPProgram();
		int _id;
		int _numOfGetNextSendingRate;
		int _numOfMonitorIntervalFinished;
		StatisticsFileHandler* statisticsFileHandler_;
		MonitorIntervalSensorBThread* monitorIntervalSensorBThread_;
		SendRateSensorBthread* sendRateSensorBthread_;
		SendRateActuatorBThread* sendRateActuatorBThread_;
		ODNN_BThread* odnn_BThread_;
		InterleaveBThread* interleaveBThread_;
		TestModelBThread* testModelBThread_;
		YieldThroughputBThread* yieldThroughputBThread_;
		ReduceThroughputBThread* reduceThroughputBThread_;
		RestoreThroughputBThread* restoreThroughputBThread_;
		MonitorNetworkStateBThread* monitorNetworkStateBThread_;
		BProgram* bProgram_;

};

#endif