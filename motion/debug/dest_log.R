# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: Decemeber 2008



foot_dest = function(foot){
dat = read.table(paste("/tmp/",foot,"_dest_log.xls",sep=""),header=T)

name = paste(foot,"_dest_log",sep="")
PDF = ".pdf"

pdf(paste(name,"-x",PDF,sep=""))
miny = min(min(dat$dest_x),min(dat$src_x))
maxy = max(max(dat$dest_x),max(dat$src_x))
#plot(dat$time,dat$dest_x,ylim=c(miny,maxy),pch="",main="",xlab="s",ylab="mm")
plot(dat$time,dat$dest_x,pch="",main="",xlab="s",ylab="mm")
names = c("dest_x")
cols = c("2")
points(dat$time,dat$dest_x,type="l",col=2)
#points(dat$time,dat$src_x,type="l",col=3)
legend("top",lwd=2,legend=names,col=cols)
zeros = dat$time
zeros[zeros > -100] =  mean(dat$dest_x) +10
points(dat$time,zeros,pch=1,col=dat$state+2)
names = c("Supporting","Swinging","Dbl Sup", "P. Dbl Sup")
legend("topleft",lwd=2,legend=names,col=c(2:5))
dev.off()

pdf(paste(name,"-y",PDF,sep=""))
plot(dat$time,dat$dest_y,pch="",main="",xlab="s",ylab="mm")
names = c("dest_y")
cols = c("2")
points(dat$time,dat$dest_y,type="l",col=2)
legend("top",lwd=2,legend=names,col=cols)
zeros = dat$time
zeros[zeros > -100] =  mean(dat$dest_y) +10
points(dat$time,zeros,pch=1,col=dat$state+2)
names = c("Supporting","Swinging","Dbl Sup", "P. Dbl Sup")
legend("topleft",lwd=2,legend=names,col=c(2:5))
dev.off()

}


foot_dest("left")
foot_dest("right")