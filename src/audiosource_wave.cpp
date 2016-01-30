

#include "audiosource_wave.h"
#include "extconfig.h"


namespace extmodem {

audiosource_wave::audiosource_wave(int sample_rate) : audiosource(sample_rate) {

}

audiosource_wave::~audiosource_wave() {
    
}

void audiosource_wave::loop_async() {
    thread_ = boost::thread(boost::bind(&audiosource_wave::loop_async_thread_proc, this));
}

void audiosource_wave::loop_async_thread_proc() {
    config* cfg = config::Instance();
    int buf_size = (cfg->frames_per_buff() > 0) ? cfg->frames_per_buff() : (get_sample_rate());
    int num_channels = get_out_channel_count();
    auto l = get_listener();
    std::vector<float> bufferf;

    bufferf.resize(buf_size);

    for (;;) {
        l->output_callback(this, bufferf.data(), bufferf.size());
        l->input_callback(this, bufferf.data(), bufferf.size());

        boost::this_thread::sleep(boost::posix_time::milliseconds( 1000 * buf_size / get_sample_rate() / num_channels ));
    }
}

}

