#!/bin/bash
KEY=~/.ssh/scc2_sennov
LOGIN=tm5u5
HOST=login1.hpc.spbstu.ru
LH="${LOGIN}@${HOST}"

ssh -i ${KEY} ${LH} 'rm -r 09Sem/*'
scp -i ${KEY} -r solution ${LH}:09Sem/solution
scp -i ${KEY} -r test ${LH}:09Sem/test
scp -i ${KEY} launcher ${LH}:09Sem/.

