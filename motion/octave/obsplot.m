startTime = 1;
endTime = 3;
plot([startTime:dt:endTime],[result_zmp(startTime/dt:endTime/dt) ; result_zmp_ref(startTime/dt:endTime/dt) ; result_x(startTime/dt:endTime/dt)])
