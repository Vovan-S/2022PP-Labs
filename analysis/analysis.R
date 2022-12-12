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
        col=cols[1], xlab="Number of processes, n", ylab="Runtime, s",
        main="C MPI (1 node)"
    )
    legend(max(NP) * 0.3, max(elapsed), legend=paste("M =", ms), fill=cols)
    for (i in 1:length(ms)) {
        lines(NP[M == ms[i]], elapsed[M == ms[i]], col=cols[i])
    }
})
dev.off()

pympi <- read.csv(file = "pympi.csv", header = TRUE,  sep = ",")
pympi_mean <- aggregate(elapsed ~ M + NP, pympi, mean)
# cmpi_mean <- cmpi_mean[cmpi_mean$NP != max(cmpi_mean$NP),]
png("../report/images/pympi1.png", width=500, height=600)
with(pympi_mean, {
    plot(
        NP[M == ms[1]], elapsed[M == ms[1]], 
        log="xy", type="l", ylim=c(min(elapsed)*0.8, max(elapsed)*1.2),
        col=cols[1], xlab="Number of processes, n", ylab="Runtime, s",
        main="Python MPI (1 node)"
    )
    legend(min(NP), 1.2*max(elapsed), legend=paste("M =", ms), fill=cols)
    for (i in 1:length(ms)) {
        lines(NP[M == ms[i]], elapsed[M == ms[i]], col=cols[i])
    }
})
dev.off()

pympi2 <- read.csv(file = "pympi2.csv", header = TRUE,  sep = ",")
pympi5 <- read.csv(file = "pympi5.csv", header = TRUE,  sep = ",")
cmpi2 <- read.csv(file = "cmpi2.csv", header = TRUE,  sep = ",")
cmpi5 <- read.csv(file = "cmpi5.csv", header = TRUE,  sep = ",")

mean(aggregate(elapsed ~ NP, cmpi2, sd)$elapsed)
mean(aggregate(elapsed ~ NP, cmpi5, sd)$elapsed)
mean(aggregate(elapsed ~ NP, pympi2, sd)$elapsed)
mean(aggregate(elapsed ~ NP, pympi5, sd)$elapsed)


n2 <- data.frame(
    np = unique(pympi2$NP), 
    cmpi = aggregate(elapsed ~ NP, cmpi2, mean)$elapsed,
    pympi = aggregate(elapsed ~ NP, pympi2, mean)$elapsed
)

png("../report/images/mpi2.png", width=500, height=600)
with(n2, {
    plot(
        np, cmpi, col="red", log="xy", type="l",
        ylim=c(min(cmpi, pympi) * 0.8, max(cmpi, pympi) * 1.2),
        xlab="Number of processes, N", ylab="Runtime, s",
        main="MPI, 2 nodes"
    )
    lines(
        np, pympi, col="blue"
    )
    legend(min(np), 1.2*max(pympi), legend=c("Python MPI", "C MPI"), fill=c("blue", "red"))
})
dev.off()

n5 <- data.frame(
    np = unique(pympi5$NP), 
    cmpi = aggregate(elapsed ~ NP, cmpi5, mean)$elapsed,
    pympi = aggregate(elapsed ~ NP, pympi5, mean)$elapsed
)

png("../report/images/mpi5.png", width=500, height=600)
with(n5, {
    plot(
        np, cmpi, col="red", log="xy", type="l",
        ylim=c(min(cmpi, pympi) * 0.8, max(cmpi, pympi) * 1.2),
        xlab="Number of processes, N", ylab="Runtime, s",
        main="MPI, 5 nodes"
    )
    lines(
        np, pympi, col="blue"
    )
    legend(min(np), 1.2*max(pympi), legend=c("Python MPI", "C MPI"), fill=c("blue", "red"))
})
dev.off()
