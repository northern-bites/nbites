# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: February 2008


dat = read.table("/tmp/joints_log.xls",header=T,na.strings=c("-"))

name = "joints_log"
PDF = ".pdf"

#labels = dat[0,]
dummy = dat[,2] #for setting limits
dummy[1] = pi
dummy[2] = -pi

chains = c(c(1:2),c(3:6),c(7:12),c(13:18),c(19:22))

plot(dat$time,dummy,pch="",main="",xlab="s",ylab="rad")
indices=c(1:22)
color_list = c()
for(i in indices){
      #print(paste("Here is the number: ", i))
      color = i %%7 + 1
      color_list = append(color_list,c(color))
      print("color list is")
      print( color_list)
      #print(color)
      points(dat$time,dat[,i],type="l",col=color)

}
legend("top",lwd=2,legend=labels(dat)[[2]],col=color_list)