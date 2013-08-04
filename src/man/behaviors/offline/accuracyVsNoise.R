# write our functions
hypot <- function(x,y) {
  return(sqrt(x^2+y^2))
}
rms <- function(data) {
  return ( sqrt( sum(data)^2 / length(data) ))
}
positionErrorRMS <- function(data) {
  return ( rms( hypot(data[,"x est"] - data[,"known x"],
                     data[,"y est"] - data[,"known y"])))
}

coreColNames <- c("x est", "y est", "h est", "x uncert", "y uncert", "h uncert", "ball x est", "ball y est", "ball x vel est", "ball y vel est", "ball x uncert", "ball y uncert", "ball x vel uncert", "ball y vel uncert", "delta f", "delta l", "delta r", "known x", "known y", "known h", "known ball x", "known ball y", "known ball x vel", "known ball y vel")

# Read in the required data
name <- "../../../fakeData/leftRight/leftRight"
#name <- "../../../fakeData/spinCenter/spinCenter"

# set the noise levels
#noiseLevels = c(0,0.02, 0.04,0.06,0.08,0.10,0.12,0.14,0.16,0.18,0.2)
noiseLevels = c((0:29)* 0.02)

## ekfData.0 <- read.table(paste(name,".ekf.noise.0",sep=""))
## ekfData.0.02 <- read.table(paste(name,".ekf.noise.0.02",sep=""))
## ekfData.0.04 <- read.table(paste(name,".ekf.noise.0.04",sep=""))
## ekfData.0.06 <- read.table(paste(name,".ekf.noise.0.06",sep=""))
## ekfData.0.08 <- read.table(paste(name,".ekf.noise.0.08",sep=""))
## ekfData.0.10 <- read.table(paste(name,".ekf.noise.0.1",sep=""))
## ekfData.0.12 <- read.table(paste(name,".ekf.noise.0.12",sep=""))
## ekfData.0.14 <- read.table(paste(name,".ekf.noise.0.14",sep=""))
## ekfData.0.16 <- read.table(paste(name,".ekf.noise.0.16",sep=""))
## ekfData.0.18 <- read.table(paste(name,".ekf.noise.0.18",sep=""))
## ekfData.0.2 <- read.table(paste(name,".ekf.noise.0.2",sep=""))

## colnames(ekfData.0) <- coreColNames
## colnames(ekfData.0.02) <- coreColNames
## colnames(ekfData.0.04) <- coreColNames
## colnames(ekfData.0.06) <- coreColNames
## colnames(ekfData.0.08) <- coreColNames
## colnames(ekfData.0.10) <- coreColNames
## colnames(ekfData.0.12) <- coreColNames
## colnames(ekfData.0.14) <- coreColNames
## colnames(ekfData.0.16) <- coreColNames
## colnames(ekfData.0.18) <- coreColNames
## colnames(ekfData.0.2) <- coreColNames

# Average the root mean squares of the mcl files
mclErrors=c()
# Get the root mean square for the EKF
ekfErrors=c()
knownX <- ekfData.0[,"known x"]
knownY <- ekfData.0[,"known y"]

## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.02[,"x est"] - knownX)^2 +
##           (ekfData.0.02[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.04[,"x est"] - knownX)^2 +
##           (ekfData.0.04[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.06[,"x est"] - knownX)^2 +
##           (ekfData.0.06[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.08[,"x est"] - knownX)^2 +
##           (ekfData.0.08[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.10[,"x est"] - knownX)^2 +
##           (ekfData.0.10[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.12[,"x est"] - knownX)^2 +
##           (ekfData.0.12[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.14[,"x est"] - knownX)^2 +
##           (ekfData.0.14[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.16[,"x est"] - knownX)^2 +
##           (ekfData.0.16[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.18[,"x est"] - knownX)^2 +
##           (ekfData.0.18[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## error <- sqrt((ekfData.0.2[,"x est"] - knownX)^2 +
##           (ekfData.0.2[,"y est"] - knownY)^2)
## ekfErr <- sqrt(sum((error)^2)/length(knownX))
## ekfErrors <- append(ekfErrors,ekfErr)

## print("EKF Data")
## print (ekfErrors)

# Loop through the 10 different noise levels
for (i in 1:length(noiseLevels)) {
  ekfData <- read.table(paste(name,".ekf.noise.",noiseLevels[i],sep=""))
  mclData.0 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".0",sep=""))
  mclData.1 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".1",sep=""))
  mclData.2 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".2",sep=""))
  mclData.3 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".3",sep=""))
  mclData.4 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".4",sep=""))
  mclData.5 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".5",sep=""))
  mclData.6 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".6",sep=""))
  mclData.7 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".7",sep=""))
  mclData.8 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".8",sep=""))
  mclData.9 <- read.table(paste(name,".mcl.noise.",noiseLevels[i],".9",sep=""))

  colnames(ekfData) <- coreColNames
  colnames(mclData.0) <- coreColNames
  colnames(mclData.1) <- coreColNames
  colnames(mclData.2) <- coreColNames
  colnames(mclData.3) <- coreColNames
  colnames(mclData.4) <- coreColNames
  colnames(mclData.5) <- coreColNames
  colnames(mclData.6) <- coreColNames
  colnames(mclData.7) <- coreColNames
  colnames(mclData.8) <- coreColNames
  colnames(mclData.9) <- coreColNames

  newErrors <- c()
  error <- sqrt((mclData.0[,"x est"] - knownX)^2 +
                (mclData.0[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.1[,"x est"] - knownX)^2 +
                (mclData.1[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.2[,"x est"] - knownX)^2 +
                (mclData.2[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.3[,"x est"] - knownX)^2 +
                (mclData.3[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.4[,"x est"] - knownX)^2 +
                (mclData.4[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.5[,"x est"] - knownX)^2 +
                (mclData.5[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.6[,"x est"] - knownX)^2 +
                (mclData.6[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.7[,"x est"] - knownX)^2 +
                (mclData.7[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.8[,"x est"] - knownX)^2 +
                (mclData.8[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)
  error <- sqrt((mclData.9[,"x est"] - knownX)^2 +
                (mclData.9[,"y est"] - knownY)^2)
  nError <- sqrt(sum((error)^2)/length(knownX))
  newErrors <- append(newErrors,nError)

  newMean = mean(newErrors)
  mclErrors <- append(mclErrors, newMean)

  error <- sqrt((ekfData[,"x est"] - knownX)^2 +
                (ekfData[,"y est"] - knownY)^2)
  ekfErr <- sqrt(sum((error)^2)/length(knownX))
  ekfErrors <- append(ekfErrors, ekfErr)
  print (paste("Noise levels are", noiseLevels[i], sep=" "))
  print ("MCL errors")
  print (mclErrors)
  print ("EKF errors")
  print (ekfErrors)
}

is.na(mclErrors) <- is.na(mclErrors)

pdf(paste(name, "-NoiseVAccuracy.pdf", sep=""))
plot(x=noiseLevels,
     y=mclErrors,
     main="Position Estimate Error RMS vs Input Noise Levels",
     xlab="Input Noise Level (SD of distance %)",
     ylab="Position Error RMS",
     pch="",
     xlim=c(min(noiseLevels), max(noiseLevels)),
     ylim=c(0,max(max(mclErrors,na.rm=TRUE), max(ekfErrors,na.rm=TRUE))))
points(approx(noiseLevels,
              mclErrors),
       lty = 1,
       col = "blue",
       type="l",
       lwd="2")
points(noiseLevels,
       ekfErrors,
       lty = 2,
       col = "red",
       type="l",
       lwd="2")
legend("topleft",
       c("MCL-100","EKF"),
       lty=c(1,2),
       col=c("blue","red"))
dev.off()
