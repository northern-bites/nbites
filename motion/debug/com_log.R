# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: Decemeber 2008


name = "com_log"
PDF = ".pdf"

file = paste("/tmp/",name,".xls",sep="")
if(!file.exists(file))
	quit("no")
dat = read.table(file,header=T,na.strings=c("-"))

pdf(paste(name,"-x",PDF,sep=""))
plot(dat$time,dat$pre_x,pch="",main="",xlab="s",ylab="mm")
names = c("com_x","pre_x","zmp_x","real_com_x","sensor_zmp_x")
cols = c("2","3","4","5","6")


zeros = dat$time
zeros[zeros > -100] = mean(dat$pre_x,na.rm=T) +5
points(dat$time,zeros,pch=1,col=dat$state+2)


points(dat$time,dat$com_x,type="l",col=2)
points(dat$time,dat$pre_x,type="l",col=3)
points(dat$time,dat$zmp_x,type="l",col=4)
points(dat$time,dat$real_com_x,type="l",col=5)
points(dat$time,dat$sensor_zmp_x,type="l",col=6)
legend("top",lwd=2,legend=names,col=cols)
dev.off()



pdf(paste(name,"-y",PDF,sep=""))
plot(dat$time,dat$pre_y,pch="",main="",xlab="s",ylab="mm")
names = c("com_y","pre_y","zmp_y","real_com_y","sensor_zmp_y")
cols = c("2","3","4","5","6")

zeros = dat$time
zeros[zeros > -100] = mean(dat$pre_y,na.rm=T) +5
points(dat$time,zeros,pch=18,col=dat$state+2)

points(dat$time,dat$com_y,type="l",col=2)
points(dat$time,dat$pre_y,type="l",col=3)
points(dat$time,dat$zmp_y,type="l",col=4)
points(dat$time,dat$real_com_y,type="l",col=5)
points(dat$time,dat$sensor_zmp_y,type="l",col=6)
legend("top",lwd=2,legend=names,col=cols)
dev.off()



pdf(paste(name,"-xy",PDF,sep=""))
plot(dat$pre_x,dat$pre_y,pch="",main="",xlab="x",ylab="y")
names = c("com","pre","zmp")
cols = c("2","3","4")

points(dat$com_x,dat$com_y,type="l",col=2)
points(dat$pre_x,dat$pre_y,type="l",col=3)
points(dat$zmp_x,dat$zmp_y,type="l",col=4)
legend("top",lwd=2,legend=names,col=cols)
dev.off()
