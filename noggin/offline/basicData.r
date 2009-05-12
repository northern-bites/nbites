# read in the data
name <- "../../../fakeData/spinCenter/spinCenter"

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

mclfile5.1 <- paste(name, ".mcl.core.5-1", sep="")
mclfile10.1 <- paste(name, ".mcl.core.10-1", sep="")
mclfile50.1 <- paste(name, ".mcl.core.50-1", sep="")
mclfile100.1 <- paste(name, ".mcl.core.100-1", sep="")
mclfile500.1 <- paste(name, ".mcl.core.500-1", sep="")
mclfile1000.1 <- paste(name, ".mcl.core.1000-1", sep="")
mclfile5best.1 <- paste(name, ".mcl.core.5-1.best", sep="")
mclfile10best.1 <- paste(name, ".mcl.core.10-1.best", sep="")
mclfile50best.1 <- paste(name, ".mcl.core.50-1.best", sep="")
mclfile100best.1 <- paste(name, ".mcl.core.100-1.best", sep="")
mclfile500best.1 <- paste(name, ".mcl.core.500-1.best", sep="")
mclfile1000best.1 <- paste(name, ".mcl.core.1000-1.best", sep="")

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
plot(frames, mclX100, main="X Estimates", xlab="Frame Number", ylab="x (cm)", pch="",xlim=frameMinMax,ylim=xMinMax)
points(frames, knownX, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfX, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclX100, lty = 3, col = "blue", type="l", lwd="2")
points(frames, ekfNAX, lty = 4, col = "purple", type="l", lwd="2")
points(frames, mclX50, lty = 5, col = "brown", type="l", lwd="2")
legend("topleft",lty=c(1,2,3,4,5), c("Actual", "EKF", "MCL 100", "EKF-NA", "MCL 50"), col=c("green", "red", "blue", "purple","brown"))
dev.off()

# Plot the y data
pdf(paste(name, "-yEsts.pdf", sep=""))
plot(frames, mclY100, main="Y Estimates", xlab="Frame Number", ylab="y (cm)", pch="",xlim=frameMinMax,ylim=yMinMax)
points(frames, knownY, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfY, lty = 2, col = "red", type="l",lwd="2")
points(frames, mclY100, lty = 3, col = "blue", type="l",lwd="2")
points(frames, ekfNAY, lty = 4, col = "purple", type="l", lwd="2")
points(frames, mclY50, lty = 5, col = "brown", type="l", lwd="2")
legend("topright",lty=c(1,2,3,4,5), c("Actual", "EKF", "MCL 100", "EKF-NA", "MCL 50"), col=c("green", "red", "blue", "purple","brown"))
dev.off()

# Plot the x data with best
pdf(paste(name, "-xEstsBest.pdf", sep=""))
plot(frames, mclX100, main="X Estimates", xlab="Frame Number", ylab="x (cm)", pch="",xlim=frameMinMax,ylim=xMinMax)
points(frames, knownX, lty = 1, col = "green", type="l", lwd="2")
points(frames, mclX100best, lty = 5, col = "red", type="l",lwd="2")
points(frames, mclX100, lty = 3, col = "blue", type="l", lwd="2")
legend("topleft",lty=c(1,3,5), c("Actual", "MCL", "MCL best"), col=c("green", "blue", "red"))
dev.off()

# Plot the y data with best
pdf(paste(name, "-yEstsBest.pdf", sep=""))
plot(frames, mclY100, main="Y Estimates", xlab="Frame Number", ylab="y (cm)", pch="",xlim=frameMinMax,ylim=yMinMax)
points(frames, knownY, lty = 1, col = "green", type="l", lwd="2")
points(frames, mclY100best, lty = 5, col = "red", type="l",lwd="2")
points(frames, mclY100, lty = 3, col = "blue", type="l",lwd="2")
legend("top",lty=c(1,3,5), c("Actual", "MCL", "MCL best"), col=c("green", "blue", "red"))
dev.off()

# Plot the h data
pdf(paste(name, "-hEsts.pdf", sep=""))
plot(frames, mclH100, main="Heading Estimates", xlab="Frame Number", ylab="heading (rad)", pch="",xlim=frameMinMax,ylim=hMinMax)
points(frames, knownH, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfH, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclH100, lty = 3, col = "blue", type="l", lwd="2")
points(frames, mclH500, lty = 4, col = "pink", type="l", lwd="2")
#points(frames, mclH100best, lty = 6, col = "pink", type="l",lwd="2")
points(frames, ekfNAH, lty = 4, col = "purple", type="l", lwd="2")
legend("topleft",lty=c(1,2,3,4,5), c("Actual", "EKF", "MCL 100", "EKF-NA", "MCL 500"), col=c("green", "red", "blue", "pink", "purple"))
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
plot(frames, mcl5Error, main="Position Error", xlab="Frame Number", ylab="error (cm)", pch="",xlim=frameMinMax,ylim=errorMinMax)
points(frames, ekfError, lty = 1, col = "red", type="l", lwd="2")
points(frames, ekfNAError, lty = 2, col = "blue", type="l", lwd="2")
points(frames, mcl5Error, lty = 3, col = "green", type="l", lwd="2")
#points(frames, mcl5bestError, lty = 2, col = "blue", type="l",lwd="2")
#points(frames, mcl10Error, lty = 1, col = "brown", type="l", lwd="2")
#points(frames, mcl10bestError, lty = 2, col = "brown", type="l",lwd="2")
#points(frames, mcl50Error, lty = 1, col = "pink", type="l", lwd="2")
#points(frames, mcl50bestError, lty = 2, col = "pink", type="l",lwd="2")
points(frames, mcl100Error, lty = 4, col = "brown", type="l", lwd="2")
#points(frames, mcl100bestError, lty = 2, col = "green", type="l",lwd="2")
#points(frames, mcl500Error, lty = 1, col = "yellow", type="l", lwd="2")
#points(frames, mcl500bestError, lty = 2, col = "yellow", type="l",lwd="2")
points(frames, mcl1000Error, lty = 5, col = "black", type="l", lwd="2")
#points(frames, mcl1000bestError, lty = 2, col = "black", type="l",lwd="2")

legend("topright", c("EKF","EKF-NA","MCL-5","MCL-100", "MCL-1000"), col=c("red","blue","green","brown","black"), lty=c(1,2,3,4,5))
dev.off()

mcldata5.1 <- read.table(mclfile5.1)
mcldata5best.1 <- read.table(mclfile5best.1)
mcldata10.1 <- read.table(mclfile10.1)
mcldata10best.1 <- read.table(mclfile10best.1)
mcldata50.1 <- read.table(mclfile50.1)
mcldata50best.1 <- read.table(mclfile50best.1)
mcldata100.1 <- read.table(mclfile100.1)
mcldata100best.1 <- read.table(mclfile100best.1)
mcldata500.1 <- read.table(mclfile500.1)
mcldata500best.1 <- read.table(mclfile500best.1)
mcldata1000.1 <- read.table(mclfile1000.1)
mcldata1000best.1 <- read.table(mclfile1000best.1)

colnames(mcldata5.1) <- coreColNames
colnames(mcldata5best.1) <- coreColNames
colnames(mcldata10.1) <- coreColNames
colnames(mcldata10best.1) <- coreColNames
colnames(mcldata50.1) <- coreColNames
colnames(mcldata50best.1) <- coreColNames
colnames(mcldata100.1) <- coreColNames
colnames(mcldata100best.1) <- coreColNames
colnames(mcldata500.1) <- coreColNames
colnames(mcldata500best.1) <- coreColNames
colnames(mcldata1000.1) <- coreColNames
colnames(mcldata1000best.1) <- coreColNames

mcl5Error.1 <- sqrt((mcldata5.1[,"x est"] - knownX)^2 + (mcldata5.1[,"y est"] - knownY)^2)
mcl10Error.1 <- sqrt((mcldata10.1[,"x est"] - knownX)^2 + (mcldata10.1[,"y est"] - knownY)^2)
mcl50Error.1 <- sqrt((mcldata50.1[,"x est"] - knownX)^2 + (mcldata50.1[,"y est"] - knownY)^2)
mcl100Error.1 <- sqrt((mcldata100.1[,"x est"] - knownX)^2 + (mcldata100.1[,"y est"] - knownY)^2)
mcl500Error.1 <- sqrt((mcldata500.1[,"x est"] - knownX)^2 + (mcldata500.1[,"y est"] - knownY)^2)
mcl1000Error.1 <- sqrt((mcldata1000.1[,"x est"] - knownX)^2 + (mcldata1000.1[,"y est"] - knownY)^2)
mcl5bestError.1 <- sqrt((mcldata5best.1[,"x est"] - knownX)^2 + (mcldata5best.1[,"y est"] - knownY)^2)
mcl10bestError.1 <- sqrt((mcldata10best.1[,"x est"] - knownX)^2 + (mcldata10best.1[,"y est"] - knownY)^2)
mcl50bestError.1 <- sqrt((mcldata50best.1[,"x est"] - knownX)^2 + (mcldata50best.1[,"y est"] - knownY)^2)
mcl100bestError.1 <- sqrt((mcldata100best.1[,"x est"] - knownX)^2 + (mcldata100best.1[,"y est"] - knownY)^2)
mcl500bestError.1 <- sqrt((mcldata500best.1[,"x est"] - knownX)^2 + (mcldata500best.1[,"y est"] - knownY)^2)
mcl1000bestError.1 <- sqrt((mcldata1000best.1[,"x est"] - knownX)^2 + (mcldata1000best.1[,"y est"] - knownY)^2)

mcl5TotalError.1 <-  sqrt(sum((mcl5Error.1)^2)/length(frames))
mcl10TotalError.1 <- sqrt(sum((mcl10Error.1)^2)/length(frames))
mcl50TotalError.1 <- sqrt(sum((mcl50Error.1)^2)/length(frames))
mcl100TotalError.1 <- sqrt(sum((mcl100Error.1)^2)/length(frames))
mcl500TotalError.1 <- sqrt(sum((mcl500Error.1)^2)/length(frames))
mcl1000TotalError.1 <- sqrt(sum((mcl1000Error.1)^2)/length(frames))
mcl5bestTotalError.1 <-  sqrt(sum((mcl5bestError.1)^2)/length(frames))
mcl10bestTotalError.1 <- sqrt(sum((mcl10bestError.1)^2)/length(frames))
mcl50bestTotalError.1 <- sqrt(sum((mcl50bestError.1)^2)/length(frames))
mcl100bestTotalError.1 <- sqrt(sum((mcl100bestError.1)^2)/length(frames))
mcl500bestTotalError.1 <- sqrt(sum((mcl500bestError.1)^2)/length(frames))
mcl1000bestTotalError.1 <- sqrt(sum((mcl1000bestError.1)^2)/length(frames))

mclfile5.2 <- paste(name, ".mcl.core.5-2", sep="")
mclfile10.2 <- paste(name, ".mcl.core.10-2", sep="")
mclfile50.2 <- paste(name, ".mcl.core.50-2", sep="")
mclfile100.2 <- paste(name, ".mcl.core.100-2", sep="")
mclfile500.2 <- paste(name, ".mcl.core.500-2", sep="")
mclfile1000.2 <- paste(name, ".mcl.core.1000-2", sep="")
mclfile5best.2 <- paste(name, ".mcl.core.5-2.best", sep="")
mclfile10best.2 <- paste(name, ".mcl.core.10-2.best", sep="")
mclfile50best.2 <- paste(name, ".mcl.core.50-2.best", sep="")
mclfile100best.2 <- paste(name, ".mcl.core.100-2.best", sep="")
mclfile500best.2 <- paste(name, ".mcl.core.500-2.best", sep="")
mclfile1000best.2 <- paste(name, ".mcl.core.1000-2.best", sep="")

mclfile5.3 <- paste(name, ".mcl.core.5-3", sep="")
mclfile10.3 <- paste(name, ".mcl.core.10-3", sep="")
mclfile50.3 <- paste(name, ".mcl.core.50-3", sep="")
mclfile100.3 <- paste(name, ".mcl.core.100-3", sep="")
mclfile500.3 <- paste(name, ".mcl.core.500-3", sep="")
mclfile1000.3 <- paste(name, ".mcl.core.1000-3", sep="")
mclfile5best.3 <- paste(name, ".mcl.core.5-3.best", sep="")
mclfile10best.3 <- paste(name, ".mcl.core.10-3.best", sep="")
mclfile50best.3 <- paste(name, ".mcl.core.50-3.best", sep="")
mclfile100best.3 <- paste(name, ".mcl.core.100-3.best", sep="")
mclfile500best.3 <- paste(name, ".mcl.core.500-3.best", sep="")
mclfile1000best.3 <- paste(name, ".mcl.core.1000-3.best", sep="")

mclfile5.4 <- paste(name, ".mcl.core.5-4", sep="")
mclfile10.4 <- paste(name, ".mcl.core.10-4", sep="")
mclfile50.4 <- paste(name, ".mcl.core.50-4", sep="")
mclfile100.4 <- paste(name, ".mcl.core.100-4", sep="")
mclfile500.4 <- paste(name, ".mcl.core.500-4", sep="")
mclfile1000.4 <- paste(name, ".mcl.core.1000-4", sep="")
mclfile5best.4 <- paste(name, ".mcl.core.5-4.best", sep="")
mclfile10best.4 <- paste(name, ".mcl.core.10-4.best", sep="")
mclfile50best.4 <- paste(name, ".mcl.core.50-4.best", sep="")
mclfile100best.4 <- paste(name, ".mcl.core.100-4.best", sep="")
mclfile500best.4 <- paste(name, ".mcl.core.500-4.best", sep="")
mclfile1000best.4 <- paste(name, ".mcl.core.1000-4.best", sep="")

mclfile5.5 <- paste(name, ".mcl.core.5-5", sep="")
mclfile10.5 <- paste(name, ".mcl.core.10-5", sep="")
mclfile50.5 <- paste(name, ".mcl.core.50-5", sep="")
mclfile100.5 <- paste(name, ".mcl.core.100-5", sep="")
mclfile500.5 <- paste(name, ".mcl.core.500-5", sep="")
mclfile1000.5 <- paste(name, ".mcl.core.1000-5", sep="")
mclfile5best.5 <- paste(name, ".mcl.core.5-5.best", sep="")
mclfile10best.5 <- paste(name, ".mcl.core.10-5.best", sep="")
mclfile50best.5 <- paste(name, ".mcl.core.50-5.best", sep="")
mclfile100best.5 <- paste(name, ".mcl.core.100-5.best", sep="")
mclfile500best.5 <- paste(name, ".mcl.core.500-5.best", sep="")
mclfile1000best.5 <- paste(name, ".mcl.core.1000-5.best", sep="")

mclfile5.6 <- paste(name, ".mcl.core.5-6", sep="")
mclfile10.6 <- paste(name, ".mcl.core.10-6", sep="")
mclfile50.6 <- paste(name, ".mcl.core.50-6", sep="")
mclfile100.6 <- paste(name, ".mcl.core.100-6", sep="")
mclfile500.6 <- paste(name, ".mcl.core.500-6", sep="")
mclfile1000.6 <- paste(name, ".mcl.core.1000-6", sep="")
mclfile5best.6 <- paste(name, ".mcl.core.5-6.best", sep="")
mclfile10best.6 <- paste(name, ".mcl.core.10-6.best", sep="")
mclfile50best.6 <- paste(name, ".mcl.core.50-6.best", sep="")
mclfile100best.6 <- paste(name, ".mcl.core.100-6.best", sep="")
mclfile500best.6 <- paste(name, ".mcl.core.500-6.best", sep="")
mclfile1000best.6 <- paste(name, ".mcl.core.1000-6.best", sep="")

mclfile5.7 <- paste(name, ".mcl.core.5-7", sep="")
mclfile10.7 <- paste(name, ".mcl.core.10-7", sep="")
mclfile50.7 <- paste(name, ".mcl.core.50-7", sep="")
mclfile100.7 <- paste(name, ".mcl.core.100-7", sep="")
mclfile500.7 <- paste(name, ".mcl.core.500-7", sep="")
mclfile1000.7 <- paste(name, ".mcl.core.1000-7", sep="")
mclfile5best.7 <- paste(name, ".mcl.core.5-7.best", sep="")
mclfile10best.7 <- paste(name, ".mcl.core.10-7.best", sep="")
mclfile50best.7 <- paste(name, ".mcl.core.50-7.best", sep="")
mclfile100best.7 <- paste(name, ".mcl.core.100-7.best", sep="")
mclfile500best.7 <- paste(name, ".mcl.core.500-7.best", sep="")
mclfile1000best.7 <- paste(name, ".mcl.core.1000-7.best", sep="")

mclfile5.8 <- paste(name, ".mcl.core.5-8", sep="")
mclfile10.8 <- paste(name, ".mcl.core.10-8", sep="")
mclfile50.8 <- paste(name, ".mcl.core.50-8", sep="")
mclfile100.8 <- paste(name, ".mcl.core.100-8", sep="")
mclfile500.8 <- paste(name, ".mcl.core.500-8", sep="")
mclfile1000.8 <- paste(name, ".mcl.core.1000-8", sep="")
mclfile5best.8 <- paste(name, ".mcl.core.5-8.best", sep="")
mclfile10best.8 <- paste(name, ".mcl.core.10-8.best", sep="")
mclfile50best.8 <- paste(name, ".mcl.core.50-8.best", sep="")
mclfile100best.8 <- paste(name, ".mcl.core.100-8.best", sep="")
mclfile500best.8 <- paste(name, ".mcl.core.500-8.best", sep="")
mclfile1000best.8 <- paste(name, ".mcl.core.1000-8.best", sep="")

mclfile5.9 <- paste(name, ".mcl.core.5-9", sep="")
mclfile10.9 <- paste(name, ".mcl.core.10-9", sep="")
mclfile50.9 <- paste(name, ".mcl.core.50-9", sep="")
mclfile100.9 <- paste(name, ".mcl.core.100-9", sep="")
mclfile500.9 <- paste(name, ".mcl.core.500-9", sep="")
mclfile1000.9 <- paste(name, ".mcl.core.1000-9", sep="")
mclfile5best.9 <- paste(name, ".mcl.core.5-9.best", sep="")
mclfile10best.9 <- paste(name, ".mcl.core.10-9.best", sep="")
mclfile50best.9 <- paste(name, ".mcl.core.50-9.best", sep="")
mclfile100best.9 <- paste(name, ".mcl.core.100-9.best", sep="")
mclfile500best.9 <- paste(name, ".mcl.core.500-9.best", sep="")
mclfile1000best.9 <- paste(name, ".mcl.core.1000-9.best", sep="")

mclfile5.10 <- paste(name, ".mcl.core.5-10", sep="")
mclfile10.10 <- paste(name, ".mcl.core.10-10", sep="")
mclfile50.10 <- paste(name, ".mcl.core.50-10", sep="")
mclfile100.10 <- paste(name, ".mcl.core.100-10", sep="")
mclfile500.10 <- paste(name, ".mcl.core.500-10", sep="")
mclfile1000.10 <- paste(name, ".mcl.core.1000-10", sep="")
mclfile5best.10 <- paste(name, ".mcl.core.5-10.best", sep="")
mclfile10best.10 <- paste(name, ".mcl.core.10-10.best", sep="")
mclfile50best.10 <- paste(name, ".mcl.core.50-10.best", sep="")
mclfile100best.10 <- paste(name, ".mcl.core.100-10.best", sep="")
mclfile500best.10 <- paste(name, ".mcl.core.500-10.best", sep="")
mclfile1000best.10 <- paste(name, ".mcl.core.1000-10.best", sep="")

mcldata5.2 <- read.table(mclfile5.2)
mcldata5best.2 <- read.table(mclfile5best.2)
mcldata10.2 <- read.table(mclfile10.2)
mcldata10best.2 <- read.table(mclfile10best.2)
mcldata50.2 <- read.table(mclfile50.2)
mcldata50best.2 <- read.table(mclfile50best.2)
mcldata100.2 <- read.table(mclfile100.2)
mcldata100best.2 <- read.table(mclfile100best.2)
mcldata500.2 <- read.table(mclfile500.2)
mcldata500best.2 <- read.table(mclfile500best.2)
mcldata1000.2 <- read.table(mclfile1000.2)
mcldata1000best.2 <- read.table(mclfile1000best.2)

colnames(mcldata5.2) <- coreColNames
colnames(mcldata5best.2) <- coreColNames
colnames(mcldata10.2) <- coreColNames
colnames(mcldata10best.2) <- coreColNames
colnames(mcldata50.2) <- coreColNames
colnames(mcldata50best.2) <- coreColNames
colnames(mcldata100.2) <- coreColNames
colnames(mcldata100best.2) <- coreColNames
colnames(mcldata500.2) <- coreColNames
colnames(mcldata500best.2) <- coreColNames
colnames(mcldata1000.2) <- coreColNames
colnames(mcldata1000best.2) <- coreColNames

mcl5Error.2 <- sqrt((mcldata5.2[,"x est"] - knownX)^2 + (mcldata5.2[,"y est"] - knownY)^2)
mcl10Error.2 <- sqrt((mcldata10.2[,"x est"] - knownX)^2 + (mcldata10.2[,"y est"] - knownY)^2)
mcl50Error.2 <- sqrt((mcldata50.2[,"x est"] - knownX)^2 + (mcldata50.2[,"y est"] - knownY)^2)
mcl100Error.2 <- sqrt((mcldata100.2[,"x est"] - knownX)^2 + (mcldata100.2[,"y est"] - knownY)^2)
mcl500Error.2 <- sqrt((mcldata500.2[,"x est"] - knownX)^2 + (mcldata500.2[,"y est"] - knownY)^2)
mcl1000Error.2 <- sqrt((mcldata1000.2[,"x est"] - knownX)^2 + (mcldata1000.2[,"y est"] - knownY)^2)
mcl5bestError.2 <- sqrt((mcldata5best.2[,"x est"] - knownX)^2 + (mcldata5best.2[,"y est"] - knownY)^2)
mcl10bestError.2 <- sqrt((mcldata10best.2[,"x est"] - knownX)^2 + (mcldata10best.2[,"y est"] - knownY)^2)
mcl50bestError.2 <- sqrt((mcldata50best.2[,"x est"] - knownX)^2 + (mcldata50best.2[,"y est"] - knownY)^2)
mcl100bestError.2 <- sqrt((mcldata100best.2[,"x est"] - knownX)^2 + (mcldata100best.2[,"y est"] - knownY)^2)
mcl500bestError.2 <- sqrt((mcldata500best.2[,"x est"] - knownX)^2 + (mcldata500best.2[,"y est"] - knownY)^2)
mcl1000bestError.2 <- sqrt((mcldata1000best.2[,"x est"] - knownX)^2 + (mcldata1000best.2[,"y est"] - knownY)^2)

mcl5TotalError.2 <-  sqrt(sum((mcl5Error.2)^2)/length(frames))
mcl10TotalError.2 <- sqrt(sum((mcl10Error.2)^2)/length(frames))
mcl50TotalError.2 <- sqrt(sum((mcl50Error.2)^2)/length(frames))
mcl100TotalError.2 <- sqrt(sum((mcl100Error.2)^2)/length(frames))
mcl500TotalError.2 <- sqrt(sum((mcl500Error.2)^2)/length(frames))
mcl1000TotalError.2 <- sqrt(sum((mcl1000Error.2)^2)/length(frames))
mcl5bestTotalError.2 <-  sqrt(sum((mcl5bestError.2)^2)/length(frames))
mcl10bestTotalError.2 <- sqrt(sum((mcl10bestError.2)^2)/length(frames))
mcl50bestTotalError.2 <- sqrt(sum((mcl50bestError.2)^2)/length(frames))
mcl100bestTotalError.2 <- sqrt(sum((mcl100bestError.2)^2)/length(frames))
mcl500bestTotalError.2 <- sqrt(sum((mcl500bestError.2)^2)/length(frames))
mcl1000bestTotalError.2 <- sqrt(sum((mcl1000bestError.2)^2)/length(frames))

mcldata5.3 <- read.table(mclfile5.3)
mcldata5best.3 <- read.table(mclfile5best.3)
mcldata10.3 <- read.table(mclfile10.3)
mcldata10best.3 <- read.table(mclfile10best.3)
mcldata50.3 <- read.table(mclfile50.3)
mcldata50best.3 <- read.table(mclfile50best.3)
mcldata100.3 <- read.table(mclfile100.3)
mcldata100best.3 <- read.table(mclfile100best.3)
mcldata500.3 <- read.table(mclfile500.3)
mcldata500best.3 <- read.table(mclfile500best.3)
mcldata1000.3 <- read.table(mclfile1000.3)
mcldata1000best.3 <- read.table(mclfile1000best.3)

colnames(mcldata5.3) <- coreColNames
colnames(mcldata5best.3) <- coreColNames
colnames(mcldata10.3) <- coreColNames
colnames(mcldata10best.3) <- coreColNames
colnames(mcldata50.3) <- coreColNames
colnames(mcldata50best.3) <- coreColNames
colnames(mcldata100.3) <- coreColNames
colnames(mcldata100best.3) <- coreColNames
colnames(mcldata500.3) <- coreColNames
colnames(mcldata500best.3) <- coreColNames
colnames(mcldata1000.3) <- coreColNames
colnames(mcldata1000best.3) <- coreColNames

mcl5Error.3 <- sqrt((mcldata5.3[,"x est"] - knownX)^2 + (mcldata5.3[,"y est"] - knownY)^2)
mcl10Error.3 <- sqrt((mcldata10.3[,"x est"] - knownX)^2 + (mcldata10.3[,"y est"] - knownY)^2)
mcl50Error.3 <- sqrt((mcldata50.3[,"x est"] - knownX)^2 + (mcldata50.3[,"y est"] - knownY)^2)
mcl100Error.3 <- sqrt((mcldata100.3[,"x est"] - knownX)^2 + (mcldata100.3[,"y est"] - knownY)^2)
mcl500Error.3 <- sqrt((mcldata500.3[,"x est"] - knownX)^2 + (mcldata500.3[,"y est"] - knownY)^2)
mcl1000Error.3 <- sqrt((mcldata1000.3[,"x est"] - knownX)^2 + (mcldata1000.3[,"y est"] - knownY)^2)
mcl5bestError.3 <- sqrt((mcldata5best.3[,"x est"] - knownX)^2 + (mcldata5best.3[,"y est"] - knownY)^2)
mcl10bestError.3 <- sqrt((mcldata10best.3[,"x est"] - knownX)^2 + (mcldata10best.3[,"y est"] - knownY)^2)
mcl50bestError.3 <- sqrt((mcldata50best.3[,"x est"] - knownX)^2 + (mcldata50best.3[,"y est"] - knownY)^2)
mcl100bestError.3 <- sqrt((mcldata100best.3[,"x est"] - knownX)^2 + (mcldata100best.3[,"y est"] - knownY)^2)
mcl500bestError.3 <- sqrt((mcldata500best.3[,"x est"] - knownX)^2 + (mcldata500best.3[,"y est"] - knownY)^2)
mcl1000bestError.3 <- sqrt((mcldata1000best.3[,"x est"] - knownX)^2 + (mcldata1000best.3[,"y est"] - knownY)^2)

mcl5TotalError.3 <-  sqrt(sum((mcl5Error.3)^2)/length(frames))
mcl10TotalError.3 <- sqrt(sum((mcl10Error.3)^2)/length(frames))
mcl50TotalError.3 <- sqrt(sum((mcl50Error.3)^2)/length(frames))
mcl100TotalError.3 <- sqrt(sum((mcl100Error.3)^2)/length(frames))
mcl500TotalError.3 <- sqrt(sum((mcl500Error.3)^2)/length(frames))
mcl1000TotalError.3 <- sqrt(sum((mcl1000Error.3)^2)/length(frames))
mcl5bestTotalError.3 <-  sqrt(sum((mcl5bestError.3)^2)/length(frames))
mcl10bestTotalError.3 <- sqrt(sum((mcl10bestError.3)^2)/length(frames))
mcl50bestTotalError.3 <- sqrt(sum((mcl50bestError.3)^2)/length(frames))
mcl100bestTotalError.3 <- sqrt(sum((mcl100bestError.3)^2)/length(frames))
mcl500bestTotalError.3 <- sqrt(sum((mcl500bestError.3)^2)/length(frames))
mcl1000bestTotalError.3 <- sqrt(sum((mcl1000bestError.3)^2)/length(frames))

mcldata5.4 <- read.table(mclfile5.4)
mcldata5best.4 <- read.table(mclfile5best.4)
mcldata10.4 <- read.table(mclfile10.4)
mcldata10best.4 <- read.table(mclfile10best.4)
mcldata50.4 <- read.table(mclfile50.4)
mcldata50best.4 <- read.table(mclfile50best.4)
mcldata100.4 <- read.table(mclfile100.4)
mcldata100best.4 <- read.table(mclfile100best.4)
mcldata500.4 <- read.table(mclfile500.4)
mcldata500best.4 <- read.table(mclfile500best.4)
mcldata1000.4 <- read.table(mclfile1000.4)
mcldata1000best.4 <- read.table(mclfile1000best.4)

colnames(mcldata5.4) <- coreColNames
colnames(mcldata5best.4) <- coreColNames
colnames(mcldata10.4) <- coreColNames
colnames(mcldata10best.4) <- coreColNames
colnames(mcldata50.4) <- coreColNames
colnames(mcldata50best.4) <- coreColNames
colnames(mcldata100.4) <- coreColNames
colnames(mcldata100best.4) <- coreColNames
colnames(mcldata500.4) <- coreColNames
colnames(mcldata500best.4) <- coreColNames
colnames(mcldata1000.4) <- coreColNames
colnames(mcldata1000best.4) <- coreColNames

mcl5Error.4 <- sqrt((mcldata5.4[,"x est"] - knownX)^2 + (mcldata5.4[,"y est"] - knownY)^2)
mcl10Error.4 <- sqrt((mcldata10.4[,"x est"] - knownX)^2 + (mcldata10.4[,"y est"] - knownY)^2)
mcl50Error.4 <- sqrt((mcldata50.4[,"x est"] - knownX)^2 + (mcldata50.4[,"y est"] - knownY)^2)
mcl100Error.4 <- sqrt((mcldata100.4[,"x est"] - knownX)^2 + (mcldata100.4[,"y est"] - knownY)^2)
mcl500Error.4 <- sqrt((mcldata500.4[,"x est"] - knownX)^2 + (mcldata500.4[,"y est"] - knownY)^2)
mcl1000Error.4 <- sqrt((mcldata1000.4[,"x est"] - knownX)^2 + (mcldata1000.4[,"y est"] - knownY)^2)
mcl5bestError.4 <- sqrt((mcldata5best.4[,"x est"] - knownX)^2 + (mcldata5best.4[,"y est"] - knownY)^2)
mcl10bestError.4 <- sqrt((mcldata10best.4[,"x est"] - knownX)^2 + (mcldata10best.4[,"y est"] - knownY)^2)
mcl50bestError.4 <- sqrt((mcldata50best.4[,"x est"] - knownX)^2 + (mcldata50best.4[,"y est"] - knownY)^2)
mcl100bestError.4 <- sqrt((mcldata100best.4[,"x est"] - knownX)^2 + (mcldata100best.4[,"y est"] - knownY)^2)
mcl500bestError.4 <- sqrt((mcldata500best.4[,"x est"] - knownX)^2 + (mcldata500best.4[,"y est"] - knownY)^2)
mcl1000bestError.4 <- sqrt((mcldata1000best.4[,"x est"] - knownX)^2 + (mcldata1000best.4[,"y est"] - knownY)^2)

mcl5TotalError.4 <-  sqrt(sum((mcl5Error.4)^2)/length(frames))
mcl10TotalError.4 <- sqrt(sum((mcl10Error.4)^2)/length(frames))
mcl50TotalError.4 <- sqrt(sum((mcl50Error.4)^2)/length(frames))
mcl100TotalError.4 <- sqrt(sum((mcl100Error.4)^2)/length(frames))
mcl500TotalError.4 <- sqrt(sum((mcl500Error.4)^2)/length(frames))
mcl1000TotalError.4 <- sqrt(sum((mcl1000Error.4)^2)/length(frames))
mcl5bestTotalError.4 <-  sqrt(sum((mcl5bestError.4)^2)/length(frames))
mcl10bestTotalError.4 <- sqrt(sum((mcl10bestError.4)^2)/length(frames))
mcl50bestTotalError.4 <- sqrt(sum((mcl50bestError.4)^2)/length(frames))
mcl100bestTotalError.4 <- sqrt(sum((mcl100bestError.4)^2)/length(frames))
mcl500bestTotalError.4 <- sqrt(sum((mcl500bestError.4)^2)/length(frames))
mcl1000bestTotalError.4 <- sqrt(sum((mcl1000bestError.4)^2)/length(frames))

mcldata5.5 <- read.table(mclfile5.5)
mcldata5best.5 <- read.table(mclfile5best.5)
mcldata10.5 <- read.table(mclfile10.5)
mcldata10best.5 <- read.table(mclfile10best.5)
mcldata50.5 <- read.table(mclfile50.5)
mcldata50best.5 <- read.table(mclfile50best.5)
mcldata100.5 <- read.table(mclfile100.5)
mcldata100best.5 <- read.table(mclfile100best.5)
mcldata500.5 <- read.table(mclfile500.5)
mcldata500best.5 <- read.table(mclfile500best.5)
mcldata1000.5 <- read.table(mclfile1000.5)
mcldata1000best.5 <- read.table(mclfile1000best.5)

colnames(mcldata5.5) <- coreColNames
colnames(mcldata5best.5) <- coreColNames
colnames(mcldata10.5) <- coreColNames
colnames(mcldata10best.5) <- coreColNames
colnames(mcldata50.5) <- coreColNames
colnames(mcldata50best.5) <- coreColNames
colnames(mcldata100.5) <- coreColNames
colnames(mcldata100best.5) <- coreColNames
colnames(mcldata500.5) <- coreColNames
colnames(mcldata500best.5) <- coreColNames
colnames(mcldata1000.5) <- coreColNames
colnames(mcldata1000best.5) <- coreColNames

mcl5Error.5 <- sqrt((mcldata5.5[,"x est"] - knownX)^2 + (mcldata5.5[,"y est"] - knownY)^2)
mcl10Error.5 <- sqrt((mcldata10.5[,"x est"] - knownX)^2 + (mcldata10.5[,"y est"] - knownY)^2)
mcl50Error.5 <- sqrt((mcldata50.5[,"x est"] - knownX)^2 + (mcldata50.5[,"y est"] - knownY)^2)
mcl100Error.5 <- sqrt((mcldata100.5[,"x est"] - knownX)^2 + (mcldata100.5[,"y est"] - knownY)^2)
mcl500Error.5 <- sqrt((mcldata500.5[,"x est"] - knownX)^2 + (mcldata500.5[,"y est"] - knownY)^2)
mcl1000Error.5 <- sqrt((mcldata1000.5[,"x est"] - knownX)^2 + (mcldata1000.5[,"y est"] - knownY)^2)
mcl5bestError.5 <- sqrt((mcldata5best.5[,"x est"] - knownX)^2 + (mcldata5best.5[,"y est"] - knownY)^2)
mcl10bestError.5 <- sqrt((mcldata10best.5[,"x est"] - knownX)^2 + (mcldata10best.5[,"y est"] - knownY)^2)
mcl50bestError.5 <- sqrt((mcldata50best.5[,"x est"] - knownX)^2 + (mcldata50best.5[,"y est"] - knownY)^2)
mcl100bestError.5 <- sqrt((mcldata100best.5[,"x est"] - knownX)^2 + (mcldata100best.5[,"y est"] - knownY)^2)
mcl500bestError.5 <- sqrt((mcldata500best.5[,"x est"] - knownX)^2 + (mcldata500best.5[,"y est"] - knownY)^2)
mcl1000bestError.5 <- sqrt((mcldata1000best.5[,"x est"] - knownX)^2 + (mcldata1000best.5[,"y est"] - knownY)^2)

mcl5TotalError.5 <-  sqrt(sum((mcl5Error.5)^2)/length(frames))
mcl10TotalError.5 <- sqrt(sum((mcl10Error.5)^2)/length(frames))
mcl50TotalError.5 <- sqrt(sum((mcl50Error.5)^2)/length(frames))
mcl100TotalError.5 <- sqrt(sum((mcl100Error.5)^2)/length(frames))
mcl500TotalError.5 <- sqrt(sum((mcl500Error.5)^2)/length(frames))
mcl1000TotalError.5 <- sqrt(sum((mcl1000Error.5)^2)/length(frames))
mcl5bestTotalError.5 <-  sqrt(sum((mcl5bestError.5)^2)/length(frames))
mcl10bestTotalError.5 <- sqrt(sum((mcl10bestError.5)^2)/length(frames))
mcl50bestTotalError.5 <- sqrt(sum((mcl50bestError.5)^2)/length(frames))
mcl100bestTotalError.5 <- sqrt(sum((mcl100bestError.5)^2)/length(frames))
mcl500bestTotalError.5 <- sqrt(sum((mcl500bestError.5)^2)/length(frames))
mcl1000bestTotalError.5 <- sqrt(sum((mcl1000bestError.5)^2)/length(frames))

mcldata5.6 <- read.table(mclfile5.6)
mcldata5best.6 <- read.table(mclfile5best.6)
mcldata10.6 <- read.table(mclfile10.6)
mcldata10best.6 <- read.table(mclfile10best.6)
mcldata50.6 <- read.table(mclfile50.6)
mcldata50best.6 <- read.table(mclfile50best.6)
mcldata100.6 <- read.table(mclfile100.6)
mcldata100best.6 <- read.table(mclfile100best.6)
mcldata500.6 <- read.table(mclfile500.6)
mcldata500best.6 <- read.table(mclfile500best.6)
mcldata1000.6 <- read.table(mclfile1000.6)
mcldata1000best.6 <- read.table(mclfile1000best.6)

colnames(mcldata5.6) <- coreColNames
colnames(mcldata5best.6) <- coreColNames
colnames(mcldata10.6) <- coreColNames
colnames(mcldata10best.6) <- coreColNames
colnames(mcldata50.6) <- coreColNames
colnames(mcldata50best.6) <- coreColNames
colnames(mcldata100.6) <- coreColNames
colnames(mcldata100best.6) <- coreColNames
colnames(mcldata500.6) <- coreColNames
colnames(mcldata500best.6) <- coreColNames
colnames(mcldata1000.6) <- coreColNames
colnames(mcldata1000best.6) <- coreColNames

mcl5Error.6 <- sqrt((mcldata5.6[,"x est"] - knownX)^2 + (mcldata5.6[,"y est"] - knownY)^2)
mcl10Error.6 <- sqrt((mcldata10.6[,"x est"] - knownX)^2 + (mcldata10.6[,"y est"] - knownY)^2)
mcl50Error.6 <- sqrt((mcldata50.6[,"x est"] - knownX)^2 + (mcldata50.6[,"y est"] - knownY)^2)
mcl100Error.6 <- sqrt((mcldata100.6[,"x est"] - knownX)^2 + (mcldata100.6[,"y est"] - knownY)^2)
mcl500Error.6 <- sqrt((mcldata500.6[,"x est"] - knownX)^2 + (mcldata500.6[,"y est"] - knownY)^2)
mcl1000Error.6 <- sqrt((mcldata1000.6[,"x est"] - knownX)^2 + (mcldata1000.6[,"y est"] - knownY)^2)
mcl5bestError.6 <- sqrt((mcldata5best.6[,"x est"] - knownX)^2 + (mcldata5best.6[,"y est"] - knownY)^2)
mcl10bestError.6 <- sqrt((mcldata10best.6[,"x est"] - knownX)^2 + (mcldata10best.6[,"y est"] - knownY)^2)
mcl50bestError.6 <- sqrt((mcldata50best.6[,"x est"] - knownX)^2 + (mcldata50best.6[,"y est"] - knownY)^2)
mcl100bestError.6 <- sqrt((mcldata100best.6[,"x est"] - knownX)^2 + (mcldata100best.6[,"y est"] - knownY)^2)
mcl500bestError.6 <- sqrt((mcldata500best.6[,"x est"] - knownX)^2 + (mcldata500best.6[,"y est"] - knownY)^2)
mcl1000bestError.6 <- sqrt((mcldata1000best.6[,"x est"] - knownX)^2 + (mcldata1000best.6[,"y est"] - knownY)^2)

mcl5TotalError.6 <-  sqrt(sum((mcl5Error.6)^2)/length(frames))
mcl10TotalError.6 <- sqrt(sum((mcl10Error.6)^2)/length(frames))
mcl50TotalError.6 <- sqrt(sum((mcl50Error.6)^2)/length(frames))
mcl100TotalError.6 <- sqrt(sum((mcl100Error.6)^2)/length(frames))
mcl500TotalError.6 <- sqrt(sum((mcl500Error.6)^2)/length(frames))
mcl1000TotalError.6 <- sqrt(sum((mcl1000Error.6)^2)/length(frames))
mcl5bestTotalError.6 <-  sqrt(sum((mcl5bestError.6)^2)/length(frames))
mcl10bestTotalError.6 <- sqrt(sum((mcl10bestError.6)^2)/length(frames))
mcl50bestTotalError.6 <- sqrt(sum((mcl50bestError.6)^2)/length(frames))
mcl100bestTotalError.6 <- sqrt(sum((mcl100bestError.6)^2)/length(frames))
mcl500bestTotalError.6 <- sqrt(sum((mcl500bestError.6)^2)/length(frames))
mcl1000bestTotalError.6 <- sqrt(sum((mcl1000bestError.6)^2)/length(frames))

mcldata5.7 <- read.table(mclfile5.7)
mcldata5best.7 <- read.table(mclfile5best.7)
mcldata10.7 <- read.table(mclfile10.7)
mcldata10best.7 <- read.table(mclfile10best.7)
mcldata50.7 <- read.table(mclfile50.7)
mcldata50best.7 <- read.table(mclfile50best.7)
mcldata100.7 <- read.table(mclfile100.7)
mcldata100best.7 <- read.table(mclfile100best.7)
mcldata500.7 <- read.table(mclfile500.7)
mcldata500best.7 <- read.table(mclfile500best.7)
mcldata1000.7 <- read.table(mclfile1000.7)
mcldata1000best.7 <- read.table(mclfile1000best.7)

colnames(mcldata5.7) <- coreColNames
colnames(mcldata5best.7) <- coreColNames
colnames(mcldata10.7) <- coreColNames
colnames(mcldata10best.7) <- coreColNames
colnames(mcldata50.7) <- coreColNames
colnames(mcldata50best.7) <- coreColNames
colnames(mcldata100.7) <- coreColNames
colnames(mcldata100best.7) <- coreColNames
colnames(mcldata500.7) <- coreColNames
colnames(mcldata500best.7) <- coreColNames
colnames(mcldata1000.7) <- coreColNames
colnames(mcldata1000best.7) <- coreColNames

mcl5Error.7 <- sqrt((mcldata5.7[,"x est"] - knownX)^2 + (mcldata5.7[,"y est"] - knownY)^2)
mcl10Error.7 <- sqrt((mcldata10.7[,"x est"] - knownX)^2 + (mcldata10.7[,"y est"] - knownY)^2)
mcl50Error.7 <- sqrt((mcldata50.7[,"x est"] - knownX)^2 + (mcldata50.7[,"y est"] - knownY)^2)
mcl100Error.7 <- sqrt((mcldata100.7[,"x est"] - knownX)^2 + (mcldata100.7[,"y est"] - knownY)^2)
mcl500Error.7 <- sqrt((mcldata500.7[,"x est"] - knownX)^2 + (mcldata500.7[,"y est"] - knownY)^2)
mcl1000Error.7 <- sqrt((mcldata1000.7[,"x est"] - knownX)^2 + (mcldata1000.7[,"y est"] - knownY)^2)
mcl5bestError.7 <- sqrt((mcldata5best.7[,"x est"] - knownX)^2 + (mcldata5best.7[,"y est"] - knownY)^2)
mcl10bestError.7 <- sqrt((mcldata10best.7[,"x est"] - knownX)^2 + (mcldata10best.7[,"y est"] - knownY)^2)
mcl50bestError.7 <- sqrt((mcldata50best.7[,"x est"] - knownX)^2 + (mcldata50best.7[,"y est"] - knownY)^2)
mcl100bestError.7 <- sqrt((mcldata100best.7[,"x est"] - knownX)^2 + (mcldata100best.7[,"y est"] - knownY)^2)
mcl500bestError.7 <- sqrt((mcldata500best.7[,"x est"] - knownX)^2 + (mcldata500best.7[,"y est"] - knownY)^2)
mcl1000bestError.7 <- sqrt((mcldata1000best.7[,"x est"] - knownX)^2 + (mcldata1000best.7[,"y est"] - knownY)^2)

mcl5TotalError.7 <-  sqrt(sum((mcl5Error.7)^2)/length(frames))
mcl10TotalError.7 <- sqrt(sum((mcl10Error.7)^2)/length(frames))
mcl50TotalError.7 <- sqrt(sum((mcl50Error.7)^2)/length(frames))
mcl100TotalError.7 <- sqrt(sum((mcl100Error.7)^2)/length(frames))
mcl500TotalError.7 <- sqrt(sum((mcl500Error.7)^2)/length(frames))
mcl1000TotalError.7 <- sqrt(sum((mcl1000Error.7)^2)/length(frames))
mcl5bestTotalError.7 <-  sqrt(sum((mcl5bestError.7)^2)/length(frames))
mcl10bestTotalError.7 <- sqrt(sum((mcl10bestError.7)^2)/length(frames))
mcl50bestTotalError.7 <- sqrt(sum((mcl50bestError.7)^2)/length(frames))
mcl100bestTotalError.7 <- sqrt(sum((mcl100bestError.7)^2)/length(frames))
mcl500bestTotalError.7 <- sqrt(sum((mcl500bestError.7)^2)/length(frames))
mcl1000bestTotalError.7 <- sqrt(sum((mcl1000bestError.7)^2)/length(frames))

mcldata5.8 <- read.table(mclfile5.8)
mcldata5best.8 <- read.table(mclfile5best.8)
mcldata10.8 <- read.table(mclfile10.8)
mcldata10best.8 <- read.table(mclfile10best.8)
mcldata50.8 <- read.table(mclfile50.8)
mcldata50best.8 <- read.table(mclfile50best.8)
mcldata100.8 <- read.table(mclfile100.8)
mcldata100best.8 <- read.table(mclfile100best.8)
mcldata500.8 <- read.table(mclfile500.8)
mcldata500best.8 <- read.table(mclfile500best.8)
mcldata1000.8 <- read.table(mclfile1000.8)
mcldata1000best.8 <- read.table(mclfile1000best.8)

colnames(mcldata5.8) <- coreColNames
colnames(mcldata5best.8) <- coreColNames
colnames(mcldata10.8) <- coreColNames
colnames(mcldata10best.8) <- coreColNames
colnames(mcldata50.8) <- coreColNames
colnames(mcldata50best.8) <- coreColNames
colnames(mcldata100.8) <- coreColNames
colnames(mcldata100best.8) <- coreColNames
colnames(mcldata500.8) <- coreColNames
colnames(mcldata500best.8) <- coreColNames
colnames(mcldata1000.8) <- coreColNames
colnames(mcldata1000best.8) <- coreColNames

mcl5Error.8 <- sqrt((mcldata5.8[,"x est"] - knownX)^2 + (mcldata5.8[,"y est"] - knownY)^2)
mcl10Error.8 <- sqrt((mcldata10.8[,"x est"] - knownX)^2 + (mcldata10.8[,"y est"] - knownY)^2)
mcl50Error.8 <- sqrt((mcldata50.8[,"x est"] - knownX)^2 + (mcldata50.8[,"y est"] - knownY)^2)
mcl100Error.8 <- sqrt((mcldata100.8[,"x est"] - knownX)^2 + (mcldata100.8[,"y est"] - knownY)^2)
mcl500Error.8 <- sqrt((mcldata500.8[,"x est"] - knownX)^2 + (mcldata500.8[,"y est"] - knownY)^2)
mcl1000Error.8 <- sqrt((mcldata1000.8[,"x est"] - knownX)^2 + (mcldata1000.8[,"y est"] - knownY)^2)
mcl5bestError.8 <- sqrt((mcldata5best.8[,"x est"] - knownX)^2 + (mcldata5best.8[,"y est"] - knownY)^2)
mcl10bestError.8 <- sqrt((mcldata10best.8[,"x est"] - knownX)^2 + (mcldata10best.8[,"y est"] - knownY)^2)
mcl50bestError.8 <- sqrt((mcldata50best.8[,"x est"] - knownX)^2 + (mcldata50best.8[,"y est"] - knownY)^2)
mcl100bestError.8 <- sqrt((mcldata100best.8[,"x est"] - knownX)^2 + (mcldata100best.8[,"y est"] - knownY)^2)
mcl500bestError.8 <- sqrt((mcldata500best.8[,"x est"] - knownX)^2 + (mcldata500best.8[,"y est"] - knownY)^2)
mcl1000bestError.8 <- sqrt((mcldata1000best.8[,"x est"] - knownX)^2 + (mcldata1000best.8[,"y est"] - knownY)^2)

mcl5TotalError.8 <-  sqrt(sum((mcl5Error.8)^2)/length(frames))
mcl10TotalError.8 <- sqrt(sum((mcl10Error.8)^2)/length(frames))
mcl50TotalError.8 <- sqrt(sum((mcl50Error.8)^2)/length(frames))
mcl100TotalError.8 <- sqrt(sum((mcl100Error.8)^2)/length(frames))
mcl500TotalError.8 <- sqrt(sum((mcl500Error.8)^2)/length(frames))
mcl1000TotalError.8 <- sqrt(sum((mcl1000Error.8)^2)/length(frames))
mcl5bestTotalError.8 <-  sqrt(sum((mcl5bestError.8)^2)/length(frames))
mcl10bestTotalError.8 <- sqrt(sum((mcl10bestError.8)^2)/length(frames))
mcl50bestTotalError.8 <- sqrt(sum((mcl50bestError.8)^2)/length(frames))
mcl100bestTotalError.8 <- sqrt(sum((mcl100bestError.8)^2)/length(frames))
mcl500bestTotalError.8 <- sqrt(sum((mcl500bestError.8)^2)/length(frames))
mcl1000bestTotalError.8 <- sqrt(sum((mcl1000bestError.8)^2)/length(frames))

mcldata5.9 <- read.table(mclfile5.9)
mcldata5best.9 <- read.table(mclfile5best.9)
mcldata10.9 <- read.table(mclfile10.9)
mcldata10best.9 <- read.table(mclfile10best.9)
mcldata50.9 <- read.table(mclfile50.9)
mcldata50best.9 <- read.table(mclfile50best.9)
mcldata100.9 <- read.table(mclfile100.9)
mcldata100best.9 <- read.table(mclfile100best.9)
mcldata500.9 <- read.table(mclfile500.9)
mcldata500best.9 <- read.table(mclfile500best.9)
mcldata1000.9 <- read.table(mclfile1000.9)
mcldata1000best.9 <- read.table(mclfile1000best.9)

colnames(mcldata5.9) <- coreColNames
colnames(mcldata5best.9) <- coreColNames
colnames(mcldata10.9) <- coreColNames
colnames(mcldata10best.9) <- coreColNames
colnames(mcldata50.9) <- coreColNames
colnames(mcldata50best.9) <- coreColNames
colnames(mcldata100.9) <- coreColNames
colnames(mcldata100best.9) <- coreColNames
colnames(mcldata500.9) <- coreColNames
colnames(mcldata500best.9) <- coreColNames
colnames(mcldata1000.9) <- coreColNames
colnames(mcldata1000best.9) <- coreColNames

mcl5Error.9 <- sqrt((mcldata5.9[,"x est"] - knownX)^2 + (mcldata5.9[,"y est"] - knownY)^2)
mcl10Error.9 <- sqrt((mcldata10.9[,"x est"] - knownX)^2 + (mcldata10.9[,"y est"] - knownY)^2)
mcl50Error.9 <- sqrt((mcldata50.9[,"x est"] - knownX)^2 + (mcldata50.9[,"y est"] - knownY)^2)
mcl100Error.9 <- sqrt((mcldata100.9[,"x est"] - knownX)^2 + (mcldata100.9[,"y est"] - knownY)^2)
mcl500Error.9 <- sqrt((mcldata500.9[,"x est"] - knownX)^2 + (mcldata500.9[,"y est"] - knownY)^2)
mcl1000Error.9 <- sqrt((mcldata1000.9[,"x est"] - knownX)^2 + (mcldata1000.9[,"y est"] - knownY)^2)
mcl5bestError.9 <- sqrt((mcldata5best.9[,"x est"] - knownX)^2 + (mcldata5best.9[,"y est"] - knownY)^2)
mcl10bestError.9 <- sqrt((mcldata10best.9[,"x est"] - knownX)^2 + (mcldata10best.9[,"y est"] - knownY)^2)
mcl50bestError.9 <- sqrt((mcldata50best.9[,"x est"] - knownX)^2 + (mcldata50best.9[,"y est"] - knownY)^2)
mcl100bestError.9 <- sqrt((mcldata100best.9[,"x est"] - knownX)^2 + (mcldata100best.9[,"y est"] - knownY)^2)
mcl500bestError.9 <- sqrt((mcldata500best.9[,"x est"] - knownX)^2 + (mcldata500best.9[,"y est"] - knownY)^2)
mcl1000bestError.9 <- sqrt((mcldata1000best.9[,"x est"] - knownX)^2 + (mcldata1000best.9[,"y est"] - knownY)^2)

mcl5TotalError.9 <-  sqrt(sum((mcl5Error.9)^2)/length(frames))
mcl10TotalError.9 <- sqrt(sum((mcl10Error.9)^2)/length(frames))
mcl50TotalError.9 <- sqrt(sum((mcl50Error.9)^2)/length(frames))
mcl100TotalError.9 <- sqrt(sum((mcl100Error.9)^2)/length(frames))
mcl500TotalError.9 <- sqrt(sum((mcl500Error.9)^2)/length(frames))
mcl1000TotalError.9 <- sqrt(sum((mcl1000Error.9)^2)/length(frames))
mcl5bestTotalError.9 <-  sqrt(sum((mcl5bestError.9)^2)/length(frames))
mcl10bestTotalError.9 <- sqrt(sum((mcl10bestError.9)^2)/length(frames))
mcl50bestTotalError.9 <- sqrt(sum((mcl50bestError.9)^2)/length(frames))
mcl100bestTotalError.9 <- sqrt(sum((mcl100bestError.9)^2)/length(frames))
mcl500bestTotalError.9 <- sqrt(sum((mcl500bestError.9)^2)/length(frames))
mcl1000bestTotalError.9 <- sqrt(sum((mcl1000bestError.9)^2)/length(frames))

mcldata5.10 <- read.table(mclfile5.10)
mcldata5best.10 <- read.table(mclfile5best.10)
mcldata10.10 <- read.table(mclfile10.10)
mcldata10best.10 <- read.table(mclfile10best.10)
mcldata50.10 <- read.table(mclfile50.10)
mcldata50best.10 <- read.table(mclfile50best.10)
mcldata100.10 <- read.table(mclfile100.10)
mcldata100best.10 <- read.table(mclfile100best.10)
mcldata500.10 <- read.table(mclfile500.10)
mcldata500best.10 <- read.table(mclfile500best.10)
mcldata1000.10 <- read.table(mclfile1000.10)
mcldata1000best.10 <- read.table(mclfile1000best.10)

colnames(mcldata5.10) <- coreColNames
colnames(mcldata5best.10) <- coreColNames
colnames(mcldata10.10) <- coreColNames
colnames(mcldata10best.10) <- coreColNames
colnames(mcldata50.10) <- coreColNames
colnames(mcldata50best.10) <- coreColNames
colnames(mcldata100.10) <- coreColNames
colnames(mcldata100best.10) <- coreColNames
colnames(mcldata500.10) <- coreColNames
colnames(mcldata500best.10) <- coreColNames
colnames(mcldata1000.10) <- coreColNames
colnames(mcldata1000best.10) <- coreColNames

mcl5Error.10 <- sqrt((mcldata5.10[,"x est"] - knownX)^2 + (mcldata5.10[,"y est"] - knownY)^2)
mcl10Error.10 <- sqrt((mcldata10.10[,"x est"] - knownX)^2 + (mcldata10.10[,"y est"] - knownY)^2)
mcl50Error.10 <- sqrt((mcldata50.10[,"x est"] - knownX)^2 + (mcldata50.10[,"y est"] - knownY)^2)
mcl100Error.10 <- sqrt((mcldata100.10[,"x est"] - knownX)^2 + (mcldata100.10[,"y est"] - knownY)^2)
mcl500Error.10 <- sqrt((mcldata500.10[,"x est"] - knownX)^2 + (mcldata500.10[,"y est"] - knownY)^2)
mcl1000Error.10 <- sqrt((mcldata1000.10[,"x est"] - knownX)^2 + (mcldata1000.10[,"y est"] - knownY)^2)
mcl5bestError.10 <- sqrt((mcldata5best.10[,"x est"] - knownX)^2 + (mcldata5best.10[,"y est"] - knownY)^2)
mcl10bestError.10 <- sqrt((mcldata10best.10[,"x est"] - knownX)^2 + (mcldata10best.10[,"y est"] - knownY)^2)
mcl50bestError.10 <- sqrt((mcldata50best.10[,"x est"] - knownX)^2 + (mcldata50best.10[,"y est"] - knownY)^2)
mcl100bestError.10 <- sqrt((mcldata100best.10[,"x est"] - knownX)^2 + (mcldata100best.10[,"y est"] - knownY)^2)
mcl500bestError.10 <- sqrt((mcldata500best.10[,"x est"] - knownX)^2 + (mcldata500best.10[,"y est"] - knownY)^2)
mcl1000bestError.10 <- sqrt((mcldata1000best.10[,"x est"] - knownX)^2 + (mcldata1000best.10[,"y est"] - knownY)^2)

mcl5TotalError.10 <-  sqrt(sum((mcl5Error.10)^2)/length(frames))
mcl10TotalError.10 <- sqrt(sum((mcl10Error.10)^2)/length(frames))
mcl50TotalError.10 <- sqrt(sum((mcl50Error.10)^2)/length(frames))
mcl100TotalError.10 <- sqrt(sum((mcl100Error.10)^2)/length(frames))
mcl500TotalError.10 <- sqrt(sum((mcl500Error.10)^2)/length(frames))
mcl1000TotalError.10 <- sqrt(sum((mcl1000Error.10)^2)/length(frames))
mcl5bestTotalError.10 <-  sqrt(sum((mcl5bestError.10)^2)/length(frames))
mcl10bestTotalError.10 <- sqrt(sum((mcl10bestError.10)^2)/length(frames))
mcl50bestTotalError.10 <- sqrt(sum((mcl50bestError.10)^2)/length(frames))
mcl100bestTotalError.10 <- sqrt(sum((mcl100bestError.10)^2)/length(frames))
mcl500bestTotalError.10 <- sqrt(sum((mcl500bestError.10)^2)/length(frames))
mcl1000bestTotalError.10 <- sqrt(sum((mcl1000bestError.10)^2)/length(frames))

# Plot error vs. num particles
ekfTotalError <- sqrt(sum((ekfError)^2)/length(frames))
ekfNATotalError <- sqrt(sum((ekfNAError)^2)/length(frames))

# Need to make this of the averages of the 10 different runs
mcl5TotalError <- mean(c(mcl5TotalError.1, mcl5TotalError.2,
                         mcl5TotalError.3,
                         mcl5TotalError.4,
                         mcl5TotalError.5,
                         mcl5TotalError.6,
                         mcl5TotalError.7,
                         mcl5TotalError.8,
                         mcl5TotalError.9,
                         mcl5TotalError.10))
mcl10TotalError <- mean(c(mcl10TotalError.1, mcl10TotalError.2,
                          mcl10TotalError.3,
                          mcl10TotalError.4,
                          mcl10TotalError.5,
                          mcl10TotalError.6,
                          mcl10TotalError.7,
                          mcl10TotalError.8,
                          mcl10TotalError.9,
                          mcl10TotalError.10))
mcl50TotalError <- mean(c(mcl50TotalError.1, mcl50TotalError.2,
                          mcl50TotalError.3,
                          mcl50TotalError.4,
                          mcl50TotalError.5,
                          mcl50TotalError.6,
                          mcl50TotalError.7,
                          mcl50TotalError.8,
                          mcl50TotalError.9,
                          mcl50TotalError.10))
mcl100TotalError <- mean(c(mcl100TotalError.1, mcl100TotalError.2,
                           mcl100TotalError.3,
                           mcl100TotalError.4,
                           mcl100TotalError.5,
                           mcl100TotalError.6,
                           mcl100TotalError.7,
                           mcl100TotalError.8,
                           mcl100TotalError.9,
                           mcl100TotalError.10))
mcl500TotalError <- mean(c(mcl500TotalError.1, mcl500TotalError.2,
                           mcl500TotalError.3,
                           mcl500TotalError.4,
                           mcl500TotalError.5,
                           mcl500TotalError.6,
                           mcl500TotalError.7,
                           mcl500TotalError.8,
                           mcl500TotalError.9,
                           mcl500TotalError.10))

mcl1000TotalError <- mean(c(mcl1000TotalError.1, mcl1000TotalError.2,
                            mcl1000TotalError.3,
                            mcl1000TotalError.4,
                            mcl1000TotalError.5,
                            mcl1000TotalError.6,
                            mcl1000TotalError.7,
                            mcl1000TotalError.8,
                            mcl1000TotalError.9,
                            mcl1000TotalError.10))

mcl5bestTotalError <- mean(c(mcl5bestTotalError.1, mcl5TotalError.2,
                             mcl5bestTotalError.3, mcl5TotalError.4,
                             mcl5bestTotalError.5, mcl5TotalError.6,
                             mcl5bestTotalError.7, mcl5TotalError.8,
                             mcl5bestTotalError.9, mcl5TotalError.10
                             ))
mcl10bestTotalError <- mean(c(mcl10bestTotalError.1, mcl10bestTotalError.2,
                              mcl10bestTotalError.3, mcl10bestTotalError.4,
                              mcl10bestTotalError.5, mcl10TotalError.6,
                              mcl10bestTotalError.7, mcl10TotalError.8,
                              mcl10bestTotalError.9, mcl10TotalError.10
                              ))
mcl50bestTotalError <- mean(c(mcl50bestTotalError.1, mcl50bestTotalError.2,
                              mcl50bestTotalError.3, mcl50bestTotalError.4,
                              mcl50bestTotalError.5, mcl50TotalError.6,
                              mcl50bestTotalError.7, mcl50TotalError.8,
                              mcl50bestTotalError.9, mcl50TotalError.10
                              ))
mcl100bestTotalError <- mean(c(mcl100bestTotalError.1, mcl100bestTotalError.2,
                               mcl100bestTotalError.3, mcl100bestTotalError.4,
                               mcl100bestTotalError.5, mcl100TotalError.6,
                               mcl100bestTotalError.7, mcl100TotalError.8,
                               mcl100bestTotalError.9, mcl100TotalError.10
                               ))
mcl500bestTotalError <- mean(c(mcl500bestTotalError.1, mcl500bestTotalError.2,
                               mcl500bestTotalError.3, mcl500bestTotalError.4,
                               mcl500bestTotalError.5, mcl500TotalError.6,
                               mcl500bestTotalError.7, mcl500TotalError.8,
                               mcl500bestTotalError.9, mcl500TotalError.10
                               ))
mcl1000bestTotalError <- mean(c(mcl1000bestTotalError.1, mcl1000bestTotalError.2,
                                mcl1000bestTotalError.3, mcl1000bestTotalError.4,
                                mcl1000bestTotalError.5, mcl1000TotalError.6,
                                mcl1000bestTotalError.7, mcl1000TotalError.8,
                                mcl1000bestTotalError.9, mcl1000TotalError.10
                                ))


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
plot(errorVParticlesX, errorVParticlesY, log="x", main="RMS of Position Error vs Number of Particles", xlab="Sample Size ", ylab="Error RMS", pch="", ylim=totalErrorMinMax)
points(errorVParticlesX, errorVParticlesY, lty = 1, col = "blue", type="l", lwd="2")
points(errorVParticlesX, errorVParticlesbestY, lty = 2, col = "purple", type="l", lwd="2")
abline(ekfTotalError, 0, lty = 3, col = "red", type="l", lwd="2")
abline(ekfNATotalError, 0, lty = 4, col = "green", type="l", lwd="2")
legend("topright", c("MCL-mean","MCL-best","EKF","EKF-NA"), lty=c(1,2,3,4),col=c("blue","purple","red","green"))
dev.off()


# get RMS of the headings
ekfHrms <- sqrt(sum((ekfH^2 - knownH^2)^2)/length(knownH))
ekfNAHrms <- sqrt(sum((ekfNAH^2 - knownH^2)^2)/length(knownH))

mcl5Hrms.1 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.1 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.1 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.1 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.1 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.1 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.1 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.1 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.1 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.1 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.1 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.1 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))

mcl5Hrms.2 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.2 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.2 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.2 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.2 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.2 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.2 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.2 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.2 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.2 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.2 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.2 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))


mcl5Hrms.3 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.3 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.3 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.3 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.3 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.3 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.3 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.3 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.3 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.3 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.3 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.3 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))

mcl5Hrms.4 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.4 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.4 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.4 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.4 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.4 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.4 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.4 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.4 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.4 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.4 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.4 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))

mcl5Hrms.5 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.5 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.5 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.5 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.5 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.5 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.5 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.5 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.5 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.5 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.5 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.5 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))

mcl5Hrms.6 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.6 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.6 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.6 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.6 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.6 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.6 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.6 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.6 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.6 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.6 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.6 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))

mcl5Hrms.7 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.7 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.7 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.7 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.7 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.7 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.7 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.7 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.7 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.7 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.7 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.7 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))

mcl5Hrms.8 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.8 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.8 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.8 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.8 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.8 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.8 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.8 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.8 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.8 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.8 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.8 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5Hrms.9 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.9 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.9 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.9 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.9 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.9 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.9 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.9 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.9 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.9 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.9 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.9 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5Hrms.10 <-  sqrt(sum((mcldata5[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10Hrms.10 <- sqrt(sum((mcldata10[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50Hrms.10 <- sqrt(sum((mcldata50[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100Hrms.10 <- sqrt(sum((mcldata100[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500Hrms.10 <- sqrt(sum((mcldata500[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000Hrms.10 <- sqrt(sum((mcldata1000[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl5bestHrms.10 <-  sqrt(sum((mcldata5best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl10bestHrms.10 <- sqrt(sum((mcldata10best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl50bestHrms.10 <- sqrt(sum((mcldata50best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl100bestHrms.10 <- sqrt(sum((mcldata100best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl500bestHrms.10 <- sqrt(sum((mcldata500best[,"h est"]^2 - knownH^2)^2)/length(knownH))
mcl1000bestHrms.10 <- sqrt(sum((mcldata1000best[,"h est"]^2 - knownH)^2)/length(knownH))

mcl5Hrms <- mean(c(
                   mcl5Hrms.1, mcl5Hrms.2,
                   mcl5Hrms.3, mcl5Hrms.4,
                   mcl5Hrms.5, mcl5Hrms.6,
                   mcl5Hrms.7, mcl5Hrms.8,
                   mcl5Hrms.9, mcl5Hrms.10
                   ))
mcl10Hrms <- mean(c(
                    mcl10Hrms.1, mcl10Hrms.2,
                    mcl10Hrms.3, mcl10Hrms.4,
                    mcl10Hrms.5, mcl10Hrms.6,
                    mcl10Hrms.7, mcl10Hrms.8,
                    mcl10Hrms.9, mcl10Hrms.10
                    ))
mcl50Hrms <- mean(c(
                    mcl50Hrms.1, mcl50Hrms.2,
                    mcl50Hrms.3, mcl50Hrms.4,
                    mcl50Hrms.5, mcl50Hrms.6,
                    mcl50Hrms.7, mcl50Hrms.8,
                    mcl50Hrms.9, mcl50Hrms.10
                    ))
mcl100Hrms <- mean(c(
                     mcl100Hrms.1, mcl100Hrms.2,
                    mcl100Hrms.3, mcl100Hrms.4,
                    mcl100Hrms.5, mcl100Hrms.6,
                    mcl100Hrms.7, mcl100Hrms.8,
                    mcl100Hrms.9, mcl100Hrms.10
                     ))
mcl500Hrms <- mean(c(
                     mcl500Hrms.1, mcl500Hrms.2,
                     mcl500Hrms.3, mcl500Hrms.4,
                     mcl500Hrms.5, mcl500Hrms.6,
                     mcl500Hrms.7, mcl500Hrms.8,
                     mcl500Hrms.9, mcl500Hrms.10
                     ))
mcl1000Hrms <- mean(c(
                      mcl1000Hrms.1, mcl1000Hrms.2,
                     mcl1000Hrms.3, mcl1000Hrms.4,
                     mcl1000Hrms.5, mcl1000Hrms.6,
                     mcl1000Hrms.7, mcl1000Hrms.8,
                     mcl1000Hrms.9, mcl1000Hrms.10
                      ))
mcl5bestHrms <- mean(c(
                       mcl5bestHrms.1, mcl5bestHrms.2,
                        mcl5bestHrms.3, mcl5bestHrms.4,
                        mcl5bestHrms.5, mcl5bestHrms.6,
                        mcl5bestHrms.7, mcl5bestHrms.8,
                        mcl5bestHrms.9, mcl5bestHrms.10
                       ))
mcl10bestHrms <- mean(c(
                        mcl10bestHrms.1, mcl10bestHrms.2,
                        mcl10bestHrms.3, mcl10bestHrms.4,
                        mcl10bestHrms.10, mcl10bestHrms.6,
                        mcl10bestHrms.7, mcl10bestHrms.8,
                        mcl10bestHrms.9, mcl10bestHrms.10
                        ))
mcl50bestHrms <- mean(c(
                        mcl50bestHrms.1, mcl50bestHrms.2,
                        mcl50bestHrms.3, mcl50bestHrms.4,
                        mcl50bestHrms.5, mcl50bestHrms.6,
                        mcl50bestHrms.7, mcl50bestHrms.8,
                        mcl50bestHrms.9, mcl50bestHrms.10
                        ))
mcl100bestHrms <- mean(c(
                         mcl100bestHrms.1, mcl100bestHrms.2,
                        mcl100bestHrms.3, mcl100bestHrms.4,
                        mcl100bestHrms.5, mcl100bestHrms.6,
                        mcl100bestHrms.7, mcl100bestHrms.8,
                        mcl100bestHrms.9, mcl100bestHrms.10
                         ))
mcl500bestHrms <- mean(c(
                         mcl500bestHrms.1, mcl500bestHrms.2,
                         mcl500bestHrms.3, mcl500bestHrms.4,
                         mcl500bestHrms.5, mcl500bestHrms.6,
                         mcl500bestHrms.7, mcl500bestHrms.8,
                         mcl500bestHrms.9, mcl500bestHrms.10
                          ))
mcl1000bestHrms <- mean(c(
                          mcl1000bestHrms.1, mcl1000bestHrms.2,
                          mcl1000bestHrms.3, mcl1000bestHrms.4,
                          mcl1000bestHrms.5, mcl1000bestHrms.6,
                          mcl1000bestHrms.7, mcl1000bestHrms.8,
                          mcl1000bestHrms.9, mcl1000bestHrms.10
                          ))


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
plot(x=errorVParticlesX, y=errorVParticlesH, log="x", main="Heading Error RMS vs Number of Particles", xlab="Sample Size", ylab="Error RMS", pch="", ylim=c(0,max(max(errorVParticlesH),max(errorVParticlesHbest))))
points(errorVParticlesX, errorVParticlesH, lty = 1, col = "blue", type="l", lwd="2")
points(errorVParticlesX, errorVParticlesHbest, lty = 2, col = "purple", type="l", lwd="2")
abline(ekfHrms, 0, lty = 3, col = "red", type="l", lwd="2")
abline(ekfNAHrms, 0, lty = 4, col = "green", type="l", lwd="2")
legend("topright", c("MCL-mean","MCL-best","EKF","EKF-NA"), lty=c(1,2,3,4),col=c("blue","purple","red","green"))
dev.off()
