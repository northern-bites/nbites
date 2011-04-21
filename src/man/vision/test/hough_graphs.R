
## Read in CSV File
zeroData <- read.csv("zero_timing_results.txt", head=TRUE, sep=",")
oneData <- read.csv("one_timing_results.txt", head=TRUE, sep=",")
coupleData <- read.csv("couple_timing_results.txt", head=TRUE, sep=",")
clutterData <- read.csv("clutter_timing_results.txt", head=TRUE, sep=",")

zeroDataClean <- zeroData[grep('Vision|Sobel|Edge|Hough',zeroData$Component.Name),]
oneDataClean <- oneData[grep('Vision|Sobel|Edge|Hough',oneData$Component.Name),]
coupleDataClean <- coupleData[grep('Vision|Sobel|Edge|Hough',coupleData$Component.Name),]
clutterDataClean <- clutterData[grep('Vision|Sobel|Edge|Hough',clutterData$Component.Name),]

names <- c("Vision","Edges", "Sobel", "Edge Peaks", "Hough Transform", "Mark Hough Edges,", "Smooth", "Hough Peaks", "Suppress")

## Colors found on
## http://www.personal.psu.edu/cab38/ColorBrewer/ColorBrewer.html
colors <- c("#d73027","#fc8d59","#fee090","#ffffffbf","#e0f3f8","#91bfdb","#4575b4")

zero <- zeroDataClean[3]
one <- oneDataClean[3]
couple <- coupleDataClean[3]
clutter <- clutterDataClean[3]

colnames(zero) <- "Zero"
colnames(one) <- "One"
colnames(couple) <- "Couple"
colnames(clutter) <- "Clutter"


t <- as.table(as.matrix(cbind(zero,one,couple,clutter)))

rownames(t) <- names

pdf(file="hough.pdf", height=7, width=11)
barplot(t,col=rainbow(7),beside=TRUE,main="Edge Detection Profiling Results")
legend("topright",names,fill=rainbow(7))
dev.off()

