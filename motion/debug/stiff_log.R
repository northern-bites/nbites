# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: February 2008

name = "stiff_log"
PDF = ".pdf"

file = paste("/tmp/",name,".xls",sep="")
if(!file.exists(file))
	quit("no")
dat = read.table(file,header=T,na.strings=c("-"))

#labels = dat[0,]
dummy = dat[,2] #for setting limits
dummy[1] = pi
dummy[2] = -pi

#note, time occupies slot 1, so the indices are shifted by one
chain_lengths = function(chainID){
      if(chainID ==1){
	return(c(2:3))
      }
      if(chainID ==2){
	return(c(4:7))
      }
      if(chainID ==3){
	return(c(8:13))
      }
      if(chainID ==4){
	return(c(14:19))
      }
      if(chainID ==5){
	return(c(20:23))
      }
      return(c())
}

c_names = c("head","larm","lleg","rleg","rarm")
for(chn in c(1:5)){
      pdf(paste(c_names[chn],"_",name,PDF,sep=""),width=10, height=7)
      indices=chain_lengths(chn)

      #find the range of the graph
      mind = min(dat[,indices])
      maxd = max(dat[,indices])
      dummy = dat[,2] #for setting limits
      dummy[1] = mind
      dummy[2] = maxd
      plot(dat$time,dummy,pch="",main=c_names[chn],xlab="s",ylab="rad")

      color_list = c()
      color = 1
      for(i in indices){
      	    color = color +1
      	    color_list = append(color_list,c(color))
      	    points(dat$time,dat[,i],type="l",col=color)
      }
      legend("bottom",lwd=2,legend=labels(dat)[[2]][indices],col=color_list)
      dev.off()
}