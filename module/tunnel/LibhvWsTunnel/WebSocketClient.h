#ifndef UDP_TUNNEL_WEBSOCKET_CLIENT_H_
#define UDP_TUNNEL_WEBSOCKET_CLIENT_H_

/*
 * 这个类是完全拷贝 Libhv WebSocketClient 的实现
 * 我们只是做了一个小改动 start -> runInLoop
 */

#include "hv/TcpClient.h"
#include "hv/WebSocketChannel.h"

#include "hv/HttpParser.h"
#include "hv/WebSocketParser.h"

namespace omg {

class WebSocketClient : public hv::TcpClientTmpl<hv::WebSocketChannel> {
public:
    std::string           url;
    std::function<void()> onopen;
    std::function<void()> onclose;
    std::function<void(const std::string& msg)> onmessage;
    // PATCH: onmessage not given opcode
    enum ws_opcode opcode() { return channel ? channel->opcode : WS_OPCODE_CLOSE; }

    WebSocketClient(hv::EventLoopPtr loop = NULL);
    virtual ~WebSocketClient();

    // url = ws://ip:port/path
    // url = wss://ip:port/path
    int open(const char* url, const http_headers& headers = DefaultHeaders);
    int open(const std::string& url, const http_headers& headers = DefaultHeaders);
    int close();
    int send(const std::string& msg);
    int send(const char* buf, int len, enum ws_opcode opcode = WS_OPCODE_BINARY);

    // setConnectTimeout / setPingInterval / setReconnect
    void setPingInterval(int ms) {
        ping_interval = ms;
    }

    // NOTE: call before open
    void setHttpRequest(const HttpRequestPtr& req) {
        http_req_ = req;
    }

    // NOTE: call when onopen
    const HttpResponsePtr& getHttpResponse() {
        return http_resp_;
    }

private:
    enum State {
        CONNECTING,
        CONNECTED,
        WS_UPGRADING,
        WS_OPENED,
        WS_CLOSED,
    } state;
    HttpParserPtr       http_parser_;
    HttpRequestPtr      http_req_;
    HttpResponsePtr     http_resp_;
    WebSocketParserPtr  ws_parser_;
    // ping/pong
    int                 ping_interval;
    int                 ping_cnt;
};

}

#endif // UDP_TUNNEL_WEBSOCKET_CLIENT_H_
