#include "pcc_bp_rc.h"

PccBPRateController::PccBPRateController(double call_freq,
        PccEventLogger* log) {
	//  std::cout << "Enter PccBPRateController::PccBPRateController(constructor)" << std::endl;
    this->_numOfGetNextSendingRate = 0;
    this->_numOfMonitorIntervalFinished = 0;
    this->initializeBPProgram();
}

PccBPRateController::~PccBPRateController() {
	std::cout << "Enter PccBPRateController::~PccBPRateController(destructor)" << std::endl;
	// delete statisticsFileHandler_;
    delete sensorBThread_;
    delete bProgram_;
}

void PccBPRateController::Reset() {
    std::cout << "Enter PccBPRateController::RESET" << std::endl;
}

QuicBandwidth PccBPRateController::GetNextSendingRate(QuicBandwidth current_rate, QuicTime cur_time) {

    ++_numOfGetNextSendingRate;
    auto start = std::chrono::steady_clock::now();
	sendRateBthread_->notifyOnGetNextSendingRate(_numOfGetNextSendingRate);
    QuicBandwidth retValue = sendRateActuatorBThread_->readNextSendingRate(_numOfGetNextSendingRate);
    
    // auto end = std::chrono::steady_clock::now();
    // std::cout << "BP-GetNextSendingRate: "<< _numOfGetNextSendingRate <<" ,microseconds: "
    //    << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
    //    << " ms," << " sendingRate: " << retValue << std::endl;
    return retValue;
}

void PccBPRateController::MonitorIntervalFinished(const MonitorInterval& mi) { 
    
    ++_numOfMonitorIntervalFinished;
    // auto start = std::chrono::steady_clock::now();

    float utility = mi.GetUtility();
    this->sensorBThread_->notifyOnMonitorEvent(mi);

    // auto end = std::chrono::steady_clock::now();
    // std::cout << "BP-MonitorIntervalFinished: "<< _numOfMonitorIntervalFinished <<" ,microseconds: "
    //    << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
    //    << " ms" << std::endl;
}

void PccBPRateController::initializeBPProgram() {

  statisticsFileHandler_ = new StatisticsFileHandler(); // TODO: add statistics file name as parameter.
  bProgram_ = new BProgram();
  sendRateBthread_ = new SendRateBthread();
  sendRateActuatorBThread_ = new SendRateActuatorBThread();
  sendRateActuatorBThread_->setStatisticsFileHandler(statisticsFileHandler_);
  sensorBThread_ = new SensorBThread();
  odnn_BThread_ = new ODNN_BThread();
  odnn_BThread_->setStatisticsFileHandler(statisticsFileHandler_);
  identifyRTTDeviationBThread_ = new IdentifyRTTDeviationBThread();
  restoreThroughputBThread_ = new RestoreThroughputBThread();
  monitorNetworkStateBThread_ = new MonitorNetworkStateBThread();

  bProgram_->addSensorThread( "SensorBThread" ); //SensorBThread is a sensor thread
  bProgram_->addSensorThread( "SendRateBthread" ); //SendRateBthread is a sensor thread
  
  bProgram_->addThread(*sensorBThread_);
  bProgram_->addThread(*sendRateBthread_);
  bProgram_->addThread(*odnn_BThread_);
  bProgram_->addThread(*sendRateActuatorBThread_);
  bProgram_->addThread(*identifyRTTDeviationBThread_);
  bProgram_->addThread(*restoreThroughputBThread_);
  bProgram_->addThread(*monitorNetworkStateBThread_);


  bProgram_->runEngine();
  bProgram_->runProgram();
}
