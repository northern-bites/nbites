dt = 0.01;
R = 1*10^(-6);
N = 120;
numPreviewFrames = N;
Qx = 0.25;  Qe = 0.3;
Ql = [1,0,0;0,1,0;0,0,1];

g = 9800;
z_h = 260;

A0 = [1, dt, 0; g/z_h*dt, 1, -g/z_h*dt; 0, 0, 1];
b0 = [0; 0; dt];
c0 = [0, 0, 1];

Bt(1,1)=c0*b0;
Bt(2:4,1)=b0(1:3);
It(1,1)=1;
It(2:4,1)=0;
Ft(1,1:3)=c0*A0;
Ft(2:4,1:3)=A0(1:3,1:3);
Qt(1:4, 1:4)=0;
Qt(1,1)=Qe;
Qt(2:4,2:4)=c0' *Qx*c0;
At(1:4,1)=It;
At(1:4,2:4)=Ft;


Pt=dare(At, Bt, Qt, R);

Gx = (1/(R+Bt'*Pt*Bt)) * Bt'*Pt*Ft;
Gi = (1/(R+Bt'*Pt*Bt)) * Bt'*Pt*It;

Ac = At - Bt*(1/(R + Bt'*Pt*Bt)) * Bt'*Pt*At;
X = -Ac'*Pt*It;
Gd(1) = -Gi;
for i=2:N,
  Gd(i) = (1/(R + Bt'*Pt*Bt))*Bt'*X;
  X = Ac'*X;
end

A = A0-b0*Gx;

L = dlqr(A', c0', Ql , R)';

endTime = 10;
for time=[0:dt:endTime],
  preview_frames = [];

  for j=[1:numPreviewFrames],
    preview_frames(j) = real( Gd(j)*(time + j*dt) );
  end;
end;
