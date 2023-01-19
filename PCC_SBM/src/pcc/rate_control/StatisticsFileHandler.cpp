#include "StatisticsFileHandler.h"

StatisticsFileHandler::StatisticsFileHandler() : log_file_samples("monitor_intervals_bpc.txt", std::ios::out)
{
	this->counter = 1;
	if (log_file_samples.is_open()) {
        std::cout << "BPC - log_file_samples opened succesufflly\n";
    }
    else
    {
        std::cout << "Error opening log_file_samples\n";
    }

    log_file_samples << "\ttype\t" << "calc_sending_rate\t" << "real_sending_rate\t" << "bytes_sent\t" << "bytes_acked\t" << "bytes_lost\t";
    log_file_samples << "send_start_time_sec\t" << "send_end_time_sec\t" << "recv_start_time_sec\t" << "recv_end_time_sec\t";
    log_file_samples << "first_ack_latency_sec\t" << "last_ack_latency_sec\t" << "packet_size\t" << "rtt_dev\t" << "utility\n";

}

StatisticsFileHandler::~StatisticsFileHandler()
{
	log_file_samples.close();
}

void StatisticsFileHandler::LogGiveSample(int bytes_sent,
                  int bytes_acked,
                  int bytes_lost,
                  double send_start_time_sec,
                  double send_end_time_sec,
                  double recv_start_time_sec,
                  double recv_end_time_sec,
                  double first_ack_latency_sec,
                  double last_ack_latency_sec,
                  int packet_size,
                  double utility,
                  float rtt_dev)
{
	m.lock();
	 if(counter < BP_COUNTER_LIMIT) 
    {
        log_file_samples.precision(7);
        log_file_samples << counter << "\t"<< 1 << "\t\t\t" << bytes_sent << "\t" << bytes_acked << "\t" << bytes_lost << "\t";
        log_file_samples << std::fixed << send_start_time_sec << "\t" << send_end_time_sec << "\t" << recv_start_time_sec << "\t" << recv_end_time_sec << "\t";
        log_file_samples << first_ack_latency_sec << "\t" << last_ack_latency_sec << "\t" << packet_size << "\t" << rtt_dev << "\t" << utility << "\n" << std::flush;
        counter = counter + 1;
    }
    m.unlock();
}

void StatisticsFileHandler::LogSendingRate(QuicBandwidth sending_rate, int event_type)
{
	m.lock();
	if(counter < BP_COUNTER_LIMIT)
    {
        log_file_samples.precision(7);
        if(event_type == 2)
        {
        	log_file_samples << counter << "\t" << event_type << "\t" << std::fixed << sending_rate <<"\n" << std::flush;
        }
        else
        {
        	log_file_samples << counter << "\t" << event_type << "\t\t" << std::fixed << sending_rate <<"\n" << std::flush;	
        }
        ++counter;
    }
    m.unlock();
}