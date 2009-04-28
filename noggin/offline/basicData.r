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
mcldata100 <- read.table(mclfile100)
mcldata100best <- read.table(mclfile100best)

# Get usable column names
coreColNames <- c("x est", "y est", "h est", "x uncert", "y uncert", "h uncert", "ball x est", "ball y est", "ball x vel est", "ball y vel est", "ball x uncert", "ball y uncert", "ball x vel uncert", "ball y vel uncert", "delta f", "delta l", "delta r", "known x", "known y", "known h", "known ball x", "known ball y", "known ball x vel", "known ball y vel")
colnames(ekfdata) <- coreColNames
colnames(ekfnadata) <- coreColNames
colnames(mcldata100) <- coreColNames
colnames(mcldata100best) <- coreColNames

# Frame data
frames <- rownames(ekfdata)

ekfX <- ekfdata[,"x est"]
ekfY <- ekfdata[,"y est"]
ekfH <- ekfdata[,"h est"]

ekfNAX <- ekfnadata[,"x est"]
ekfNAY <- ekfnadata[,"y est"]
ekfNAH <- ekfnadata[,"h est"]

mclX <- mcldata100[,"x est"]
mclY <- mcldata100[,"y est"]
mclH <- mcldata100[,"h est"]

mcl2X <- mcldata100best[,"x est"]
mcl2Y <- mcldata100best[,"y est"]
mcl2H <- mcldata100best[,"h est"]

knownX <- mcldata100[,"known x"]
knownY <- mcldata100[,"known y"]
knownH <- mcldata100[,"known h"]

# EKF difference data
ekfXDiff <- ekfX - knownX
ekfYDiff <- ekfY - knownY
ekfHDiff <- ekfH - knownH

# MCL difference data
mclXDiff <- mclX - knownX
mclYDiff <- mclY - knownY
mclHDiff <- mclH - knownH

# Plot minMaxs
frameMinMax = c(1,length(frames))
xMinMax = c(min(c(min(ekfX),min(knownX),min(mclX),min(mcl2X))),max(c(max(ekfX),max(knownX),max(mclX),max(mcl2X))))
yMinMax = c(min(c(min(ekfY),min(knownY),min(mclY))),max(c(max(ekfY),max(knownY),max(mclY))))
hMinMax = c(min(c(min(ekfH),min(knownH),min(mclH))),max(c(max(ekfH),max(knownH),max(mclH))))

# Plot the x data
pdf(paste(name, "-xEsts.pdf", sep=""))
plot(frames, mclX, main="X Estimates", xlab="Frame #", ylab="x est (cm)", pch="",xlim=frameMinMax,ylim=xMinMax)
points(frames, knownX, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfX, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclX, lty = 5, col = "blue", type="l", lwd="2")
points(frames, ekfNAX, lty = 4, col = "purple", type="l", lwd="2")
legend("topleft",lty=c(1,2,5), c("Known", "EKF", "MCL", "EKF-NA"), fill=c("green", "red", "blue", "purple"))
dev.off()

# Plot the h data
pdf(paste(name, "-yEsts.pdf", sep=""))
plot(frames, mclY, main="Y Estimates", xlab="Frame #", ylab="y est (cm)", pch="",xlim=frameMinMax,ylim=yMinMax)
points(frames, knownY, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfY, lty = 2, col = "red", type="l",lwd="2")
points(frames, mclY, lty = 5, col = "blue", type="l",lwd="2")
points(frames, ekfNAY, lty = 4, col = "purple", type="l", lwd="2")
legend("topright",lty=c(1,2,5), c("Known", "EKF", "MCL", "EKF-NA"), fill=c("green", "red", "blue", "purple"))
dev.off()

# Plot the h data
pdf(paste(name, "-hEsts.pdf", sep=""))
plot(frames, mclH, main="H Estimates", xlab="Frame #", ylab="h est (cm)", pch="",xlim=frameMinMax,ylim=hMinMax)
points(frames, knownH, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfH, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclH, lty = 5, col = "blue", type="l", lwd="2")
points(frames, ekfNAH, lty = 4, col = "purple", type="l", lwd="2")
legend("topright",lty=c(1,2,5), c("Known", "EKF", "MCL", "EKF-NA"), fill=c("green", "red", "blue", "purple"))
dev.off()
