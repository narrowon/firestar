#pragma once

#include "../../../game/game.hpp"

class StrafePred final
{
public:
    // Main calculation function
public:
    std::deque<float> m_prediction_history;
    float m_smoothed_yaw = 0.0f;
    float m_last_raw_yaw{};
    // --- Constants ---
    static constexpr size_t MIN_POINTS_FOR_FIT = 3;
    static constexpr float MIN_POINT_SEPARATION = 1.0f;     // Minimum distance between any two points
    static constexpr float MAX_YAW_DIFF_CONSISTENCY = 0.1f; // Max allowed change between consecutive yaw differences (radians) - smaller means straighter arcs
    static constexpr float MIN_YAW_CHANGE_MAGNITUDE = 0.05f; // Minimum absolute difference between consecutive yaws to avoid fitting circles to near-straight lines (radians)
    static constexpr float MIN_VALID_RADIUS = 4.0f;         // Minimum radius for generated circle points (debug) and yaw calculation
    static constexpr float MIN_YAW_PER_TICK = -7.0f;        // Min clamp value for predicted yaw change (degrees or radians per tick - adjust units as needed)
    static constexpr float MAX_YAW_PER_TICK = 7.0f;         // Max clamp value
    static constexpr int DEBUG_CIRCLE_POINTS = 64;          // Number of points for debug circle visualization
    static constexpr size_t SMOOTHING_WINDOW = 30;      // Number of frames for moving average
    static constexpr float MAX_SMOOTHING_DEVIATION = 32.0f; // Radians beyond which to reset smoothing
    static constexpr float DYNAMIC_FILTER_STRENGTH = 0.6f; // Base filter strength (0.0-1.0)
    float g_predicted_ad_pos = 0.0f;

    // --- Configuration Constants (Consider making these tunable) ---
// How much the filter strength is influenced by player speed (higher = more influence)
    // Decay factor for weighting historical yaw differences (lower = faster decay)
    static constexpr float HISTORY_WEIGHT_DECAY = 0.90f;
    // Minimum points required even when prediction is perfectly stable
    static constexpr size_t MIN_REQUIRED_POINTS_BASE = 3;
    // Additional points required based on instability (higher = more points needed when unstable)
    static constexpr size_t MIN_REQUIRED_POINTS_STABILITY_FACTOR = 5;
    // --- End Configuration Constants ---

    bool pointsSane(const std::vector<vec3>& world_points, float& out_strafe_direction);

    // Projects points to 2D (XY plane) and centers them around their centroid
    bool projectAndNormalizePoints(const std::vector<vec3>& world_points, vec3& out_offset, std::vector<vec2>& out_normalized_points);

    // Fits a circle to the 2D points using least squares
    bool fitCircleToPoints(const std::vector<vec2>& points, float& out_cx, float& out_cy, float& out_cr);
    float calcStrafeHitchance(const std::vector<vec2>& points, float cx, float cy, float cr);
    bool performCircleFit(const std::vector<vec2>& points, float& cx, float& cy, float& cr);

    std::vector<vec2> removeOutliers(const std::vector<vec2>& points, float cx, float cy, float cr);
    // Generates points on the fitted circle for debugging
    bool generateCirclePoints(float cx, float cy, float cr, float z, const vec3& offset, int num_points, std::vector<vec3>& out_circle_points);
    bool validateCircleFit(const std::vector<vec3>& actual_points, const float cx, const float cy, const float cr, const vec3& offset);
    bool calc(const std::vector<vec3>& points, const float player_speed, float& yaw_per_tick);
    bool isADStrafe(const std::vector<vec3>& world_points, float& out_strafe_axis, float& out_last_dir); bool predictADStrafe(const std::vector<vec3>& world_points, float player_speed, float& out_predicted_pos);
    bool predictMovement(const std::vector<vec3>& world_points, float player_speed, vec3& predicted_position);
};


MAKE_UNIQUE(StrafePred, strafe_pred);