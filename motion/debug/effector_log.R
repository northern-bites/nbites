# R file to read in a debug walking output and generate pdf reports
# Instructions: Install the R language (package 'r-base' in debian)
#               run 'make' in the terminal
# Author: Johannes Strom
# Date: February 2008


name = "effector_log"
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
chain_indices = function(chainID){
      start = chainID*3 -1
      end = start +2
      return(c(start:end))
}

c_names = c("head","larm","lleg","rleg","rarm")
for(chn in c(1:5)){
      indices=chain_indices(chn)
      dim="_x"
      pdf(paste(c_names[chn],"_",name,dim,PDF,sep=""))
      plot(dat$time,dat[,indices[1]],type="l",
           main=paste(c_names[chn],dim,sep=""),xlab="s",ylab="mm",col=3)
      legend("top",lwd=2,legend=labels(dat)[[2]][indices[1]],col=3)
      dev.off()

      dim="_y"
      pdf(paste(c_names[chn],"_",name,dim,PDF,sep=""))
      plot(dat$time,dat[,indices[2]],type="l",
           main=paste(c_names[chn],dim,sep=""),xlab="s",ylab="mm",col=4)
      legend("top",lwd=2,legend=labels(dat)[[2]][indices[2]],col=4)
      dev.off()

      dim="_z"
      pdf(paste(c_names[chn],"_",name,dim,PDF,sep=""))
      plot(dat$time,dat[,indices[3]],type="l",
           main=paste(c_names[chn],dim,sep=""),xlab="s",ylab="mm",col=5)
      legend("top",lwd=2,legend=labels(dat)[[2]][indices[3]],col=5)
      dev.off()

}