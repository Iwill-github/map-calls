#include <cstddef>

enum class FilterMode { movingAverage, firstDifference };

struct SignalView {
    const float* samples;
    std::size_t count;
};

struct SignalStats {
    std::size_t count;
    float mean;
    float peak;
};

namespace {

using Filter = float (*)(SignalView input, std::size_t index);

std::size_t previousIndex(std::size_t index)
{
    if (index == 0) {
        return 0;
    }
    return index - 1;
}

float sampleAt(SignalView input, std::size_t index)
{
    return input.samples[index];
}

float averageWindow(float first, float middle, float last)
{
    return (first + middle + last) / 3.0f;
}

float movingAverage(SignalView input, std::size_t index)
{
    const std::size_t previous = previousIndex(index);
    return averageWindow(sampleAt(input, previousIndex(previous)),
                         sampleAt(input, previous), sampleAt(input, index));
}

float firstDifference(SignalView input, std::size_t index)
{
    return sampleAt(input, index) - sampleAt(input, previousIndex(index));
}

Filter selectFilter(FilterMode mode)
{
    if (mode == FilterMode::firstDifference) {
        return firstDifference;
    }
    return movingAverage;
}

float sampleMagnitude(float sample)
{
    if (sample < 0.0f) {
        return -sample;
    }
    return sample;
}

void trackPeak(float& peak, float sample)
{
    const float magnitude = sampleMagnitude(sample);
    if (magnitude > peak) {
        peak = magnitude;
    }
}

void updateStats(SignalStats& stats, float sample)
{
    stats.mean += sample;
    trackPeak(stats.peak, sample);
}

SignalStats summarizeSignal(SignalView input, Filter filter)
{
    SignalStats stats{input.count, 0.0f, 0.0f};
    for (std::size_t index = 0; index < input.count; ++index) {
        updateStats(stats, filter(input, index));
    }
    stats.mean /= static_cast<float>(stats.count);
    return stats;
}

} // namespace

// For nonempty input, samples refers to count readable finite values.
SignalStats analyzeSignal(SignalView input, FilterMode mode)
{
    if (input.samples == nullptr || input.count == 0) {
        return {};
    }
    const Filter filter = selectFilter(mode);
    return summarizeSignal(input, filter);
}
