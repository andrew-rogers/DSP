/**
 *
 * @licstart  The following is the entire license notice for the 
 *  JavaScript code in this page.
 *
 * Copyright (C) 2015  Andrew Rogers
 *
 *
 * The JavaScript code in this page is free software: you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License (GNU GPL) as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.  The code is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU GPL for more details.
 *
 * As additional permission under GNU GPL version 3 section 7, you
 * may distribute non-source (e.g., minimized or compacted) forms of
 * that code without the copy of the GNU GPL normally required by
 * section 4, provided you include this license notice and a URL
 * through which recipients can access the Corresponding Source.
 *
 * @licend  The above is the entire license notice
 * for the JavaScript code in this page.
 *
 */

/*
 Complex numbers are represented as a two-element array, the first element
 is the real part and the second element is the imaginary part.

 var complex_num=[real, imag];

 Real numbers are simply represented by their real part.

 var G=6.674e-11;
*/

function c2string(c)
{
  var rs=""+c[0];
  if(c[1]<0.0){
    var is=" -i"+(-c[1]);
  }else{
    var is=" +i"+c[1];
  }
  return(rs+is);
}

function expi(theta)
{
  var real=Math.cos(theta);
  var imag=Math.sin(theta);
  return([real,imag]);
}

function conj(a)
{
  return([a[0],-a[1]]);
}

function cadd(a,b)
{
  return([a[0]+b[0],a[1]+b[1]]);
}

function cmul(a,b)
{
  var real=a[0]*b[0]-a[1]*b[1];
  var imag=a[0]*b[1]+a[1]*b[0];
  return([real,imag]);
}

function rcmul(r,c)
{
  return([r*c[0], r*c[1]]);
}

function csub(a,b)
{
  return([a[0]-b[0],a[1]-b[1]]);
}

function cdiv(a,b)
{
  var real=a[0]*b[0]+a[1]*b[1];
  var imag=a[1]*b[0]-a[0]*b[1];
  var d=b[0]*b[0]+b[1]*b[1];
  return([real/d,imag/d]);
}

function cdivabs(a,b)
{
  return(Math.sqrt((a[0]*a[0]+a[1]*a[1])/(b[0]*b[0]+b[1]*b[1])));
}

// Evaluate a polynomial whose coefficients are specified in p
function cpolyval(p,x)
{
  var y=[p[0]];
  y.length=x.length;
  for(var i=0; i<x.length; i++) y[i]=p[0];
  for(var j=1; j<p.length; j++){
    for(var i=0; i<x.length; i++){
      y[i]=cadd(cmul(y[i],x[i]),p[j]);
    }
  }
}
