# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: March 2009


name = "zmpacc_log"
PDF = ".pdf"

file = paste("/tmp/",name,".xls",sep="")
if(!file.exists(file))
	quit("no")
dat = read.table(file,header=T,na.strings=c("-"))

pdf(paste(name,"-x",PDF,sep=""),width=50,height=30)
plot(dat$time,dat$accX,pch="",main="",xlab="s",ylab="mm")
names = c("acc_x","filteredX","uncertX")
cols = c("2","3", "4")

points(dat$time,dat$accX,type="l",col=cols[1])
points(dat$time,dat$filteredAccX,type="l",col=cols[2])
points(dat$time,dat$filteredAccXUnc,type="l",col=cols[3])


legend("top",lwd=2,legend=names,col=cols)
dev.off()

pdf(paste(name,"-y",PDF,sep=""),width=50,height=30)
plot(dat$time,dat$accY,pch="",main="",xlab="s",ylab="mm")
names = c("acc_y","filteredY","uncertY")
cols = c("2","3", "4")

points(dat$time,dat$accY,type="l",col=cols[1])
points(dat$time,dat$filteredAccY,type="l",col=cols[2])
points(dat$time,dat$filteredAccYUnc,type="l",col=cols[3])


legend("top",lwd=2,legend=names,col=cols)
dev.off()

pdf(paste(name,"-z",PDF,sep=""),width=50,height=30)
plot(dat$time,dat$accZ,pch="",main="",xlab="s",ylab="mm")
names = c("acc_z","filteredZ","uncertZ")
cols = c("2","3", "4")

points(dat$time,dat$accZ,type="l",col=cols[1])
points(dat$time,dat$filteredAccZ,type="l",col=cols[2])
points(dat$time,dat$filteredAccZUnc,type="l",col=cols[3])


legend("top",lwd=2,legend=names,col=cols)
dev.off()