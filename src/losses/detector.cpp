#include "detector.hpp"

namespace giraffic::losses
{
    period::period(std::int32_t periodNumber, std::int32_t packetsByPeriod)
        : m_packets_by_period(packetsByPeriod), m_current(packetsByPeriod, -1), m_expected(packetsByPeriod), m_last_packet(0), m_losses(-1)
    {
        std::iota(m_expected.begin(), m_expected.end(), periodNumber* packetsByPeriod);
    }

    period::~period()
    {

    }

    std::int32_t period::GetLoss()
    {
        if (m_losses == -1)
        {
            CalcLoss();
        }

        return m_losses;
    }

    std::int32_t period::GetTotal()
    {
        return (std::int32_t)m_expected.size();
    }

    void period::CalcLoss()
    {
        std::vector<std::int32_t> _intersection;
        std::sort(m_current.begin(), m_current.end());
        std::set_intersection(m_expected.begin(), m_expected.end(), m_current.begin(), m_current.end(), std::back_inserter(_intersection));

        m_losses = m_expected.size() - _intersection.size();
    }

    void period::AddPacket(std::int32_t packetNumber)
    {
        m_current[m_last_packet] = packetNumber;
        ++m_last_packet;
    }

    detector::detector(const std::chrono::seconds& evaluatePeriod, const std::chrono::seconds& currentPeriod, std::int32_t packetsPerEvaluatePeriod)
        : m_evaluate_period(evaluatePeriod), m_current_period(currentPeriod), m_packets_by_period(packetsPerEvaluatePeriod), m_last_period(0)
    {

    }

    void detector::OnPacketReceived(std::int32_t packetSerialNumber)
    {
        auto _now = std::chrono::system_clock::now();

        if (m_last_point == std::chrono::system_clock::time_point{})
        {
            m_last_point = _now;
        }

        auto _dur = std::chrono::duration_cast<std::chrono::seconds>(_now - m_last_point);

        if (_dur >= m_evaluate_period)
        {
            ++m_last_period;
        }

        std::unique_lock<std::shared_mutex> _lock(m_packets_lock);
        auto  _packet = m_packets.try_emplace(m_last_period, m_last_period, m_packets_by_period);
        _packet.first->second.AddPacket(packetSerialNumber);

        if (_dur >= m_evaluate_period)
        {
            m_last_point = std::chrono::system_clock::now();
        }
    }    

    std::int32_t detector::GetCurrentLoss()
    {
        std::shared_lock<std::shared_mutex> _lock(m_packets_lock);

        if (m_packets.size() < m_current_period.count())
        {
            return 0;
        }

        return CalcLoss(m_current_period.count());
    }

    std::int32_t detector::GetAverageLoss()
    {
        std::shared_lock<std::shared_mutex> _lock(m_packets_lock);

        return CalcLoss(m_packets.size());
    }

    std::int32_t detector::CalcLoss(std::int32_t periodsCount)
    {
        auto _beg = m_packets.end();
        auto _end = m_packets.end();

        std::advance(_beg, -periodsCount);

        std::int32_t _total = 0;
        std::int32_t _loss = 0;

        for (auto _it = _beg; _it != _end; ++_it)
        {
            _total += _it->second.GetTotal();
            _loss += _it->second.GetLoss();
        }

        return ((std::double_t)_loss / (std::double_t)_total) * 100;
    }
}