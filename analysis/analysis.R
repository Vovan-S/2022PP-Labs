require("dplyr")

setwd("analysis")

ms <- unique(openmp_mean$M)
cols <- c('red', 'blue', 'green', 'orange', 'purple')

openmp <- read.csv(file = "openmp.csv", header = TRUE,  sep = ",")
openmp_mean <- aggregate(elapsed ~ M + NP, openmp, mean)
png("../report/images/openmp1.png", width=500, height=600)
with(openmp_mean, plot(
    NP[M == ms[1]], elapsed[M == ms[1]], 
    log="xy", type="l", ylim=c(min(elapsed)*0.8, max(elapsed)*1.2),
    col=cols[1], xlab="Number of threads, n", ylab="Runtime, s",
    main="OpenMP"
))
with(openmp_mean, legend(
    max(NP) * 0.3, max(elapsed), legend=paste("M =", ms),
    fill=cols    
))
for (i in 1:length(ms)) {
    with(openmp_mean, lines(
        NP[M == ms[i]], elapsed[M == ms[i]],
        col=cols[i]
    ))
}
dev.off()

pthreads <- read.csv(file = "pthreads.csv", header = TRUE,  sep = ",")
pthreads_mean <- aggregate(elapsed ~ M + NP, pthreads, mean)
png("../report/images/pthreads1.png", width=500, height=600)
with(pthreads_mean, {
    plot(
        NP[M == ms[1]], elapsed[M == ms[1]], 
        log="xy", type="l", ylim=c(min(elapsed)*0.8, max(elapsed)*1.2),
        col=cols[1], xlab="Number of threads, n", ylab="Runtime, s",
        main="Posix threads"
    )
    legend(max(NP) * 0.3, max(elapsed) * 1.2, legend=paste("M =", ms), fill=cols)
    for (i in 1:length(ms)) {
        lines(NP[M == ms[i]], elapsed[M == ms[i]], col=cols[i])
    }
})
dev.off()

cmpi <- read.csv(file = "cmpi.csv", header = TRUE,  sep = ",")
cmpi_mean <- aggregate(elapsed ~ M + NP, cmpi, mean)
cmpi_mean <- cmpi_mean[cmpi_mean$NP != max(cmpi_mean$NP),]
png("../report/images/cmpi1.png", width=500, height=600)
with(cmpi_mean, {
    plot(
        NP[M == ms[1]], elapsed[M == ms[1]], 
        log="xy", type="l", ylim=c(min(elapsed)*0.8, max(elapsed)*1.2),
        col=cols[1], xlab="Number of threads, n", ylab="Runtime, s",
        main="C MPI (1 node)"
    )
    legend(max(NP) * 0.3, max(elapsed), legend=paste("M =", ms), fill=cols)
    for (i in 1:length(ms)) {
        lines(NP[M == ms[i]], elapsed[M == ms[i]], col=cols[i])
    }
})
dev.off()

