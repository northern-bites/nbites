# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: Decemeber 2008



foot_locus = function(foot){

name = paste(foot,"_sensor_log",sep="")
PDF = ".pdf"

file = paste("/tmp/",name,".xls",sep="")
if(!file.exists(file))
	quit("no")
dat = read.table(file,header=T,na.strings=c("-"))

dat$range = dat$angleX
dat$range[1] = .6
dat$range[2] = -.6

pdf(paste(name,PDF,sep=""))
plot(dat$time,dat$range,pch="",main=paste(foot,"leg","x"),xlab="s",ylab="rads")
names = c("bodyAngleX","angleX","sensorAngleX",
	  "bodyAngleY","angleY","sensorAngleY")
cols = c(2,3,4,
         5,6,1)
i =1
points(dat$time,dat$bodyAngleX,type="l",col=cols[i]); i = i +1;
points(dat$time,dat$angleX,type="l",col=cols[i]); i = i +1;
points(dat$time,dat$sensorAngleX,type="l",col=cols[i]); i = i +1;

points(dat$time,dat$bodyAngleY,type="l",col=cols[i]); i = i +1;
points(dat$time,dat$angleY,type="l",col=cols[i]); i = i +1;
points(dat$time,dat$sensorAngleY,type="l",col=cols[i]); i = i +1;

legend("top",lwd=2,legend=names,col=cols)

zeros = rep(-1.0,length(dat$time))
points(dat$time,zeros,pch=1,col=dat$state+2)
names = c("Supporting","Swinging","Dbl Sup", "P. Dbl Sup")
legend("topleft",lwd=2,legend=names,col=c(2:5))
dev.off()

}


foot_locus("left")
foot_locus("right")