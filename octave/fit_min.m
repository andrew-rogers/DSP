function [p]=fit_min(f,h,N)
  
  if nargin<3
    N=2; % if degree is not given then use quadratic.
  endif
  
  % Find minimum
  [h_min,I_min]=min(h);
  
  dev=(mean(h)-h_min)/10;
  
  % Find range that deviates above minimum by dev
  I_curve=find(h<(h_min+dev));
  I0=I_curve(1);
  I1=I_curve(end);
  
  % Fit a degree N polynomial to the valley
  p=polyfit(f(I0:I1),h(I0:I1),N);
endfunction
