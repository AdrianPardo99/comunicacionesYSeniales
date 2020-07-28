pkg load control
pkg load signal
R=1
L=0.000047
C=0.0000000018
num=[1/L 0]
denom=[1 R/L 1/(L*C)]
f=1/(2*pi*sqrt(L*C))
w=2*pi*f
w_range=[.8*w:20:1.2*w]
h=bode(tf(num,denom),w_range)

magh=abs(h)

plot(w_range,magh)
shg
freqs(num,denom,w_range)
pause(10)
