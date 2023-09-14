#include "nghttp.h"
#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif // HAVE_UNISTD_H
#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#endif // HAVE_FCNTL_H
#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif // HAVE_NETINET_IN_H
#include <netinet/tcp.h>
#include <getopt.h>
 
#include <cassert>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <tuple>

#include <openssl/err.h>

#ifdef HAVE_JANSSON
#  include <jansson.h>
#endif // HAVE_JANSSON

#include "app_helper.h"
#include "HtmlParser.h"
#include "util.h"
#include "base64.h"
#include "tls.h"
#include "template.h"
#include "ssl_compat.h"

namespace nghttp2 {



/// HttpClient 回调函数
#pragma region HTTPClient------callback
namespace {
  void verto_readcb(verto_ctx *verto_loop, verto_ev *ev) {
    printf("\nThis is verto_readcb\n\n");

    // auto client = static_cast<HttpClient *>(verto_get_private(ev));
    // if (client->do_read() != 0)  {
    //   client->verto_disconnect();
    // }
    // printf("\n      Finish verto_readcb\n");

  }
}



namespace {
void verto_writecb(verto_ctx *verto_loop, verto_ev *ev) {
  printf("\nThis is verto_writecb\n\n");

//   auto client = static_cast<HttpClient *>(verto_get_private(ev));
//   auto rv = client->do_write();
//   if(rv == HttpClient::ERR_CONNECT_FAIL) {
//     client->verto_connect_fail();
//     return;
//   }
//   if (rv != 0) {
//     client->verto_disconnect();
//   }
//   printf("\n      Finish verto_writecb\n\n");

}
}



namespace {
void verto_timeoutcb(verto_ctx *verto_loop, verto_ev *ev) {
  printf("\nThis is verto_timeoutcb\n\n");
//   auto client = static_cast<HttpClient *>(verto_get_private(ev));
//   std::cerr << "[ERROR] Timeout" << std::endl;
//   client->verto_disconnect();
//   printf("\n      Finish timeoutcb\n");
}
}



namespace {
void verto_settings_timeout_cb(verto_ctx *verto_loop, verto_ev *ev) {
  printf("\nThis is verto_settings_timeout_cb\n\n");
//   auto client = static_cast<HttpClient *>(verto_get_private(ev));
//   verto_del(ev);

//   nghttp2_session_terminate_session(client->session, NGHTTP2_SETTINGS_TIMEOUT);
//   client->verto_signal_write();

  printf("\n      Finish verto_settings_timeout_cb\n\n");
}
}
#pragma endregion




#pragma region HttpClient 构造函数
HttpClient::HttpClient(const nghttp2_session_callbacks *callbacks, verto_ctx *verto_loop, SSL_CTX *ssl_ctx)
    : wb(&mcpool),
      session(nullptr),
      callbacks(callbacks),
      verto_loop(verto_loop),
      ssl_ctx(ssl_ctx),
      ssl(nullptr),
      addrs(nullptr),
      next_addr(nullptr),
      cur_addr(nullptr),
      complete(0),
      success(0),
      settings_payloadlen(0),
      state(ClientState::IDLE),
      upgrade_response_status_code(0),
      fd(-1),
      upgrade_response_complete(false) {
      

  printf("\nlibverto: Client = %p,  verto_loop = %p, verto_wev = %p, verto_rev = %p, verto_wt = %p, verto_rt = %p, verto_setting_timer = %p\n\n\n", 
        this, verto_loop, verto_wev, verto_rev, verto_wt, verto_rt, verto_settings_timer);

  verto_wev = verto_add_io(verto_loop, VERTO_EV_FLAG_IO_WRITE, verto_writecb, 1);
  verto_rev = verto_add_io(verto_loop, VERTO_EV_FLAG_IO_READ, verto_readcb, 0);

  verto_wt = verto_add_timeout(verto_loop, VERTO_EV_FLAG_NONE, verto_timeoutcb, 0);
  verto_rt = verto_add_timeout(verto_loop, VERTO_EV_FLAG_NONE, verto_timeoutcb, 0);
  verto_settings_timer = verto_add_timeout(verto_loop, VERTO_EV_FLAG_NONE, verto_settings_timeout_cb, 10);

  verto_set_private(verto_wev, this, NULL);
  verto_set_private(verto_rev, this, NULL);
  verto_set_private(verto_wt,  this, NULL);
  verto_set_private(verto_rt,  this, NULL);
  verto_set_private(verto_settings_timer,  this, NULL);
  printf("\nThis libverto HttpClient\n");
}
#pragma endregion






/// 关闭Client连接
#pragma region HttpClient::verto_disconnect
void HttpClient::verto_disconnect() {
  printf("\nThis is libverto disconnect\n");

  state = ClientState::IDLE;

  for (auto req = std::begin(reqvec); req != std::end(reqvec); ++req) {
    if ((*req)->continue_timer) {
      (*req)->continue_timer->stop();
    }
  }

  verto_del(verto_settings_timer);
  verto_del(verto_rt);
  verto_del(verto_wt);
  verto_del(verto_rev);
  verto_del(verto_wev);

  nghttp2_session_del(session);
  session = nullptr;

  if (ssl) {
    SSL_set_shutdown(ssl, SSL_get_shutdown(ssl) | SSL_RECEIVED_SHUTDOWN);
    ERR_clear_error();
    SSL_shutdown(ssl);
    SSL_free(ssl);
    ssl = nullptr;
  }

  if (fd != -1) {
    shutdown(fd, SHUT_WR);
    close(fd);
    fd = -1;
  }

  printf("\n      Finish libverto disconnect\n");
}
#pragma endregion












}

