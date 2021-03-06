#include <lo2s/monitor/cpu_set_monitor.hpp>

#include <lo2s/error.hpp>
#include <lo2s/topology.hpp>
#include <lo2s/util.hpp>

#include <csignal>

namespace lo2s
{
namespace monitor
{
CpuSetMonitor::CpuSetMonitor() : MainMonitor()
{
    trace_.register_monitoring_tid(gettid(), "CpuSetMonitor", "CpuSetMonitor");

    for (const auto& cpu : Topology::instance().cpus())
    {
        Log::debug() << "Create cstate recorder for cpu #" << cpu.id;
        auto ret = monitors_.emplace(std::piecewise_construct, std::forward_as_tuple(cpu.id),
                                     std::forward_as_tuple(cpu.id, trace_));
        assert(ret.second);
        (void)ret;
    }
}

void CpuSetMonitor::run()
{
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss, SIGINT);

    auto ret = pthread_sigmask(SIG_BLOCK, &ss, NULL);
    if (ret)
    {
        Log::error() << "Failed to set pthread_sigmask: " << ret;
        throw std::runtime_error("Failed to set pthread_sigmask");
    }

    for (auto& monitor_elem : monitors_)
    {
        monitor_elem.second.start();
    }

    trace_.register_tids(read_all_tid_exe());

    int sig;
    ret = sigwait(&ss, &sig);
    if (ret)
    {
        throw make_system_error();
    }

    trace_.register_tids(read_all_tid_exe());

    for (auto& monitor_elem : monitors_)
    {
        monitor_elem.second.stop();
    }

    for (auto& monitor_elem : monitors_)
    {
        monitor_elem.second.merge_trace();
    }
}
}
}
