// Test that bootstrapping results match expected results from parameterized statistics.

#include <sm/vvec>
#include <sm/bootstrap>
#include <sm/random>

int main()
{
    int rtn = 0;

    // Test bootstrap computation of standard error of the mean for a distribution. To do so,
    // generate numbers from a normal distribution. Args of rand_normal constructor are mean, sigma.

    unsigned int num_bootstraps = 500;
    sm::vvec<double> absdiffs (num_bootstraps, 0.0);
    sm::rand_normal<double, std::mt19937_64> rnorm (5, 1);

    unsigned int num_samples = 1000;
    sm::vvec<double> normally_distributed (num_samples, 0.0);

    for (unsigned int i = 0; i < num_bootstraps; ++i) {
        normally_distributed.set_from (rnorm.get(num_samples));
        double eom = sm::bootstrap<double>::error_of_mean (normally_distributed, 512);
        double absdiff = std::abs(eom - normally_distributed.std()/std::sqrt(normally_distributed.size()));
        absdiffs[i] = absdiff;
    }

    std::cout << "Mean abs diff: " << absdiffs.mean() << " with std. err " << (absdiffs.std()/std::sqrt(absdiffs.size())) << std::endl;
    std::cout << "So, that means that absdiffs.mean() should be less than " << absdiffs.mean() + (2.0 * (absdiffs.std()/std::sqrt(absdiffs.size()))) << std::endl;

    if (absdiffs.mean() > 0.000924) {
        std::cerr << "Test of error_of_mean failed\n";
        --rtn;
    }

    // Now test the t-test 100 times:
    int n = 0;
    int sig_diff_fails = 0;
    int not_sig_diff_fails = 0;
    sm::vvec<double> non_sig_diffs (100, 0.0);
    while (n < 100) {
        // First a significantly different distribution
        sm::rand_normal<double, std::mt19937_64> rnorm_sigdiff (5.5, 1);
        sm::vvec<double> dist_sigdiff;
        dist_sigdiff.set_from (rnorm_sigdiff.get(num_samples));
        sm::vec<double,2> asl_sigdiff = sm::bootstrap<double>::ttest_equalityofmeans (normally_distributed, dist_sigdiff, num_bootstraps);
        std::cout << "Achieved significance level for different dist: " << asl_sigdiff[0] << " (with minasl: " << asl_sigdiff[1] << ")\n";
        // asl_sigdiff[0] should be 0.
        if (asl_sigdiff[0] > asl_sigdiff[1]) {
            std::cerr << "The distribution with a different mean has ASL = " << asl_sigdiff[0] << " which is TOO BIG.\n";
            sig_diff_fails++;
        }

        // Now a not significantly different (in terms of the mean) distribution
        sm::rand_normal<double, std::mt19937_64> rnorm_nonsigdiff (5.0, 1.5);
        sm::vvec<double> dist_nonsigdiff;
        dist_nonsigdiff.set_from (rnorm_nonsigdiff.get(num_samples));
        sm::vec<double,2> asl_nonsigdiff = sm::bootstrap<double>::ttest_equalityofmeans (normally_distributed, dist_nonsigdiff, num_bootstraps);
        std::cout << "Achieved significance level for non-different dist: " << asl_nonsigdiff[0] << " (with minasl: " << asl_nonsigdiff[1] << ")\n";
        // asl_nonsigdiff[0] should be about 0.4, but can be as low as 0.044
        non_sig_diffs[n] = asl_nonsigdiff[0];
        if (asl_nonsigdiff[0] < 0.001) {
            std::cerr << "The distribution with a NON different mean has ASL = " << asl_nonsigdiff[0] << " which is TOO SMALL.\n";
            not_sig_diff_fails++;
        }
        ++n;
    }

    sm::range<double> r = non_sig_diffs.minmax();
    std::cout << "Range of significance levels for non different distributions was " << r << std::endl;

    // Statistical variability seems to allow quite a few unexpected results. Pass the
    // test as long as we have less than 10% fails in both types of test
    if (sig_diff_fails < 10 && not_sig_diff_fails < 10) {
        std::cout << "Test passed (tests both < 10)";
    } else {
        std::cout << "Test FAILED (tests >= 10)";
    }
    std::cout << " with sig_diff_fails = " << sig_diff_fails
              << " and not_sig_diff_fails = " << not_sig_diff_fails << std::endl;

    return rtn;
}
