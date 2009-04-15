# read in the data
name <- "./fakeData/leftRight/leftRight"

ekffile <- paste(name, ".ekf.core", sep="")
mclfile <- paste(name, ".mcl.core", sep="")

ekfdata <- read.table(ekffile)
mcldata <- read.table(mclfile)

# Get usable column names
coreColNames <- c("x est", "y est", "h est", "x uncert", "y uncert", "h uncert", "ball x est", "ball y est", "ball x vel est", "ball y vel est", "ball x uncert", "ball y uncert", "ball x vel uncert", "ball y vel uncert", "delta f", "delta l", "delta r", "known x", "known y", "known h", "known ball x", "known ball y", "known ball x vel", "known ball y vel")
colnames(ekfdata) <- coreColNames
colnames(mcldata) <- coreColNames

# Frame data
frames <- rownames(ekfdata)

# EKF difference data
ekfXDiff <- ekfdata["x est"] - ekfdata["known x"]
ekfYDiff <- ekfdata["y est"] - ekfdata["known y"]
ekfHDiff <- ekfdata["h est"] - ekfdata["known h"]

# MCL difference data
mclXDiff <- mcldata["x est"] - mcldata["known x"]
mclYDiff <- mcldata["y est"] - mcldata["known y"]
mclHDiff <- mcldata["h est"] - mcldata["known h"]

# Plot the x data
pdf(paste(name, ".xEsts.pdf", sep=""))
plot(frames, mcldata[,"x est"], main="X Estimates", xlab="frame", ylab="x est", pch="")
points(frames, ekfdata[,"known x"], pch = "o", col = "blue")
points(frames, ekfdata[,"x est"], pch = "x", col = "red")
points(frames, mcldata[,"x est"], pch = "+", col = "green")
dev.off()

# Plot the y data
pdf(paste(name, ".yEsts.pdf", sep=""))
plot(frames, mcldata[,"y est"], main="Y Estimates", xlab="frame", ylab="y est", pch="")
points(frames, ekfdata[,"known y"], pch = "o", col = "blue")
points(frames, ekfdata[,"y est"], pch = "x", col = "red")
points(frames, mcldata[,"y est"], pch = "+", col = "green")
dev.off()

# Plot the h data
pdf(paste(name, ".hEsts.pdf", sep=""))
plot(frames, mcldata[,"h est"], main="H Estimates", xlab="frame", ylab="h est", pch="")
points(frames, ekfdata[,"known h"], pch = "o", col = "blue")
points(frames, ekfdata[,"h est"], pch = "x", col = "red")
points(frames, mcldata[,"h est"], pch = "+", col = "green")
dev.off()
