# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: Decemeber 2008


name = "zmp_log"
PDF = ".pdf"

file = paste("/tmp/",name,".xls",sep="")
if(!file.exists(file))
	quit("no")
dat = read.table(file,header=T,na.strings=c("-"))

pdf(paste(name,"-x",PDF,sep=""),width=16, height=4)
plot(dat$time,dat$pre_x,pch="",main="",xlab="s",ylab="mm")
names = c("ekf_zmp_x","com_x","pre_x","zmp_x")
cols = c("1","2","3","4")

points(dat$time,dat$ekf_zmp_x,type="l",col=1)
points(dat$time,dat$com_x,type="l",col=2)
points(dat$time,dat$pre_x,type="l",col=3)
points(dat$time,dat$com_px,type="l",col=4)

legend("top",lwd=2,legend=names,col=cols)
dev.off()

pdf(paste(name,"-y",PDF,sep=""),width=16, height=4)
plot(dat$time,dat$pre_y,pch="",main="",xlab="s",ylab="mm")
names = c("ekf_zmp_y","com_y","pre_y","zmp_y")
cols = c("1","2","3","4")

points(dat$time,dat$ekf_zmp_y,type="l",col=1)
points(dat$time,dat$com_y,type="l",col=2)
points(dat$time,dat$pre_y,type="l",col=3)
points(dat$time,dat$com_py,type="l",col=4)

legend("top",lwd=2,legend=names,col=cols)
dev.off()
