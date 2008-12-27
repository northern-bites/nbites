# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: Decemeber 2008


dat = read.table("/tmp/com_log.xls",header=T)

name = "com_log"
PDF = ".pdf"

pdf(paste(name,"-x",PDF,sep=""))
plot(dat$time,dat$pre_x,pch="",main="",xlab="s",ylab="mm")
names = c("com_x","pre_x")
cols = c("2","3")
points(dat$time,dat$com_x,type="l",col=2)
points(dat$time,dat$pre_x,type="l",col=3)
legend("top",lwd=2,legend=names,col=cols)
dev.off()

pdf(paste(name,"-y",PDF,sep=""))
plot(dat$time,dat$pre_y,pch="",main="",xlab="s",ylab="mm")
names = c("com_y","pre_y")
cols = c("2","3")
points(dat$time,dat$com_y,type="l",col=2)
points(dat$time,dat$pre_y,type="l",col=3)
legend("top",lwd=2,legend=names,col=cols)
dev.off()