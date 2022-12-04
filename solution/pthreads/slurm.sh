#!/bin/bash
#SBATCH --job-name=lab1-pthreads
#SBATCH --partition tornado
#SBATCH --nodes=1
#SBATCH --output=./res/out
#SBATCH --error=./res/err
#SBATCH --time=00:10:00

if [[ -f /etc/profile.d/modules-basis.sh ]]; then
source /etc/profile.d/modules-basis.sh
fi

module add compiler/gcc/11.2.0
module add python/3.9

make rebuild
make $1