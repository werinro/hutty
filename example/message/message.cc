#include "MessageInitializerChannelHandler.h"
#include "../../strap/bootstrap.h"
#include "../../io/niosocketchannel.h"
#include "../../schedule/threadeventloopgroup.h"


using namespace wlr;

int main(int argc, char**argv)
{
	ThreadPool* boos_loop = new ThreadPool(2, 3);
	ThreadPool* worker_loop = new ThreadPool(10, 100, 1024 << 10);

	FutureListener* listener = new LambdaFutureListener([](wlr::LambdaFutureListener::State state, wlr::ChannelFuture* future, std::string msg) {
		LOG_DEBUG("future state = %d, future = %p, msg = %s\n", state, future, msg.c_str());
	});

	ServerBootstrap* server_boot = new ServerBootstrap(new wlr::ThreadEventLoopGroup(boos_loop), new wlr::ThreadEventLoopGroup(worker_loop));
	server_boot->channel(new NioSocketChannelFactory())
				->handler(new MessageInitializerChannelHandler())
				->listener(listener)
				->address(32145, "0.0.0.0");
	wlr::ChannelFuture* future = server_boot->sync();
	server_boot->waitForClose();

	return 0;
}

