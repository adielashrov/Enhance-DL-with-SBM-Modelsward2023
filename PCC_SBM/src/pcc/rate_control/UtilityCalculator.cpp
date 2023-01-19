#include "UtilityCalculator.h"

float UtilityCalculator::ComputeAvgRTT(MonitorInterval* interval)
{
	float sum_rtt = 0.0;
    float num_of_rtt = 0.0;
    float avg_rtt = 0.0;
    
    for (const PacketRttSample& rtt_sample : interval->GetPacketRttSamples()) {
        sum_rtt = sum_rtt + static_cast<float>(rtt_sample.rtt);
        ++num_of_rtt;
    }

    if(num_of_rtt > 0)
    {
        avg_rtt = sum_rtt / num_of_rtt;
    }

    return avg_rtt;
}

float UtilityCalculator::ComputeRttDeviation(MonitorInterval* interval)
{
	float rtt_dev = 0.0;
    float num_of_rtt = 0.0;
    if(interval->GetPacketRttSamples().size() < 2)
    {
        return rtt_dev;
    }

    float avg_rtt = UtilityCalculator::ComputeAvgRTT(interval); 
    for (const PacketRttSample& rtt_sample : interval->GetPacketRttSamples()) {
        float delta_rtt_sample = static_cast<float>(rtt_sample.rtt) - avg_rtt;
        rtt_dev += delta_rtt_sample * delta_rtt_sample; //sqaure
        ++num_of_rtt;
    }
    rtt_dev = sqrt(rtt_dev / num_of_rtt);
    return rtt_dev;
}