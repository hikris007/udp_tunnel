//#include <CLI/CLI.hpp>
//#include <iostream>
//#include <spdlog/sinks/basic_file_sink.h>
//#include "header/Logger.h"
//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_color_sinks.h"
//#include "hv/EventLoop.h"

//int main() {
//    auto logger = spdlog::stdout_color_mt("console");
//    spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
//    logger->info("Hello, {}!", "world");
//    logger->error("This is an error message");
//
//    hv::EventLoopPtr eventLoopPtr = std::make_shared<hv::EventLoop>();
//
//    hv::TimerID id = eventLoopPtr->setInterval(1000, [&logger](hv::TimerID timerID){
//        logger->info("Hello, {}!", timerID);
//    });
//
//    logger->info("Hello jjj, {}!", id);
//
//    eventLoopPtr->run();
//    return 0;
//}
//int main() {
//    // 获取当前时间点
//    auto now = std::chrono::system_clock::now();
//
//    // 转换为自纪元以来的毫秒数
//    auto epoch_time = now.time_since_epoch();
//    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch_time).count();
//
//    // 输出
//    std::cout << "当前时间戳 (毫秒级): " << millis << std::endl;
//    LOGGER_INFO("Hey {}", "Kris");
//    return 0;
//}

#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include "boost/statechart/custom_reaction.hpp"
#include <iostream>
#include <string>

using namespace boost::statechart;
namespace sc = boost::statechart;

//定义状态码
enum CameraStateCode{
    CameraStateCodeClosed,
    CameraStateCodeOpening,
    CameraStateCodeOpened,
    CameraStateCodeCloseing
};

//定义事件
struct CameraEventOpening : event<CameraEventOpening> {};
struct CameraEvetOpened : event<CameraEvetOpened> {};
struct CameraEventCloseing : event<CameraEventCloseing> {};
struct CameraEventClosed : event<CameraEventClosed> {};

struct CameraStateClosed;
struct CameraStateOpening;
struct CameraStateOpened;
struct CameraStateClosing;


//定义一个状态机
struct StateManager : state_machine<StateManager, CameraStateClosed>
{
    StateManager(void)
    {
        std::cout << "construct StateManager" << std::endl;
    }

    ~StateManager(void)
    {
        std::cout << "distruct StateManager" << std::endl;
    }

    int current_state_code_ = CameraStateCodeClosed;

    int CurrentState()  {
        return current_state_code_;
    }

};

struct CameraStateClosed : simple_state<CameraStateClosed, StateManager> {

    typedef sc::custom_reaction<CameraEventOpening> reactions;
    sc::result react(const CameraEventOpening & event) {
        context<StateManager>().current_state_code_ = CameraStateCodeOpening;
        return transit<CameraStateOpening>();
    }

};

struct CameraStateOpening : simple_state<CameraStateOpening, StateManager> {

    typedef sc::custom_reaction<CameraEvetOpened> reactions;
    sc::result react(const CameraEvetOpened & event) {
        context<StateManager>().current_state_code_ = CameraStateCodeOpened;
        return transit<CameraStateOpened>();
    }
};

struct CameraStateOpened : simple_state<CameraStateOpened, StateManager> {

    typedef sc::custom_reaction<CameraEventCloseing> reactions;
    sc::result react(const CameraEventCloseing & event) {
        context<StateManager>().current_state_code_ = CameraStateCodeCloseing;
        return transit<CameraStateClosing>();
    }
};

struct CameraStateClosing : simple_state<CameraStateClosing, StateManager> {

    typedef sc::custom_reaction<CameraEventClosed> reactions;
    sc::result react(const CameraEventClosed & event) {
        context<StateManager>().current_state_code_ = CameraStateCodeClosed;
        return transit<CameraStateClosed>();
    }
};

using namespace std;

int main()
{
    StateManager state_manager;
    state_manager.initiate();
    int current_state_code = state_manager.current_state_code_;
    cout << current_state_code << endl;
    state_manager.process_event(CameraEventOpening());
    current_state_code = state_manager.CurrentState();
    cout << current_state_code << endl;
    state_manager.process_event(CameraEvetOpened());
    current_state_code = state_manager.CurrentState();
    cout << current_state_code << endl;
    state_manager.process_event(CameraEventCloseing());
    current_state_code = state_manager.CurrentState();
    cout << current_state_code << endl;
    std::string str = "1.1.1.1:8080:";
//    int index = str.find_last_of(":");
    int index = str.find(":");
    std::cout << index << std::endl;
    return 0;
}
