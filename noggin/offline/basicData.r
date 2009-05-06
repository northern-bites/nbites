# read in the data
name <- "../../../fakeData/rightLeft/rightLeft"

ekffile <- paste(name, ".ekf.core", sep="")
ekfNoAmbigFile <- paste(name, ".ekf.core.na", sep="")
mclfile5 <- paste(name, ".mcl.core.5", sep="")
mclfile10 <- paste(name, ".mcl.core.10", sep="")
mclfile50 <- paste(name, ".mcl.core.50", sep="")
mclfile100 <- paste(name, ".mcl.core.100", sep="")
mclfile500 <- paste(name, ".mcl.core.500", sep="")
mclfile1000 <- paste(name, ".mcl.core.1000", sep="")
mclfile5best <- paste(name, ".mcl.core.5.best", sep="")
mclfile10best <- paste(name, ".mcl.core.10.best", sep="")
mclfile50best <- paste(name, ".mcl.core.50.best", sep="")
mclfile100best <- paste(name, ".mcl.core.100.best", sep="")
mclfile500best <- paste(name, ".mcl.core.500.best", sep="")
mclfile1000best <- paste(name, ".mcl.core.1000.best", sep="")

ekfdata <- read.table(ekffile)
ekfnadata <- read.table(ekfNoAmbigFile)
mcldata5 <- read.table(mclfile5)
mcldata5best <- read.table(mclfile5best)
mcldata10 <- read.table(mclfile10)
mcldata10best <- read.table(mclfile10best)
mcldata50 <- read.table(mclfile50)
mcldata50best <- read.table(mclfile50best)
mcldata100 <- read.table(mclfile100)
mcldata100best <- read.table(mclfile100best)
mcldata500 <- read.table(mclfile500)
mcldata500best <- read.table(mclfile500best)
mcldata1000 <- read.table(mclfile1000)
mcldata1000best <- read.table(mclfile1000best)

# Get usable column names
coreColNames <- c("x est", "y est", "h est", "x uncert", "y uncert", "h uncert", "ball x est", "ball y est", "ball x vel est", "ball y vel est", "ball x uncert", "ball y uncert", "ball x vel uncert", "ball y vel uncert", "delta f", "delta l", "delta r", "known x", "known y", "known h", "known ball x", "known ball y", "known ball x vel", "known ball y vel")
colnames(ekfdata) <- coreColNames
colnames(ekfnadata) <- coreColNames
colnames(mcldata5) <- coreColNames
colnames(mcldata5best) <- coreColNames
colnames(mcldata10) <- coreColNames
colnames(mcldata10best) <- coreColNames
colnames(mcldata50) <- coreColNames
colnames(mcldata50best) <- coreColNames
colnames(mcldata100) <- coreColNames
colnames(mcldata100best) <- coreColNames
colnames(mcldata500) <- coreColNames
colnames(mcldata500best) <- coreColNames
colnames(mcldata1000) <- coreColNames
colnames(mcldata1000best) <- coreColNames

# Frame data
frames <- rownames(ekfdata)

ekfX <- ekfdata[,"x est"]
ekfY <- ekfdata[,"y est"]
ekfH <- ekfdata[,"h est"]

ekfNAX <- ekfnadata[,"x est"]
ekfNAY <- ekfnadata[,"y est"]
ekfNAH <- ekfnadata[,"h est"]

mclX100 <- mcldata100[,"x est"]
mclY100 <- mcldata100[,"y est"]
mclH100 <- mcldata100[,"h est"]
mclX100best <- mcldata100best[,"x est"]
mclY100best <- mcldata100best[,"y est"]
mclH100best <- mcldata100best[,"h est"]

mclX5 <- mcldata5[,"x est"]
mclY5 <- mcldata5[,"y est"]
mclH5 <- mcldata5[,"h est"]
mclX5best <- mcldata5best[,"x est"]
mclY5best <- mcldata5best[,"y est"]
mclH5best <- mcldata5best[,"h est"]

mclX10 <- mcldata10[,"x est"]
mclY10 <- mcldata10[,"y est"]
mclH10 <- mcldata10[,"h est"]
mclX10best <- mcldata10best[,"x est"]
mclY10best <- mcldata10best[,"y est"]
mclH10best <- mcldata10best[,"h est"]


mclX50 <- mcldata50[,"x est"]
mclY50 <- mcldata50[,"y est"]
mclH50 <- mcldata50[,"h est"]
mclX50best <- mcldata50best[,"x est"]
mclY50best <- mcldata50best[,"y est"]
mclH50best <- mcldata50best[,"h est"]

mclX500 <- mcldata500[,"x est"]
mclY500 <- mcldata500[,"y est"]
mclH500 <- mcldata500[,"h est"]
mclX500best <- mcldata500best[,"x est"]
mclY500best <- mcldata500best[,"y est"]
mclH500best <- mcldata500best[,"h est"]

mclX1000 <- mcldata1000[,"x est"]
mclY1000 <- mcldata1000[,"y est"]
mclH1000 <- mcldata1000[,"h est"]
mclX1000best <- mcldata1000best[,"x est"]
mclY1000best <- mcldata1000best[,"y est"]
mclH1000best <- mcldata1000best[,"h est"]


knownX <- mcldata100[,"known x"]
knownY <- mcldata100[,"known y"]
knownH <- mcldata100[,"known h"]





# Plot minMaxs
frameMinMax = c(1,length(frames))
xMinMax = c(min(c(min(ekfX),min(knownX),min(mclX100),min(mclX100best))),max(c(max(ekfX),max(knownX),max(mclX100),max(mclX100best))))
yMinMax = c(min(c(min(ekfY),min(knownY),min(mclY100))),max(c(max(ekfY),max(knownY),max(mclY100))))
hMinMax = c(min(c(min(ekfH),min(knownH),min(mclH100))),max(c(max(ekfH),max(knownH),max(mclH100))))

# Plot the x data
pdf(paste(name, "-xEsts.pdf", sep=""))
plot(frames, mclX100, main="X Estimates", xlab="Frame #", ylab="x est (cm)", pch="",xlim=frameMinMax,ylim=xMinMax)
points(frames, knownX, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfX, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclX100, lty = 5, col = "blue", type="l", lwd="2")
points(frames, ekfNAX, lty = 4, col = "purple", type="l", lwd="2")
points(frames, mclX50, lty = 5, col = "brown", type="l", lwd="2")
legend("topleft",lty=c(1,2,5), c("Truth", "EKF", "MCL 100", "EKF-NA", "MCL 50"), fill=c("green", "red", "blue", "purple","brown"))
dev.off()

# Plot the y data
pdf(paste(name, "-yEsts.pdf", sep=""))
plot(frames, mclY100, main="Y Estimates", xlab="Frame #", ylab="y est (cm)", pch="",xlim=frameMinMax,ylim=yMinMax)
points(frames, knownY, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfY, lty = 2, col = "red", type="l",lwd="2")
points(frames, mclY100, lty = 5, col = "blue", type="l",lwd="2")
points(frames, ekfNAY, lty = 4, col = "purple", type="l", lwd="2")
points(frames, mclY50, lty = 5, col = "brown", type="l", lwd="2")
legend("topright",lty=c(1,2,5), c("Truth", "EKF", "MCL 100", "EKF-NA", "MCL 50"), fill=c("green", "red", "blue", "purple","brown"))
dev.off()

# Plot the x data with best
pdf(paste(name, "-xEstsBest.pdf", sep=""))
plot(frames, mclX100, main="X Estimates", xlab="Frame #", ylab="x est (cm)", pch="",xlim=frameMinMax,ylim=xMinMax)
points(frames, knownX, lty = 1, col = "green", type="l", lwd="2")
points(frames, mclX100best, lty = 6, col = "red", type="l",lwd="2")
points(frames, mclX100, lty = 5, col = "blue", type="l", lwd="2")
legend("topleft",lty=c(1,2,5), c("Truth", "MCL", "MCL best"), fill=c("green", "blue", "red"))
dev.off()

# Plot the y data with best
pdf(paste(name, "-yEstsBest.pdf", sep=""))
plot(frames, mclY100, main="Y Estimates", xlab="Frame #", ylab="y est (cm)", pch="",xlim=frameMinMax,ylim=yMinMax)
points(frames, knownY, lty = 1, col = "green", type="l", lwd="2")
points(frames, mclY100best, lty = 6, col = "red", type="l",lwd="2")
points(frames, mclY100, lty = 5, col = "blue", type="l",lwd="2")
legend("top",lty=c(1,2,5), c("Truth", "MCL", "MCL best"), fill=c("green", "blue", "red"))
dev.off()

# Plot the h data
pdf(paste(name, "-hEsts.pdf", sep=""))
plot(frames, mclH100, main="H Estimates", xlab="Frame #", ylab="h est (rad)", pch="",xlim=frameMinMax,ylim=hMinMax)
points(frames, knownH, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfH, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclH100, lty = 5, col = "blue", type="l", lwd="2")
points(frames, mclH500, lty = 5, col = "pink", type="l", lwd="2")
#points(frames, mclH100best, lty = 6, col = "pink", type="l",lwd="2")
points(frames, ekfNAH, lty = 4, col = "purple", type="l", lwd="2")
legend("topleft",lty=c(1,2,5), c("Truth", "EKF", "MCL 100", "EKF-NA", "MCL 500"), fill=c("green", "red", "blue", "purple", "pink"))
dev.off()

# Plot the h data
pdf(paste(name, "-hEsts2.pdf", sep=""))
plot(frames, mclH100, main="H Estimates", xlab="Frame #", ylab="h est (rad^2)", pch="",xlim=frameMinMax,ylim=c(0,max(hMinMax)^2))
points(frames, knownH^2, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfH^2, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclH100^2, lty = 5, col = "blue", type="l", lwd="2")
points(frames, mclH100best^2, lty = 6, col = "pink", type="l",lwd="2")
points(frames, ekfNAH^2, lty = 4, col = "purple", type="l", lwd="2")
legend("topleft",lty=c(1,2,5), c("Truth", "EKF", "MCL", "EKF-NA", "MCL best"), fill=c("green", "red", "blue", "purple", "pink"))
dev.off()

# Get the errors of each method
ekfError <- sqrt((ekfX - knownX)^2 + (ekfY - knownY)^2)
ekfNAError <- sqrt((ekfNAX - knownX)^2 + (ekfNAY - knownY)^2)
mcl5Error <- sqrt((mclX5 - knownX)^2 + (mclY5 - knownY)^2)
mcl10Error <- sqrt((mclX10 - knownX)^2 + (mclY10 - knownY)^2)
mcl50Error <- sqrt((mclX50 - knownX)^2 + (mclY50 - knownY)^2)
mcl100Error <- sqrt((mclX100 - knownX)^2 + (mclY100 - knownY)^2)
mcl500Error <- sqrt((mclX500 - knownX)^2 + (mclY500 - knownY)^2)
mcl1000Error <- sqrt((mclX1000 - knownX)^2 + (mclY1000 - knownY)^2)
mcl5bestError <- sqrt((mclX5best - knownX)^2 + (mclY5best - knownY)^2)
mcl10bestError <- sqrt((mclX10best - knownX)^2 + (mclY10best - knownY)^2)
mcl50bestError <- sqrt((mclX50best - knownX)^2 + (mclY50best - knownY)^2)
mcl100bestError <- sqrt((mclX100best - knownX)^2 + (mclY100best - knownY)^2)
mcl500bestError <- sqrt((mclX500best - knownX)^2 + (mclY500best - knownY)^2)
mcl1000bestError <- sqrt((mclX1000best - knownX)^2 + (mclY1000best - knownY)^2)

errorMinMax = c(
  min(c(min(ekfError),
        min(ekfNAError),
        min(mcl5Error),
        #min(mcl5bestError),
        #min(mcl10Error),
        #min(mcl10bestError),
        #min(mcl50Error),
        #min(mcl50bestError),
        min(mcl100Error),
        #min(mcl100bestError),
        #min(mcl500Error),
        #min(mcl500bestError),
        min(mcl1000Error)
        #min(mcl1000bestError)
        )),
  max(c(max(ekfError),
        max(ekfNAError),
        max(mcl5Error),
        #max(mcl5bestError),
        #max(mcl10Error),
        #max(mcl10bestError),
        #max(mcl50Error),
        #max(mcl50bestError),
        max(mcl100Error),
        #max(mcl100bestError),
        #max(mcl500Error),
        #max(mcl500bestError),
        max(mcl1000Error)
        #max(mcl1000bestError)
        )))

# Plot error vs. time (frame)
pdf(paste(name, "-errorVFrame.pdf", sep=""))
plot(frames, mcl5Error, main="Position Error", xlab="Frame #", ylab="error (cm)", pch="",xlim=frameMinMax,ylim=errorMinMax)
points(frames, ekfError, lty = 1, col = "red", type="l", lwd="2")
points(frames, ekfNAError, lty = 2, col = "blue", type="l", lwd="2")
points(frames, mcl5Error, lty = 1, col = "green", type="l", lwd="2")
#points(frames, mcl5bestError, lty = 2, col = "blue", type="l",lwd="2")
#points(frames, mcl10Error, lty = 1, col = "brown", type="l", lwd="2")
#points(frames, mcl10bestError, lty = 2, col = "brown", type="l",lwd="2")
#points(frames, mcl50Error, lty = 1, col = "pink", type="l", lwd="2")
#points(frames, mcl50bestError, lty = 2, col = "pink", type="l",lwd="2")
points(frames, mcl100Error, lty = 1, col = "brown", type="l", lwd="2")
#points(frames, mcl100bestError, lty = 2, col = "green", type="l",lwd="2")
#points(frames, mcl500Error, lty = 1, col = "yellow", type="l", lwd="2")
#points(frames, mcl500bestError, lty = 2, col = "yellow", type="l",lwd="2")
points(frames, mcl1000Error, lty = 1, col = "black", type="l", lwd="2")
#points(frames, mcl1000bestError, lty = 2, col = "black", type="l",lwd="2")

legend("topright", c("EKF","EKF-NA","MCL-5","MCL-100", "MCL-1000"), fill=c("red","blue","green","brown","black"))
dev.off()

# Plot error vs. num particles
ekfTotalError <- sqrt(sum((ekfError)^2)/length(frames))
ekfNATotalError <- sqrt(sum((ekfNAError)^2)/length(frames))
mcl5TotalError <-  sqrt(sum((mcl5Error)^2)/length(frames))
mcl10TotalError <- sqrt(sum((mcl10Error)^2)/length(frames))
mcl50TotalError <- sqrt(sum((mcl50Error)^2)/length(frames))
mcl100TotalError <- sqrt(sum((mcl100Error)^2)/length(frames))
mcl500TotalError <- sqrt(sum((mcl500Error)^2)/length(frames))
mcl1000TotalError <- sqrt(sum((mcl1000Error)^2)/length(frames))
mcl5bestTotalError <-  sqrt(sum((mcl5bestError)^2)/length(frames))
mcl10bestTotalError <- sqrt(sum((mcl10bestError)^2)/length(frames))
mcl50bestTotalError <- sqrt(sum((mcl50bestError)^2)/length(frames))
mcl100bestTotalError <- sqrt(sum((mcl100bestError)^2)/length(frames))
mcl500bestTotalError <- sqrt(sum((mcl500bestError)^2)/length(frames))
mcl1000bestTotalError <- sqrt(sum((mcl1000bestError)^2)/length(frames))

totalErrorMinMax = c(
  min(c(min(ekfTotalError),
        min(ekfNATotalError),
        min(mcl5TotalError),
        min(mcl5bestTotalError),
        min(mcl10TotalError),
        min(mcl10bestTotalError),
        min(mcl50TotalError),
        min(mcl50bestTotalError),
        min(mcl100TotalError),
        min(mcl100bestTotalError),
        min(mcl500TotalError),
        min(mcl500bestTotalError),
        min(mcl1000TotalError),
        min(mcl1000bestTotalError)
        )),
  max(c(max(ekfTotalError),
        max(ekfNATotalError),
        max(mcl5TotalError),
        max(mcl5bestTotalError),
        max(mcl10TotalError),
        max(mcl10bestTotalError),
        max(mcl50TotalError),
        max(mcl50bestTotalError),
        max(mcl100TotalError),
        max(mcl100bestTotalError),
        max(mcl500TotalError),
        max(mcl500bestTotalError),
        max(mcl1000TotalError),
        max(mcl1000bestTotalError)
        )))

errorVParticlesX <- c(5,10,50,100,500,1000)
errorVParticlesY <- c(mcl5TotalError,
                      mcl10TotalError,
                      mcl50TotalError,
                      mcl100TotalError,
                      mcl500TotalError,
                      mcl1000TotalError)
errorVParticlesbestY <- c(mcl5bestTotalError,
                          mcl10bestTotalError,
                          mcl50bestTotalError,
                          mcl100bestTotalError,
                          mcl500bestTotalError,
                          mcl1000bestTotalError)

pdf(paste(name, "-errorVparticles.pdf", sep=""))
plot(errorVParticlesX, errorVParticlesY, log="x", main="RMS of Position Error vs Num Particles", xlab="Num particles", ylab="Error RMS", pch="", ylim=totalErrorMinMax)
points(errorVParticlesX, errorVParticlesY, lty = 1, col = "blue", type="l", lwd="2")
points(errorVParticlesX, errorVParticlesbestY, lty = 1, col = "purple", type="l", lwd="2")
abline(ekfTotalError, 0, lty = 3, col = "red", type="l", lwd="2")
abline(ekfNATotalError, 0, lty = 2, col = "green", type="l", lwd="2")
legend("topright", c("EKF","EKF-NA","MCL-mean","MCL-best"), fill=c("red","green","blue","purple"))
dev.off()


# get RMS of the headings
ekfHrms <- sqrt(sum((ekfH - knownH)^2)/length(knownH))
ekfNAHrms <- sqrt(sum((ekfNAH - knownH)^2)/length(knownH))
mcl5Hrms <-  sqrt(sum((mclH5 - knownH)^2)/length(knownH))
mcl10Hrms <- sqrt(sum((mclH10 - knownH)^2)/length(knownH))
mcl50Hrms <- sqrt(sum((mclH50 - knownH)^2)/length(knownH))
mcl100Hrms <- sqrt(sum((mclH100 - knownH)^2)/length(knownH))
mcl500Hrms <- sqrt(sum((mclH500 - knownH)^2)/length(knownH))
mcl1000Hrms <- sqrt(sum((mclH1000 - knownH)^2)/length(knownH))
mcl5bestHrms <-  sqrt(sum((mclH5best - knownH)^2)/length(knownH))
mcl10bestHrms <- sqrt(sum((mclH10best - knownH)^2)/length(knownH))
mcl50bestHrms <- sqrt(sum((mclH50best - knownH)^2)/length(knownH))
mcl100bestHrms <- sqrt(sum((mclH100best - knownH)^2)/length(knownH))
mcl500bestHrms <- sqrt(sum((mclH500best - knownH)^2)/length(knownH))
mcl1000bestHrms <- sqrt(sum((mclH1000best - knownH)^2)/length(knownH))


errorVParticlesH <- c(mcl5Hrms,
                      mcl10Hrms,
                      mcl50Hrms,
                      mcl100Hrms,
                      mcl500Hrms,
                      mcl1000Hrms)
errorVParticlesHbest <- c(mcl5bestHrms,
                          mcl10bestHrms,
                          mcl50bestHrms,
                          mcl100bestHrms,
                          mcl500bestHrms,
                          mcl1000bestHrms)
print(errorVParticlesH)
pdf(paste(name, "-HeadingRMS.pdf", sep=""))
plot(x=errorVParticlesX, y=errorVParticlesH, log="x", main="Heading Error RMS vs Num Particles", xlab="Num particles", ylab="Error RMS", pch="", ylim=c(0,max(max(errorVParticlesH),max(errorVParticlesHbest))))
points(errorVParticlesX, errorVParticlesH, lty = 1, col = "blue", type="l", lwd="2")
points(errorVParticlesX, errorVParticlesHbest, lty = 1, col = "purple", type="l", lwd="2")
abline(ekfHrms, 0, lty = 3, col = "red", type="l", lwd="2")
abline(ekfNAHrms, 0, lty = 2, col = "green", type="l", lwd="2")
legend("topright", c("EKF","EKF-NA","MCL-mean","MCL-best"), fill=c("red","green","blue","purple"))
dev.off()
