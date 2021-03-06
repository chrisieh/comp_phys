#include <iostream>
#include <fstream>
#include <vector>
#include "bootstrap.h"
#include "binning_method.h"
#include "../autocorrelation/autocorrelation.h"

struct ans_binning_method {
    double mean = 0.0;
    double stddev = 0.0;
    double stddev_error = 0.0;
};

ans_binning_method binning_method(const std::vector<double> &series,
                                  std::size_t block_sz) {
    ans_binning_method ret;

    // block the series and calculate mean of bins
    std::vector<double> mean_bins;
    block_mean(series.cbegin(), series.cend(), std::back_inserter(mean_bins),
               block_sz);

    // mean and standard deviation of new series
    ret.mean = mean(mean_bins.cbegin(), mean_bins.cend());
    ret.stddev = std_err(mean_bins.cbegin(), mean_bins.cend());

    // observable called on bootstrap samples (standard deviation)
    auto vec_stddev = [](const std::vector<double> &v) {
        return std_err(v.cbegin(), v.cend());
    };

    // estimate error of standard deviation using bootstrap (100 samples)
    ret.stddev_error =
        bootstrap_error(mean_bins.cbegin(), mean_bins.cend(), vec_stddev, 1000);

    return ret;
}

#include <iterator>
#include <algorithm>
int main() {
    // Time series from exercise 3
    auto series = time_series(100000, 0.7, 0.1);

    std::ofstream os("binning_method.tsv");
    for (std::size_t block_sz = 2; os && block_sz <= 50000; block_sz *= 1.5) {
        auto result = binning_method(series, block_sz);
        os << block_sz << "\t" << result.mean << "\t"
           << result.stddev << "\t" << result.stddev_error << "\n";
    }

    return 0;
}
