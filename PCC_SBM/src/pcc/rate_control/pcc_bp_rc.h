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
#include "ODNN_BThread.h"
#include "MonitorNetworkStateBThread.h"
#include "ReduceThroughputBThread.h"
#include "RestoreThroughputBThread.h"
#include "SendRateActuatorBThread.h"
#include "StatisticsFileHandler.h"

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
		ReduceThroughputBThread* reduceThroughputBThread_;
		RestoreThroughputBThread* restoreThroughputBThread_;
		MonitorNetworkStateBThread* monitorNetworkStateBThread_;
		BProgram* bProgram_;

};

#endif