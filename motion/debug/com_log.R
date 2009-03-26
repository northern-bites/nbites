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

pdf(paste(name,"-x",PDF,sep=""),width=80, height=20)
plot(dat$time,dat$pre_x,pch="",main="",xlab="s",ylab="mm")
names = c("com_x","pre_x","zmp_x","real_com_x","sensor_zmp_x")
cols = c("2","3","4","5","6")


zeros = dat$time
zeros[zeros > -100] = mean(dat$pre_x,na.rm=T) +5
points(dat$time,zeros,pch=1,col=dat$state+2)


points(dat$time,dat$com_x,type="l",col=2)
points(dat$time,dat$pre_x,type="l",col=3)
points(dat$time,dat$zmp_x,type="l",col=4)
points(dat$time,dat$real_com_x,type="p",col=5)
points(dat$time,dat$sensor_zmp_x,type="l",col=6)
legend("top",lwd=2,legend=names,col=cols)
dev.off()



pdf(paste(name,"-y",PDF,sep=""),width=80, height=20)
plot(dat$time,dat$pre_y,pch="",main="",xlab="s",ylab="mm")
names = c("com_y","pre_y","zmp_y","real_com_y","sensor_zmp_y")
cols = c("2","3","4","5","6")

zeros = dat$time
zeros[zeros > -100] = mean(dat$pre_y,na.rm=T) +5
points(dat$time,zeros,pch=18,col=dat$state+2)

points(dat$time,dat$com_y,type="l",col=2)
points(dat$time,dat$pre_y,type="l",col=3)
points(dat$time,dat$zmp_y,type="l",col=4)
points(dat$time,dat$real_com_y,type="p",col=5)
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


dat$accel_x = -((-9.8)/.31)*(dat$sensor_zmp_x - dat$com_x)
dat$accel_y = -((-9.8)/.31)*(dat$sensor_zmp_y - dat$com_y)

pdf(paste(name,"-accel",PDF,sep=""))
plot(dat$time,dat$angleX,ylim=c(-20,20),pch="",main="",xlab="x",ylab="y")
names = c("accX","accY","accZ")
cols = c("2","3","4")
points(dat$time,dat$accX,type="l",col=2)
points(dat$time,dat$accY,type="l",col=3)
points(dat$time,dat$accZ,type="l",col=4)
legend("top",lwd=2,legend=names,col=cols)
dev.off()


pdf(paste(name,"-angle",PDF,sep=""))
plot(dat$time,dat$angleX,ylim=c(-pi/20,pi/20),pch="",main="",xlab="x",ylab="y")
names = c("angleX","angleY")
cols = c("2","3")

points(dat$time,dat$angleX,type="l",col=2)
points(dat$time,dat$angleY,type="l",col=3)
points(dat$time,dat$accX/40 + .1,type="l",col=4)
points(dat$time,dat$accY/40 - .1,type="l",col=5)
legend("top",lwd=2,legend=names,col=cols)
dev.off()


pdf(paste(name,"-rightfsr",PDF,sep=""),width=80, height=20)
plot(dat$time,dat$rrr,ylim=c(0,4000),pch="",main="",xlab="x",ylab="y")
names = c("rear right","rear left","front right","front left")
cols = c("2","3","4","5")
points(dat$time, dat$rrr,type ="l",col=2)
points(dat$time, dat$rrl,type ="l",col=3)
points(dat$time, dat$rfr,type ="l",col=4)
points(dat$time, dat$rfl,type ="l",col=5)
legend("top",lwd=2,legend=names,col=cols)
dev.off()

pdf(paste(name,"-leftfsr",PDF,sep=""),width=80, height=20)
plot(dat$time,dat$rrr,ylim=c(0,4000),pch="",main="",xlab="x",ylab="y")
names = c("rear right","rear left","front right","front left")
cols = c("2","3","4","5")
points(dat$time, dat$lrr,type ="l",col=2)
points(dat$time, dat$lrl,type ="l",col=3)
points(dat$time, dat$lfr,type ="l",col=4)
points(dat$time, dat$lfl,type ="l",col=5)
legend("top",lwd=2,legend=names,col=cols)
dev.off()