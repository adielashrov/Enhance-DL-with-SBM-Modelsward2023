#include "pcc_python_rc.h"
#include <algorithm>

std::mutex PccPythonRateController::interpreter_lock_;
bool PccPythonRateController::python_initialized_ = false;

void PccPythonRateController::InitializePython() {
    Py_Initialize();
    PyRun_SimpleString("import sys");

    std::stringstream set_argv_ss;
    set_argv_ss << "sys.argv = [";
    wchar_t** unicode_args = new wchar_t*[Options::argc];
    for (int i = 0; i < Options::argc; ++i) {
        const char* arg = Options::argv[i];
        if (i == 0) {
            set_argv_ss << "\"" << arg << "\"";
        } else {
            set_argv_ss << ", \"" << arg << "\"";
        }
        int len = strlen(arg);
        std::wstring wc(len, L'#' );
        mbstowcs(&wc[0], arg, len);
        unicode_args[i] = &wc[0];
    }
    set_argv_ss << "]";
    std::string set_argv_str = set_argv_ss.str();
    PyRun_SimpleString(set_argv_str.c_str());

    python_initialized_ = true;
}

int PccPythonRateController::GetNextId() {
    static int next_id = 0;
    int id = next_id;
    ++next_id;
    return id;
}

PccPythonRateController::PccPythonRateController(double call_freq,
        PccEventLogger* log) :  log_file_samples("monitor_intervals_aurora_dl.txt", std::ios::out) {

    std::lock_guard<std::mutex> lock(interpreter_lock_);
    if (!python_initialized_) {
        InitializePython();
    }
    
    id = GetNextId();
    has_time_offset = false;
    time_offset_usec = 0;

    printf("PccPythonRateController initial - time_offset_usec %ld\n", time_offset_usec);

    const char* python_path_arg = Options::Get("-pypath="); // The location in which the pcc_addon.py file can be found.
    if (python_path_arg != NULL) {
        int python_path_arg_len = strlen(python_path_arg);
        char python_path_cmd_buf[python_path_arg_len + 50];
        sprintf(&python_path_cmd_buf[0], "sys.path.append(\"%s\")", python_path_arg);
        PyRun_SimpleString(&python_path_cmd_buf[0]);
        //std::cerr << "Adding python path: " << python_path_arg << std::endl;
    }

    const char* python_filename = "pcc_rate_controller";
    const char* python_filename_arg = Options::Get("-pyhelper=");
    if (python_filename_arg != NULL) {
        python_filename = python_filename_arg;
    }
    
    module = PyImport_ImportModule(python_filename);
    if (module == NULL) {
        std::cerr << "ERROR: Could not load python module: " << python_filename << std::endl;
        PyErr_Print();
        exit(-1);
    }
    
    PyObject* init_func = PyObject_GetAttrString(module, "init");
    if (init_func == NULL) {
        std::cerr << "ERROR: Could not load python function: init" << std::endl;
        PyErr_Print();
        exit(-1);
    }
    PyObject* id_obj = PyLong_FromLong(id);
    static PyObject* args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, id_obj);
    
    PyObject* init_result = PyObject_CallObject(init_func, args);
    PyErr_Print();
    
    give_sample_func = PyObject_GetAttrString(module, "give_sample");
    if (give_sample_func == NULL) {
        std::cerr << "ERROR: Could not load python function: give_sample" << std::endl;
        PyErr_Print();
        exit(-1);
    }
    
    get_rate_func = PyObject_GetAttrString(module, "get_rate");
    if (get_rate_func == NULL) {
        std::cerr << "ERROR: Could not load python function: get_rate" << std::endl;
        PyErr_Print();
        exit(-1);
    }
    
    reset_func = PyObject_GetAttrString(module, "reset");
    if (reset_func == NULL) {
        std::cerr << "ERROR: Could not load python function: reset" << std::endl;
        PyErr_Print();
        exit(-1);
    }

    counter = 1;
    _numOfGetNextSendingRate = 0;
    _numOfMonitorIntervalFinished = 0;
    // We should have 2+10 fields - starts with sending rate
    log_file_samples << "\ttype\t" << "sending_rate\t" << "bytes_sent\t" << "bytes_acked\t" << "bytes_lost\t";
    log_file_samples << "send_start_time_sec\t" << "send_end_time_sec\t" << "recv_start_time_sec\t" << "recv_end_time_sec\t";
    log_file_samples << "first_ack_latency_sec\t" << "last_ack_latency_sec\t" << "packet_size\t" << "utility\n" << std::flush;
}

PccPythonRateController::~PccPythonRateController() {
    log_file_samples.close();
    // log_file_sending_rate.close();
}

void PccPythonRateController::Reset() {
    std::cout << "Starting Reset" << std::endl;
    std::lock_guard<std::mutex> lock(interpreter_lock_);
    PyObject* id_obj = PyLong_FromLong(id);
    static PyObject* args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, id_obj);
    
    PyObject* result = PyObject_CallObject(reset_func, args);
    PyErr_Print();
}

void PccPythonRateController::GiveSample(int bytes_sent,
                                         int bytes_acked,
                                         int bytes_lost,
                                         double send_start_time_sec,
                                         double send_end_time_sec,
                                         double recv_start_time_sec,
                                         double recv_end_time_sec,
                                         double first_ack_latency_sec,
                                         double last_ack_latency_sec,
                                         int packet_size,
                                         double utility) {
    // Ignore - 
    // time calcualtaions as first step
    // packet_size is fixed - not documented!
    if(counter < COUNTER_LIMIT) 
    {
        log_file_samples.precision(7);
        //log_file_samples.precision(15);
        // V1 - of writing to the log file

        // log_file_samples << counter << "\t\t\t" << "bytes_sent: " << bytes_sent << " ,bytes_acked: " << bytes_acked << " ,bytes_lost: " << bytes_lost << " ,utility: " << utility << "\n"; 

        // V2 - of writing to the log file

        log_file_samples << counter << "\t"<< 1 << "\t\t" << bytes_sent << "\t" << bytes_acked << "\t" << bytes_lost << "\t";
        log_file_samples << std::fixed << send_start_time_sec << "\t" << send_end_time_sec << "\t" << recv_start_time_sec << "\t" << recv_end_time_sec << "\t";
        log_file_samples << first_ack_latency_sec << "\t" << last_ack_latency_sec << "\t" << packet_size << "\t" << utility << "\n";
        counter = counter + 1;
    }
    

    std::lock_guard<std::mutex> lock(interpreter_lock_);
    static PyObject* args = PyTuple_New(11);
    
    // flow_id
    PyTuple_SetItem(args, 0, PyLong_FromLong(id));
    
    // bytes_sent
    PyTuple_SetItem(args, 1, PyLong_FromLong(bytes_sent));
    
    // bytes_acked
    PyTuple_SetItem(args, 2, PyLong_FromLong(bytes_acked));
    
    // bytes_lost
    PyTuple_SetItem(args, 3, PyLong_FromLong(bytes_lost));
    
    // send_start_time
    PyTuple_SetItem(args, 4, PyFloat_FromDouble(send_start_time_sec));
    
    // send_end_time
    PyTuple_SetItem(args, 5, PyFloat_FromDouble(send_end_time_sec));
    
    // recv_start_time
    PyTuple_SetItem(args, 6, PyFloat_FromDouble(recv_start_time_sec));
    
    // recv_end_time
    PyTuple_SetItem(args, 7, PyFloat_FromDouble(recv_end_time_sec));

    // rtt_samples
    PyObject* rtt_samples = PyList_New(2);
    PyList_SetItem(rtt_samples, 0, PyLong_FromLong(first_ack_latency_sec));
    PyList_SetItem(rtt_samples, 1, PyLong_FromLong(last_ack_latency_sec));
    PyTuple_SetItem(args, 8, rtt_samples);
    
    // packet_size
    PyTuple_SetItem(args, 9, PyLong_FromLong(packet_size));
    
    // recv_end_time
    PyTuple_SetItem(args, 10, PyFloat_FromDouble(utility));
    
    PyObject_CallObject(give_sample_func, args);

}

void PccPythonRateController::MonitorIntervalFinished(const MonitorInterval& mi) {
    
    // ++_numOfMonitorIntervalFinished;
    // auto start = std::chrono::steady_clock::now();
    
    if (!has_time_offset) {
        time_offset_usec = mi.GetSendStartTime();    
        has_time_offset = true;
    }
    GiveSample(
        mi.GetBytesSent(),
        mi.GetBytesAcked(),
        mi.GetBytesLost(),
        (mi.GetSendStartTime() - time_offset_usec) / (double)USEC_PER_SEC,
        (mi.GetSendEndTime() - time_offset_usec) / (double)USEC_PER_SEC,
        (mi.GetRecvStartTime() - time_offset_usec) / (double)USEC_PER_SEC,
        (mi.GetRecvEndTime() - time_offset_usec) / (double)USEC_PER_SEC,
        mi.GetFirstAckLatency() / (double)USEC_PER_SEC,
        mi.GetLastAckLatency() / (double)USEC_PER_SEC,
        mi.GetAveragePacketSize(),
        mi.GetUtility()
    );
    
    // auto end = std::chrono::steady_clock::now();
    // std::cout << "PccPython-MonitorIntervalFinished: "<< _numOfMonitorIntervalFinished <<" ,microseconds: "
    //    << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
    //    << " ms" << std::endl;
}

QuicBandwidth PccPythonRateController::GetNextSendingRate(QuicBandwidth current_rate, QuicTime cur_time) {

    // ++_numOfGetNextSendingRate;
    // auto start = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(interpreter_lock_);
    
    PyObject* id_obj = PyLong_FromLong(id);
    static PyObject* args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, id_obj);
    
    PyObject* result = PyObject_CallObject(get_rate_func, args);
    if (result == NULL) {
        std::cout << "ERROR: Failed to call python get_rate() func" << std::endl;
        PyErr_Print();
        exit(-1);
    }
    
    double result_double = PyFloat_AsDouble(result);
    PyErr_Print();
    if (!PyFloat_Check(result)) {
        std::cout << "ERROR: Output from python get_rate() is not a float" << std::endl;
        exit(-1);
    }
    Py_DECREF(result);


    double sending_rate_log_div_by_10 = log10(result_double) / 10.0;
    if(counter < COUNTER_LIMIT)
    {
        log_file_samples.precision(7);
        
        // V1 - hard coded
        
        //log_file_samples << counter << "\t" << "current_rate: " << current_rate << " ,next_sending_rate: " << result_double << "\n";
        log_file_samples << counter << "\t" << 2 << "\t" << std::fixed << result_double << "\n" << std::flush;
        ++counter;
    }
    /*
    if (log_file_sending_rate.is_open()) {
        log_file_sending_rate << counter <<"\t" << result_double << std::endl;
        ++counter;
    }
    else
    {
        // show message:
        std::cout << "Error opening log_file_sending_rate";
    }*/

    // auto end = std::chrono::steady_clock::now();
    // std::cout << "PccPython-GetNextSendingRate: "<< _numOfGetNextSendingRate <<" ,microseconds: "
    //    << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
    //    << " ms" << std::endl;

    return result_double;
}
