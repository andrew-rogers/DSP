pkg load signal

f1=500;
f2=3900;
fs=9000;

t=(0:999)/fs;
x=sin(2*pi*f1*t)+sin(2*pi*f2*t);

figure
bode_plot(x,fs,100,fs/2);

Ny=1101
y1=resample_fractional(x,Ny);

figure
bode_plot(y1,fs,100,fs/2);

Ny=950
y2=resample_fractional(x,Ny);

figure
bode_plot(y2,fs,100,fs/2);