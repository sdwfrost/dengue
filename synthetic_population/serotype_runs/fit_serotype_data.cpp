#include <iostream>
#include <vector>
#include <numeric>
#include <unistd.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "mpi.h"
#include "AbcSmc.h"

using namespace std;

const int RUN_LENGTH_YEARS = 36;
const gsl_rng* RNG = gsl_rng_alloc (gsl_rng_taus2);


void setup_mpi(MPI_par &m, int &argc, char **argv) {
    /* MPI variables */
    m.comm  = MPI_COMM_WORLD;
    m.info  = MPI_INFO_NULL;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(m.comm, &m.mpi_size);
    MPI_Comm_rank(m.comm, &m.mpi_rank);
}


vector<long double> generateSerotypeSequence(vector<long double> args, const MPI_par* mp) {
    const long double geometric_mean_run = args[0];
    const long double geometric_mean_gap = args[1];
    vector<long double> obs_run_and_gap_means (2,0.0);
    vector<int> runs;
    vector<int> gaps;
    
    enum State {GAP, RUN};
    // get rid of anything there now

    const float p_gap = 1.0/geometric_mean_gap;
    const float p_run = 1.0/geometric_mean_run;
    const float p_gap_start = geometric_mean_gap/(geometric_mean_gap + geometric_mean_run);

    int years_so_far = 0;
    State state = p_gap_start > gsl_rng_uniform(RNG) ? GAP : RUN;
    while (years_so_far < RUN_LENGTH_YEARS) {
        if (state == GAP) {
            unsigned int gap = gsl_ran_geometric(RNG, p_gap);
            // We may not be starting at the beginning of a gap
            // Also: gsl_rng_uniform_int() returns ints on [0,n-1]
            if (years_so_far == 0) gap = gsl_rng_uniform_int(RNG, gap) + 1;
            // Truncate if we've gone over the observation window
            if (years_so_far + gap > RUN_LENGTH_YEARS) gap = RUN_LENGTH_YEARS - years_so_far;
            //cerr << "g" << gap << " ";
            gaps.push_back(gap);
            years_so_far += gap;
            state = RUN; // switch state
        } else {
            unsigned int run = gsl_ran_geometric(RNG, p_run);
            if (years_so_far == 0) run = gsl_rng_uniform_int(RNG, run) + 1;
            if (years_so_far + run > RUN_LENGTH_YEARS) run = RUN_LENGTH_YEARS - years_so_far;
            //cerr << "r" << run << " ";
            runs.push_back(run);
            years_so_far += run;
            state = GAP; // switch state
        }
    }
    //cerr << endl;

    if (runs.size() > 0) {
        obs_run_and_gap_means[0] = accumulate(runs.begin(), runs.end(), 0.0) / runs.size();
    } else {
        obs_run_and_gap_means[0] = 1; // minimum possible value
    }

    if (gaps.size() > 0) {
        obs_run_and_gap_means[1] = accumulate(gaps.begin(), gaps.end(), 0.0) / gaps.size();
    } else {
        obs_run_and_gap_means[1] = 1;
    }

    return obs_run_and_gap_means;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "\n\tUsage: ./executable abc_config.json\n\n";
        return 100;
    }

    MPI_par mp;
    setup_mpi(mp, argc, argv);

    gsl_rng_set(RNG, time (NULL) * getpid()); 
    AbcSmc* abc = new AbcSmc(mp);
    abc->set_simulator(generateSerotypeSequence);
    abc->parse_config(string(argv[1]));
    abc->run(RNG);

    MPI_Finalize(); 
    return 0;
}
