pkg load control
pkg load signal

for p=1:4;
Tm=1/(p*2000);

m=1;
n=0:p*32-1;
k=0:p*32-1;
  x=sin(2*pi*62.5*n*Tm); # n*TM es el tiempo, 62.5 es la ω
  X=fft(x);

  subplot(3,1,1); # Señal muestreada
    stem(k,x);
    titulo=sprintf("x[n]=sin(2*pi*%d*t), Fm=%d, N=%d, intervalo muestrado 0.016s",62.5*m,p*2000,p*2000*.016);
    title(titulo);
  subplot(3,1,2);
    stem(k,real(X));
    title("Real axis DFT x[n]");
    axis([0,p*32-1,-1.5,1.5]);
  subplot(3,1,3);
    stem(k,imag(X));
    title("Imaginary axis DFT x[n]");
    axis([0,p*32-1,-33,33]);
  nombre=sprintf("sameTimeDiferentFrec%02d.jpg",p);
  print(nombre);
  pause(5);
end
