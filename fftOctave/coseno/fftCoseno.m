pkg load control
pkg load signal
k=0:31;
Tm=1/2000;
n=0:31;

for m=0:32 #Obtiene la fft para 33 señales coseno diferentes (en ω)
  x=cos(2*pi*m*62.5*n*Tm); # n * Tm es el tiempo, m * 62.5 es la frecuencia (ω)
  X=fft(x)
  subplot(3,1,1); #Senial muestreada
    stem(k,x)
    titulo=sprintf("x[n]=cos(2*pi*%d*t)",62.5*m);
    title(titulo);
    axis([0,31,-1.5,1.5]);
  subplot(3,1,2); #Parte real de la senial muestreada
    stem(k,real(X));
    title("Real DFT x[n]");
    axis([0,31,-33,33]);
  subplot(3,1,3); #Parte imaginaria de la senial muestreada
    stem(k,imag(X));
    title("Img DFT x[n]");
    axis([0,31,-33,33]);
  nombre=sprintf("Figura%02d.jpg",m);
  print(nombre)
end
