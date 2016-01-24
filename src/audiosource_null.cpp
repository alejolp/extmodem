
#include "audiosource_null.h"


namespace extmodem {

audiosource_null::audiosource_null(int sample_rate) : audiosource(sample_rate) {}

audiosource_null::~audiosource_null() {
	
}

void audiosource_null::loop_async() {
	
}

}

