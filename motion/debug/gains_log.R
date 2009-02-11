# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: Decemeber 2008


name = "gains_log"
PDF = ".pdf"

file = paste("/tmp/",name,".xls",sep="")
if(!file.exists(file))
	quit("no")
dat = read.table(file,header=T,na.strings=c("-"))

pdf(paste(name,PDF,sep=""))
names = c("gain")
cols = c("2")
plot(dat$time,dat$gain,type="l",
     main="G_d(j) function: the preview gain for a ZMP
           reference value 'index' frames in the future",
					      xlab="index",ylab="value",col=cols)
legend("top",lwd=2,legend=names,col=cols)
dev.off()
