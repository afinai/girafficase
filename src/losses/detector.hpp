#pragma once

#include <numeric>
#include <chrono>
#include <map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <iterator>
#include <atomic>

namespace giraffic
{
    namespace losses
    {
        class period
        {
        public:
            period(std::int32_t periodNumber, std::int32_t packetsByPeriod);
            ~period();

            void AddPacket(std::int32_t packetNumber);
            std::int32_t GetLoss();
            std::int32_t GetTotal();

        private:
            void CalcLoss();

        private:
            std::int32_t m_losses;
            std::int32_t m_last_packet;
            std::int32_t m_packets_by_period;
            std::vector<std::int32_t> m_expected;
            std::vector<std::int32_t> m_current;
        };

        class detector
        {
        public:
            detector(const std::chrono::seconds& evaluatePeriod, const std::chrono::seconds& currentPeriod, std::int32_t packetsPerEvaluatePeriod);
            ~detector() = default;

            detector(const detector&) = delete;
            detector& operator=(const detector&) = delete;
            detector(detector&&) = delete;
            detector&& operator=(detector&&) = delete;

        public:
            void OnPacketReceived(std::int32_t packetSerialNumber);
            std::int32_t GetCurrentLoss();
            std::int32_t GetAverageLoss();

        private:
            std::int32_t CalcLoss(std::int32_t periodsCount);

        private:
            std::int32_t m_packets_by_period;
            std::atomic<std::int32_t> m_last_period;
            std::chrono::seconds m_evaluate_period;
            std::chrono::seconds m_current_period;
            std::chrono::system_clock::time_point m_last_point;
            std::shared_mutex m_packets_lock;
            std::map<std::int32_t, period> m_packets;
        };
    }
}