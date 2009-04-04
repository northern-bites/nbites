tupo = 2;

fileload = load('/tmp/matlab_com_log.xls');
global fileload

function [output] = filewalk(y)
  global fileload
  index = uint32(y*50) + 1;
  output = fileload(index, 5);
  %output = 0;
endfunction

function [output] = sensorzmp(y)
  global fileload
  index = uint32(y*50) + 1;
  output = fileload(index, 9);
endfunction

function [output] = stepwalk(y)
  amplitude = 10;
  stopWalkTime = 4;
  x = y - 65*0.02;
  if x > 0 && x < stopWalkTime
    output = amplitude*ceil(x );
    return;
  else
    if x >= stopWalkTime
      output = amplitude*stopWalkTime;
      return;
    else
      output = 0;
      return;
    end
  end
endfunction

function [output] = osciwalk(y)
  phase = pi * 2;
  amplitude = 10;
  x = y - 2;
  if x > 0 && x < 5
    output = amplitude*sin(x*phase);
    return;
  else
    output = 0;
    return;
  end
endfunction

function [output] = osciwalk_forward(y)
  phase = pi*2.5;
  amplitude = 10;
  x = y - 2;
  if x > 0 && x < 4*pi
    output = amplitude * ( x*phase + sin(x*phase) );
    return;
  else
    output = 0;
    return;
  end
endfunction

function [output] = continuous_forward(y)
  phase = pi*2.5;
  amplitude = 10;
  stopWalkTime = 4;
  x = y - 2;
  if x > 0 && x < stopWalkTime
    if ceil(x) - x < .1
      output = 1/amplitude * (x * phase + sin(x * phase) );
      return;
    else
      output = amplitude*ceil(x );
      return;
    endif
  else
    if x >= stopWalkTime
      output = amplitude*stopWalkTime;
      return;
    else
      output = 0;
      return;
    end
  end
endfunction

function [output] = pRef(i)
  output = filewalk(i);
  %output = stepwalk(i);
  %output = osciwalk(i);
  %output = osciwalk_forward(i);
  %output = continuous_forward(i);
endfunction

function [output]=disturb(i)
  amplitude = 10;
  if i > 2
    if i < 5
      output = amplitude;
      return;
    end
  end
  output = 0;
endfunction

% Now we start our simulation
x=[0;0;0];
result_x = [];
result_vel = [];
result_zmp = [];
result_zmp_ref = [];
trackingError = 0;

endTime = 10;

for time=[0:dt:endTime],
  preview_frames = [];

  for j=[1:numPreviewFrames],
    preview_frames(j) = real( Gd(j)*pRef(time + j*dt, fileload) );
  end;
  trackingError = trackingError + (c0*x - pRef(time));
  u = -Gi*trackingError - sum(preview_frames);
  %disturbance = disturb(time);
  disturbance = 0.;
  % psensor would normally get calculated using accelerometers on the robot
  % Here, we just assume the sensors always agree with our model (exactly = ).
  %psensor = pRef(time) + disturbance;
  psensor = sensorzmp(time, fileload); % this doesn't work
  x=A*x - L*(psensor - c0*x) + b0*u;
  result_x = [result_x x(1)];
  result_vel = [result_vel x(2)];
  result_zmp = [result_zmp c0*x];
  result_zmp_ref = [result_zmp_ref pRef(time)];
end;
clf();
plot([dt:dt:endTime+dt],[result_zmp; result_zmp_ref; result_x])
% the legend doesn't work =/
%legend(["zmp","zmp_ref","pos"],pos=4)

