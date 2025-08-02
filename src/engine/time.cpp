#include "time.h"

namespace Charcoal {
    void Time::set_fps_cap(int64_t fps_cap) {
        if (fps_cap == 0) {
            min_frame_time = 0;
        }
        else {
            min_frame_time = ONE_SECOND_NS / fps_cap;
        }
    }

    void Time::update(int64_t ns, bool new_frame) {
        // todo: overflow protection
        if (new_frame) {
            time_ns_last = time_ns_now;
            ++frame_count;
        }
        time_ns_now = ns;
        time_ns_delta = time_ns_now - time_ns_last;
    }

    int64_t Time::get_frame_count() {
        return frame_count;
    }

    int64_t Time::get_fps_cap() {
        if (min_frame_time == 0) {
            return 0;
        } else {
            return ONE_SECOND_NS / min_frame_time;
        }
    }

    int64_t Time::get_min_frame_time_ns() {
        return min_frame_time;
    }

    int64_t Time::get_delta_ns() {
        return time_ns_delta;
    }

    float Time::ns_to_f32(int64_t ns) {
        return static_cast<float>(static_cast<double>(ns) / static_cast<double>(Time::ONE_SECOND_NS));
    }

    double Time::ns_to_f64(int64_t ns) {
        return static_cast<double>(ns) / static_cast<double>(Time::ONE_SECOND_NS);
    }
}
