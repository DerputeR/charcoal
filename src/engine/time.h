#pragma once
#include <cstdint>

namespace Charcoal {
/**
 * @class Time
 * @brief Helper class for keeping track of time.
 *
 */
class Time {
    static constexpr int64_t ONE_SECOND_NS = 1000000000;

    int64_t min_frame_time = 0;

    // bool vsync_enabled = false;
    // bool vsync_adaptive = true;
    int64_t time_ns_last = 0;
    int64_t time_ns_now = 0;
    int64_t time_ns_delta = 0;

    int64_t frame_count = 0;
    int64_t total_time = 0;

public:
    /**
     * @brief Sets the FPS cap, which in turn updates the reported minimum frame
     * time.
     *
     * @param fps_cap Targetted maximum FPS. Values <= 0 means uncapped
     * framerate.
     */
    void set_fps_cap(int64_t fps_cap);

    /**
     * @brief Triggers an update of this time instance, updating the frame count
     * and the delta time calculation.
     *
     * @param ns The time that has passed, in nanoseconds, since the last
     * update. This is TOTAL time, NOT delta time.
     * @param new_frame If true, this update marks the beginning of a new frame.
     */
    void update(int64_t ns, bool new_frame);

    /**
     * @brief Gets the frame count this class has tracked so far.
     *
     * @return The frame count.
     */
    int64_t get_frame_count() const;

    /**
     * @brief Gets the minimum time, in nanoseconds, required to pass before the
     * next frame should be rendered.
     *
     * @return The minimum frame time in nanoseconds
     */
    int64_t get_min_frame_time_ns() const;

    /**
     * @brief Gets the time that has passed since the last frame.
     *
     * @return The delta time in nanoseconds
     */
    int64_t get_delta_ns() const;

    /**
     * @brief Helper function to convert time in nanoseconds (as an integer) to
     * time in seconds (as a float)
     *
     * @param ns Time in nanoseconds
     * @return Time in seconds
     */
    static float ns_to_f32(int64_t ns);

    /**
     * @brief Helper function to convert time in nanoseconds (as an integer) to
     * time in seconds (as a double)
     *
     * @param ns Time in nanoseconds
     * @return Time in seconds
     */
    static double ns_to_f64(int64_t ns);

    /**
     * @brief Gets the total time that has been added so far.
     *
     * @return The total time in nanoseconds
     */
    int64_t get_total_time() const;
};
} // namespace Charcoal
