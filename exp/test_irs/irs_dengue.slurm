#!/bin/bash
#SBATCH --job-name=who-dengue
#SBATCH --output=/ufrc/longini/tjhladish/irs_weekly_output/reg_%A_%a-vv.out
#SBATCH --error=./auto_output/reg_%A_%a-vv.err

#SBATCH --mail-type=ALL
#SBATCH --mail-user=tjhladish@gmail.com

#SBATCH --account=epi
#SBATCH --qos=epi-b
#SBATCH --workdir=/home/tjhladish/work/dengue/exp/test_irs
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=4000
#SBATCH --time=24:00:00
#SBATCH --array=0-2699

module load gcc/5.2.0 gsl
for i in `seq 1 100`;
do
    ./abc_sql abc_vacvec.json --simulate
done
