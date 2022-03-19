#include "MessageInitializerChannelHandler.h"
#include "../../strap/bootstrap.h"
#include "../../io/niosocketchannel.h"
#include "../../schedule/threadeventloopgroup.h"


using namespace wlr;

int main(int argc, char** argv)
{
	wlr::EventLoopGroup* boos_group = new wlr::ThreadEventLoopGroup(2);
	wlr::EventLoopGroup* worker_group = new wlr::ThreadEventLoopGroup();

	FutureListener* listener = new LambdaFutureListener([](wlr::LambdaFutureListener::State state, wlr::ChannelFuture* future, std::string msg) {
		LOG_DEBUG("future state = %d, future = %p, msg = %s\n", state, future, msg.c_str());
	});

	ServerBootstrap* server_boot = new ServerBootstrap(boos_group, worker_group);
	server_boot->channel(new NioSocketChannelFactory())
				->handler(new MessageInitializerChannelHandler())
				->listener(listener)
				->address(32145, "0.0.0.0");
	wlr::ChannelFuture* future = server_boot->sync();
	server_boot->waitForClose();
	LOG_DEBUG("future = %p\n", future);

	return 0;
}

