#!/bin/bash
ssh -i ~/.ssh/scc2_sennov tm5u5@login1.hpc.spbstu.ru 'rm -r 09Sem/*'
scp -i ~/.ssh/scc2_sennov -r solution tm5u5@login1.hpc.spbstu.ru:09Sem/solution
scp -i ~/.ssh/scc2_sennov -r test tm5u5@login1.hpc.spbstu.ru:09Sem/test
scp -i ~/.ssh/scc2_sennov launcher tm5u5@login1.hpc.spbstu.ru:09Sem/.

# rsync -azv --rsh="ssh -i ~/.ssh/scc2_sennov" solution tm5u5@login1.hpc.spbstu.ru:09Sem/solution
