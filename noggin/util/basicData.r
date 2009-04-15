# read in the data
name <- "./fakeData/rightLeft/rightLeft"

ekffile <- paste(name, ".ekf.core", sep="")
mclfile <- paste(name, ".mcl.core", sep="")
mclfile2 <- paste(name, ".mcl.core.2", sep="")

ekfdata <- read.table(ekffile)
mcldata <- read.table(mclfile)
mcldata2 <- read.table(mclfile2)

# Get usable column names
coreColNames <- c("x est", "y est", "h est", "x uncert", "y uncert", "h uncert", "ball x est", "ball y est", "ball x vel est", "ball y vel est", "ball x uncert", "ball y uncert", "ball x vel uncert", "ball y vel uncert", "delta f", "delta l", "delta r", "known x", "known y", "known h", "known ball x", "known ball y", "known ball x vel", "known ball y vel")
colnames(ekfdata) <- coreColNames
colnames(mcldata) <- coreColNames
colnames(mcldata2) <- coreColNames

# Frame data
frames <- rownames(ekfdata)

ekfX <- ekfdata[,"x est"]
ekfY <- ekfdata[,"y est"]
ekfH <- ekfdata[,"h est"]

mclX <- mcldata[,"x est"]
mclY <- mcldata[,"y est"]
mclH <- mcldata[,"h est"]

mcl2X <- mcldata2[,"x est"]
mcl2Y <- mcldata2[,"y est"]
mcl2H <- mcldata2[,"h est"]

knownX <- mcldata[,"known x"]
knownY <- mcldata[,"known y"]
knownH <- mcldata[,"known h"]

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
pdf(paste(name, ".xEsts.pdf", sep=""))
plot(frames, mclX, main="X Estimates", xlab="Frame #", ylab="x est (cm)", pch="",xlim=frameMinMax,ylim=xMinMax)
points(frames, knownX, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfX, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclX, lty = 5, col = "blue", type="l", lwd="2")
#points(frames, mcl2X, lty = 4, col = "purple", type="l", lwd="2")
legend("topleft",lty=c(1,2,5), c("Known", "EKF", "MCL"), fill=c("green", "red", "blue"))
dev.off()

# Plot the h data
pdf(paste(name, ".yEsts.pdf", sep=""))
plot(frames, mclY, main="Y Estimates", xlab="Frame #", ylab="y est (cm)", pch="",xlim=frameMinMax,ylim=yMinMax)
points(frames, knownY, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfY, lty = 2, col = "red", type="l",lwd="2")
points(frames, mclY, lty = 5, col = "blue", type="l",lwd="2")
legend("topright",lty=c(1,2,5), c("Known", "EKF", "MCL"), fill=c("green", "red", "blue"))
dev.off()

# Plot the h data
pdf(paste(name, ".hEsts.pdf", sep=""))
plot(frames, mclH, main="H Estimates", xlab="Frame #", ylab="h est (cm)", pch="",xlim=frameMinMax,ylim=hMinMax)
points(frames, knownH, lty = 1, col = "green", type="l", lwd="2")
points(frames, ekfH, lty = 2, col = "red", type="l", lwd="2")
points(frames, mclH, lty = 5, col = "blue", type="l", lwd="2")
legend("topleft", lty=c(1,2,5), c("Known", "EKF", "MCL"), fill=c("green", "red", "blue"))
dev.off()
