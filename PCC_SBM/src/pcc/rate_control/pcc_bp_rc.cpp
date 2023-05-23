#include "pcc_bp_rc.h"

PccBPRateController::PccBPRateController(double call_freq,
        PccEventLogger* log) {
    this->_numOfGetNextSendingRate = 0;
    this->_numOfMonitorIntervalFinished = 0;
    this->initializeBPProgram();
}

PccBPRateController::~PccBPRateController() {
	// delete statisticsFileHandler_;
    delete monitorIntervalSensorBThread_;
    delete bProgram_;
}

void PccBPRateController::Reset() {
    // std::cout << "Enter PccBPRateController::RESET" << std::endl;
}

QuicBandwidth PccBPRateController::GetNextSendingRate(QuicBandwidth current_rate, QuicTime cur_time) {

    ++_numOfGetNextSendingRate;
    auto start = std::chrono::steady_clock::now();
	sendRateSensorBthread_->notifyOnGetNextSendingRate(_numOfGetNextSendingRate);
    QuicBandwidth retValue = sendRateActuatorBThread_->readNextSendingRate(_numOfGetNextSendingRate);
    
    return retValue;
}

void PccBPRateController::MonitorIntervalFinished(const MonitorInterval& mi) { 
    
    ++_numOfMonitorIntervalFinished;
    float utility = mi.GetUtility();
    this->monitorIntervalSensorBThread_->notifyOnMonitorEvent(mi);
}

void PccBPRateController::initializeBPProgram() {

  statisticsFileHandler_ = new StatisticsFileHandler();
  bProgram_ = new BProgram();
  sendRateSensorBthread_ = new SendRateSensorBthread();
  sendRateActuatorBThread_ = new SendRateActuatorBThread();
  sendRateActuatorBThread_->setStatisticsFileHandler(statisticsFileHandler_);
  monitorIntervalSensorBThread_ = new MonitorIntervalSensorBThread();
  odnn_BThread_ = new ODNN_BThread();
  odnn_BThread_->setStatisticsFileHandler(statisticsFileHandler_);
  reduceThroughputBThread_ = new ReduceThroughputBThread();
  restoreThroughputBThread_ = new RestoreThroughputBThread();
  monitorNetworkStateBThread_ = new MonitorNetworkStateBThread();

  bProgram_->addSensorThread( "MonitorIntervalSensorBThread" ); //MonitorIntervalSensorBThread is a sensor thread
  bProgram_->addSensorThread( "SendRateSensorBthread" ); //SendRateSensorBthread is a sensor thread
  
  bProgram_->addThread(*monitorIntervalSensorBThread_);
  bProgram_->addThread(*sendRateSensorBthread_);
  bProgram_->addThread(*odnn_BThread_);
  bProgram_->addThread(*sendRateActuatorBThread_);
  bProgram_->addThread(*reduceThroughputBThread_);
  bProgram_->addThread(*restoreThroughputBThread_);
  bProgram_->addThread(*monitorNetworkStateBThread_);


  bProgram_->runEngine();
  bProgram_->runProgram();
}
