#pragma once
#include <cstdint>

namespace Charcoal::Engine {
    class Time {
        static constexpr int64_t ONE_SECOND_NS = 1000000000;

        int64_t min_frame_time = 0;

        // bool vsync_enabled = false;
        // bool vsync_adaptive = true;
        int64_t time_ns_last = 0;
        int64_t time_ns_now = 0;
        int64_t time_ns_delta = 0;

        int64_t frame_count = 0;

    public:
        void set_fps_cap(int64_t fps_cap);
        int64_t get_fps_cap();
        void update(int64_t ns, bool new_frame);
        int64_t get_frame_count();
        int64_t get_min_frame_time_ns();
        int64_t get_delta_ns();
        /*float get_delta_f32();*/
        /*double get_delta_f64();*/

        static float ns_to_f32(int64_t ns);
        static double ns_to_f64(int64_t ns);
    };
}
