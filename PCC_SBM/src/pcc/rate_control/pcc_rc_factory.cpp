#include "pcc_rc_factory.h"

PccRateController* PccRateControllerFactory::Create(const std::string& name, double call_freq, PccEventLogger* log) {
    
    if (name == "ixp") {
        return new PccIxpRateController(call_freq, log);
    } else if (name == "python") {
        return new PccPythonRateController(call_freq, log);
    }
    else if (name == "bp") {
        return new PccBPRateController(call_freq, log);   
    }
    return new PccIxpRateController(call_freq, log);
}
